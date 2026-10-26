#!/bin/bash
# Installer for the Encrypted Password Database RHEL binaries package.
# Run it from the top directory of the unpacked package. It installs
# to your user account (${HOME}/.encrypted_password_database), no root
# access needed. Supports RHEL 8, 9 and 10.

echo "Installing Encrypted Password Database for Linux"

cd $(dirname $0)

RHEL_VERSION=0

# Check for rhel8
/bin/cat /etc/redhat-release 2>/dev/null | grep -i Ootpa >/dev/null 2>&1
if [ $? -eq 0 ]; then RHEL_VERSION=8; fi

# Check for rhel9
/bin/cat /etc/redhat-release 2>/dev/null | grep -i Plow >/dev/null 2>&1
if [ $? -eq 0 ]; then RHEL_VERSION=9; fi

# Check for rhel10
/bin/cat /etc/redhat-release 2>/dev/null | grep -i Coughlan >/dev/null 2>&1
if [ $? -eq 0 ]; then RHEL_VERSION=10; fi

# RHEL rebuilds (Rocky, AlmaLinux) use other code names
if [ ${RHEL_VERSION} -eq 0 ] && [ -f /etc/os-release ]; then
    RHEL_VERSION=$(. /etc/os-release; echo "${ID_LIKE} ${ID}" | grep -qw rhel && echo ${VERSION_ID%%.*})
    if [ -z "${RHEL_VERSION}" ]; then RHEL_VERSION=0; fi
fi

if [ ${RHEL_VERSION} -eq 0 ]; then
   echo "ERROR: Could not detect Red Hat release version"
   exit 1
fi

if [ ${RHEL_VERSION} -lt 8 ] || [ ${RHEL_VERSION} -gt 10 ]; then
    echo "ERROR: Red Hat version ${RHEL_VERSION} is not supported for this release, RHEL 8, 9 and 10 only"
    exit 1
fi

echo "Red Hat version ${RHEL_VERSION}"

if [ ! -f rhel${RHEL_VERSION}/encrypted_password_database ] || [ ! -f rhel${RHEL_VERSION}/epdb_auth_tool ]; then
    echo "ERROR: Missing rhel${RHEL_VERSION} binaries, run this script from the unpacked package directory"
    exit 1
fi

# Report system libraries the binaries need that are not installed
missing=$(ldd rhel${RHEL_VERSION}/encrypted_password_database rhel${RHEL_VERSION}/epdb_auth_tool 2>/dev/null | grep 'not found' | awk '{ print $1 }' | sort -u)
if [ "${missing}" != "" ]; then
    echo "WARNING: Missing system libraries:"
    for lib in ${missing}; do echo "    ${lib}"; done
    echo "Install the packages that provide them, for example:"
    echo "    sudo dnf install gtk3 webkit2gtk3 pcre2-utf32 libXxf86vm libSM libXtst"
fi

if [ ! -d ${HOME}/.encrypted_password_database ]; then
    mkdir -pv ${HOME}/.encrypted_password_database/{bin,backups,certs,data,docs,keys,logs}
    chmod -R 700  ${HOME}/.encrypted_password_database
fi

if [ ! -d ${HOME}/.encrypted_password_database ]; then
    echo "ERROR: Could not make directory ${HOME}/.encrypted_password_database"
    exit 1
fi

echo "Installing program files"
if [ ! -d ${HOME}/.encrypted_password_database/bin ]; then mkdir -pv  ${HOME}/.encrypted_password_database/bin; fi
/bin/cp -fv rhel${RHEL_VERSION}/encrypted_password_database ${HOME}/.encrypted_password_database/bin/.
chmod 755 ${HOME}/.encrypted_password_database/bin/encrypted_password_database

if [ ! -f ${HOME}/.encrypted_password_database/bin/encrypted_password_database ]; then
    echo "ERROR: Missing binary for application ${HOME}/.encrypted_password_database/bin/encrypted_password_database"
    exit 1
fi

/bin/cp -fv rhel${RHEL_VERSION}/epdb_auth_tool ${HOME}/.encrypted_password_database/bin/.
chmod 755 ${HOME}/.encrypted_password_database/bin/epdb_auth_tool

if [ ! -f ${HOME}/.encrypted_password_database/bin/epdb_auth_tool ]; then
    echo "ERROR: Missing binary for application ${HOME}/.encrypted_password_database/bin/epdb_auth_tool"
    exit 1
fi

echo "Installing Desktop files"
/bin/mkdir -pv ${HOME}/.local/share/applications
/bin/mkdir -pv ${HOME}/.local/share/icons
/bin/cp -fpv common/encrypted_password_database.desktop ${HOME}/.local/share/applications
chmod 755 ${HOME}/.local/share/applications/encrypted_password_database.desktop
/bin/cp -fv common/encrypted_password_database.png ${HOME}/.local/share/icons/.
chmod 644 ${HOME}/.local/share/icons/encrypted_password_database.png
sed -i s,'${HOME}',"${HOME}",g ${HOME}/.local/share/applications/encrypted_password_database.desktop

if [ -d ${HOME}/Desktop ]; then
    /bin/cp -fpv ${HOME}/.local/share/applications/encrypted_password_database.desktop ${HOME}/Desktop/.
fi

echo "Install complete"
exit 0;
