#!/bin/bash
# Creates a new CA and server certificate for Mosquitto TLS, starting clean.
#
# Usage: ./server-certs.sh <ca-passphrase> [hostname]
#   ca-passphrase  passphrase for the CA key (min. 4 characters)
#   hostname       name clients use with -h (default: desktop.lan)
#
# WARNING: every run deletes the old certificates and creates a NEW CA.
# Copy the new ca.crt to every client, and recreate any client certificates.
#
# Output goes to ./server-certs: ca.crt, ca.key, server.crt, server.key

set -euo pipefail

if [[ $# -lt 1 || ${#1} -lt 4 ]]; then
  echo "Usage: $0 <ca-passphrase (min. 4 chars)> [hostname]" >&2
  exit 1
fi

export CA_PASS="$1"
HOST="${2:-desktop.lan}"
IP=""              # optional: set to the server's LAN IP to also allow connecting by IP
DIR="server-certs"
CA_DAYS=3650
SERVER_DAYS=720

# --- Start clean ---
echo "Removing old certificate files"
rm -rf "$DIR"
# leftovers from older versions of this script in the current folder
rm -f ca.crt ca.key ca.srl server.crt server.key server.csr server.srl v3.ext
mkdir -p "$DIR"
cd "$DIR"

# --- CA ---
echo "Creating CA key and certificate (valid $CA_DAYS days)"
openssl genrsa -aes256 -passout env:CA_PASS -out ca.key 2048
# CN must differ from the server's CN, otherwise verification fails
openssl req -new -x509 -days "$CA_DAYS" -key ca.key -passin env:CA_PASS \
  -out ca.crt -subj "/CN=Mosquitto Home CA"

# --- v3.ext with the names clients will connect to ---
SAN="DNS:$HOST"
SHORT="${HOST%%.*}"
[[ "$SHORT" != "$HOST" ]] && SAN="$SAN,DNS:$SHORT"
[[ -n "$IP" ]] && SAN="$SAN,IP:$IP"

cat > v3.ext <<EOF
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage=digitalSignature,keyEncipherment
extendedKeyUsage=serverAuth
subjectAltName=$SAN
EOF

# --- Server key and certificate (no prompts, no challenge password) ---
echo "Creating server key and certificate for $SAN"
openssl genrsa -out server.key 2048
openssl req -new -key server.key -out server.csr -subj "/CN=$HOST"
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -passin env:CA_PASS \
  -CAcreateserial -out server.crt -days "$SERVER_DAYS" -extfile v3.ext
rm -f server.csr

# --- Check the result ---
echo
openssl verify -CAfile ca.crt server.crt
openssl x509 -in server.crt -noout -subject -enddate -ext subjectAltName
echo
echo "Copy ca.crt, server.crt and server.key from $DIR to the Mosquitto certs folder,"
echo "and give clients the new ca.crt."
