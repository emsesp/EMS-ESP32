#!/usr/bin/env python3
"""Regenerate test/test_api/test_api.h from the native-test-create build, then run the API tests.

Workflow:
  1. Run `pio run -e native-test-create -t exec` and capture its output.
  2. Extract everything between the START/END "CUT HERE" markers.
  3. Write that block to test/test_api/test_api.h.
  4. Run `pio run -e native-test -t exec`.
  
run with `python3 ./scripts/generate_test_api.py`
"""

import subprocess
import sys
from pathlib import Path

START_MARKER = "// ---------- START - CUT HERE ----------"
END_MARKER = "// ---------- END - CUT HERE ----------"

# project root is the parent of this script's "scripts" directory
PROJECT_ROOT = Path(__file__).resolve().parent.parent
OUTPUT_HEADER = PROJECT_ROOT / "test" / "test_api" / "test_api.h"


def run(cmd, capture):
    """Run a command in the project root. Streams to the console; optionally captures stdout."""
    print(f"\n>>> {' '.join(cmd)}\n", flush=True)
    if capture:
        # capture stdout while still echoing it so the user sees progress
        result = subprocess.run(cmd, cwd=PROJECT_ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        print(result.stdout, end="", flush=True)
        return result.returncode, result.stdout
    result = subprocess.run(cmd, cwd=PROJECT_ROOT)
    return result.returncode, None


def extract_between_markers(output):
    """Return the text strictly between the START and END markers (markers excluded)."""
    lines = output.splitlines()
    start = end = None
    for i, line in enumerate(lines):
        if START_MARKER in line and start is None:
            start = i
        elif END_MARKER in line and start is not None:
            end = i
            break
    if start is None or end is None:
        return None
    return "\n".join(lines[start + 1 : end]) + "\n"


def main():
    # 1. build + exec the generator
    code, output = run(["pio", "run", "-e", "native-test-create", "-t", "exec"], capture=True)
    if code != 0:
        print(f"\nERROR: 'native-test-create' exec failed with exit code {code}", file=sys.stderr)
        return code

    # 2. extract the header content
    content = extract_between_markers(output)
    if content is None:
        print(f"\nERROR: could not find content between markers:\n  {START_MARKER}\n  {END_MARKER}", file=sys.stderr)
        return 1

    # 3. write it to test_api.h
    OUTPUT_HEADER.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_HEADER.write_text(content, encoding="utf-8")
    print(f"\nWrote {len(content.splitlines())} lines to {OUTPUT_HEADER}", flush=True)

    # 4. build + exec the actual tests
    code, _ = run(["pio", "run", "-e", "native-test", "-t", "exec"], capture=False)
    if code != 0:
        print(f"\nERROR: 'native-test' exec failed with exit code {code}", file=sys.stderr)
    return code


if __name__ == "__main__":
    sys.exit(main())
