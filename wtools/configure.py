#!/usr/bin/env python
# encoding: utf-8

import os
import shutil
import multiprocessing
import subprocess
from waflib import Logs
from waflib.Configure import conf

def clone_repository(dir, repository, tag):
	# Check if the repository directory exists
	if not os.path.exists(dir):
		print(f"Cloning repository {repository}")
		# Clone the repository with the specified tag
		clone_command = ["git", "clone", "--branch", tag, repository, dir]
		subprocess.run(clone_command, check=True)
		print("Repository cloned successfully.")
	else:
		raise("Repository directory already exists! Something went wrong.")

def execute(cmd, cwd, env_vars=None):
	print(cmd)
	bcmd = cmd.split(' ')
	env = os.environ.copy()
	if env_vars:
		env.update(env_vars)
	subprocess.run(bcmd, cwd=cwd, check=True,  env=env)
	#p = subprocess.Popen(bcmd, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	#(stdout, stderr) = p.communicate()
	#return (stdout, stderr)

def copytree(src, dst, symlinks=False, ignore=None):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            shutil.copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)

@conf
def check_fmt(conf):
	includes = os.path.abspath('ext/fmt-bin/include/')
	libs = os.path.abspath('ext/fmt-bin/lib/')

	version_code = "\n".join([
		'#include <fmt/core.h>',
		'int main() {',
		'	fmt::print("{}.{}.{}\\n", FMT_VERSION / 10000, (FMT_VERSION / 100) % 100, FMT_VERSION % 100);',
		'	return 0;',
		'}',
	])

	try:
		v = conf.check(stlib='fmt', stlibpath=[libs], includes=[includes], uselib_store='FMT', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library fmt version', v.strip())
	except:
		clone_repository('ext/fmt', 'https://github.com/fmtlib/fmt', '11.0.2')
		installdir = os.path.abspath('ext/fmt-bin/')
		conf.msg('Building local fmt', installdir)
		if not os.path.exists('ext/fmt/build'):
			os.mkdir('ext/fmt/build')

		cwd = os.path.abspath('ext/fmt/build')
		execute('cmake -DCMAKE_POSITION_INDEPENDENT_CODE=True -DCMAKE_INSTALL_PREFIX={} ../'.format(installdir), cwd)
		execute('make -j{}'.format(multiprocessing.cpu_count()), cwd)
		execute('make install', cwd)

		v = conf.check(lib='fmt', libpath=[libs], includes=[includes], uselib_store='FMT', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library fmt version', v.strip())

@conf
def check_ffi(conf):
	includes = os.path.abspath('ext/libffi-bin/include/')
	libs = os.path.abspath('ext/libffi-bin/lib/')

	version = 'v3.4.7'
	version_code = "\n".join([
		'#include <stdio.h>',
		'#include <ffi.h>',
		'void print_version() {',
		f'	printf("{version}");',
		'}',
		'int main() {',
		'	ffi_cif cif;',
		'	ffi_type *ret_type = &ffi_type_void;',
		'	ffi_type *arg_types[] = { NULL };',
		'	ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 0, ret_type, arg_types);',
		'	if (status != FFI_OK) {',
		'		return -1;',
		'	}',
		'	void (*func_ptr)(void) = &print_version;',
		'	ffi_call(&cif, FFI_FN(func_ptr), NULL, NULL);',
		'	return 0;',
		'}',
	])
	try:
		if not os.path.exists(os.path.join(libs, 'libffi.a')):
			raise
		v = conf.check(stlib='ffi', stlibpath=[libs], includes=[includes], uselib_store='FFI', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library ffi version', v.strip())
	except Exception as e:
		clone_repository('ext/libffi', 'https://github.com/libffi/libffi.git', version)
		installdir = os.path.abspath('ext/libffi-bin/')
		conf.msg('Building local libffi', installdir)
		cwd = os.path.abspath('ext/libffi')
		execute('./autogen.sh', cwd)
		execute('./configure --disable-shared --enable-static --prefix={}'.format(installdir), cwd, env_vars={ 'CFLAGS': '-fPIC', 'CXXFLAGS': '-fPIC' })
		execute('make -j{}'.format(multiprocessing.cpu_count()), cwd)
		execute('make install', cwd)

		v = conf.check(stlib='ffi', stlibpath=[libs], includes=[includes], uselib_store='FFI', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library ffi version', v.strip())

@conf
def check_googletest(conf):
	Logs.pprint('BLUE', 'Checking for test dependencies')
	includes = os.path.abspath('ext/googletest-bin/include/')
	libs = os.path.abspath('ext/googletest-bin/lib/')

	try:
		conf.check(stlib='gtest', stlibpath=[libs], includes=[includes], uselib_store='GOOGLETEST', define_ret=True)
	except Exception as e:
		clone_repository('ext/googletest', 'https://github.com/google/googletest.git', 'v1.14.0')
		installdir = os.path.abspath('ext/googletest-bin/')
		conf.msg('Building local googletest', installdir)
		if not os.path.exists('ext/googletest/build'):
			os.mkdir('ext/googletest/build')
		# needs to be built with -fPIC in order to work
		cwd = os.path.abspath('ext/googletest/build')
		execute('cmake -DCMAKE_POSITION_INDEPENDENT_CODE=True -DCMAKE_INSTALL_PREFIX={} ../'.format(installdir), cwd)
		execute('make -j{}'.format(multiprocessing.cpu_count()), cwd)
		execute('make install', cwd)

		conf.check(stlib='gtest', stlibpath=[libs], includes=[includes], uselib_store='GOOGLETEST', define_ret=True)


@conf
def check_dependencies_tools(conf):
	Logs.pprint('BLUE', 'Checking for system dependencies tools and libraries')
	conf.find_program('pkg-config', mandatory=True)
	conf.find_program('cmake', mandatory=True)
	conf.check(lib="z")
	Logs.pprint('BLUE', 'Checking for local dependencies tools and libraries')
