#!/usr/bin/env python
# encoding: utf-8

from waflib import Options
from waflib import Task
from waflib.TaskGen import feature, after_method
from waflib.Configure import conf
import os
import zipfile
import requests

def configure(conf):
	d8_path = os.path.abspath('ext/android/build-tools/36.0.0')
	try:
		conf.find_program('d8', var='D8', path_list=[d8_path], mandatory=True)
		version = conf.cmd_and_log(f'{conf.env['D8'][0]} --version').strip().split(' ')[1]
		conf.msg('Checking for d8 version', version)
	except Exception as e:
		# Install Android SDK command-line tools
		android_home = os.path.abspath('ext/android')
		os.makedirs(android_home, exist_ok=True)
		tools_zip = os.path.join(android_home, 'tools.zip')
		cmdline_tools = os.path.join(android_home, '.')

		url = 'https://dl.google.com/android/repository/commandlinetools-linux-13114758_latest.zip'
		conf.msg('Downloading Android command-line tools', url)
		with requests.get(url, stream=True) as r:
			r.raise_for_status()
			with open(tools_zip, 'wb') as f:
				for chunk in r.iter_content(chunk_size=8192):
					f.write(chunk)

		with zipfile.ZipFile(tools_zip, 'r') as zip_ref:
			for member in zip_ref.infolist():
				extracted_path = zip_ref.extract(member, cmdline_tools)
				perm = member.external_attr >> 16
				if perm:
					os.chmod(extracted_path, perm)
		os.remove(tools_zip)

		# Accept licenses and install build-tools 36.0.0
		conf.msg('Installing Android build-tools', "36.0.0")
		conf.cmd_and_log(f'yes | {android_home}/cmdline-tools/bin/sdkmanager --sdk_root={android_home} --licenses')
		conf.cmd_and_log(f'{android_home}/cmdline-tools/bin/sdkmanager --sdk_root={android_home} "platform-tools" "build-tools;36.0.0"')

		conf.find_program('d8', var='D8', path_list=[d8_path], mandatory=True)
		version = conf.cmd_and_log(f'{conf.env['D8'][0]} --version').strip().split(' ')[1]
		conf.msg('Checking for d8 version', version)

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
