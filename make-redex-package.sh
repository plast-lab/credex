#!/usr/bin/env bash

# Helper script to create redex archive.

# Stop on first error.
set -e

function build() {
    TMP_DIR=`mktemp -d`

    # Workaround for static linking failure on Fedora 29.
    export CFLAGS="-L$(realpath .libs)"

    autoreconf -ivf
    ./configure --prefix=${TMP_DIR}

    if [ "${MAX_PARALLEL}" == "1" ]; then
	make -j
    else
	make -j4
    fi

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
}

function packageDeb() {
    DEB_BUILD_DIR=credex-ubuntu-18.04
    DEB_PREFIX=opt/credex
    local ZIP_PATH="$1"

    pushd packages

    pushd ${DEB_BUILD_DIR}
    rm -rf ${DEB_PREFIX}
    mkdir -p ${DEB_PREFIX}
    cd ${DEB_PREFIX}
    unzip ${ZIP_PATH}
    popd

    dpkg-deb --build ${DEB_BUILD_DIR}

    popd
}


for OPT in $*; do
    if [ "${OPT}" == "--deb" ]; then
	PACKAGE_DEB=1
    elif [ "${OPT}" == "--max-parallel" ]; then
	MAX_PARALLEL=1
    elif [ "${OPT}" == "--skip-build" ]; then
	SKIP_BUILD=1
    elif [ "${OPT}" == "--help" ]; then
	echo 'Usage: make-redex-package.sh [--deb]'
	echo
	echo 'Options:'
	echo '  --deb           Generate a .deb file (on Ubuntu).'
	echo '  --max-parallel  Use as many parallel workers in the make job server as possible.'
	echo '  --skip-build    Skip the build phase.'
	exit
    fi
done

CURRENT_DIR="${PWD}"
HEAD_HASH=`git rev-parse HEAD`
REDEX_ZIP="${CURRENT_DIR}/credex-${HEAD_HASH}.zip"

if [ "${SKIP_BUILD}" != "1" ]; then
    build
fi

if [ "${PACKAGE_DEB}" == "1" ]; then
    packageDeb "$(realpath ${REDEX_ZIP})"
fi
