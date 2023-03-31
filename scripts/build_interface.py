from pathlib import Path
from shutil import copytree, rmtree, copyfileobj
import os
import gzip

# brotli has better compression than gzip but requires https so leaving here for future
# import brotli

Import("env")


def gzipFile(file):
    with open(file, 'rb') as f_in:
        with gzip.open(file + '.gz', 'wb') as f_out:
            copyfileobj(f_in, f_out)
    os.remove(file)

    # brotli version:
    # with open(file + '.br', 'wb') as f_out:
    #     with open(file, 'rb') as f_in:
    #         f_out.write(brotli.compress(f_in.read(), quality=11))
    #     os.remove(file)


def flagExists(flag):
    buildFlags = env.ParseFlags(env["BUILD_FLAGS"])
    for define in buildFlags.get("CPPDEFINES"):
        if (define == flag or (isinstance(define, list) and define[0] == flag)):
            return True


def buildWeb():
    os.chdir("interface")
    print("Building web interface...")
    try:
        env.Execute("yarn")
        env.Execute("yarn run typesafe-i18n --no-watch")
        with open("./src/i18n/i18n-util.ts") as r:
            text = r.read().replace("Locales = 'pl'", "Locales = 'en'")
        with open("./src/i18n/i18n-util.ts", "w") as w:
            w.write(text)
        print("Setting locale to 'en'")
        env.Execute("yarn run build")

        buildPath = Path("build")
        wwwPath = Path("../data/www")

        if wwwPath.exists() and wwwPath.is_dir():
            rmtree(wwwPath)

        print("Copying web files from build to data/www...")
        copytree(buildPath, wwwPath)
        for currentpath, folders, files in os.walk(wwwPath):
            for file in files:
                gzipFile(os.path.join(currentpath, file))

    finally:
        os.chdir("..")


if (len(BUILD_TARGETS) == 0 or "upload" in BUILD_TARGETS):
    buildWeb()
else:
    print("Skipping build web interface for target(s): " + ", ".join(BUILD_TARGETS))
