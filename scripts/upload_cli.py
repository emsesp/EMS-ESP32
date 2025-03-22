
# Modified from https://github.com/ayushsharma82/ElegantOTA
#
# Requires Python (sudo apt install python3-pip) and then create a virtual environment:
#  cd scripts
#  python3 -m venv venv
#  source ./venv/bin/activate
#  pip install -r requirements.txt
#
# Run using for example:
#  python3 upload_cli.py -i 10.10.10.175 -f ../build/firmware/EMS-ESP-3_7_0-dev_34-ESP32S3-16MB+.bin

import argparse
import requests
import hashlib
from urllib.parse import urlparse
import time
from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
from tqdm import tqdm
from termcolor import cprint


def print_success(x): return cprint(x, 'green')
def print_fail(x): return cprint(x, 'red')


def upload(file, ip, username, password):

    # Print welcome message
    print()
    print("EMS-ESP Firmware Upload")

    # first check authentication
    emsesp_url = "http://" + f'{ip}'
    parsed_url = urlparse(emsesp_url)
    host_ip = parsed_url.netloc

    signon_url = f"{emsesp_url}/rest/signIn"

    signon_headers = {
        'Host': host_ip,
        'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0',
        'Accept': '*/*',
        'Accept-Language': 'en-US',
        'Accept-Encoding': 'gzip, deflate',
        'Referer': f'{emsesp_url}',
        'Content-Type': 'application/json',
        'Connection': 'keep-alive'
    }

    username_password = {
        "username": username,
        "password": password
    }

    response = requests.post(
        signon_url, json=username_password, headers=signon_headers, auth=None)

    if response.status_code != 200:
        print_fail("Authentication failed (code " +
                   str(response.status_code) + ")")
        return

    print_success("Authentication successful")
    access_token = response.json().get('access_token')

    # start the upload
    with open(file, 'rb') as firmware:
        md5 = hashlib.md5(firmware.read()).hexdigest()

        firmware.seek(0)

        encoder = MultipartEncoder(fields={
            'MD5': md5,
            'file': (file, firmware, 'application/octet-stream')}
        )

        bar = tqdm(desc='Upload Progress',
                   total=encoder.len,
                   dynamic_ncols=True,
                   unit='B',
                   unit_scale=True,
                   unit_divisor=1024
                   )

        monitor = MultipartEncoderMonitor(
            encoder, lambda monitor: bar.update(monitor.bytes_read - bar.n))

        post_headers = {
            'Host': host_ip,
            'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0',
            'Accept': '*/*',
            'Accept-Language': 'en-US',
            'Accept-Encoding': 'gzip, deflate',
            'Referer': f'{emsesp_url}',
            'Connection': 'keep-alive',
            'Content-Type': monitor.content_type,
            'Content-Length': str(monitor.len),
            'Origin': f'{emsesp_url}',
            'Authorization': 'Bearer ' + f'{access_token}'
        }

        upload_url = f"{emsesp_url}/rest/uploadFile"

        response = requests.post(
            upload_url, data=monitor, headers=post_headers, auth=None)

        bar.close()
        time.sleep(0.1)

        if response.status_code != 200:
            print_fail("Upload failed (code " + response.status.code + ").")
        else:
            print_success("Upload successful. Rebooting device.")
            restart_headers = {
                'Host': host_ip,
                'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0',
                'Accept': '*/*',
                'Accept-Language': 'en-US',
                'Accept-Encoding': 'gzip, deflate',
                'Referer': f'{emsesp_url}',
                'Content-Type': 'application/json',
                'Connection': 'keep-alive',
                'Authorization': 'Bearer ' + f'{access_token}'
            }
            restart_url = f"{emsesp_url}/api/system/restart"
            response = requests.get(restart_url, headers=restart_headers)
            if response.status_code != 200:
                print_fail("Restart failed (code " +
                           str(response.status_code) + ")")

        print()


# main
parser = argparse.ArgumentParser(description="EMS-ESP Firmware Upload")
parser.add_argument("-f", "--file", metavar="FILE",
                    required=True, type=str, help="firmware file")
parser.add_argument("-i", "--ip", metavar="IP", type=str,
                    default="ems-esp.local", help="IP address of EMS-ESP")
parser.add_argument("-u", "--username", metavar="USERNAME",
                    type=str, default="admin", help="admin user")
parser.add_argument("-p", "--password", metavar="PASSWORD",
                    type=str, default="admin", help="admin password")
args = parser.parse_args()
upload(**vars(args))
