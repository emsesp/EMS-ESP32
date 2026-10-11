
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
#
# Prefer a numeric IP over .local — mDNS (especially under WSL2) adds latency
# and can stall on IPv6 before falling back to IPv4.

import argparse
import hashlib
import socket
import sys
import time
from pathlib import Path
from urllib.parse import urlparse

import requests
from requests.adapters import HTTPAdapter
from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
from termcolor import cprint
from tqdm import tqdm

USER_AGENT = 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0'
UPLOAD_BLOCK_SIZE = 64 * 1024
UPLOAD_SNDBUF = 256 * 1024


def print_success(x):
    return cprint(x, 'green')


def print_fail(x):
    return cprint(x, 'red')


def resolve_ipv4(host):
    """Resolve hostname to IPv4 so we skip mDNS/IPv6 retries on every request."""
    if host.startswith('['):
        return host
    if ':' in host and host.count(':') == 1:
        host = host.rsplit(':', 1)[0]
    try:
        socket.inet_pton(socket.AF_INET, host)
        return host
    except OSError:
        pass
    infos = socket.getaddrinfo(host, 80, socket.AF_INET, socket.SOCK_STREAM)
    if not infos:
        raise socket.gaierror(f'No IPv4 address for {host}')
    return infos[0][4][0]


class FastHTTPAdapter(HTTPAdapter):
    """Larger write chunks and socket send buffer for streaming POSTs."""

    def init_poolmanager(self, connections, maxsize, block=False, **pool_kwargs):
        socket_options = [
            (socket.IPPROTO_TCP, socket.TCP_NODELAY, 1),
            (socket.SOL_SOCKET, socket.SO_SNDBUF, UPLOAD_SNDBUF),
        ]
        pool_kwargs.setdefault('socket_options', socket_options)
        pool_kwargs.setdefault('blocksize', UPLOAD_BLOCK_SIZE)
        super().init_poolmanager(connections, maxsize, block=block, **pool_kwargs)


def create_base_headers(host_ip, emsesp_url):
    """Create base headers used across all requests."""
    return {
        'Host': host_ip,
        'User-Agent': USER_AGENT,
        'Accept': '*/*',
        'Accept-Language': 'en-US',
        'Referer': emsesp_url,
        'Connection': 'keep-alive'
    }


def upload(file, ip, username, password):
    """Upload firmware to EMS-ESP device."""
    print()
    print("EMS-ESP Firmware Upload")

    file_path = Path(file)
    if not file_path.exists():
        print_fail(f"File not found: {file}")
        return

    parsed_url = urlparse(f"http://{ip}")
    host_name = parsed_url.hostname or ip
    try:
        host_ip = resolve_ipv4(host_name)
    except OSError as e:
        print_fail(f"Could not resolve {host_name} to IPv4: {e}")
        return

    if host_ip != host_name:
        print_success(f"Resolved {host_name} -> {host_ip}")

    emsesp_url = f"http://{host_ip}"
    session = requests.Session()
    session.mount('http://', FastHTTPAdapter(pool_connections=2, pool_maxsize=2))

    try:
        signon_url = f"{emsesp_url}/rest/signIn"
        signon_headers = create_base_headers(host_ip, emsesp_url)
        signon_headers['Content-Type'] = 'application/json'

        response = session.post(
            signon_url,
            json={"username": username, "password": password},
            headers=signon_headers,
            timeout=15)

        if response.status_code != 200:
            print_fail(f"Authentication failed (code {response.status_code})")
            return

        print_success("Authentication successful")
        access_token = response.json().get('access_token')

        firmware = file_path.read_bytes()
        md5 = hashlib.md5(firmware).hexdigest()

        encoder = MultipartEncoder(fields={
            'MD5': md5,
            'file': (file_path.name, firmware, 'application/octet-stream')
        })

        bar = tqdm(
            desc='Upload Progress',
            total=encoder.len,
            dynamic_ncols=True,
            unit='B',
            unit_scale=True,
            unit_divisor=1024,
            mininterval=0.2
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
        started = time.perf_counter()
        response = session.post(
            upload_url, data=monitor, headers=post_headers, timeout=None)
        elapsed = time.perf_counter() - started

        bar.close()

        if response.status_code != 200:
            print_fail(f"Upload failed (code {response.status_code})")
        else:
            kibs = (encoder.len / 1024) / elapsed if elapsed > 0 else 0
            print_success(
                f"Upload successful ({kibs:.0f} KiB/s in {elapsed:.1f}s). Rebooting device.")
            restart_headers = create_base_headers(host_ip, emsesp_url)
            restart_headers.update({
                'Content-Type': 'application/json',
                'Authorization': f'Bearer {access_token}'
            })
            restart_url = f"{emsesp_url}/api/system/restart"
            try:
                response = session.get(
                    restart_url, headers=restart_headers, timeout=10)
                if response.status_code != 200:
                    print_fail(f"Restart failed (code {response.status_code})")
            except requests.RequestException:
                pass

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
