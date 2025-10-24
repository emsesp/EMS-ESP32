
Import("env")
import os
import shutil

def force_clean(source, target, env):
    """Remove build directory before building"""
    build_dir = env.subst("$BUILD_DIR")
    if os.path.exists(build_dir):
        print(f"Force cleaning: {build_dir}")
        shutil.rmtree(build_dir)

# Register the callback to run before building
env.AddPreAction("$BUILD_DIR/${PROGNAME}$PROGSUFFIX", force_clean)
