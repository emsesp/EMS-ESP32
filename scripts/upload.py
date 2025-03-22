
# Modified from https://github.com/ayushsharma82/ElegantOTA
#
# This is called during the PlatformIO upload process, when the target is 'upload'.
# Use the file upload_cli.py for manual uploads outside PIO.
#
# To use create a pio_local.ini file in the project root and add the following:
#  [env]
#  upload_protocol = custom
#  custom_emsesp_ip = ems-esp.local
#  custom_username = admin
#  custom_password = admin
#
# and
#  extra_scripts = scripts/upload.py
#

import requests
import hashlib
from urllib.parse import urlparse
import time
import os

Import("env")

try:
    from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
    from tqdm import tqdm
    from termcolor import cprint
except ImportError:
    env.Execute("$PYTHONEXE -m pip install requests_toolbelt")
    env.Execute("$PYTHONEXE -m pip install tqdm")
    env.Execute("$PYTHONEXE -m pip install termcolor")
    from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
    from tqdm import tqdm
    from termcolor import cprint


def print_success(x): return cprint(x, 'green')
def print_fail(x): return cprint('Error: '+x, 'red')


def on_upload(source, target, env):

    # make sure we have set the upload_protocol to custom
    if env.get('UPLOAD_PROTOCOL') != 'custom':
        print_fail(
            "Please set upload_protocol = custom in your pio_local.ini file when using upload.py")
        return

    # first check authentication
    try:
        username = env.GetProjectOption('custom_username')
        password = env.GetProjectOption('custom_password')
        emsesp_ip = env.GetProjectOption('custom_emsesp_ip')
    except:
        print_fail('Missing settings. Add these to your pio_local.ini file: \n\ncustom_username=username\ncustom_password=password\ncustom_emsesp_ip=ems-esp.local\n')
        return

    emsesp_url = "http://" + env.GetProjectOption('custom_emsesp_ip')
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
        signon_url, json=username_password, headers=signon_headers)

    if response.status_code != 200:
        print_fail("Authentication with EMS-ESP failed (code " +
                   str(response.status_code) + ")")
        return

    print_success("Authentication with EMS-ESP successful")
    access_token = response.json().get('access_token')

    # start the upload
    firmware_path = str(source[0])

    with open(firmware_path, 'rb') as firmware:
        md5 = hashlib.md5(firmware.read()).hexdigest()

        firmware.seek(0)

        encoder = MultipartEncoder(fields={
            'MD5': md5,
            'file': (firmware_path, firmware, 'application/octet-stream')}
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
            upload_url, data=monitor, headers=post_headers)

        bar.close()
        time.sleep(0.1)

        print()

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


if env.get('UPLOAD_PROTOCOL') == 'custom':
    env.Replace(UPLOADCMD=on_upload)
