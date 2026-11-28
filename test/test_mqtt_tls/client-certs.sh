#!/bin/bash
# Creates a client certificate signed by the CA from server-certs.sh.
#
# Usage: ./client-certs.sh <ca-passphrase> [client-name]
#   ca-passphrase  passphrase of server-certs/ca.key
#   client-name    CN of the certificate, also the MQTT username when the
#                  broker uses use_identity_as_username (default: client)
#
# Output: client-certs/<client-name>.crt and .key
# Rerun after every server-certs.sh run, because that creates a new CA.

set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <ca-passphrase> [client-name]" >&2
  exit 1
fi

export CA_PASS="$1"
NAME="${2:-client}"
CA_CERT="server-certs/ca.crt"
CA_KEY="server-certs/ca.key"
DIR="client-certs"
DAYS=720

if [[ ! -f "$CA_CERT" || ! -f "$CA_KEY" ]]; then
  echo "CA not found: run server-certs.sh first ($CA_CERT and $CA_KEY needed)" >&2
  exit 1
fi
openssl rsa -in "$CA_KEY" -passin env:CA_PASS -noout 2>/dev/null \
  || { echo "Wrong passphrase for $CA_KEY" >&2; exit 1; }

# --- Start clean for this client ---
mkdir -p "$DIR"
rm -f "$DIR/$NAME".{crt,key,csr} "$DIR/$NAME.ext"
# leftovers from the old version of this script in the current folder
rm -f client.crt client.key client.csr

cat > "$DIR/$NAME.ext" <<EOF
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage=digitalSignature,keyEncipherment
extendedKeyUsage=clientAuth
EOF

# --- Client key and certificate (no prompts, no challenge password) ---
echo "Creating client key and certificate for CN=$NAME"
openssl genrsa -out "$DIR/$NAME.key" 2048
openssl req -new -key "$DIR/$NAME.key" -out "$DIR/$NAME.csr" -subj "/CN=$NAME"
openssl x509 -req -in "$DIR/$NAME.csr" -CA "$CA_CERT" -CAkey "$CA_KEY" \
  -passin env:CA_PASS -CAcreateserial -out "$DIR/$NAME.crt" -days "$DAYS" \
  -extfile "$DIR/$NAME.ext"
rm -f "$DIR/$NAME.csr" "$DIR/$NAME.ext"

# --- Check the result ---
echo
openssl verify -CAfile "$CA_CERT" "$DIR/$NAME.crt"
openssl x509 -in "$DIR/$NAME.crt" -noout -subject -enddate
echo
echo "Give the client $DIR/$NAME.crt, $DIR/$NAME.key and $CA_CERT."
