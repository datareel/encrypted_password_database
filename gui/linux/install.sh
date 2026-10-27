#!/bin/bash

echo "Installing E-Password DB for Linux"

if [ ! -d ${HOME}/.encrypted_password_database ]; then
    mkdir -pv ${HOME}/.encrypted_password_database/{bin,backups,certs,data,docs,keys,logs}
    chmod -R 700  ${HOME}/.encrypted_password_database
fi

if [ ! -d ${HOME}/.encrypted_password_database ]; then
    echo "ERROR: Could not make directory ${HOME}/.encrypted_password_database"
    exit 1
fi

if [ ! -d ${HOME}/.encrypted_password_database/bin ]; then mkdir -pv  ${HOME}/.encrypted_password_database/bin; fi
/bin/cp -fv encrypted_password_database ${HOME}/.encrypted_password_database/bin/.
chmod 755 ${HOME}/.encrypted_password_database/bin/encrypted_password_database

if [ ! -f ${HOME}/.encrypted_password_database/bin/encrypted_password_database ]; then
    echo "ERROR: Missing binary for application ${HOME}/.encrypted_password_database/bin/encrypted_password_database"
    exit 1
fi

/bin/mkdir -pv ${HOME}/.local/share/applications
/bin/mkdir -pv ${HOME}/.local/share/icons
/bin/cp -fpv encrypted_password_database.desktop ${HOME}/.local/share/applications
chmod 755 ${HOME}/.local/share/applications/encrypted_password_database.desktop
/bin/cp -fv encrypted_password_database.png ${HOME}/.local/share/icons/.
chmod 644 ${HOME}/.local/share/icons//encrypted_password_database.png
# 10/27/2026: Icons at every size for the desktop icon theme
if [ -d icons ]; then
    for f in icons/*x*.png; do
	n=$(basename ${f} .png)
	/bin/mkdir -p ${HOME}/.local/share/icons/hicolor/${n}/apps
	/bin/cp -f ${f} ${HOME}/.local/share/icons/hicolor/${n}/apps/encrypted_password_database.png
    done
    /bin/mkdir -p ${HOME}/.local/share/icons/hicolor/scalable/apps
    /bin/cp -f icons/scalable.svg ${HOME}/.local/share/icons/hicolor/scalable/apps/encrypted_password_database.svg
    command -v gtk-update-icon-cache >/dev/null 2>&1 && gtk-update-icon-cache -q -t ${HOME}/.local/share/icons/hicolor 2>/dev/null
fi
sed -i s,'${HOME}',"${HOME}",g ${HOME}/.local/share/applications/encrypted_password_database.desktop

/bin/cp -fpv ${HOME}/.local/share/applications/encrypted_password_database.desktop ${HOME}/Desktop/.

echo "Install complete"
exit 0;

