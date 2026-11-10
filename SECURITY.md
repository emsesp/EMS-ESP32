# Security Policy

## Reporting a vulnerability

Please don't report security problems in a public issue, discussion or chat.

- Report it privately on GitHub: [Report a vulnerability](https://github.com/emsesp/EMS-ESP32/security/advisories/new).
- If you can't use GitHub, use the [Contact Form](https://emsesp.org/About/#-contact).

Say which version and board you use, what an attacker needs (for example access to your network, a guest account, or the USB port) and how to reproduce it. EMS-ESP is maintained by volunteers. We aim to reply within a week, and we'll keep you updated until it's fixed.

Please give us time to publish a fix before you disclose the problem, normally up to 90 days. We credit reporters in the advisory and the release notes, unless you'd rather not be named.

## Supported versions

Security fixes go into the latest release only. Update to it to get them.

| Version         | Security fixes |
| --------------- | -------------- |
| 3.10.x          | Yes            |
| 3.9.x and older | No             |

## How fixes are published

A fix comes in a new release. Once it is out, we publish a [GitHub security advisory](https://github.com/emsesp/EMS-ESP32/security/advisories) with the affected and fixed versions, and a CVE number where one is assigned. The release notes link to it.

## Security model

EMS-ESP is designed for a home network that you trust. See [Securing your gateway](https://docs.emsesp.org/Securing) for how to set it up.

- **Don't expose it to the internet.** Use a VPN, or Home Assistant or an HTTPS reverse proxy in front of it. The Web UI and `/api` use plain HTTP, so anyone on the same network can see that traffic.
- **The Web UI and `/api` need a sign-in or an API key**, also for reading values. Guests and Read keys can read, and run the Commands an admin marked "Guests may run this command". Admins and Read and Write keys can change settings and write to the heating system. In the Web UI, changes to the firmware, users and security settings need the password entered again. Opening an `/api` address in a browser asks for a sign-in, which only reads, also for admins.
- **MQTT and Modbus are trusted channels.** Anything that can publish to EMS-ESP's MQTT topics or reach the Modbus port can write to the heating system. Protect them with the broker's access control lists and your firewall. MQTT can use TLS with a check of the broker's certificate. By default MQTT can change values and run Commands but not system commands; the MQTT setting "Commands received over MQTT" can turn commands off or allow everything. A factory reset or erasing the crash dump is never accepted over MQTT, and retained commands are ignored.
- **The telnet console** needs a Web UI username and password and is off on new installs. Admin commands on the serial console need an admin password.
- **Writes to the heating system** are checked against each value's minimum and maximum, and read-only mode blocks all writes.
- **Stored passwords** are hashed. Backups leave out passwords and keys unless an admin includes them.
- **Firmware** uploaded over the network is checked for an EMS-ESP signature (see below).

## Physical access

Anyone with access to the gateway's USB port can read its flash, flash other firmware and use the serial console. Gateways, including those sold by BBQKees Electronics, ship without secure boot or flash encryption, so that owners can always install any firmware they want. Keep the gateway where other people can't get to it.

A factory reset erases all settings, including passwords and keys, and any crash dump from the flash. Do one before you sell or give away a gateway.

## Firmware signing

From 3.10.0, EMS-ESP checks the signature of firmware uploaded in the Web UI, installed from a URL, sent over the network or uploaded in the Recovery firmware. Anything that isn't signed by EMS-ESP, including filesystem images, is only installed after the user agrees to install a custom build. Flashing over USB isn't checked. To install your own build, see [Installing your own build](CONTRIBUTING.md#installing-your-own-build).

Firmware is signed with ECDSA P-256 over SHA-256 of the whole `.bin`, and the signature is appended to the file, so signed files also install on older versions. The public keys are in `lib/FirmwareSignature/firmware_keys.h`.

Only firmware built and published by the GitHub Actions of this repository is signed by EMS-ESP. Those builds are shown as "Official build" on the Version page, anything else as "Custom build". The signing key is never stored in the repository and isn't available to pull requests or forks.

You can check a downloaded file with `python3 scripts/sign_firmware.py verify <file.bin>` (needs `pip install cryptography`).
