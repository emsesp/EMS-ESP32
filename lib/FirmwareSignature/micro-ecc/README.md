micro-ecc by Kenneth MacKay, https://github.com/kmackay/micro-ecc, BSD 2-clause license (LICENSE.txt).

Copied from the ESP-IDF 5.5.5 bootloader (components/bootloader/subproject/components/micro-ecc), which uses it to
verify signed images. Only the files needed for ECDSA verification on secp256r1 are kept, and platform-specific.inc
is replaced by a stub without a random number generator. It's compiled through ../uecc_p256.c, which sets the options.
