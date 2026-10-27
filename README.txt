Encrypted Password Database

Release 2026.103

Supports Windows 11 and Windows Server 2022 or later (64-bit)
Supports Red Hat Enterprise Linux 8, 9, and 10

The Encrypted Password Database is an enterprise level application for
storing authentication information and automating application password
management.

RELEASE 2026.103: KEY MANAGEMENT, RECORDS, LOGS AND AUTOMATION
-------------------------------------------------------------
* --remove-user=name removes an RSA key or smart card user (GUI: Users
  > Remove Users). The command line rewrites the database atomically:
  a copy is changed, re-sealed and checked, then renamed over the
  original, and a backup copy is kept (none with self-destruct on).
  The last RSA or smart card user is only removed after opening with
  the password or key file, since a database made with RSA keys or
  smart cards only has a random password nobody knows. Removing a user
  does not revoke a key the user already recovered: run --rekey after.
* --rekey gives the database a new master key: a new salt and database
  ID, every field encrypted again, every kept RSA and smart card user
  rewrapped with their public key (no card login). Nothing is dropped
  silently: a user or password without a choice in the rekey input
  stops the rekey with a list of what is missing. The new database
  must open and hold the same records before it replaces the original.
  See REKEY below.
* --add-record, --update-record and --delete-record. The record name
  may be given on the command line, the field values are read from
  stdin, --record-fd or --record-file as name=value lines or JSON, never
  from argv. See RECORDS FROM THE COMMAND LINE below.
* Access logs: a per-user log (--access-log, GUI Security > Access Log,
  off by default) and an encrypted in-file log of the last 160 entries
  (--access-log-in-file=on, GUI Security > In-File Access Log), both
  tamper evident, --show-access-log and --verify-access-log. See ACCESS
  LOG below.
* --check-version reports v1 or v2 for each database without a
  credential or smart card and exits 5 if any is v1. Opening a v1
  database warns on stderr (GUI: a dialog once per file per session)
  and names the upgrade command.
* Optional self-destruct after too many failed opens (off by default),
  see SELF-DESTRUCT below.
* GUI undo and redo of record changes: Edit > Undo (Ctrl+Z) and Redo
  (Ctrl+Y or Ctrl+Shift+Z) for adds, changes, deletes, cut and paste,
  grid edits, import and merge (each one step), up to 100 steps, in
  memory only, wiped when the database closes (lock, auto-lock, close,
  exit). Each undo is saved at once and logged as an undo. Key
  management (add or remove users, upgrade, self-destruct settings)
  cannot be undone and asks before it clears the history. Clear
  Clipboard moved from Ctrl+Z to Ctrl+L.
* Command line versions of GUI features: --show-record, --get-field,
  --db-info, --export (GUI CSV format or JSON), --import (JSON or CSV,
  all or nothing), --merge-from, --change-password and
  --generate-password, plus the automation options, see AUTOMATION AND
  PRODUCTION USE below.
* The record, undo, log, rekey and self-destruct logic is in
  src/epdb_core.cpp, src/db_auth.cpp and fcrypt's fckeymgmt.cpp, which
  the GUI and epdb_auth_tool share; the GUI only wires menus and
  dialogs to it.
* GUI fixes: Remove Users re-seals a v2 database (the database
  integrity check failed after it), and new config files get their
  numeric settings (they were written as spaces).
* Built with fcrypt 2026.106 and DataReel 6.1. The database format is
  unchanged: the new settings use bytes 2026.102 and older never read
  (see db_auth.h) and 2026.102 opens databases written by this release.

BEHAVIOR CHANGES IN RELEASE 2026.103
------------------------------------
* epdb_auth_tool exit codes: 2 when the credential does not open the
  database and 3 when the database, record or user is not found, where
  2026.102 exited 1 for every error. Scripts that test for exit code 1
  should test for non-zero. All codes are listed under AUTOMATION AND
  PRODUCTION USE.

RECORDS FROM THE COMMAND LINE
-----------------------------
Field values are read from stdin, --record-fd=n or --record-file=file
(a file only you can read), never from the command line:

$ printf 'username=alice\npassword=S3cret\nnotes=line 1\\nline 2\n' | ./epdb_auth_tool --password-file=pw --add-record="My Bank" db.ehd
$ printf 'password=N3w\n' | ./epdb_auth_tool --password-file=pw --update-record="My Bank" db.ehd
$ ./epdb_auth_tool --password-file=pw --delete-record="My Bank" --yes db.ehd

name=value format: one field per line. Field names are title, username,
password, previous-password, security-questions, account-information,
website-link and notes (also field1 to field8, or the database's own
column name; case, spaces and _ do not matter). In values \n is a
newline, \t a tab and \\ a backslash. Blank lines and lines starting
with # are skipped. An update changes only the fields given, an empty
value clears a field and title= renames the record. A delete asks at a
terminal unless --yes is given. Each works with every way of opening
the database (password, key file, RSA key, smart card).

JSON format (also the --json output, so an export imports back exactly):

  {"title": "My Bank", "username": "alice", "password": "S3cret",
   "previous-password": "", "security-questions": "",
   "account-information": "", "website-link": "", "notes": ""}

Every key is optional except title; values are strings. --add-record
and --update-record take one object, --import takes an object or an
array and applies it as one atomic change (all or nothing, --dry-run
to check it). JSON is detected by a leading { or [, or with
--input-format=json; --import also reads the GUI's CSV format. Unknown
keys, duplicate keys, values that are not strings and input over 1 MB
are refused with the position of the problem, and nothing is changed.
JSON is parsed with cJSON 1.7.18 (fcrypt/3plibs/cjson, MIT license).

REKEY
-----
--rekey reads name=value lines from stdin, --rekey-fd or --rekey-file:

  password=<current password>     Keep the database password (implied when
                                  opening with --password-file/-fd or --key)
  new-password=<password>         Give the database a new password
  drop-password=yes               Random secret nobody knows: only the RSA
                                  key and smart card users can open it
  rsa:<user>=<public key file>    Keep and rewrap an RSA user
  smartcard:<user>=<cert file>    Keep and rewrap a smart card user (PEM or DER)
  smartcard:<user>=card           Read the cert from the card in the reader, no PIN
  drop-rsa:<user>=yes             Remove an RSA user
  drop-smartcard:<user>=yes       Remove a smart card user

--rekey-new-password-file=file or --rekey-new-password-fd=n makes a
random new database password (64 hex digits) and writes it there first;
it is never printed. --change-password is a rekey with a new password
from --new-password-fd or --new-password-file (it needs rsa: and
smartcard: lines for the database's RSA and smart card users, since
their key blocks are rewrapped).

The database password derives the master key (PBKDF2 with the
database's salt), there is no separate password slot. Keeping the
password still gives a new master key because the salt is new, which
revokes users who only recovered the old master key. Someone who knows
the password can still open the database: use new-password to revoke
them too. A rekey that would leave nothing to open the database is
refused, and the original files are kept as a backup
(name.rekey-backup-YYYYMMDD-HHMMSS.ehd/.ehx) that opens with the old
credentials.

ACCESS LOG
----------
Per-user log, off by default: epdb_auth_tool --access-log[=file] or the
GUI's Security > Access Log (AccessLog = 1 and AccessLogFile in the
config file, .cfg on Linux, .ini on Windows). The default file is
~/.encrypted_password_database/logs/access.log
(%USERPROFILE%\.encrypted_password_database\logs\access.log on
Windows), directory mode 700, file mode 600. One file is shared by
every database: each entry has the database path and ID. Format, one
line per entry, tab separated:

  FCLOG1 time= user= host= pid= tool= file= id= auth= slot= action=
  record= result= detail= mac=

time is UTC; auth is password, key-file, rsa-key or smartcard; action
is open, add-record, update-record, delete-record, add-user,
remove-user, rekey, upgrade, undo, redo, import, merge, export,
show-record, get-field, settings, access-log-on, access-log-off or
self-destruct; record is the record name; result is ok or fail. No
password, PIN or record value is ever logged. mac is an HMAC-SHA256
chain per database keyed from the master key (fcrypt include/
fckeymgmt.h), so --verify-access-log finds changed, removed, reordered
or inserted entries; entries of failed opens are covered by the next
tagged entry; removing whole entries from the end cannot be detected.
After a rekey the entries are checked with the old key and tagged again
with the new one. If the log cannot be written the program warns once
and goes on.

In-file log: --access-log-in-file=on|off (GUI Security > In-File Access
Log) keeps the last 160 entries in the database file, encrypted and
authenticated with keys derived from the master key: time, action,
auth method, result and a keyed hash of the user name, no host or name
in the clear. When it is on and the database cannot be written
(read-only), the entry goes to the per-user log instead, marked "in-file
log unavailable (read-only)". An open never fails because of logging.
After a rekey the entries are re-encrypted under the new key.
--show-access-log lists both logs, --verify-access-log checks both.

SELF-DESTRUCT
-------------
--self-destruct=on --self-destruct-attempts=n (1 to 99, default 5), or
the GUI's Security > Self-Destruct Settings. Off by default, per
database, v2 only. Each failed open (wrong password, key file, RSA key
or passphrase, a wrong smart card PIN, a card key that does not match)
is counted in the database file before the error is shown: "N of M
attempts used, the records will be destroyed after M", with a stronger
warning when one attempt is left. A good open resets the count. A PIN
that is not sent (the PIN safeguards) is not counted, and the PIN is
never retried. At the limit the key blocks, the salt and keys of the v2
header, the config and every record in both files are overwritten with
random data, the file is kept and marked destroyed, and --check-version
and every open report it (exit code 6).

With self-destruct on, a database that cannot be written (read-only
file or media) is refused before any credential is tried, so a
read-only copy cannot be used to skip the counter (copy it to writable
storage), and --remove-user and --rekey keep no backup copy.

Self-destruct only stops guesses made through epdb_auth_tool and the
GUI. Anyone with a copy of the file can attack it offline, or reset the
counter by editing the file (the counter cannot be protected without
the key). A destroyed database cannot be recovered except from your own
backups.

AUTOMATION AND PRODUCTION USE
-----------------------------
* --non-interactive (or --batch): never prompt. A missing password,
  passphrase, PIN or input is an error. --yes confirms a delete.
* --quiet: only errors and the requested output.
* --json: --db-list, --db-find-key, --show-record, --db-info,
  --list-users, --check-version, --export=json, --import and
  --merge-from.
* --get-field=record:field writes only that value to stdout (or
  --output-file), with no trailing newline; the value is never logged.
* --dry-run: --remove-user, --rekey, --add-record, --update-record,
  --delete-record, --import and --merge-from are done and checked on a
  copy that is then removed.
* --lock-timeout=seconds: every write takes an advisory lock on
  db.ehd.lock (flock on Linux, LockFileEx on Windows) and waits this
  long (default 10) for another writer, then exits 4. Record changes,
  imports and merges are atomic (a checked copy renamed over the
  database).
* --output-file=file writes the requested output to a new file only
  you can read; clear text records written to a terminal get a warning.
* Errors go to stderr and requested output to stdout.
* Exit codes: 0 ok, 1 usage or other error, 2 the credential did not
  open the database, 3 database, record or user not found, 4 locked by
  another writer, 5 --check-version found a v1 database, 6 destroyed by
  self-destruct, 7 read or write error (also a read-only database with
  self-destruct on).

Using AWS KMS and Secrets Manager

There is no AWS code in this release. Pipe the secret into a -fd option.
Linux (bash):

$ aws kms decrypt --ciphertext-blob fileb://pw.enc --query Plaintext --output text | base64 -d | ./epdb_auth_tool --password-fd=0 --non-interactive --db-list db.ehd
$ aws secretsmanager get-secret-value --secret-id epdb/pw --query SecretString --output text | ./epdb_auth_tool --password-fd=0 --non-interactive --get-field="My Bank:password" db.ehd
$ aws secretsmanager get-secret-value --secret-id epdb/rsa-pass --query SecretString --output text | ./epdb_auth_tool --rsa-key=$HOME/.keys/id_rsa.pem --rsa-key-username=$USER --rsa-key-passphrase-fd=0 --non-interactive --db-list db.ehd

Windows (PowerShell):

PS> aws secretsmanager get-secret-value --secret-id epdb/pw --query SecretString --output text | .\epdb_auth_tool.exe --password-fd=0 --non-interactive --db-list db.ehd
PS> [Text.Encoding]::UTF8.GetString([Convert]::FromBase64String((aws kms decrypt --ciphertext-blob fileb://pw.enc --query Plaintext --output text))) | .\epdb_auth_tool.exe --password-fd=0 --non-interactive --db-list db.ehd

The secret never lands in argv, shell history or a temp file this way.
When stdin carries the password, give record input with --record-fd or
--record-file. AWS CloudHSM's PKCS#11 library may work with
--smartcard-provider, but it is untested.

RELEASE 2026.102: SMART CARDS ON RHEL 10 AND WINDOWS
----------------------------------------------------
Release 2026.102 turns on smart card support in the RHEL 10 and
64-bit Windows builds. It was tested with a DoD CAC through
ActivClient (Windows) and OpenSC (RHEL 10 and Windows), and with a
YubiKey PIV key through OpenSC. A database given smart card access on
Linux opens on Windows with the same card.

* Smart cards use the card's PKCS#11 module directly (fcrypt
  2026.105), no OpenSSL pkcs11 engine is needed. Windows uses the
  ActivClient module when it is installed, otherwise OpenSC. RHEL uses
  OpenSC. See SMART CARDS AND DOD CAC CARDS below.
* PIN safeguards so testing or a typing mistake does not lock a card:
  the PIN is sent once per open and never retried, no card login is
  made after a failed one until the program is restarted, an empty,
  too long or control character PIN is never sent, and a locked PIN,
  one try left or low tries reported by the card stop the login (the
  GUI asks, default No; epdb_auth_tool has
  --smartcard-allow-pin-count-low and --smartcard-allow-final-try).
* --smartcard-slot=N (GUI: Smart Card Slot, SC_slot) selects the card
  when more than one is inserted. A cert ID found on more than one
  card, or on no card, is refused and no PIN is sent.
* epdb_auth_tool --add-smartcard-cert reads the cert from the card in
  the reader, --add-smartcard-cert-file takes an exported PEM or DER
  (.cer) cert. GUI Add Users can read the cert from the card.
* smartcard_list.bat (Windows) and smartcard_list.sh (Linux) list the
  cards, PIN flags, slots and cert IDs without a PIN.
* EC card keys are refused with a clear message, the database key is
  wrapped with RSA (DoD CACs use RSA 2048).
* RHEL 10 is the default Linux build (make), use makefile_rhel8 or
  makefile_rhel9 on older releases.
* The New Database, Open and Add Users dialogs use two columns so they
  fit smaller screens, such as a 958 pixel tall remote desktop.
* Built with fcrypt 2026.105 and DataReel 6.1. The database format is
  unchanged from 2026.101.

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
* Smart card support was not in the 2026.101 RHEL 10 and Windows
  binaries, it is in 2026.102.
* 2026.101 was built with fcrypt 2026.104 and DataReel 6.1. Windows builds use
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

https://datareel.com/downloads/encrypted_password_database/encrypted_password_database_2026_102_windows_binaries.zip

The "encrypted_password_database.exe" and "epdb_auth_tool.exe" are
standalone EXE files that can be run with no installer needed. They
need the Microsoft Visual C++ Redistributable (x64), which most
Windows systems already have:

https://aka.ms/vs/17/release/vc_redist.x64.exe

smartcard_list.bat lists the smart cards and their cert IDs without a
PIN, see SMART CARDS AND DOD CAC CARDS.

INSTALLING ON LINUX
------------------
The binaries for RHEL 8, 9 and 10 include an installer that installs
to your user account without root access. To install from the command
line:

cd ${HOME}/Downloads
wget https://datareel.com/downloads/encrypted_password_database/encrypted_password_database_2026_102_rhel_8_9_10_binaries.tar.gz
tar xvfz encrypted_password_database_2026_102_rhel_8_9_10_binaries.tar.gz
cd encrypted_password_database_2026_102_rhel_8_9_10_binaries
./rh_install.sh

The installer lists any system libraries that are missing. Most
desktop systems have them. To add them on RHEL 10:

sudo dnf install gtk3 pcre2-utf32 libXxf86vm libSM libICE

On RHEL 8 and 9:

sudo dnf install gtk3 webkit2gtk3 pcre2-utf32 libXxf86vm libSM libXtst

For smart cards (DoD CAC, PIV, YubiKey) install OpenSC and start pcscd:

sudo dnf install opensc pcsc-lite pcsc-lite-ccid
sudo systemctl enable --now pcscd

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
(.encrypted_password_database/encrypted_password_database.cfg in your
home directory, %USERPROFILE% on Windows). A YubiKey is often not seen
by ActivClient, use the OpenSC module for it.

Finding the slot and the cert ID without a PIN:

  Windows: smartcard_list.bat (in the Windows package) lists every
           reader slot, the card's PIN flags and every cert and key
           with its ID, label and type, through ActivClient and
           OpenSC. It writes %TEMP%\smartcard_list.log.
  Linux:   smartcard_list.sh [module] (installed in
           ~/.encrypted_password_database/bin by rh_install.sh)

Each card is shown as "Slot n (0xID)", use the 0xID with
--smartcard-slot when more than one card is inserted. Use the
encryption cert:

  Module       Card                 Cert ID
  OpenSC       CAC, PIV, YubiKey    03 ("Certificate for Key Management",
                                    YubiKey PIV slot 9D)
  ActivClient  CAC                  the long hex ID of "Encryption - NAME",
                                    or label_Encryption - NAME

Do not use 01 (PIV Authentication), 02 (Digital Signature) or 04
(Card Authentication). The cert ID can also be id_03, label_name,
slot_N-id_03, slot_N-label_name or a "pkcs11:id=%03" URI. Quote a
label with spaces on the command line:

> epdb_auth_tool --smartcard --smartcard-username=myname "--smartcard-cert-id=label_Encryption - LAST.FIRST.M.1234567890" --db-list mydb.ehd

Remote desktop: with smart card redirection on in the RDP client
(Local Resources, More, Smart cards), programs in the RDP session see
only the client computer's cards, not the readers of the remote
computer or VM. Turn it off, or use the computer's console.

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
* With more than one card inserted (for example a CAC and a YubiKey,
  which both have ID 03 with OpenSC), a cert ID found on more than one
  card, or on no card, is refused and no PIN is sent. Select the card
  with --smartcard-slot=N in epdb_auth_tool, or the Smart Card Slot
  field of the GUI open dialog (saved as SC_slot in the configuration
  file). N is the slot ID shown in parentheses by
  pkcs11-tool --list-token-slots, decimal or 0x hex. Only that slot is
  used.
* After a wrong PIN the error says one PIN try was used and what the
  card reports about the tries left.

Add your card to a database you can open with a password:

> epdb_auth_tool --password --add-smartcard-cert --smartcard-cert-id=03 --add-username=myname mydb.ehd

Add another user's card from a cert they exported (PEM or DER .cer):

> epdb_auth_tool --password --add-smartcard-cert-file=user.cer --add-username=user mydb.ehd

Open the database with the card:

> epdb_auth_tool --smartcard --smartcard-username=myname --smartcard-cert-id=03 --db-list mydb.ehd

In the GUI, File/New with "Use Smart Card" (and a password) creates a
database with smart card access. Users/Add Users adds a card from an
exported cert file or with "Or read the cert from the smart card in
the reader" and the cert ID. Open the database with the Use Smart Card
fields of the open dialog: username, PIN, cert ID and, with more than
one card inserted, the Smart Card Slot. Creating a database and adding
a card do not use the PIN.
