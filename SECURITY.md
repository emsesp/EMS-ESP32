# Security Policy

## Reporting a Vulnerability

Please report any security vulnerabilities using the [Contact Form](https://emsesp.org/About/#-contact).

## Firmware signing

From 3.9.0, EMS-ESP only installs firmware and filesystem images signed by EMS-ESP when they're uploaded in the Web UI, installed from a URL, sent over the network or uploaded in the Recovery firmware. Flashing over USB isn't checked. To install your own build, see [Installing your own build](CONTRIBUTING.md#installing-your-own-build).

### The keys

Firmware is signed with ECDSA P-256 over SHA-256 of the whole `.bin`. The signature is appended to the file, so signed files also install on older versions. The firmware contains the public keys of two signing keys, in `lib/FirmwareSignature/firmware_keys.h`:

- **Release key:** signs every official release and development build. It is held by the project maintainer (proddy). It exists only as the `EMSESP_SIGNING_KEY` secret of the `release` environment in this repository, and as an encrypted offline backup. It is not kept on development machines.
- **Spare key:** never used for signing until the release key has to be replaced. It is held offline by the project maintainer only, and never stored on GitHub or on a computer that builds firmware.

Only firmware built by the GitHub Actions of this repository is signed, so only those builds are shown as "Official build". The `release` environment can only be used by the `dev` and `main` branches and `v*` tags, and every run that signs waits until a member of the [owners](https://github.com/orgs/emsesp/teams/owners) team approves it. Only owners can create `v*` tags. Pull requests and forks never get the key. Before a release is published, the workflow checks every `.bin` against the public keys.

You can check a downloaded file yourself with `python3 scripts/sign_firmware.py verify <file.bin>` (needs `pip install cryptography`).

### Replacing a key

The release key is replaced when it is lost or may have leaked, or when a maintainer with access to it leaves the project:

1. Create a new spare key with `python3 scripts/sign_firmware.py genkey <new_spare.pem>` on an offline computer and store it offline.
2. In `firmware_keys.h`, remove the old release key, make the spare the first entry and add the public key of the new spare (`python3 scripts/sign_firmware.py pubkey <new_spare.pem>`).
3. Replace the `EMSESP_SIGNING_KEY` secret of the `release` environment with the old spare, which is now the release key, and publish a release.

Devices accept that release because they already trust the spare key. Once it's installed, the old release key is no longer accepted. Releases signed with the old key can then no longer be installed from the Web UI, so the files of the latest previous stable release are signed again with the new key.

If the release key may have leaked, firmware signed with it is still accepted by devices that haven't been updated yet. This is announced in a security advisory, asking everyone to update.

If both keys are lost or have leaked, the only way to move devices to new keys is a release installed with `allow unsigned` on the serial console or flashed over USB.
