# Wrap the C/C++ compilers with ccache when it is on PATH.
# No-op locally unless ccache is installed; CI installs it via ccache-action.
Import("env")

import shutil

ccache = shutil.which("ccache")
if ccache:

    def wrap(tool):
        cmd = env.get(tool)
        if not cmd:
            return
        resolved = env.subst(cmd) if isinstance(cmd, str) else cmd
        if isinstance(resolved, str):
            if resolved.startswith("ccache"):
                return
            env[tool] = f"{ccache} {resolved}"
            return
        if isinstance(resolved, (list, tuple)):
            if resolved and "ccache" in str(resolved[0]):
                return
            env[tool] = [ccache, *resolved]

    wrap("CC")
    wrap("CXX")
    print("ccache enabled")
