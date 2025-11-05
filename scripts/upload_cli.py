
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
import hashlib
import sys
import time
from pathlib import Path

import requests
from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
from termcolor import cprint
from tqdm import tqdm

# Constants
USER_AGENT = 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0'
CHUNK_SIZE = 8192  # 8KB chunks for MD5 calculation


def print_success(x):
    return cprint(x, 'green')


def print_fail(x):
    return cprint(x, 'red')


def calculate_md5(file_path):
    """Calculate MD5 hash of a file in chunks for memory efficiency."""
    md5_hash = hashlib.md5()
    with open(file_path, 'rb') as f:
        for chunk in iter(lambda: f.read(CHUNK_SIZE), b''):
            md5_hash.update(chunk)
    return md5_hash.hexdigest()


def create_base_headers(host_ip, emsesp_url):
    """Create base headers used across all requests."""
    return {
        'Host': host_ip,
        'User-Agent': USER_AGENT,
        'Accept': '*/*',
        'Accept-Language': 'en-US',
        'Accept-Encoding': 'gzip, deflate',
        'Referer': emsesp_url,
        'Connection': 'keep-alive'
    }


def upload(file, ip, username, password):
    """Upload firmware to EMS-ESP device."""
    # Print welcome message
    print()
    print("EMS-ESP Firmware Upload")

    # Validate file exists
    file_path = Path(file)
    if not file_path.exists():
        print_fail(f"File not found: {file}")
        return

    # Setup URLs and headers
    emsesp_url = f"http://{ip}"
    host_ip = ip
    
    # Use a session for connection pooling and persistence
    session = requests.Session()
    
    try:
        # Authentication
        signon_url = f"{emsesp_url}/rest/signIn"
        signon_headers = create_base_headers(host_ip, emsesp_url)
        signon_headers['Content-Type'] = 'application/json'

        username_password = {
            "username": username,
            "password": password
        }

        response = session.post(signon_url, json=username_password, headers=signon_headers)

        if response.status_code != 200:
            print_fail(f"Authentication failed (code {response.status_code})")
            return

        print_success("Authentication successful")
        access_token = response.json().get('access_token')

        # Calculate MD5 hash
        print("Calculating MD5 hash...")
        md5 = calculate_md5(file_path)

        # Start the upload
        with open(file_path, 'rb') as firmware:
            encoder = MultipartEncoder(fields={
                'MD5': md5,
                'file': (file, firmware, 'application/octet-stream')
            })

            bar = tqdm(
                desc='Upload Progress',
                total=encoder.len,
                dynamic_ncols=True,
                unit='B',
                unit_scale=True,
                unit_divisor=1024
            )

            monitor = MultipartEncoderMonitor(
                encoder, lambda monitor: bar.update(monitor.bytes_read - bar.n))

            post_headers = create_base_headers(host_ip, emsesp_url)
            post_headers.update({
                'Content-Type': monitor.content_type,
                'Content-Length': str(monitor.len),
                'Origin': emsesp_url,
                'Authorization': f'Bearer {access_token}'
            })

            upload_url = f"{emsesp_url}/rest/uploadFile"
            response = session.post(upload_url, data=monitor, headers=post_headers)

            bar.close()
            time.sleep(0.1)

            if response.status_code != 200:
                print_fail(f"Upload failed (code {response.status_code})")
            else:
                print_success("Upload successful. Rebooting device.")
                restart_headers = create_base_headers(host_ip, emsesp_url)
                restart_headers.update({
                    'Content-Type': 'application/json',
                    'Authorization': f'Bearer {access_token}'
                })
                restart_url = f"{emsesp_url}/api/system/restart"
                response = session.get(restart_url, headers=restart_headers)
                if response.status_code != 200:
                    print_fail(f"Restart failed (code {response.status_code})")

    except requests.RequestException as e:
        print_fail(f"Network error: {e}")
        sys.exit(1)
    except IOError as e:
        print_fail(f"File error: {e}")
        sys.exit(1)
    except Exception as e:
        print_fail(f"Unexpected error: {e}")
        sys.exit(1)
    finally:
        session.close()

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
