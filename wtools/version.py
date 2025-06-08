#!/usr/bin/env python
# encoding: utf-8

import json
from waflib import Utils
from waflib import Task
from waflib.TaskGen import feature
from waflib.Configure import conf

class gen_version(Task.Task):
	color   = 'PINK'

	def uid(self):
		try:
			return self.uid_
		except AttributeError:
			m = Utils.md5(self.__class__.__name__.encode('utf-8'))
			up = m.update
			up(self.outputs[0].abspath().encode('utf-8'))
			up(self.version.encode('utf-8'))
			up(json.dumps(self.gitinfo).encode('utf-8'))
			self.uid_ = m.digest()
			return self.uid_

	def run(self):
		with open(self.outputs[0].abspath(), 'w') as f:
			commit = self.gitinfo['sha1']
			if self.gitinfo['dirty']:
				commit += '-dirty'
			content = '#pragma once\nconstexpr std::string_view __GitHash = "{}";\nconstexpr std::string_view __Version = "{}";\n'.format(commit, self.version)
			f.write(content)

@feature('gen_version')
def process_verion(self):
	outnode = self.bld.path.find_or_declare('src/version.in.hpp')
	tsk = self.create_task('gen_version', [], [outnode])
	tsk.failOnError = True
	tsk.gitinfo = self.gitinfo
	tsk.version = self.version

@conf
def version(ctx, *k, **kw):
	kw['features'] = 'gen_version'
	return ctx(*k, **kw)
