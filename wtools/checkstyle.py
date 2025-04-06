#!/usr/bin/env python
# encoding: utf-8

from waflib import Options
from waflib import Task
from waflib.TaskGen import feature
from waflib.Configure import conf

def configure(conf):
	conf.find_program('clang-format', mandatory=True)


class clang_format(Task.Task):
	color   = 'YELLOW'
	run_str = 'clang-format -style=file -i ${SRC}'
	shell = False

@feature('checkstyle')
def process_style(self):
	for src in self.inputs:
		tsk = self.create_task('clang_format', src, [])
		tsk.failOnError = True

@conf
def checkstyle(ctx, *k, **kw):
	kw['features'] = 'checkstyle'
	return ctx(*k, **kw)
