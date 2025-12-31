#!/usr/bin/env python3

"""
Utility functions for running executables with streaming input and CSV output extraction.
"""

import subprocess
import sys
import os
from pathlib import Path


def run_with_streaming_input(program_path, test_command, output_file=None):
    """
    Run the executable and stream text input to it.
    
    Args:
        program_path (str): Path to the executable to run
        test_command (str): Command to send to the executable
        output_file (str, optional): Path to save CSV output. If None, no file is saved.
    
    Returns:
        int: Return code of the executed process
    """
    try:
        # Start the process with pipes for stdin, stdout, and stderr
        process = subprocess.Popen(
            [str(program_path)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding='utf-8',
            errors='replace',  # Replace invalid UTF-8 bytes instead of crashing
            bufsize=1  # Line buffered
        )
        
        # add "test " to test_command if it doesn't already start with "test "
        if not test_command.startswith("test "):
            test_command = "test " + test_command

        # Stream input line by line
        for line in test_command.strip().split('\n'):
            process.stdin.write(line + '\n')
            process.stdin.flush()

        # Close stdin to signal end of input
        process.stdin.close()

        # Read and collect output between CSV START and CSV END, then export to file
        in_cvs_section = False
        csv_output = []
        
        for line in process.stdout:
            if "---- CSV START ----" in line:
                in_cvs_section = True
                continue
            elif "---- CSV END ----" in line:
                in_cvs_section = False
                continue
            elif in_cvs_section:
                csv_output.append(line)
                # print(line, end='')
        
        # Export CSV output to file if output_file is specified
        if output_file:
            # Ensure the output directory exists
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            # delete the output file if it exists
            if os.path.exists(output_file):
                os.remove(output_file)
            
            # Export CSV output to file
            with open(output_file, 'w', encoding='utf-8', errors='replace') as f:
                f.writelines(csv_output)
            print(f"CSV file created: {output_file} ({os.path.getsize(output_file)} bytes)")

        # Wait for process to complete
        return_code = process.wait()

        # Print any errors
        stderr_output = process.stderr.read()
        if stderr_output:
            print("\nErrors:", file=sys.stderr)
            print(stderr_output, file=sys.stderr)

        return return_code

    except Exception as e:
        print(f"Error running executable: {e}", file=sys.stderr)
        return 1


def run_executable_with_command(program_path, command, output_file=None):
    """
    Simplified interface to run an executable with a command and optionally save output.
    
    Args:
        program_path (str): Path to the executable to run
        command (str): Command to send to the executable
        output_file (str, optional): Path to save CSV output. If None, no file is saved.
    
    Returns:
        int: Return code of the executed process
    """
    return run_with_streaming_input(program_path, command, output_file)


def main():
    """Command-line interface for running executables with streaming input."""
    if len(sys.argv) < 3:
        print("Usage: python3 run_executable.py <program_path> <command> [output_file]")
        print("Example: python3 run_executable.py ./emsesp entity_dump ./output.csv")
        sys.exit(1)
    
    program_path = sys.argv[1]
    command = sys.argv[2]
    output_file = sys.argv[3] if len(sys.argv) > 3 else None
    
    print(f"Running: {program_path}")
    print(f"Command: {command}")
    if output_file:
        print(f"Output file: {output_file}")
    
    return_code = run_with_streaming_input(program_path, command, output_file)
    
    if return_code == 0:
        print("Execution completed successfully!")
    else:
        print(f"Execution failed with return code: {return_code}")
    
    sys.exit(return_code)


if __name__ == "__main__":
    main()
