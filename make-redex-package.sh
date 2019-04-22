#!/usr/bin/env bash

# Helper script to create redex archive.

TMP_DIR=`mktemp -d`
CURRENT_DIR="${PWD}"
HEAD_HASH=`git rev-parse HEAD`
REDEX_ZIP="${CURRENT_DIR}/credex-${HEAD_HASH}.zip"

# Workaround for static linking failure on Fedora 29.
export CFLAGS="-L$(realpath .libs)"

autoreconf -ivf
./configure --prefix=${TMP_DIR}
make -j4

make install
CONFIG_DIR="${CURRENT_DIR}/config"
if [ ! -d "${CONFIG_DIR}" ]; then
    echo "Config directory not found, will not be packaged: ${CONFIG_DIR}"
else
    mkdir ${TMP_DIR}/config
    cp ${CONFIG_DIR}/*.config ${TMP_DIR}/config
fi

echo "Redex installed in ${TMP_DIR}, packaging..."

cd ${TMP_DIR}
rm -f ${REDEX_ZIP}
zip -r ${REDEX_ZIP} .
echo "Created archive: ${REDEX_ZIP}"
cd ${CURRENT_DIR}

rm -rf ${TMP_DIR}
