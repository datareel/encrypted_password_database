Encrypted Password Database

Release 2026.101

Supports Windows 11 and Windows Server 2022 or later (64-bit)
Supports Red Hat Enterprise Linux 8, 9, and 10

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

Platforms and builds:
* Binaries are released for RHEL 8, 9 and 10 and for 64-bit Windows 11
  and Windows Server 2022 or later. RHEL 6 and 7 and 32-bit Windows
  binaries are no longer released.
* Smart card support is not in the 2026.101 RHEL 10 and Windows
  binaries. It is turned on for RHEL 10 and Windows in the source after
  2026.101, see SMART CARDS AND DOD CAC CARDS below.
* Built with fcrypt 2026.104 and DataReel 6.1. Windows builds use
  OpenSSL 3.5.x LTS, Linux builds use the OpenSSL from the OS.

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
Download and unzip the 64-bit Windows binaries:

https://datareel.com/downloads/encrypted_password_database/encrypted_password_database_2026_101_windows_binaries.zip

The "encrypted_password_database.exe" and "epdb_auth_tool.exe" are
standalone EXE files that can be run with no installer needed. They
need the Microsoft Visual C++ Redistributable (x64), which most
Windows systems already have:

https://aka.ms/vs/17/release/vc_redist.x64.exe

INSTALLING ON LINUX
------------------
The binaries for RHEL 8, 9 and 10 include an installer that installs
to your user account without root access. To install from the command
line:

cd ${HOME}/Downloads
wget https://datareel.com/downloads/encrypted_password_database/encrypted_password_database_2026_101_rhel_8_9_10_binaries.tar.gz
tar xvfz encrypted_password_database_2026_101_rhel_8_9_10_binaries.tar.gz
cd encrypted_password_database_2026_101_rhel_8_9_10_binaries
./rh_install.sh

The installer lists any system libraries that are missing. Most
desktop systems have them. To add them on RHEL 10:

sudo dnf install gtk3 pcre2-utf32 libXxf86vm libSM libICE

On RHEL 8 and 9:

sudo dnf install gtk3 webkit2gtk3 pcre2-utf32 libXxf86vm libSM libXtst

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

The Windows build requires wxWidgets version 3.2.4 or higher, built
for x64 in release mode (BUILD=release TARGET_CPU=X64).

For instructions on how to build wxWidgets for Windows please see the
following readme file: README_windows_wxwidgets.txt

For instructions on how to build Windows 11 or Windows Server
development VM please see the following readme file: README_windows_setup.txt 

To build the Windows executables download the latest release or clone
the GIT repo. In the Windows build example below this document will
assume we placed the code on a data drive in a git directory.  

On the data drive change your directory to the
encrypted_password_database build subdirectory:

> E:
> cd E:\git\encrypted_password_database

Setup your Visual Studio environment for a 64-bit x64 build:

> cd  E:\git\encrypted_password_database\env
> msvc64_vs2026.bat

Edit the msvc64.env file to setup the path to your 64-bit OpenSSL,
ZLIB, and wxWidgets builds (console\msvc\msvc64.env has the same
OpenSSL and ZLIB settings for the console program):

> notepad msvc64.env

# Setup path to ZLIB
ZCODE_DIR = ../../../../3plibs/zlib64-1.3.1

# Setup path to OPENSSL library
OPENSSL_DIR = ../../../../3plibs/openssl64

# Set path to wxWidgets library here
WXDIR = ../../../../3plibs/wxWidgets64-3.2.4

The makefiles use msvc64.env when the Visual Studio environment
targets x64. For a 32-bit x86 build for legacy systems run
msvc32_vs2026.bat instead, and the makefiles use msvc.env with the
32-bit OpenSSL, ZLIB and wxWidgets builds.

To build the Windows GUI:

> cd  E:\git\encrypted_password_database\gui\msvc

Run nmake to build the executable:
 
> nmake

To run the Windows program: 

> encrypted_password_database.exe

This is a standalone EXE file that can be run with no installer
needed (it needs the Visual C++ Redistributable).

To build the Windows console program:

> cd  E:\git\encrypted_password_database\console\msvc

Run nmake to build the executable:
 
> nmake


SOURCE CODE BUILD ON LINUX
--------------------------
Building the binaries from the source code repository will require you
to build wxWidgets version 3.2.4 or higher. To download and build the
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
> vi linux.env

WXSRCDIR = $(HOME)/3plibs/wxWidgets-3.2.4

Change the WXSRCDIR to your wxWidget build installation path. 

> cd ${HOME}/git/encrypted_password_database/gui/linux
RHEL 10 is the default build, the makefile is the same as
makefile_rhel10. Use makefile_rhel8 or makefile_rhel9 to build on RHEL 8
or RHEL 9. For RHEL 10:

> cat makefile_rhel10

all:
        make PROJECT=encrypted_password_database FINAL=1 64BITCFG=1 RHEL=10 LIBRARIES="-lXxf86vm -lSM -lICE" -f ../../env/linux.mak

install:
        ./install.sh

clean:
        make PROJECT=encrypted_password_database FINAL=1 64BITCFG=1 RHEL=10 -f ../../env/linux.mak clean

The extra LIBRARIES are needed when the wxWidgets build was made on
RHEL 8. You can also set the wxWidgets path on the command line, for
example: make WXSRCDIR=$HOME/3plibs/wxWidgets-3.2.4_rhel9 -f makefile_rhel9

Run the make to build the GUI

> make

Set up the build environment for the command line utility build:

> cd ${HOME}/git/encrypted_password_database/console/linux
The default makefile is for RHEL 10, use makefile_rhel8 or
makefile_rhel9 on RHEL 8 or RHEL 9. Smart card support is turned on in
every makefile, for example in makefile_rhel10:

SMARTCARD_DEMACS = -D__ENABLE_SMART_CARD__ -D__RHEL10__

Run the make to build the command line utility programs:

> make

To run the command line tests:

> ./run_cli_tests.sh

To run the smart card tests with a test card or a SoftHSM2 token, see
the usage at the top of run_smartcard_tests.sh. List the card without
a PIN first, then run the tests (they stop at the first PIN error):

> SC_LIST_ONLY=1 ./run_smartcard_tests.sh
> SC_PIN=pin SC_CERT=cert.pem SC_ID=03 ./run_smartcard_tests.sh

The wrong PIN and PIN flag tests (SC_TEST_WRONG_PIN=1) only run with a
SoftHSM2 token.

SMART CARDS AND DOD CAC CARDS
-----------------------------
Smart card access uses the fcrypt smart card code. Builds that use
OpenSSL 3 (RHEL 9, RHEL 10 and Windows) load the card's PKCS#11 module
directly, no OpenSSL pkcs11 engine (libp11) is used or needed. RHEL 8
builds still use the pkcs11 engine. The PKCS#11 module is:

  RHEL:    /usr/lib64/opensc-pkcs11.so (dnf install opensc pcsc-lite,
           systemctl enable --now pcscd)
  Windows: C:\Program Files\HID Global\ActivClient\acpkcs211.dll if
           ActivClient is installed, otherwise
           C:\Program Files\OpenSC Project\OpenSC\pkcs11\opensc-pkcs11.dll

Set a different module with --smartcard-provider=path in
epdb_auth_tool, or with SC_modulePath in the GUI configuration file
(encrypted_password_database.cfg in the program's home directory).

The cert ID can be a hex ID such as 03, or id_03, label_name,
slot_N-id_03, slot_N-label_name or a "pkcs11:id=%03" URI. List the
keys and certs on the card with the OpenSC pkcs11-tool:

> pkcs11-tool --list-objects
> pkcs11-tool --module "C:\Program Files\HID Global\ActivClient\acpkcs211.dll" --list-objects

A DOD CAC card holds PIV Authentication, Digital Signature and Key
Management (encryption) keys. Use the Key Management key, since DOD
can recover escrowed encryption keys when a card is replaced. With
OpenSC its ID is 03. ActivClient shows a long hex ID and a label such
as "Encryption - name", use that ID or label_Encryption - name. Keep a
password or another user on every database and add access for a new
card before the old card expires.

The card key must be RSA (CAC cards use RSA 2048). EC keys, such as
P-256 or P-384 keys on some PIV cards and YubiKeys, are refused.

PIN safeguards. CAC cards lock after 3 wrong PINs, so:
* The PIN is sent to the card at most once per database open, and
  after any failed login no more logins are made until the program is
  run again (the GUI must be restarted). Nothing retries a PIN: not a
  loop over database files, not a fallback, not a second key.
* The PIN is not sent when it is empty, too long (it is never cut
  short), has control characters or is outside the card's PIN length.
* The PIN is not sent when the card reports its PIN is locked. When
  the card reports one PIN try left or low PIN tries, the PIN is only
  sent after you confirm: the GUI asks (default No), epdb_auth_tool
  asks for low tries at a terminal and otherwise needs
  --smartcard-allow-pin-count-low or --smartcard-allow-final-try.
  Some PKCS#11 modules, possibly ActivClient, do not report the PIN
  tries, so no warning does not mean tries are left. Check the PIN
  tries left with the card software first.
* When the cert ID is not found on any card and more than one card is
  inserted, no card is guessed.
* After a wrong PIN the error says one PIN try was used and what the
  card reports about the tries left.

Add your card to a database you can open with a password:

> epdb_auth_tool --password --add-smartcard-cert --smartcard-cert-id=03 --add-username=myname mydb.ehd

Add another user's card from a cert they exported (PEM or DER .cer):

> epdb_auth_tool --password --add-smartcard-cert-file=user.cer --add-username=user mydb.ehd

Open the database with the card:

> epdb_auth_tool --smartcard --smartcard-username=myname --smartcard-cert-id=03 --db-list mydb.ehd

In the GUI use Users/Add Users and either an exported cert file or
"Or read the cert from the smart card in the reader" with the cert ID,
then open the database with the Use Smart Card fields of the open
dialog.
