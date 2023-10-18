from pathlib import Path
import os

Import("env")


def buildWeb():
    os.chdir("interface")
    print("Building web interface...")
    try:
        env.Execute("yarn")
        env.Execute("yarn typesafe-i18n --no-watch")
        with open("./src/i18n/i18n-util.ts") as r:
            text = r.read().replace("Locales = 'pl'", "Locales = 'en'")
        with open("./src/i18n/i18n-util.ts", "w") as w:
            w.write(text)
        print("Setting WebUI locale to 'en'")
        env.Execute("yarn build")
        env.Execute("yarn webUI")
    finally:
        os.chdir("..")


if not (env.IsCleanTarget()):
    buildWeb()
