from pathlib import Path
import os
import subprocess
import shutil

Import("env")


def get_pnpm_executable():
    """Get the appropriate pnpm executable for the current platform."""
    # Try different pnpm executable names
    pnpm_names = ['pnpm', 'pnpm.cmd', 'pnpm.exe']

    for name in pnpm_names:
        if shutil.which(name):
            return name

    # Fallback to pnpm if not found
    return 'pnpm'


def run_command_in_directory(command, directory):
    """Run a command in a specific directory."""
    try:
        result = subprocess.run(
            command,
            shell=True,
            cwd=directory,
            check=True,
            capture_output=True,
            text=True
        )

        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(result.stderr)

        return True

    except subprocess.CalledProcessError as e:
        print(f"Command failed: {command}")
        print(f"Error: {e}")
        if e.stdout:
            print(f"Output: {e.stdout}")
        if e.stderr:
            print(f"Error output: {e.stderr}")
        return False
    except Exception as e:
        print(f"Unexpected error running command '{command}': {e}")
        return False


def buildWeb():
    interface_dir = Path("interface")
    pnpm_exe = get_pnpm_executable()

    # Set CI environment variable to make pnpm use silent mode
    os.environ['CI'] = 'true'

    print("Building web interface...")

    # Check if interface directory exists
    if not interface_dir.exists():
        print(f"Error: Interface directory '{interface_dir}' not found!")
        return False

    # Check if pnpm is available
    if not shutil.which(pnpm_exe):
        print(f"Error: '{pnpm_exe}' not found in PATH!")
        return False

    try:
        # Run pnpm commands in the interface directory
        commands = [
            f"{pnpm_exe} install",
            f"{pnpm_exe} build_webUI"
        ]

        for command in commands:
            print(f"Running: {command}")
            if not run_command_in_directory(command, interface_dir):
                return False

        # Modify i18n-util.ts file
        i18n_file = interface_dir / "src" / "i18n" / "i18n-util.ts"
        if i18n_file.exists():
            with open(i18n_file, 'r') as r:
                text = r.read().replace("Locales = 'pl'", "Locales = 'en'")
            with open(i18n_file, 'w') as w:
                w.write(text)
            print("Setting WebUI locale to 'en'")
        else:
            print(
                f"Warning: {i18n_file} not found, skipping locale modification")

        print("Web interface build completed successfully!")
        return True

    except Exception as e:
        print(f"Error building web interface: {e}")
        return False


def build_webUI(*args, **kwargs):
    success = buildWeb()
    if not success:
        print("Web interface build failed!")
        env.Exit(1)
    env.Exit(0)


# Create custom target that only runs the script and then exits, without continuing with the pio workflow
env.AddCustomTarget(
    name="build",
    dependencies=None,
    actions=[build_webUI],
    title="build web interface",
    description="installs pnpm packages, updates libraries and builds web UI",
    always_build=True
)
