#!/usr/bin/env python
# encoding: utf-8

import os, platform

from waflib import Options
from waflib import Utils, Context
from waflib import TaskGen, Task, Node
from waflib.TaskGen import feature, extension
from waflib.Configure import conf
from waflib.Errors import WafError
from waflib import Logs

class gtest(Task.Task):
    color = "BLUE"
    always_run = True

    def __str__(self):
        # display suitename testname
        return "%s: %s" % (self.__class__.__name__, self.program)

    def _getenv(self):
        bld = self.generator.bld
        env = dict(os.environ)
        self.libpath += [bld.bldnode.abspath()]
        if platform.system() == "Windows":
            if "PATH" in os.environ:
                os.putenv(
                    "PATH",
                    os.pathsep.join([lib for lib in self.libpath])
                    + os.pathsep
                    + os.environ["PATH"],
                )
                env["PATH"] = (
                    os.pathsep.join([lib for lib in self.libpath])
                    + os.pathsep
                    + os.environ["PATH"]
                )
            else:
                os.putenv("PATH", os.pathsep.join([lib for lib in self.libpath]))
                env["PATH"] = os.pathsep.join([lib for lib in self.libpath])
        else:
            if "LD_LIBRARY_PATH" in os.environ:
                env["LD_LIBRARY_PATH"] = (
                    os.pathsep.join([lib for lib in self.libpath])
                    + os.pathsep
                    + os.environ["LD_LIBRARY_PATH"]
                )
            else:
                env["LD_LIBRARY_PATH"] = os.pathsep.join([lib for lib in self.libpath])
        return env

    def run(self):
        env = self.env
        bld = self.generator.bld
        wd = bld.bldnode.abspath()

        def to_list(xx):
            if isinstance(xx, str):
                return [xx]
            return xx

        self.last_cmd = lst = []
        lst.extend([self.inputs[0].abspath()])
        lst.extend(to_list(self.args))
        lst = [x for x in lst if x]

        try:
            if self.verbose:
                self.generator.bld.cmd_and_log(lst, cwd=wd, env=self._getenv())
            else:
                self.generator.bld.cmd_and_log(lst, cwd=wd, env=self._getenv(), quiet=0)
        except WafError as e:
            Logs.error(e.stdout)
            Logs.error(e.stderr)


@feature("gtest")
def process_test(self):
    bldtsk = self.bld.get_tgen_by_name(self.program)
    if not bldtsk:
        raise Utils.WafError(
            "no target of name %r necessary for %r" % (self.program, self)
        )
    lnk_task = getattr(bldtsk, "link_task", None)
    if not lnk_task:
        raise Utils.WafError("no lnk_task %r ???" % (self))

    if platform.system() == "Windows":
        inputNode = self.path.find_or_declare(self.program + ".exe")
    else:
        inputNode = self.path.find_or_declare(self.program)

    report = self.path.find_or_declare('report_{}.json'.format(self.program))
    tsk = self.create_task("gtest", [inputNode], [])
    tsk.name = "run_gtest_" + self.program
    tsk.program = self.program

    tsk.args = ['--gtest_output={}:{}'.format('json', report.abspath())]
    tsk.libpath = getattr(self, "libpath", [])
    tsk.verbose = Options.options.verbose
    runafter = getattr(self, "after", None)
    if runafter != None:
        tsk.set_run_after(self.bld.get_tgen_by_name(runafter))
    tsk.set_run_after(lnk_task)

@conf
def runGoogleTest(ctx, *k, **kw):
    kw["features"] = "gtest"
    return ctx(*k, **kw)

