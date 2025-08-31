#! /usr/bin/env python
# encoding: utf-8

import os, sys
from waflib import Options, Utils
sys.path += ['wtools']
from wtools import configure, version, test, git

from waflib.Tools.compiler_c import c_compiler
from waflib.Tools.compiler_cxx import cxx_compiler

top = '.'
out = 'wbuild'

APPNAME='sandvik'
VERSION='1.0.0'

def options(opt):
	opt.load('python')
	opt.load('compiler_c')
	opt.load('compiler_cxx')

	default_prefix= opt.path.abspath() + os.sep + 'delivery'
	opt.add_option('--prefix',dest='prefix',default=default_prefix,help='installation prefix [default: %r]'%default_prefix)

	opt.add_option('--debug', action='store_true', default=False, help = 'configure build in debug mode', dest = 'debug')
	opt.add_option('--tests', action='store_true', default=False, help='Launch all tests for the target', dest='tests')
	opt.add_option('--test-name', action='store', default=None, help='Name of test to run', dest='test_name')

def configure(conf):
	conf.load('python')
	conf.check_python_version((3,0,0))

	conf.load('java')
	conf.load('d8', tooldir='wtools')

	conf.git_submodule_update()

	conf.load('doxygen')

	# set clang/clang++ as default compiler
	#c_compiler['linux'] = ['clang'] + c_compiler['linux']
	#cxx_compiler['linux'] = ['clang++'] + cxx_compiler['linux']

	conf.load('compiler_c compiler_cxx')
	conf.msg('Checking for c/c++ compiler version', '.'.join(conf.env['CC_VERSION']))

	# c/cxx flags
	cflags = ['-fPIC', '-Wall', '-Werror', '-O3', '-Wno-unused-parameter', '-fno-strict-aliasing', '-fomit-frame-pointer', '-march=native']
	cxxflags = cflags + []

	conf.env.DEFINES.clear()
	if Options.options.debug:
		cxxflags.append('-g')
		conf.env.DEFINES.append('__debug__')

	#conditional c/cxx flags
	dflags = {'c++only' : ['--std=c++20'], 'all' : []}

	for l, flags in dflags.items():
		for flag in flags:
			if conf.check_cxx(cxxflags=flag, mandatory=True):
				cxxflags.append(flag)
				if l == 'all':
					cflags.append(flag)

	conf.env.CFLAGS = cflags
	conf.env.CLANGFLAGS = cflags
	conf.env.CLANGXXFLAGS = cxxflags
	conf.env.CXXFLAGS = cxxflags

	#code formater
	conf.load('checkstyle', tooldir='wtools')
	#git use to clone dependencies and get the current version/commit
	conf.find_program('git', mandatory=True)

	#-------------------------------------------------
	#check for system libraries
	#-------------------------------------------------
	conf.check(lib="pthread")

	#-------------------------------------------------
	#check for local libraries
	#-------------------------------------------------
	conf.check_dependencies_tools()
	conf.check_fmt()
	conf.check_args()
	conf.check_lief()
	conf.check_ffi()
	conf.check_axml()

	#-------------------------------------------------
	#check for test libraries
	#-------------------------------------------------
	conf.check_googletest()

	conf.env['JAVACFLAGS'] += [
		'-Xlint:-options'       # suppress the bootstrap classpath warning
	]
	#we are done :)

def build(bld):
	gitinfo = git.getInfos(bld)
	#-------------------------------------------------
	# check style
	#-------------------------------------------------
	checkstyle_sources = bld.path.ant_glob(['sanddirt/**/*.java', 'src/**/*.cpp', 'src/**/*.hpp', 'src/**/*.c', 'src/**/*.h'])
	bld.checkstyle(
		inputs = checkstyle_sources,
	)
	#-------------------------------------------------
	# generate version.in.hpp file
	#-------------------------------------------------
	bld.version(gitinfo=gitinfo, version=VERSION, output='src/version.in.hpp',)
	bld.add_group() #make sure formatting is done before going further
	#-------------------------------------------------
	# build java runtime sanddirt.jar
	#-------------------------------------------------

	bld(features   = 'javac jar d8',
		srcdir     = 'sanddirt/', # folder containing the sources to compile
		outdir     = 'sanddirt', # folder where to output the classes (in the build directory)
		compat     = '8', # java compatibility version number
		sourcepath = ['sanddirt'],
		classpath  = ['.', '..'],
		jaropts = [], # can be used to give files
		basedir    = 'sanddirt', # folder containing the classes and other files to package (must match outdir)
		destfile   = 'sanddirt.jar', # do not put the destfile in the folder of the java classes!
	)
	#-------------------------------------------------
	# build sandvik static/shared library
	#-------------------------------------------------
	sources = bld.path.ant_glob(['src/**/*.cpp', 'src/**/*.c'], excl=['src/main.cpp'])
	bld.shlib(
		source          = sources,
		name            = APPNAME,
		target          = APPNAME,
		includes        = ['src'],
		use             = ['FMT', 'LIEF', 'FFI', 'AXML', 'PTHREAD'],
		linkflags       = ["-Wl,-z,defs"],
		install_path    = '${PREFIX}/lib',
	)
	bld.stlib(
		source          = sources,
		name            = APPNAME + '_static',
		target          = APPNAME,
		includes        = ['src'],
		use             = ['FMT', 'LIEF', 'FFI', 'AXML', 'PTHREAD'],
		linkflags       = ["-Wl,-z,defs"],
		install_path    = '${PREFIX}/lib',
	)

	#-------------------------------------------------
	# build sandvik runtime environment
	#-------------------------------------------------
	bld.program(
		source          = 'src/main.cpp',
		name            = "vm_sandvik",
		target          = "sandvik",
		includes        = ['src'],
		use             = [APPNAME + '_static', 'FMT', 'ARGS', 'LIEF', 'FFI', 'AXML', 'PTHREAD'],
		linkflags       = ["-rdynamic", "-Wl,-z,defs"],
		install_path    = '${PREFIX}',
	)

	#-------------------------------------------------
	# install include files
	#-------------------------------------------------
	include_files = bld.path.ant_glob(['src/**/*.hpp', 'src/**/*.h'])
	bld.install_files('${PREFIX}/include', include_files, cwd=bld.path.find_dir('src'))

	#-------------------------------------------------
	# tests
	#-------------------------------------------------
	bld.add_group() #wait for everything to be built before running tests
	if Options.options.tests == True or Options.options.test_name != None:
		#run tests
		tests = [test.UnitTest(bld, Options.options.test_name)]
		for tt in tests:
			tt.run()
