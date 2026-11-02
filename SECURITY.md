# Security Policy

## Reporting a Vulnerability

Please report any security vulnerabilities using the [Contact Form](https://emsesp.org/About/#-contact).

## Firmware signing

From 3.9.0, EMS-ESP only installs firmware and filesystem images signed by EMS-ESP when they're uploaded in the Web UI, installed from a URL, sent over the network or uploaded in the Recovery firmware. Flashing over USB isn't checked. To install your own build, see [Installing your own build](CONTRIBUTING.md#installing-your-own-build).

Firmware is signed with ECDSA P-256 over SHA-256 of the whole `.bin`, and the signature is appended to the file, so signed files also install on older versions. The public keys are in `lib/FirmwareSignature/firmware_keys.h`.

Only firmware built and published by the GitHub Actions of this repository is signed by EMS-ESP. Those builds are shown as "Official build" on the Version page, anything else as "Custom build". The signing key is never stored in the repository and isn't available to pull requests or forks.

You can check a downloaded file with `python3 scripts/sign_firmware.py verify <file.bin>` (needs `pip install cryptography`).
