"""
Print makefile progress
From https://stackoverflow.com/questions/451413/make-makefile-progress-indication
"""

import argparse
import math
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--stepno", type=int, required=True)
    parser.add_argument("--nsteps", type=int, required=True)
    parser.add_argument("remainder", nargs=argparse.REMAINDER)
    args = parser.parse_args()

    nchars = int(math.log(args.nsteps, 10)) + 1
    fmt_str = "[{:Xd}/{:Xd}]({:6.2f}%)".replace("X", str(nchars))
    progress = 100 * args.stepno / args.nsteps
    sys.stdout.write(fmt_str.format(args.stepno, args.nsteps, progress))
    for item in args.remainder:
        sys.stdout.write(" ")
        sys.stdout.write(item)
    sys.stdout.write("\n")


if __name__ == "__main__":
    main()
