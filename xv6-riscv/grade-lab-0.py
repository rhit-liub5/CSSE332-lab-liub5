#!/usr/bin/env python

import re
from gradelib import *

r = Runner(save("xv6.out"))

@test(5, "warmup, add with pointers")
def test_add_with_pointers():
    r.run_qemu(shell_script(['warmup 1']))
    r.match("^test_add_with_pointers\\(\\d+\\): OK\.",
            no=[".*Assertion FAILED.*"])

@test(5, "warmup, ensure correct order")
def test_ensure_correct_order():
    r.run_qemu(shell_script(['warmup 2']))
    # In here, given the way the regex matching is happening (line by line), we
    # must provide a regex for each line that we'd like to match. If a regex
    # matches more than one line, then it won't work because they will remove it
    # from the list.
    r.match("^test_ensure_correct_order\\(\\d+\\): OK\.",
            no=[".*Assertion FAILED.*"])

@test(10, "warmup, special equals")
def test_special_equals():
    r.run_qemu(shell_script(['warmup 3']))
    r.match("^test_special_equals\\(\\d+\\): OK\.",
            no=[".*Assertion FAILED.*"])

@test(10, "warmup, string with q")
def test_string_with_q():
    r.run_qemu(shell_script(['warmup 4']))
    r.match("^test_string_with_q\\(\\d+\\): OK\.",
            no=[".*Assertion FAILED.*"])

@test(2, "sleep, no arguments")
def test_sleep_no_args():
    r.run_qemu(shell_script(['sleep']))
    r.match(no=["exec .* failed", "$ sleep\n$"])

@test(3, "sleep, returns")
def test_sleep_returns():
    r.run_qemu(shell_script(['sleep', 'echo OK']))
    r.match("^OK$", no=["exec .* failed", "$ sleep\n$"])

@test(15, "sleep, makes syscall")
def test_sleep():
    r.run_qemu(shell_script(['sleep 10', 'echo FAIL']),
               stop_breakpoint('sys_sleep'))
    r.match("\\$ sleep 10", no=['FAIL'])

@test(20, "arraylist, all")
def test_arraylist():
    r.run_qemu(shell_script(['arraylist']))
    r.match(".*OK.", no=[".*Assertion FAILED.*"])

@test(20, "find, in current directory")
def test_find_curdir():
    fn = random_str()
    r.run_qemu(shell_script([
        'echo > %s' % fn,
        'find . %s' % fn
    ]))
    r.match('\./%s' % fn)

@test(10, "find, recursive")
def test_find_recursive():
    needle = random_str()
    dirs = [random_str() for _ in range(3)]
    r.run_qemu(shell_script([
        'mkdir %s' % dirs[0],
        'echo > %s/%s' % (dirs[0], needle),
        'mkdir %s/%s' % (dirs[0], dirs[1]),
        'echo > %s/%s/%s' % (dirs[0], dirs[1], needle),
        'mkdir %s' % dirs[2],
        'echo > %s/%s' % (dirs[2], needle),
        'find . %s' % needle
    ]))
    r.match('./%s/%s' % (dirs[0], needle),
            './%s/%s/%s' % (dirs[0], dirs[1], needle),
            './%s/%s' % (dirs[2], needle))

run_tests()

