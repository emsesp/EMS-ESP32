#!/usr/bin/env python3
"""
Signs EMS-ESP firmware so EMS-ESP 3.9 and later show it as an official build.

The signature is ECDSA P-256 over SHA-256 of the whole .bin, DER encoded and zero padded to 512 bytes, appended to
the file. Old EMS-ESP versions and the ESP32 ignore the extra bytes, so a signed file installs everywhere.
lib/FirmwareSignature checks it on the device, with the public keys in lib/FirmwareSignature/firmware_keys.h.

The private key comes from the EMSESP_SIGNING_KEY environment variable, either the PEM text or the path to the
PEM file, or from ~/.ems-esp/firmware_signing_key.pem.

  python3 scripts/sign_firmware.py genkey <key.pem>          create a new key
  python3 scripts/sign_firmware.py pubkey <key.pem>          print the public key for firmware_keys.h
  python3 scripts/sign_firmware.py sign <file.bin> [...]     sign in place and rewrite the .md5 next to it
  python3 scripts/sign_firmware.py verify <file.bin> [...]   check against the keys in firmware_keys.h
"""

import hashlib
import os
import re
import sys
from pathlib import Path

SIGNATURE_SIZE = 512
DEFAULT_KEY_FILE = Path.home() / ".ems-esp" / "firmware_signing_key.pem"
KEYS_HEADER = Path(__file__).resolve().parent.parent / "lib" / "FirmwareSignature" / "firmware_keys.h"


def _crypto():
    from cryptography.hazmat.primitives import hashes, serialization
    from cryptography.hazmat.primitives.asymmetric import ec

    return hashes, serialization, ec


def load_key_pem():
    """The PEM bytes of the signing key, or None if there is none."""
    value = os.environ.get("EMSESP_SIGNING_KEY", "").strip()
    if value:
        if "-----BEGIN" in value:
            return value.encode()
        return Path(value).expanduser().read_bytes()
    if DEFAULT_KEY_FILE.exists():
        return DEFAULT_KEY_FILE.read_bytes()
    return None


def _private_key(pem):
    _, serialization, ec = _crypto()
    key = serialization.load_pem_private_key(pem, password=None)
    if not isinstance(key, ec.EllipticCurvePrivateKey) or key.curve.name != "secp256r1":
        raise ValueError("the signing key must be an ECDSA P-256 key")
    return key


def public_key_bytes(key):
    """Uncompressed point without the 0x04 prefix, as in firmware_keys.h."""
    _, serialization, _ = _crypto()
    point = key.public_bytes(serialization.Encoding.X962, serialization.PublicFormat.UncompressedPoint)
    return point[1:]


def _keys_in(text):
    keys = []
    for block in re.findall(r"\{([^{}]*)\}", text):
        values = bytes(int(v, 16) for v in re.findall(r"0x([0-9a-fA-F]{2})", block))
        if len(values) == 64:
            keys.append(values)
    return keys


def known_public_keys():
    """The public keys compiled into the firmware."""
    return _keys_in(KEYS_HEADER.read_text().split("FIRMWARE_PUBLIC_KEYS", 1)[1])


def _key_lines(raw):
    lines = [", ".join(f"0x{b:02x}" for b in raw[i : i + 16]) for i in range(0, 64, 16)]
    return "    {" + ",\n     ".join(lines) + "},"


def is_signed_by(data, public_keys):
    """True if data ends with a valid signature from one of the public keys."""
    hashes, _, ec = _crypto()
    from cryptography.exceptions import InvalidSignature
    from cryptography.hazmat.primitives.asymmetric.utils import decode_dss_signature

    if len(data) <= SIGNATURE_SIZE:
        return False
    image, tail = data[:-SIGNATURE_SIZE], data[-SIGNATURE_SIZE:]
    if tail[0] != 0x30 or tail[1] > 0x7F:
        return False
    der = tail[: 2 + tail[1]]
    if any(tail[len(der) :]):
        return False
    try:
        decode_dss_signature(der)
    except ValueError:
        return False

    for raw in public_keys:
        public_key = ec.EllipticCurvePublicKey.from_encoded_point(ec.SECP256R1(), b"\x04" + raw)
        try:
            public_key.verify(der, image, ec.ECDSA(hashes.SHA256()))
            return True
        except InvalidSignature:
            pass
    return False


def sign_bytes(data, pem):
    """data with the signature appended. Data that's already signed by this key is returned unchanged."""
    hashes, _, ec = _crypto()
    key = _private_key(pem)
    if is_signed_by(data, [public_key_bytes(key.public_key())]):
        return data
    signature = key.sign(data, ec.ECDSA(hashes.SHA256()))
    return data + signature + b"\x00" * (SIGNATURE_SIZE - len(signature))


def sign_file(bin_file, pem):
    """Signs a .bin in place and rewrites the .md5 next to it if there is one. Returns the MD5."""
    bin_file = Path(bin_file)
    data = sign_bytes(bin_file.read_bytes(), pem)
    bin_file.write_bytes(data)
    md5 = hashlib.md5(data).hexdigest()
    md5_file = bin_file.with_suffix(".md5")
    if md5_file.exists():
        md5_file.write_text(md5)
    return md5


def main(argv):
    if len(argv) < 3 or argv[1] not in ("genkey", "pubkey", "sign", "verify"):
        print(__doc__)
        return 1

    command, files = argv[1], argv[2:]
    _, serialization, ec = _crypto()

    if command == "genkey":
        path = Path(files[0]).expanduser()
        if path.exists():
            print(f"{path} already exists")
            return 1
        key = ec.generate_private_key(ec.SECP256R1())
        pem = key.private_bytes(
            serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8, serialization.NoEncryption()
        )
        path.parent.mkdir(parents=True, exist_ok=True)
        fd = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
        with os.fdopen(fd, "wb") as f:
            f.write(pem)
        print(f"Key saved to {path}. Keep it secret and keep a backup")
        return 0

    if command == "pubkey":
        raw = public_key_bytes(_private_key(Path(files[0]).expanduser().read_bytes()).public_key())
        print(_key_lines(raw))
        return 0

    if command == "sign":
        pem = load_key_pem()
        if pem is None:
            print("No signing key, set EMSESP_SIGNING_KEY")
            return 1
        for f in files:
            md5 = sign_file(f, pem)
            print(f"Signed {f} (MD5 {md5})")
        return 0

    keys = known_public_keys()
    failed = 0
    for f in files:
        ok = is_signed_by(Path(f).read_bytes(), keys)
        print(f"{f}: {'signed' if ok else 'NOT signed'}")
        failed += not ok
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
