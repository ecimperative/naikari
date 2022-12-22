#!/bin/bash

set -e

usage() {
    cat <<EOF
WINDOWS INSTALL SCRIPT FOR NAEV
This script is called by "meson install" if building for Windows.
It builds the appropriate NSIS installer into the root naev directory.
Requires mingw-w64-x86_64-nsis to be installed.
EOF
    exit 1
}

# Defaults
NIGHTLY="false"
STAGING="$MESON_SOURCE_ROOT/extras/windows/installer/bin"

while getopts n: OPTION "$@"; do
    case $OPTION in
    n)
        NIGHTLY="${OPTARG}"
        ;;
    *)
        usage
        ;;
    esac
done

BUILD_DATE="$(date +%Y%m%d)"

# MinGW DLL search paths
MINGW_BUNDLEDLLS_SEARCH_PATH="/mingw64/bin:/usr/x86_64-w64-mingw32/bin:/usr/x86_64-w64-mingw32/sys-root/mingw/bin:/usr/lib/mxe/usr/x86_64-w64-mingw32.shared/bin"
# Include all subdirs (mingw-bundledlls can't search recursively) of in-tree and out-of-tree subproject dirs.
# Normally, Meson builds everything out-of-tree, but some subprojects have their own build systems which do as they please.
for MESON_SUBPROJ_DIR in "${MESON_SOURCE_ROOT}/subprojects" "${MESON_BUILD_ROOT}/subprojects"; do
    echo "Searching ${MESON_SUBPROJ_DIR}"
    if [ -d "${MESON_SUBPROJ_DIR}" ]; then
        MINGW_BUNDLEDLLS_SEARCH_PATH+=$(find "${MESON_SUBPROJ_DIR}" -type d -printf ":%p")
    fi
done
export MINGW_BUNDLEDLLS_SEARCH_PATH

# Check version exists and set VERSION variable.
VERSION="$(<"$STAGING/dat/VERSION")"
if [[ "$NIGHTLY" == "true" ]]; then
    export VERSION="$VERSION+DEBUG.$BUILD_DATE"
fi
SUFFIX="$VERSION-win64"

# Move compiled binary to staging folder.
echo "copying naev binary to staging area"
mv "$STAGING/naikari.exe" "$STAGING/naikari-$SUFFIX.exe"

# Collect DLLs
echo "Collecting DLLs in staging area"
"$MESON_SOURCE_ROOT/extras/windows/mingw-bundledlls/mingw-bundledlls" --copy "$STAGING/naikari-$SUFFIX.exe"

# Create distribution folder
echo "creating distribution folder if it doesn't exist"
mkdir -p "${MESON_BUILD_ROOT}/dist"

# Build installer
makensis -DSUFFIX="$SUFFIX" "$MESON_SOURCE_ROOT/extras/windows/installer/naikari.nsi"

# Move installer to distribution directory
mv "$MESON_SOURCE_ROOT/extras/windows/installer/naikari-$SUFFIX.exe" "${MESON_BUILD_ROOT}/dist"

echo "Successfully built Windows Installer for $SUFFIX"

echo "Cleaning up staging area"
rm -rf "$STAGING"
