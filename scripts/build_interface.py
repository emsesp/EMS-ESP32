from pathlib import Path
import os

Import("env")


def buildWeb():
    os.chdir("interface")
    print("Building web interface...")
    try:
        env.Execute("pnpm install")
        env.Execute("pnpm typesafe-i18n")
        with open("./src/i18n/i18n-util.ts") as r:
            text = r.read().replace("Locales = 'pl'", "Locales = 'en'")
        with open("./src/i18n/i18n-util.ts", "w") as w:
            w.write(text)
        print("Setting WebUI locale to 'en'")
        env.Execute("pnpm build")
        env.Execute("pnpm webUI")
    finally:
        os.chdir("..")


# Don't buuld webUI if called from GitHub Actions
if "NO_BUILD_WEBUI" in os.environ:
    print("!! Skipping the build of the web interface !!")
else:
    if not (env.IsCleanTarget()):
        buildWeb()
