#!/bin/bash

echo "Installing E-Password DB command line tool for Linux"

if [ ! -d ${HOME}/.encrypted_password_database ]; then
    mkdir -pv ${HOME}/.encrypted_password_database/{bin,backups,certs,data,docs,keys,logs}
    chmod -R 700  ${HOME}/.encrypted_password_database
fi

if [ ! -d ${HOME}/.encrypted_password_database ]; then
    echo "ERROR: Could not make directory ${HOME}/.encrypted_password_database"
    exit 1
fi

if [ ! -d ${HOME}/.encrypted_password_database/bin ]; then mkdir -pv  ${HOME}/.encrypted_password_database/bin; fi
/bin/cp -fv epdb_auth_tool ${HOME}/.encrypted_password_database/bin/.
chmod 755 ${HOME}/.encrypted_password_database/bin/epdb_auth_tool

if [ ! -f ${HOME}/.encrypted_password_database/bin/epdb_auth_tool ]; then
    echo "ERROR: Missing binary for application ${HOME}/.encrypted_password_database/bin/epdb_auth_tool"
    exit 1
fi

echo "Install complete"
exit 0;
