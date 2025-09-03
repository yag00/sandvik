#!/usr/bin/env python
# encoding: utf-8

from waflib import Task, Node
from waflib.TaskGen import extension
from waflib.TaskGen import feature, before

class rc(Task.Task):
	color   = 'PINK'
	run_str = '${LD} -r -b binary ${SRC} -o ${TGT}'

@feature('resources')
@before('process_source')
def create_resources(self):
	r = getattr(self, 'resources', [])
	if not isinstance(r, list):
		r = r.split(',')

	for res in r:
		if not isinstance(res, Node.Node):
			file = self.bld.path.find_or_declare(res)
			out='%s.%d.o' % (file.name, self.idx)
			task = self.create_task('rc', file, self.bld.path.find_or_declare(out))
			try:
				self.compiled_tasks.append(task)
			except AttributeError:
				self.compiled_tasks=[task]

@extension('.xml')
def process_xml(self, node):
	out='%s.%d.o' % (node.name, self.idx)
	task= self.create_task('rc', node, node.parent.find_or_declare(out))
	try:
		self.compiled_tasks.append(task)
	except AttributeError:
		self.compiled_tasks=[task]
	return task

def configure(conf):
	conf.find_program('ld', var='LD')
