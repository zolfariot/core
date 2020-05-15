#!/bin/bash
SCRIPTPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd "$SCRIPTPATH"

if [ ! -d openssl ]; then
  git clone -b OpenSSL_1_1_1g --depth=1 https://github.com/openssl/openssl.git
fi
