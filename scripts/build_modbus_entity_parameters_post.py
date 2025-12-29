import subprocess
import os
import sys
import shutil
from pathlib import Path

# This script is used to update the modbus_entity_parameters.hpp file with the data from the dump_entities.csv file.
# It is used in the build_modbus target.

def get_python_executable():
    """Get the appropriate Python executable for the current platform."""
    # Try different Python executable names
    python_names = ['python3', 'python', 'py']
    
    for name in python_names:
        if shutil.which(name):
            return name
    
    # Fallback to sys.executable if available
    return sys.executable


def csv_to_header(csv_file_path, header_file_path, script_path):

    # Ensure the output directory exists
    Path(header_file_path).parent.mkdir(parents=True, exist_ok=True)
    
    # delete the output file if it exists
    if os.path.exists(header_file_path):
        os.remove(header_file_path)
    
    # Read CSV file and pipe to Python script to generate header
    python_exe = get_python_executable()
    
    with open(csv_file_path, 'r') as csv_file:
        with open(header_file_path, 'w') as header_file:
            subprocess.run(
                [python_exe, script_path],
                stdin=csv_file,
                stdout=header_file,
                check=True
            )
    
    print(f"Generated header file: {header_file_path} ({os.path.getsize(header_file_path)} bytes)")


def main():
    csv_file = os.path.join("docs", "dump_entities.csv")
    header_file = os.path.join("src", "core", "modbus_entity_parameters.hpp")
    script_file = os.path.join("scripts", "update_modbus_registers.py")
    
    csv_to_header(csv_file, header_file, script_file)

if __name__ == "__main__":
    main()