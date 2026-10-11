
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
# Prefer a numeric IP over .local — mDNS (especially under WSL2) adds latency
# and can stall on IPv6 before falling back to IPv4.

import hashlib
import os
import socket
import time
from urllib.parse import urlparse

import requests
from requests.adapters import HTTPAdapter

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

# http.client/urllib3 default is 8–16 KiB, which starves a LAN TCP window.
UPLOAD_BLOCK_SIZE = 64 * 1024
UPLOAD_SNDBUF = 256 * 1024


def print_success(x):
    cprint(x, 'green')


def print_fail(x):
    cprint(f'Error: {x}', 'red')


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


def make_session():
    session = requests.Session()
    adapter = FastHTTPAdapter(pool_connections=2, pool_maxsize=2)
    session.mount('http://', adapter)
    return session


def build_headers(host_ip, emsesp_url, content_type='application/json', access_token=None, extra_headers=None):
    """Build common HTTP headers with optional overrides."""
    headers = {
        'Host': host_ip,
        'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0',
        'Accept': '*/*',
        'Accept-Language': 'en-US',
        'Referer': emsesp_url,
        'Content-Type': content_type,
        'Connection': 'keep-alive'
    }

    if access_token:
        headers['Authorization'] = f'Bearer {access_token}'

    if extra_headers:
        headers.update(extra_headers)

    return headers


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
    except Exception:
        print_fail(f'Missing settings. Add these to your pio_local.ini file:\n\ncustom_username=username\ncustom_password=password\ncustom_emsesp_ip=ems-esp.local\n')
        return

    parsed_url = urlparse(f"http://{emsesp_ip}")
    host_name = parsed_url.hostname or emsesp_ip
    try:
        host_ip = resolve_ipv4(host_name)
    except OSError as e:
        print_fail(f"Could not resolve {host_name} to IPv4: {e}")
        return

    if host_ip != host_name:
        print_success(f"Resolved {host_name} -> {host_ip}")

    emsesp_url = f"http://{host_ip}"
    session = make_session()

    try:
        signon_url = f"{emsesp_url}/rest/signIn"
        signon_headers = build_headers(host_ip, emsesp_url)

        response = session.post(
            signon_url,
            json={"username": username, "password": password},
            headers=signon_headers,
            timeout=15)

        if response.status_code != 200:
            print_fail("Authentication with EMS-ESP failed (code " +
                       str(response.status_code) + ")")
            return

        print_success("Authentication with EMS-ESP successful")
        access_token = response.json().get('access_token')

        firmware_path = str(source[0])
        with open(firmware_path, 'rb') as firmware_file:
            firmware = firmware_file.read()

        md5 = hashlib.md5(firmware).hexdigest()
        filename = os.path.basename(firmware_path)

        encoder = MultipartEncoder(fields={
            'MD5': md5,
            'file': (filename, firmware, 'application/octet-stream')}
        )

        bar = tqdm(desc='Upload Progress',
                   total=encoder.len,
                   dynamic_ncols=True,
                   unit='B',
                   unit_scale=True,
                   unit_divisor=1024,
                   mininterval=0.2
                   )

        monitor = MultipartEncoderMonitor(
            encoder, lambda monitor: bar.update(monitor.bytes_read - bar.n))

        post_headers = build_headers(
            host_ip,
            emsesp_url,
            content_type=monitor.content_type,
            access_token=access_token,
            extra_headers={
                'Content-Length': str(monitor.len),
                'Origin': emsesp_url
            }
        )

        upload_url = f"{emsesp_url}/rest/uploadFile"
        started = time.perf_counter()
        response = session.post(
            upload_url, data=monitor, headers=post_headers, timeout=None)
        elapsed = time.perf_counter() - started

        bar.close()
        print()

        if response.status_code != 200:
            print_fail(f"Upload failed (code {response.status_code}).")
        else:
            kibs = (encoder.len / 1024) / elapsed if elapsed > 0 else 0
            print_success(
                f"Upload successful ({kibs:.0f} KiB/s in {elapsed:.1f}s). Rebooting device.")
            restart_headers = build_headers(
                host_ip, emsesp_url, access_token=access_token)
            restart_url = f"{emsesp_url}/api/system/restart"
            try:
                response = session.get(
                    restart_url, headers=restart_headers, timeout=10)
                if response.status_code != 200:
                    print_fail(f"Restart failed (code {response.status_code})")
            except requests.RequestException:
                # Device often drops the connection as it reboots — that's OK.
                pass

        print()
    finally:
        session.close()


if env.get('UPLOAD_PROTOCOL') == 'custom':
    env.Replace(UPLOADCMD=on_upload)
