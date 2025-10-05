#!/usr/bin/env python
# encoding: utf-8

import os
import shutil
import multiprocessing
import subprocess
from waflib import Logs
from waflib.Configure import conf
import re

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
def git_submodule_update(conf):
	try:
		# Initialize and update git submodules
		conf.cmd_and_log(['git', 'submodule', 'init'])
		conf.cmd_and_log(['git', 'submodule', 'update'])
	except Exception as e:
		Logs.pprint('RED', 'Error updating git submodules: {}'.format(e))

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
def check_lief(conf):
	includes = os.path.abspath('ext/LIEF-bin/include/')
	libs = os.path.abspath('ext/LIEF-bin/lib/')

	version_code = "\n".join([
		'#include <iostream>',
		'#include <LIEF/version.h>',
		'int main() {',
		'	std::cout << LIEF_VERSION << std::endl;',
		'	return 0;',
		'}',
		])
	try:
		v = conf.check(stlib='LIEF', stlibpath=[libs], includes=[includes], uselib_store='LIEF', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library LIEF version', v.strip())
	except Exception as e:
		installdir = os.path.abspath('ext/LIEF-bin/')
		conf.msg('Building local LIEF', installdir)
		if not os.path.exists('ext/LIEF/build'):
			os.mkdir('ext/LIEF/build')
		# needs to be built with -fPIC in order to work
		cwd = os.path.abspath('ext/LIEF/build')
		execute('cmake -DCMAKE_POSITION_INDEPENDENT_CODE=True -DCMAKE_INSTALL_PREFIX={} ../'.format(installdir), cwd)
		execute('make -j{}'.format(multiprocessing.cpu_count()), cwd)
		execute('make install', cwd)

		v = conf.check(stlib='LIEF', stlibpath=[libs], includes=[includes], uselib_store='LIEF', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library LIEF version', v.strip())

@conf
def check_axml(conf):
	includes = os.path.abspath('ext/axml-parser-bin/include/')
	libs = os.path.abspath('ext/axml-parser-bin/lib/')

	version_code = "\n".join([
		'#include <iostream>',
		'#include <axml/axml_file.h>',
		'int main() {',
		'	std::cout << "master" << std::endl;',
		'	return 0;',
		'}',
		])
	try:
		v = conf.check(stlib='axml_parser', stlibpath=[libs], includes=[includes], uselib_store='AXML', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library axml-parser version', v.strip())
	except Exception as e:
		installdir = os.path.abspath('ext/axml-parser-bin/')
		conf.msg('Building local axml-parser', installdir)
		if not os.path.exists('ext/axml-parser/build'):
			os.mkdir('ext/axml-parser/build')
		# needs to be built with -fPIC in order to work
		cwd = os.path.abspath('ext/axml-parser/build')
		execute('cmake -DCMAKE_POSITION_INDEPENDENT_CODE=True -DCMAKE_INSTALL_PREFIX={} ../'.format(installdir), cwd)
		execute('make -j{}'.format(multiprocessing.cpu_count()), cwd)
		# Manually install libaxml_parser.a and includes
		lib_file = os.path.join(cwd, 'libaxml_parser.a')
		install_lib_dir = os.path.join(installdir, 'lib')
		install_include_dir = os.path.join(installdir, 'include')

		if not os.path.exists(install_lib_dir):
			os.makedirs(install_lib_dir)
		if not os.path.exists(install_include_dir):
			os.makedirs(install_include_dir)

		shutil.copy2(lib_file, install_lib_dir)
		copytree(os.path.join(cwd, '../include'), install_include_dir)

		v = conf.check(stlib='axml_parser', stlibpath=[libs], includes=[includes], uselib_store='AXML', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library axml version', v.strip())

@conf
def check_args(conf):
	includes = os.path.abspath('ext/args/')

	version_code = "\n".join([
		'#include <iostream>',
		'#include <args.hxx>',
		'int main() {',
		'	std::cout << ARGS_VERSION;',
		'	return 0;',
		'}',
		])
	try:
		v = conf.check(header_name='args.hxx', includes=[includes], uselib_store='ARGS', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library args version', v.strip())
	except Exception as e:
		v = conf.check(header_name='args.hxx', includes=[includes], uselib_store='ARGS', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library args version', v.strip())
	conf.undefine('HAVE_ARGS_HXX')


@conf
def check_googletest(conf):
	Logs.pprint('BLUE', 'Checking for test dependencies')
	includes = os.path.abspath('ext/googletest-bin/include/')
	libs = os.path.abspath('ext/googletest-bin/lib/')

	try:
		conf.check(stlib='gtest', stlibpath=[libs], includes=[includes], uselib_store='GOOGLETEST', define_ret=True)
	except Exception as e:
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
def check_xxhash(conf):
	includes = os.path.abspath('ext/xxHash-bin/include/')
	libs = os.path.abspath('ext/xxHash-bin/lib/')

	version_code = "\n".join([
		'#include <stdio.h>',
		'#include <xxhash.h>',
		'int main() {',
		'    printf("%u\\n", XXH_versionNumber());',
		'    return 0;',
		'}',
	])
	try:
		v = conf.check(stlib='xxhash', stlibpath=[libs], includes=[includes], uselib_store='XXHASH', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library xxhash version', v.strip())
	except Exception as e:
		installdir = os.path.abspath('ext/xxHash-bin/')
		conf.msg('Building local xxhash', installdir)
		if not os.path.exists('ext/xxHash/build'):
			os.mkdir('ext/xxHash/build')
		cwd = os.path.abspath('ext/xxHash/build')
		execute('cmake -DCMAKE_INSTALL_PREFIX={} -DBUILD_SHARED_LIBS=OFF -DXXHASH_BUILD_XXHSUM=OFF ../cmake_unofficial'.format(installdir), cwd)
		execute('cmake --build .', cwd)
		execute('cmake --build . --target install', cwd)

		v = conf.check(stlib='xxhash', stlibpath=[libs], includes=[includes], uselib_store='XXHASH', fragment=version_code, execute=True, define_ret=True)
		conf.msg('Checking for library xxhash version', v.strip())

@conf
def check_dependencies_tools(conf):
	Logs.pprint('BLUE', 'Checking for system dependencies tools and libraries')
	conf.find_program('pkg-config', mandatory=True)
	conf.find_program('cmake', mandatory=True)
	cmake_version = conf.cmd_and_log(['cmake', '--version']).splitlines()[0]
	match = re.search(r'version\s+(\d+)\.(\d+)\.(\d+)', cmake_version)
	if not match:
		conf.fatal('Could not determine cmake version')
	major, minor, patch = map(int, match.groups())
	if (major, minor) < (3, 24):
		conf.fatal('CMake >= 3.24 is required (found {}.{}.{})'.format(major, minor, patch))
	conf.msg('Checking for cmake version >= 3.24', '{}.{}.{}'.format(major, minor, patch))
	Logs.pprint('BLUE', 'Checking for local dependencies tools and libraries')
