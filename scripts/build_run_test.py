import os
import shutil
import subprocess
import sys
Import("env")

# Import the streaming function from the separate module
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


def build_run_test(source, target, env):
    
     # Get the executable path
    program_path = source[0].get_abspath()

    # Get output file and test command from environment variable or use defaults
    output_file = os.path.join("docs", env.GetProjectOption("custom_output_file", "dump_default_output.txt"))
    test_command = env.GetProjectOption("custom_test_command", "test entity_dump")

    # run the test command and save the output to the output file
    run_with_streaming_input(program_path, test_command, output_file)
    
    # if we have a post command defined run it
    post_script = env.GetProjectOption("custom_post_script", None)    
    if post_script:
        print(f"Running post script: {post_script}")
        python_exe = get_python_executable()
        subprocess.run([python_exe, post_script, program_path], check=True)
 
env.AddCustomTarget(
    "build",
    "$BUILD_DIR/${PROGNAME}$PROGSUFFIX",
    build_run_test
)