#!/usr/bin/env python
# encoding: utf-8

import os, platform
import json
from jinja2 import FileSystemLoader, Environment

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
    tsk = self.create_task("gtest", [inputNode], [report])
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

class gtestreport(Task.Task):
    color = "BLUE"
    always_run = True

    def __str__(self):
        # display suitename testname
        return "%s" % (self.__class__.__name__)

    def run(self):
        data = {
            'disabled': 0,
            'errors': 0,
            'failures': 0,
            'name': 'AllTests',
            'tests': 0,
            'testsuites': []
        }
        for report in self.inputs:
            with open(report.abspath()) as report_file:
                report_data = json.load(report_file)
                data['disabled'] += report_data['disabled']
                data['errors'] += report_data['errors']
                data['failures'] += report_data['failures']
                data['tests'] += report_data['tests']
                data['testsuites'] += report_data['testsuites']

        with open(self.outputs[0].abspath(), "w") as outfile:
            templateLoader = FileSystemLoader(searchpath=".")
            templateEnv = Environment(loader=templateLoader)
            template = templateEnv.get_template("wtools/gtest_template.html")
            outfile.write(template.render(test_overview=data, test_suites=data['testsuites']))

        Logs.pprint('BLUE', "Test Report Summary:")
        for suite in data['testsuites']:
            Logs.pprint('CYAN', "Test Suite: %s" % suite['name'])
            for case in suite['testsuite']:
                status = "PASS" if case['status'] == "RUN" and case['result'] == "COMPLETED" and not 'failures' in case else "FAIL"
                if status == "FAIL":
                    Logs.pprint('RED', f"  {status} - Test: {case['name']}")
                else:
                    Logs.info(f"  {status} - Test: {case['name']}")
        Logs.info("Total tests: %d" % data['tests'])
        if data['disabled'] > 0:
            Logs.info("Disabled tests: %d" % data['disabled'])
        if data['errors'] > 0:
            Logs.info("Errors: %d" % data['errors'])
        if data['failures'] > 0:
            Logs.pprint('RED', "Failures: %d" % data['failures'])

        if data['failures'] == 0 and data['errors'] == 0:
            Logs.info("All tests passed successfully!")
        else:
            Logs.error("Some tests failed. Please check the report for details.")
            raise WafError("Test failures detected. Build failed.")


@feature("gtest-report")
def process_report(self):
    html_report = self.path.find_or_declare("report.html")
    tsk = self.create_task("gtestreport", self.reports, [html_report])
    tsk.name = "run_gtest_report"
    tsk.reports = self.reports

    # Ensure this task runs after all gtest tasks
    for tgen in self.bld.task_gen_cache_names.values():
        if "gtest" in getattr(tgen, "features", []):
            for gtest_task in tgen.tasks:
                print(gtest_task)
                tsk.set_run_after(gtest_task)

@conf
def runGoogleTestReport(ctx, *k, **kw):
    kw["features"] = "gtest-report"
    return ctx(*k, **kw)

