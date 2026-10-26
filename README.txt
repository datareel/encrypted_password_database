Encrypted Password Database

Release 2026.101

Supports Window 10, Windows 11, and Windows 2025 server
Supports Red Hat Enterprise Linux 6, 7, 8, 9, and 10 

The Encrypted Password Database is an enterprise level application for
storing authentication information and automating application password
management.

RELEASE 2026.101: SECURITY FIXES AND DATABASE FORMAT V2
-------------------------------------------------------
Release 2026.101 fixes the issues found by a security review on
10/25/2026 and adds a new database format.

Database format v2:
* Passwords and key files are turned into the database key once per
  open with PBKDF2-HMAC-SHA256 (600000 iterations, 32-byte random salt)
  instead of 1000 iterations for every field. Separate keys for field
  encryption, field MACs and a database MAC are derived from it.
* A MAC over the whole database, bound to each record and column,
  detects entries that were changed, swapped, added or removed outside
  the program. A failed check is reported when the database is opened.
  The GUI warns that the file may have been tampered with and lets you
  accept and re-seal it, with No as the default; epdb_auth_tool fails
  and --reseal-db accepts the current contents.
* Users added to a v2 database use RSA-OAEP with SHA-256.
* New databases are created as v2. Existing databases keep working and
  are only upgraded on request: File/Upgrade Database Format in the GUI
  or epdb_auth_tool --upgrade-db. The upgrade first saves the database
  to name.v1-backup-<date>-<time>.ehd and .ehx (mode 600), checks every
  entry of the upgraded database against the original, and leaves the
  original unchanged if anything fails. RSA and smart card users of the
  v1 database can open the upgraded database.
* Releases before 2026.101 cannot open a v2 database.

Security fixes:
* Damaged or crafted database files no longer crash the program or
  overflow memory (user list, encrypted strings, index file, records).
* epdb_auth_tool checks the password or key before listing or
  searching, stops on decrypt errors, exits 1 on any error, and writes
  prompts and messages to stderr.
* RSA and smart card opens check the recovered key, and a database
  name that was never set no longer accepts any password.
* Builds use stack protection, FORTIFY_SOURCE, full RELRO and a
  non-executable stack, and epdb_auth_tool is a position independent
  executable.

Changes in behaviour:
* Private RSA key and AES key files must be owned by you with no group
  or other access (chmod 600 file), like ssh private keys.
* epdb_auth_tool needs database file names that end in .ehd.
* The Password and Previous Password columns are masked; View/Show
  passwords shows them. Copied values are cleared from the clipboard
  after 30 seconds (ClipboardClearSeconds) and the open databases are
  closed after 15 idle minutes (AutoLockMinutes).
* Only http and https links open, after asking. Other text in a cell
  is never started as a program.
* CSV export writes a mode 600 file and prefixes values that start
  with = + - @ with a quote; the import removes it.
* Merge does not support v2 databases yet. Export to CSV and import.
* epdb_auth_tool has --password-file, --password-fd and the same for
  the RSA passphrase and smart card PIN. --password=value still works
  but prints a warning. New output options: --csv, --field=n, --null.

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


INSTALLING ON WINDOWS
---------------------
The "encrypted_password_database.exe"  is a standalone EXE file that
can be run on any Windows system with no installer needed.

INSTALLING ON LINUX
------------------
The Binaries for RHEL distribution includes and installer that can
install to your user account without root access. To install from the
command line: 

cd ${HOME}/Downloads
wget https://datareel.com/downloads/encrypted_password_database/encrypted_password_database_2023_109_rhel_7_8_9_binaries.tar.gz
tar xvfz encrypted_password_database_2023_109_rhel_7_8_9_binaries.tar.gz
cd encrypted_password_database_2023_109_rhel_7_8_9_binaries
./rh_install.sh

To run the Encrypted Password Database GUI from the command line:

${HOME}/.encrypted_password_database/bin/encrypted_password_database

For GNOME desktops the Encrypted Password Database will be added to your applications list
For GNOME classic desktops you will have a desktop icon and entry
under Applications->Other

To run the command line tool:

${HOME}/.encrypted_password_database/bin/epdb_auth_tool --help

SOURCE CODE BUILD ON WINDOWS
----------------------------
For Windows the current release supports Microsoft Visual Studio 2022 or 2026. 

For instructions on how to install Microsoft Visual Studio 2022 please
see the following readme file: README_VisualStudio_2022.txt 

For instructions on how to install Microsoft Visual Studio 2026 please
see the following readme file: README_VisualStudio.txt 

The Windows build requires the OpenSSL library. 

For instructions on how to build OpenSSL for Windows please see the
following readme file: README_windows_openssl.txt 

The Windows build requires wxWidgets version 3.2.3 or higher.

For instructions on how to build wxWidgets for Windows please see the
following readme file: README_windows_wxwidgets.txt

For instructions on how to build Windows 11 or 2025 server development
VM please see the following readme file: README_windows_setup.txt 

To build the Windows executables download the latest release or clone
the GIT repo. In the Windows build example below this document will
assume we placed the code on a data drive in a git directory.  

On the data drive change your directory to the
encrypted_password_database build subdirectory:

> E:
> cd E:\git\encrypted_password_database

Setup your Visual Studio environment:

> cd  E:\git\encrypted_password_database\env
> msvc.bat

Edit the msvc.env file to setup the path to your OpenSSL, ZLIB, and
wxWidgets builds: 

> notepad msvc.env

# Setup path to ZLIB
ZCODE_DIR = ../../../../3plibs/zlib32-1.3.1

# Setup path to OPENSSL library
OPENSSL_DIR = ../../../../3plibs/openssl32

# Set path to wxWidgets library here
WXDIR = ../../../../3plibs/wxWidgets32-3.2.4

To build the Windows GUI:

> cd  E:\git\encrypted_password_database\gui\msvc

Run nmake to build the executable:
 
> nmake

To run the Windows program: 

> encrypted_password_database.exe

This is a standalone EXE file that can be ran on any Windows system   
with no installer needed.

To build the Windows console program:

> cd  E:\git\encrypted_password_database\console\msvc

Run nmake to build the executable:
 
> nmake


SOURCE CODE BUILD ON LINUX
--------------------------
Building the binaries from the source code repository will require you
to build wxWidgets version 3.2.3 or higher. To download and build the
wxWidgets source code please visit the wxWidgets home page and follow
the latest download and build instructions for Linux wxGTK:

 https://www.wxwidgets.org/

To build the latest release of the Encrypted Password Database:

> mkdir -pv ${HOME}/git
> cd ${HOME}/git
> wget https://datareel.com/downloads/encrypted_password_database/encrypted_password_database-<version>.tar.gz
> tar xzf encrypted_password_database-<version>.tar.gz
> mv encrypted_password_database-<version> encrypted_password_database
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

