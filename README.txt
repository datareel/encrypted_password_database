Encrypted Password Database

The Encrypted Password Database is an enterprise level application for
storing authentication information and automating application password
management.

FEATURES
--------
    AES CBC 256 bit symmetric encryption
    Encrypt/Decrypt using a password, symmetric key, RSA key, or smart card
    Multi user access using RSA keys
    Multi user access using smart cards
    Supports DOD CAC smart cards
    Graphical User Interface to manage database entries
    Command line tool for automation
    Command line tool for scripts and embedded processes to get passwords
    
DOCUMENTATION
-------------
For the latest updates and documents please visit the online help page:

https://datareel.com/encrypted_password_database/help/


INSTALLING
----------
The Binaries for RHEL distribution includes and installer that can
install to your user account without root access. To install from the
command line: 

cd ${HOME}/Downloads
wget https://datareel.com/downloads/encrypted_password_database/encrypted_password_database_2023_108_rhel_7_8_9_binaries.tar.gz
tar xvfz encrypted_password_database_2023_108_rhel_7_8_9_binaries.tar.gz
cd encrypted_password_database_2023_108_rhel_7_8_9_binaries
./rh_install.sh

To run the Encrypted Password Database GUI from the command line:

${HOME}/.encrypted_password_database/bin/encrypted_password_database

For GNOME desktops the Encrypted Password Database will be added to your applications list
For GNOME classic desktops you will have a desktop icon and entry
under Applications->Other

To run the command line tool:

${HOME}/.encrypted_password_database/bin/epdb_auth_tool --help

SOURCE CODE BUILD
-----------------
Building the binaries from the source code repository will require you
to build wxWidgets version 3.2.3 or higher. To download and build the
wxWidgets source code please visit the wxWidgets home page and follow
the latest download and build instructions for Linux wxGTK:

 https://www.wxwidgets.org/

To build the latest release of the Encrypted Password Database:

> mkdir -pv ${HOME}/git
> git clone https://github.com/datareel/encrypted_password_database.git
> cd encrypted_password_database

Set up the build environment for the GUI build:

> cd ${HOME}/git/encrypted_password_database/env
> vi linux.mak

WXSRCDIR = $(HOME)/3plibs/wxWidgets-3.2.3

Change the WXSRCDIR to your wxWidget build installation path. 

> cd ${HOME}/git/encrypted_password_database/gui/linux
> vi makefile

        make PROJECT=encrypted_password_database FINAL=1 64BITCFG=1 RHEL=8 -f ../../env/linux.mak

install:
        ./install.sh

clean:
        make PROJECT=encrypted_password_database FINAL=1 64BITCFG=1 RHEL=8 -f ../../env/linux.mak clean

Change the RHEL version to match your major RHEL release version from
/etc/redhat-release

Run the make to build the GUI

> make

Set up the build environment for the command line utility build:

> cd ${HOME}/git/encrypted_password_database/console/linux
> vi makefile

SMARTCARD_DEMACS = -D__ENABLE_SMART_CARD__ -D__RHEL8__

Change the RHEL version to match your major RHEL release version from
/etc/redhat-release 

Run the make to build the command line utility programs:

> make

