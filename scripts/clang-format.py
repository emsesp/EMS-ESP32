#!/usr/bin/env python3

# copied from esphome
# run from Linux using ./scripts/clang-forrmat.py

from helpers import get_output, src_files, filter_changed
import argparse
import multiprocessing
import os
import queue
import re
import subprocess
import sys
import threading
import click

sys.path.append(os.path.dirname(__file__))


def run_format(args, queue, lock, failed_files):
    """Takes filenames out of queue and runs clang-format on them."""
    while True:
        path = queue.get()
        invocation = ['clang-format']
        if args.inplace:
            invocation.append('-i')
        else:
            invocation.extend(['--dry-run', '-Werror'])
        invocation.append(path)

        proc = subprocess.run(
            invocation, capture_output=True, encoding='utf-8')
        if proc.returncode != 0:
            with lock:
                print()
                print(
                    "\033[0;32m************* File \033[1;32m{}\033[0m".format(path))
                print(proc.stdout)
                print(proc.stderr)
                print()
                failed_files.append(path)
        queue.task_done()


def progress_bar_show(value):
    return value if value is not None else ''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-j', '--jobs', type=int,
                        default=multiprocessing.cpu_count(),
                        help='number of format instances to be run in parallel.')
    parser.add_argument('files', nargs='*', default=[],
                        help='files to be processed (regex on path)')
    parser.add_argument('-i', '--inplace', action='store_true',
                        help='reformat files in-place')
    parser.add_argument('-c', '--changed', action='store_true',
                        help='only run on changed files')
    args = parser.parse_args()

    try:
        get_output('clang-format', '-version')
    except:
        print("""
        Oops. It looks like clang-format is not installed. 
        
        Please check you can run "clang-format -version" in your terminal and install
        clang-format (v11 or v12) if necessary.
        
        Note you can also upload your code as a pull request on GitHub and see the CI check
        output to apply clang-format.
        """)
        return 1

    files = []
    # all files
    # for path in git_ls_files(['*.cpp', '*.h', '*.tcc']):
    #     files.append(os.path.relpath(path, os.getcwd()))

    # just under src
    for path in src_files(['.cpp', '.h']):
        files.append(os.path.relpath(path, os.getcwd()))

    if args.files:
        file_name_re = re.compile('|'.join(args.files))
        files = [p for p in files if file_name_re.search(p)]

    if args.changed:
        files = filter_changed(files)

    files.sort()

    failed_files = []
    try:
        task_queue = queue.Queue(args.jobs)
        lock = threading.Lock()
        for _ in range(args.jobs):
            t = threading.Thread(target=run_format,
                                 args=(args, task_queue, lock, failed_files))
            t.daemon = True
            t.start()

        # Fill the queue with files.
        with click.progressbar(files, width=30, file=sys.stderr,
                               item_show_func=progress_bar_show) as bar:
            for name in bar:
                task_queue.put(name)

        # Wait for all threads to be done.
        task_queue.join()

    except KeyboardInterrupt:
        print()
        print('Ctrl-C detected, goodbye.')
        os.kill(0, 9)

    sys.exit(len(failed_files))


if __name__ == '__main__':
    main()
