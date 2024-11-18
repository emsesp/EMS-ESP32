# pre-reqs:
#  1a) via python3 standalone - (sudo apt install python3-pip)
#  1b) via PlatformIO's penv - (using the "PlatformIO Core CLI" menu option)
#
#  Setup with:
#   cd scripts
#   python3 -m venv venv
#   source ./venv/bin/activate
#   pip install -r requirements.txt

# -s skip all checks, just run the test. default False.
# -i ip address of the device. default ems-esp.local.
# -w wait time in seconds between each test. default 10.
# -c count, the number of tests to run. default 1.
# -u url, custom URL to test against like "/api/boiler/info". optional.
# -t bearer token for the Restart command. optional. default is admin/admin's token.
# -h help

python3 memory_test.py -s True -i 10.10.10.175 -w 5 -c 30 -u "/api/boiler/info" # V3
# python3 memory_test.py -s True -i 10.10.10.175 -w 5 -c 30 -u "/api?device=boiler&cmd=info" # V2

# python3 memory_test.py -i 10.10.10.175 -n memory
