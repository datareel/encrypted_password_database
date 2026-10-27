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
    # 10/27/2026: The desktop rereads the icon theme when its folder changes
    touch ${HOME}/.local/share/icons/hicolor 2>/dev/null
    # Refresh the icon cache only if there is one: a new cache in the
    # user's folder would hide icons other programs add later
    if [ -f ${HOME}/.local/share/icons/hicolor/icon-theme.cache ]; then
        command -v gtk-update-icon-cache >/dev/null 2>&1 && gtk-update-icon-cache -q -t ${HOME}/.local/share/icons/hicolor 2>/dev/null
    fi
fi
sed -i s,'${HOME}',"${HOME}",g ${HOME}/.local/share/applications/encrypted_password_database.desktop
# 10/27/2026: Older installs named the icon by its file path, which kept
# the old icon in the task bar. The new .desktop file uses the themed
# icon name and StartupWMClass, and the old icon file is replaced above.
command -v update-desktop-database >/dev/null 2>&1 && update-desktop-database -q ${HOME}/.local/share/applications 2>/dev/null

if [ -d ${HOME}/Desktop ]; then
    /bin/cp -fpv ${HOME}/.local/share/applications/encrypted_password_database.desktop ${HOME}/Desktop/.
fi

echo "Install complete"
exit 0;

