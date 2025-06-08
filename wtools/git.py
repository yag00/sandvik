#!/usr/bin/env python
# encoding: utf-8

import subprocess

def getInfos(ctx):
	if ctx.env.GIT:
		dirty = ''
		sha1 = ''
		shortsha1 = ''

		p = subprocess.Popen([ctx.env.GIT[0], "describe", "--always", "--tags", "--dirty"], stdout=subprocess.PIPE)
		(out, err) = p.communicate()
		rev = out.strip()
		dirty = False
		if b'dirty' in rev:
			dirty = True
		p = subprocess.Popen([ctx.env.GIT[0], "rev-parse", "HEAD"], stdout=subprocess.PIPE)
		(out, err) = p.communicate()
		sha1 = out.strip()

		return {'sha1' : sha1.decode("utf8"), 'dirty' : dirty}
	else:
		return {'sha1' : 'unknown', 'dirty' : True}