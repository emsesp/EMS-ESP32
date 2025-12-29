import subprocess
import os
import sys
import shutil
from pathlib import Path

# This creates the files
# - dump_entities.csv
# - Modbus-Entity-Registers.md
# - dump_telegrams.csv

# Modbus-Entity-Registers.md is used in the emsesp.org documentation. You can also create it manually with the generate-modbus-register-doc.py script.
# cat ./docs/dump_entities.csv | python3 ./scripts/generate-modbus-register-doc.py    # Import the streaming function from the separate module
from run_executable import run_with_streaming_input


def get_python_executable():
    """Get the appropriate Python executable for the current platform."""
    # Try different Python executable names
    python_names = ['python3', 'python', 'py']

    for name in python_names:
        if shutil.which(name):
            return name

    # Fallback to sys.executable if available
    return sys.executable


def csv_to_md(csv_file_path, output_file_path, script_path):

    # Ensure the output directory exists and remove it
    Path(output_file_path).parent.mkdir(parents=True, exist_ok=True)

    if os.path.exists(output_file_path):
        os.remove(output_file_path)

    # Read CSV file and pipe to Python script to generate header
    python_exe = get_python_executable()

    with open(csv_file_path, 'r') as csv_file:
        with open(output_file_path, 'w') as output_file:
            subprocess.run(
                [python_exe, script_path],
                stdin=csv_file,
                stdout=output_file,
                check=True
            )

    print(
        f"Generated MD file: {output_file_path} ({os.path.getsize(output_file_path)} bytes)")


def main(program_path="./emsesp"):
    csv_file = os.path.join("docs", "dump_entities.csv")
    output_file = os.path.join("docs", "Modbus-Entity-Registers.md")
    script_file = os.path.join("scripts", "generate-modbus-register-doc.py")

    # generate the MD file
    csv_to_md(csv_file, output_file, script_file)

    # final step is to run the telegram_dump test command and generate the dump_telegrams.csv file
    # run the test command and generate the dump_telegrams.csv file
    test_command = "test telegram_dump"
    telegram_output_file = os.path.join("docs", "dump_telegrams.csv")
    print(f"Running test command: telegram_dump > {telegram_output_file}")
    run_with_streaming_input(program_path, test_command, telegram_output_file)


if __name__ == "__main__":
    # Get program path from command line argument or use default
    program_path = sys.argv[1] if len(sys.argv) > 1 else "./emsesp"
    main(program_path)
