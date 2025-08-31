#!/usr/bin/env python
# encoding: utf-8

from waflib import Options
from waflib import Task
from waflib.TaskGen import feature
from waflib.Configure import conf
import os

def configure(conf):
	conf.find_program('clang-format', mandatory=True)


class clang_format(Task.Task):
	color   = 'YELLOW'
	run_str = 'clang-format -style=file:${FORMATFILE} -i ${SRC}'
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
