#!/usr/bin/env python
# encoding: utf-8

from waflib import Options
from waflib import Task
from waflib.TaskGen import feature, after_method
from waflib.Configure import conf
import os
import glob

def configure(conf):
	sdk_root = os.environ.get('ANDROID_SDK_ROOT') or os.environ.get('ANDROID_HOME')
	if not sdk_root:
		conf.fatal('ANDROID_SDK_ROOT or ANDROID_HOME environment variable not set')

	build_tools_dir = os.path.join(sdk_root, 'build-tools')
	versions = sorted(glob.glob(os.path.join(build_tools_dir, '*')), reverse=True)
	if not versions:
		conf.fatal('No build-tools found in %s' % build_tools_dir)

	latest_build_tools = versions[0]
	d8_path = os.path.join(latest_build_tools, 'd8')
	conf.find_program('d8', var='D8', path_list=[latest_build_tools], mandatory=True)

class d8(Task.Task):
	color   = 'BLUE'
	run_str = '${D8} --file-per-class-file --output ${TGT} ${SRC}'
	shell = False

@feature('d8')
@after_method('apply_java', 'use_javac_files', 'jar_files')
def process_jar2dex(self):
	# Ensure the destfile attribute is set
	if not hasattr(self, 'destfile'):
		raise ValueError("The 'destfile' attribute is not set for this task generator.")
	jar = self.path.find_or_declare(self.destfile)
	dexjar = self.path.find_or_declare(jar.name.replace('.jar', '.dex.jar'))
	# Create the d8 task
	tsk = self.create_task('d8', jar, dexjar)
	tsk.failOnError = True

@conf
def d8(ctx, *k, **kw):
	kw['features'] = 'd8'
	return ctx(*k, **kw)
