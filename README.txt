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
* epdb_auth_tool writes requested output (--get-field, --export,
  --show-record) in binary mode on Windows, so the bytes are the same
  as on Linux.
* Built with fcrypt 2026.106 and DataReel 6.1. The database format is
  unchanged: the new settings use bytes 2026.102 and older never read
  (see db_auth.h) and 2026.102 opens databases written by this release.

THE GUI (DEVELOPMENT BRANCH gui-phase3, NOT YET RELEASED)
---------------------------------------------------------
The GUI was redesigned. Every earlier menu item and accelerator is kept.
* Main window: menus File, Edit, Records (was Database), View, Users,
  Security, Tools and Help. The toolbar has New, Open, Lock, Add, Edit,
  Delete, Copy user, Copy password, Open link and Print. Each open
  database is a tab with a search box above the records (all fields but
  the passwords, or one field) and a record details pane right of the
  grid with copy buttons, password show and hide, and an open button for
  the website link. Copied values are cleared from the clipboard after
  the time set in Preferences (30 seconds by default). The status bar
  shows how the database was unlocked (for example "Unlocked with smart
  card CAC (slot 1)"), the number of records, the format and the
  auto-lock countdown.
* Unlock Database (File > Open, Ctrl+O): the database file (with the
  recent files) and one unlock method, Password, Key file, Private key
  or Smart card. Only that method's fields are shown, so methods cannot
  be mixed. The method and names used the last time are remembered for
  each file in recent_files.txt next to the config file (no password,
  key, PIN or passphrase is kept).
* Smart cards: the Unlock, Create Database and Add User dialogs list the
  cards in the readers (reader slot, cert ID, key type and size and the
  PIN status: Ready, Few tries left, One PIN try left, PIN locked, No
  user in this database) without a PIN or a card login, and find a card
  that is inserted while the dialog is open. A card is chosen for you
  only when one card can open the database: the card and cert ID used
  the last time, or the only RSA key whose size matches a smart card
  user of the database. With two cards that could match, nothing is
  chosen and you pick the card. Locked cards and EC keys are shown but
  cannot be chosen. The PIN safeguards are unchanged: the PIN is sent at
  most once, never empty or cut short, not at all after a failed card
  login until the program is restarted, and when the card is low on
  tries or has one try left a warning comes first with Cancel as the
  default (the PIN is not sent unless you choose Send the PIN).
  Planned for the next release (DBv3), two database format changes:
  - each smart card user gets the SHA-256 of its certificate's public
    key (SPKI), so the exact card is picked with no PIN (today the card
    is matched as above);
  - longer multi-line record fields (today 255 bytes each, at least a
    longer Notes field), carried through CSV import and export,
    printing and epdb_auth_tool.
  This release does not change the database format.
* Create Database: name and folder (the full path is shown), and the
  ways it can be unlocked: master password (with a strength meter and
  Generate), or a key file, RSA public keys and smart cards (no PIN is
  needed to add a card).
* Records: Add, Edit and Display share one two-column form. Security
  questions, Account information and Notes are multi-line. Every field
  holds up to 255 bytes (newlines count), shown by a counter. Editing a
  password moves the old one to Previous password. CSV export writes a
  line break in a field as a space and printing writes it as " / ", as
  every record is one line.
* Users > Add User (RSA public key, the smart card in the reader or an
  exported cert file) and Remove User (pick the user from a list).
* Edit > Preferences: File Locations, Security (encryption strength,
  clipboard clear time, auto-lock minutes, access log) and Appearance.
* Tools > Command Builder: shows the epdb_auth_tool command for an
  operation (records, export and import, merge, users, rekey, password
  change and generation, version check, access logs) for bash,
  PowerShell and cmd.exe as you fill in the options, with Copy, Save as
  Script and Test with --dry-run. It never puts a password, PIN or
  passphrase on the command line: they are read with the -file= or
  -fd= options from a file only you can read or a pipe (for example
  from AWS Secrets Manager), and record values from --record-file or
  stdin. Also on the details pane: Copy as command.
* Keyboard: Ctrl+N new, Ctrl+O open, Ctrl+Shift+L lock, Ctrl+F search,
  Ctrl+Z undo, Ctrl+Y or Ctrl+Shift+Z redo, Ctrl+C/X/V copy, cut and
  paste a cell, Ctrl+L clear the clipboard, Ctrl+Shift+U and
  Ctrl+Shift+C copy the username and password, Ctrl+Shift+O open the
  link, Alt+F find, Alt+A add, Alt+C edit, Alt+D delete, Ctrl+P print,
  F1 about. The window X and Esc cancel a dialog, they never save.
* High DPI: the icons are SVGs drawn at the screen scale, every dialog
  uses sizers and the system font, window and column sizes are kept in
  DIPs, and the Windows program is per-monitor DPI aware.
* Built with wxWidgets 3.3.4 (static) on RHEL 10 and Windows.

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
Windows installer (64-bit, administrator install):

https://datareel.com/downloads/encrypted_password_database/encrypted_password_database_2026_103_windows_setup.exe

The setup installs the GUI, epdb_auth_tool.exe, smartcard_list.bat and
the README files to C:\Program Files\DataReel\Encrypted Password
Database, adds Start Menu shortcuts, installs the Microsoft Visual C++
x64 runtime if it is missing, and can add the program folder to the
system PATH (optional task) and a desktop shortcut. Silent install:

encrypted_password_database_2026_103_windows_setup.exe /VERYSILENT /SUPPRESSMSGBOXES /TASKS="addpath"

Upgrading: releases before 2026.102 were zip files with no installer.
Install the setup, then delete the old unzipped EXE files (or keep
them, they are not used by the installed program). Your databases,
keys and settings are in %USERPROFILE%\.encrypted_password_database
and are used by the installed program as they are. The setup and the
uninstaller never change or delete that folder. Later setups upgrade
the installed program in place. The installer is not code signed, so
Windows SmartScreen may warn about an unknown publisher: choose More
info, then Run anyway.

Or download and unzip the 64-bit Windows binaries:

https://datareel.com/downloads/encrypted_password_database/encrypted_password_database_2026_103_windows_binaries.zip

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
wget https://datareel.com/downloads/encrypted_password_database/encrypted_password_database_2026_103_rhel_8_9_10_binaries.tar.gz
tar xvfz encrypted_password_database_2026_103_rhel_8_9_10_binaries.tar.gz
cd encrypted_password_database_2026_103_rhel_8_9_10_binaries
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

The install script also installs the program icon at every size in
${HOME}/.local/share/icons/hicolor (16 to 256 pixels and a scalable SVG).
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

The Windows build requires wxWidgets 3.3.4, built as static libraries
with the dynamic runtime (SHARED=0 RUNTIME_LIBS=dynamic), x64 in
3plibs\wxWidgets64-3.3.4 and x86 in 3plibs\wxWidgets32-3.3.4. The older
wxWidgets 3.2.4 builds still work with nmake WXVER=324.

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

# wxWidgets 3.3.4 by default, nmake WXVER=324 for 3.2.4
WXDIR = ../../../../3plibs/wxWidgets64-3.3.4

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
needed (it needs the Visual C++ Redistributable). The program icon,
version information and the manifest (version 6 common controls and
per-monitor DPI awareness) are built into the exe from
gui\encrypted_password_database.rc.

To build the Windows console program:

> cd  E:\git\encrypted_password_database\console\msvc

Run nmake to build the executable:
 
> nmake


SOURCE CODE BUILD ON LINUX
--------------------------
The RHEL 10 GUI is built with a static wxWidgets 3.3.4 (gtk3, -fPIC so
the program is a PIE). Build it once from the wxWidgets source
(https://www.wxwidgets.org/, wxWidgets-3.3.4.tar.bz2):

> mkdir -p ${HOME}/3plibs/build && cd ${HOME}/3plibs/build
> tar xjf wxWidgets-3.3.4.tar.bz2
> mkdir wx334_epdb_gtk3_static && cd wx334_epdb_gtk3_static
> ../wxWidgets-3.3.4/configure --prefix=${HOME}/3plibs/wxWidgets-3.3.4_epdb_rhel10 \
    --disable-shared --with-gtk=3 --disable-webview --disable-mediactrl \
    --without-opengl --without-libnotify --without-sdl \
    --with-libpng=builtin --with-libjpeg=builtin --with-libtiff=builtin \
    --with-regex=builtin --with-expat=builtin --with-zlib=builtin \
    CFLAGS="-fPIC -O2" CXXFLAGS="-fPIC -O2"
> make -j$(nproc) && make install

It needs gtk3-devel (and the X11 and webp devel packages wx-config
lists). The builds used for EPDB are archived, see WXWIDGETS AND OPENSSL
BUILD ARCHIVES below.

To build the latest release of the Encrypted Password Database:

> mkdir -pv ${HOME}/git
> cd ${HOME}/git
> wget https://datareel.com/downloads/encrypted_password_database/encrypted_password_database-<version>.tar.gz
> tar xzf encrypted_password_database-<version>.tar.gz
> mv encrypted_password_database-<version> encrypted_password_database
> cd encrypted_password_database

The RHEL 10 build takes the wxWidgets compile and link flags from
${HOME}/3plibs/wxWidgets-3.3.4_epdb_rhel10/bin/wx-config
(env/wx3.3.4_rhel10.env). Another wxWidgets 3.3 build can be used with
make WX_CONFIG=/path/to/bin/wx-config, and make WXVER=324 builds with
wxWidgets 3.2.4 in WXSRCDIR = $(HOME)/3plibs/wxWidgets-3.2.4 (env/linux.env),
as the RHEL 8 and RHEL 9 builds still do.

> cd ${HOME}/git/encrypted_password_database/gui/linux
RHEL 10 is the default build, the makefile is the same as
makefile_rhel10. Use makefile_rhel8 or makefile_rhel9 to build on RHEL 8
or RHEL 9 (wxWidgets 3.2.4). For RHEL 10:

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

The GUI icons are SVGs built into the program. After changing
gui/icons/svg/icons.txt run python3 gui/tools/make_icons.py, which also
makes the Windows .ico and the Linux icons (needs rsvg-convert and
python3-pillow). The command builder tests are in gui/tests:

> cd ${HOME}/git/encrypted_password_database/gui/tests
> g++ -o cmdbuild_test cmdbuild_test.cpp ../epdb_cmdbuild.cpp && ./cmdbuild_test

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

WXWIDGETS AND OPENSSL BUILD ARCHIVES
------------------------------------
The third party builds EPDB is built with are kept in 3plibs, with
archive copies (README, sha256 file, build commands and logs, no
intermediate .obj files) on the AWS server:
* wxWidgets 3.3.4: RHEL 10 static gtk3, Windows x64 and x86 static /MD
  (VS 2026): ~/3plibs/wxwindows/builds on the AWS server (also
  ~/3plibs/archives on the build workstation).
  wxWidgets-3.3.4_epdb_rhel10_static_gtk3_x86_64_2026-10-27.tar.gz
  wxWidgets-3.3.4_epdb_win64_vc_x64_lib_MD_vs2026_2026-10-27.tar.gz
  wxWidgets-3.3.4_epdb_win32_vc_lib_MD_vs2026_2026-10-27.tar.gz
* OpenSSL 3.5.9 for Windows, x64 and x86, static, FIPS enabled:
  ~/3plibs/openssl/builds on the AWS server.
  openssl-3.5.9_win64_VC-WIN64A_static_MT-Zl_fips_vs2026_2026-10-27.tar.gz
  openssl-3.5.9_win32_VC-WIN32_static_MT-Zl_fips_vs2026_2026-10-27.tar.gz
Each folder has a README with the build commands and how the env files
point to the builds (env/wx3.3.4_rhel10.env, env/msvc64.env and
env/msvc.env).

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
home directory on Linux, %USERPROFILE%\.encrypted_password_database\
encrypted_password_database.ini on Windows). An existing configuration
file is kept as it is: an SC_modulePath that no longer exists falls
back to the default module, SC_enginePath is ignored by OpenSSL 3
builds, and SC_cert_id is only the default shown in the dialogs. A YubiKey is often not seen
by ActivClient, use the OpenSC module for it.

Finding the slot and the cert ID without a PIN:

  GUI:     the Unlock, Create Database and Add User dialogs list the
           cards, slots, cert IDs, key sizes and PIN status themselves
           (no PIN, no card login). The PKCS#11 module is set under
           Unlock > Smart card > Advanced.
  fcrypt:  fcrypt --smartcard-list [--smartcard-provider=module] lists
           the same without a login, --smartcard-match=cert.pem shows
           which card and cert ID hold a cert.

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

BUILDING THE WINDOWS INSTALLER
------------------------------
gui\msvc\installer\epdb.iss (Inno Setup 6) packs the files of the
Windows release zip into encrypted_password_database_<v_v>_windows_setup.exe.
Build the release zip first, unzip it, then:

> cd gui\msvc\installer
> build_installer.bat <unzipped zip folder> "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Redist\MSVC\14.52.37025\vc_redist.x64.exe"

The setup is written to gui\msvc\installer\output. Keep the AppId in
epdb.iss for every release so new setups upgrade in place.
