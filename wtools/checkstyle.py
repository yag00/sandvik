#!/usr/bin/env python
# encoding: utf-8

from waflib import Options
from waflib import Task
from waflib.TaskGen import feature
from waflib.Configure import conf
import os
import requests

def configure(conf):
	path = os.path.abspath('ext')
	try:
		conf.find_program('clang-format', path_list=[path], mandatory=True)
		version = conf.cmd_and_log(f"{conf.env['CLANG_FORMAT'][0]} --version").strip().split('version')[1].strip()
		conf.msg('Checking for clang-format version', version)
	except Exception as e:
		url = 'https://github.com/cpp-linter/clang-tools-static-binaries/releases/download/master-6e612956/clang-format-21_linux-amd64'
		conf.msg('Downloading clang-format', url)
		clangformat = os.path.abspath('ext/clang-format')
		with requests.get(url, stream=True) as r:
			r.raise_for_status()
			with open(clangformat, 'wb') as f:
				for chunk in r.iter_content(chunk_size=8192):
					f.write(chunk)
		os.chmod(clangformat, 0o755)
		conf.find_program('clang-format', path_list=[path], mandatory=True)
		version = conf.cmd_and_log(f"{conf.env['CLANG_FORMAT'][0]} --version").strip().split('version')[1].strip()
		conf.msg('Checking for clang-format version', version)



class clang_format(Task.Task):
	color   = 'YELLOW'
	run_str = '${CLANG_FORMAT} -style=file:${FORMATFILE} -i ${SRC}'
	shell = False

@feature('checkstyle')
def process_style(self):
	# Find or declare nodes for .clang-format and .clang-format-java
	clang_format_cpp_node = self.path.find_node('.clang-format')
	clang_format_java_node = self.path.find_node('.clang-format-java')
	for src in self.inputs:
		tsk = self.create_task('clang_format', src, [])
		ext = os.path.splitext(src.abspath())[1]
		if ext == '.java':
			tsk.env.FORMATFILE = clang_format_java_node.abspath()
		else:
			tsk.env.FORMATFILE = clang_format_cpp_node.abspath()
		tsk.failOnError = True

@conf
def checkstyle(ctx, *k, **kw):
	kw['features'] = 'checkstyle'
	return ctx(*k, **kw)
