import os

Import("env")

def run_native():
    print("Running native...")
    os.system("pio run -e native")

if not (env.IsCleanTarget()):
    run_native()
    
