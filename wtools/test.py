#!/usr/bin/env python
# encoding: utf-8

import sys, os, platform
from wtools import testrunner

def setenv(bld):
	libpath = [bld.bldnode.abspath()] + bld.env.LIBPATH_ZMQ
	if platform.system() == "Windows":
		if 'PATH' in os.environ:
			os.putenv('PATH', os.pathsep.join([lib for lib in libpath]) + os.pathsep + os.environ['PATH'])
		else:
			os.putenv('PATH', os.pathsep.join([lib for lib in libpath]))
	else:
		if 'LD_LIBRARY_PATH' in os.environ:
			os.environ['LD_LIBRARY_PATH'] = (os.pathsep.join([lib for lib in libpath]) + os.pathsep + os.environ['LD_LIBRARY_PATH'])
		else:
			os.environ['LD_LIBRARY_PATH'] = (os.pathsep.join([lib for lib in libpath]))

class UnitTest:
	bld = None

	def __init__(self, bld, testname=None):
		self.bld = bld
		self.testname = testname

	def run(self):
		bld = self.bld

		bld.stlib(
			name		= 'googletest',
			target		= 'googletest',
			source		= bld.path.ant_glob(['ext/googletest/googletest/src/*.cc']),
			includes	= ['ext/googletest/googletest', 'ext/googletest/googletest/include'],
			install     = None,
		)

		excl = []
		if self.testname == None:
			tests = bld.path.ant_glob(['tests/unit/**/test_*.cpp'], excl=excl)
		else:
			tests = bld.path.ant_glob(['tests/unit/**/test_{}.cpp'.format(self.testname)], excl=excl)

		bld.program(
			name		= 'test_all',
			target		= 'test_all',
			source		= tests,
			defines		= ['__unit_test__'],
			includes	= ['.', 'src', 'ext/googletest/googletest/include', 'ext/fmt/include'],
			use			= ['sandvik', 'googletest', 'PTHREAD', 'FMT'],
			install     = None,
		)

		xxx = []
		for test in tests:
			testName = os.path.splitext(os.path.basename(test.abspath()))[0]

			#check  duplicate tests (same name)
			if testName in xxx:
				bld.fatal("error %s already exists" % testName)
			xxx += [testName]

			bld.program(
				name		= testName,
				target		= testName,
				source		= [test],
				defines		= ['__unit_test__'],
				includes	= ['.', 'src', 'ext/googletest/googletest/include'],
				use			= ['sandvik', 'googletest', 'PTHREAD', 'FMT'],
				install     = None,
			)

			bld.runGoogleTest(program = testName)
