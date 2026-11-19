# Encrypted Password Database (EPDB): AI-assisted install, build, upgrade and support

This file is for AI-assisted work on the Encrypted Password Database
(EPDB): it tells an AI agent (Gemini CLI, Claude Code, GitHub Copilot,
Codex and others) how to assist a site's own administrators and
developers to install EPDB, build it from source, upgrade older releases
to the current release, 2026.107, maintain a fork, and troubleshoot. The
agent assists; it does not replace the administrators or DataReel
support, and it never acts autonomously (section 0). The people review
and approve every step, and problems the agent cannot solve go to
DataReel (section 8.4). `GEMINI.md` and `CLAUDE.md` in this folder only
import this file.

The full documentation is `README.txt` (release notes, the command line
tool, the database format) and the help pages at
https://datareel.com/encrypted_password_database/help/. This file points
to them instead of repeating them. When this file and the README or the
code disagree, the README and the code win: say so to the person you work
for.

## Before you use AI assistance

**Review all of your agency's or company's AI usage policies, and get formal
written approval from your management team, in the form of a business use
case and justification, before you use any AI assistance with EPDB.** AI
desktops and command line tools may be used with this guide only in strict
compliance with those policies and that approval.

- **Use only approved enterprise AI models.** An AI agent working with EPDB
  may process potentially sensitive data: host names, user names, file
  paths, command output and logs. Use only your agency's or company's
  enterprise AI models that have built-in safeguards against disclosing
  information and that guarantee your internal information will not be
  used to train public models.
- **Warning: public or free AI tools, open source AI models and self-built
  AI models** may expose your information to public learning models or to
  other parties, depending on how the model is hosted, configured and
  licensed. Use them only after a comprehensive review of what information
  might be exposed and what the potential risks are.
- The entire risk of AI-assisted use rests with the user. DataReel cannot
  be held accountable for actions taken, commands run or other outcomes of
  AI assistance (see https://datareel.com/terms.php).
- **No endorsement.** Any mention of a specific AI tool, model or product
  in this guide, such as Gemini, Gemini CLI, Claude Code or GitHub Copilot,
  is a reference only. It is not an endorsement or a recommendation to use
  that product. Product names are trademarks of their respective owners.

**For the agent:** at the start of a session, ask the person to confirm
that this AI-assisted use is approved under their organization's AI usage
policy and that you are an approved enterprise AI model. If they cannot
confirm it, stop and do not ask for or read any data from their systems.

## 0. Human in the loop: the agent never acts on its own

AI-assisted means the agent assists a person; it never acts
autonomously. Every change to a system or a database is made by a person
or explicitly approved by one, one step at a time.

- **Advisory mode is the default.** The agent explains the next step and
  prints the exact command and the computer it is for. The person runs it
  and pastes back only output that holds no secret (rule 1). Use this mode
  unless the organization allows the agent to run tools.
- **If tool execution is allowed,** each command still needs its own
  explicit approval before it runs. Never use auto-approve modes: no
  Gemini CLI `--yolo` or auto-accept, no Claude Code permission bypass
  (`--dangerously-skip-permissions`, bypass mode), no auto-run in other
  agents. Commands that open a database are run by the person, never by
  the agent (rule 1).
- **No unattended work:** no scheduled, background, batch or scripted
  runs of changing commands, and no loops over databases or computers.
  One database and one computer per approval.
- **Read-only checks first** (section 8.1); they are shown and approved
  the same way.

**Works with strict AI policies.** These files contain no code that runs
by itself: `AGENTS.md`, `GEMINI.md` and `CLAUDE.md` are text. Everything in
this guide works in advisory mode, with no tool execution by the agent.

## 1. Rules for agents (read these first)

An EPDB database holds an organization's passwords. A wrong step can lose
every record or expose them. Follow these rules on every task:

1. **Never see a secret.** Never ask for, read, store or repeat a database
   password, key file, RSA private key or passphrase, ML-KEM private key
   or passphrase, smart card PIN, or any record value. Do not read `.ehd`
   or `.ehx` files, key files, PIN or password files, exported CSV or
   JSON, or backups. Public keys (`*.pub.pem`, `rsa_pub.pem`) and certs
   are not secret. Commands that print
   record values (`--db-list`, `--db-find-key`, `--show-record`,
   `--get-field`, `--export`, `--db-config`) are run by the person, and
   their output is never pasted back. Ask for the exit code and the error
   text only.
2. **Show, then the person runs or approves it.** Before any command that
   changes a computer or a database (installers, `rh_install.sh`,
   `--add-record`, `--update-record`, `--delete-record`, `--import`,
   `--merge-from`, `--remove-user`, `--rekey`, `--upgrade-db`,
   `--rebuild-index`, `--reseal-db`, self-destruct and access log
   settings), print the command and the computer. In advisory mode the
   person runs it; otherwise wait for an explicit yes for that one command
   (section 0).
3. **Back up before changing.** Before any database change, the person
   copies the `.ehd` and `.ehx` files to a dated backup folder only they
   can read. Never delete a database, a backup or a `.jnl`,
   `.change.ehd` or `.lock` file; move aside only with the person's OK.
4. **Use `--dry-run` first** for `--remove-user`, `--rekey`,
   `--delete-record`, `--add-record`, `--update-record`, `--import` and
   `--merge-from`: it does the change on a copy and leaves the database
   unchanged.
5. **Stop on any failed check.** If a check fails or the output is not what
   this guide or the README says to expect, stop and report. Do not
   improvise a fix on a database that holds real passwords.
6. **Never weaken a safeguard.** Do not suggest `--reseal-db` unless the
   person knows why the file changed (section 8.3). Do not suggest
   `--smartcard-allow-final-try` or `--smartcard-allow-pin-count-low`, do
   not turn off self-destruct, the access log or the writer lock, and do
   not loosen file modes (600 files, 700 folders), to make something work.
   Report instead.
7. **Smart cards: never guess or retry a PIN.** A wrong PIN uses up a try,
   and a locked card (a DoD CAC) needs the card office. List cards with
   `smartcard_list.sh` or `smartcard_list.bat`, which never ask for a PIN.
   Test with a SoftHSM2 software token, never with a person's real card.
8. **Passwords and PINs never on the command line.** Use
   `--password-file`, `--password-fd`, `--smartcard-pin-file` or the
   prompts, with files only the user can read (`chmod 600`), not
   `--rsa-key-passphrase=` or `--smartcard-pin=` values that other users
   can see.
9. **No secrets or site data in files you write** (passwords, keys, host
   names, user names), unless the person asks for that file on their own
   systems.
10. **Keep a log.** List each command, on which computer, and its result
    (exit code and non-secret output), and give it to the person at the
    end.

## 2. What EPDB is

EPDB stores authentication information in encrypted database files and
gives it to people (the GUI) and to scripts and services (the command
line tool `epdb_auth_tool`). Every field is encrypted with AES-256-CBC. A
database opens with a password, a key file, an RSA private key or a smart
card (PKCS#11, DoD CAC, PIV), and several users can each have their own
RSA key or smart card on the same database. Since 2026.105 every save is
crash-safe (a write-ahead journal) and only one program at a time can
write a database. Since 2026.107 an RSA or smart card user can also be a
quantum-resistant user, who needs the RSA key or card AND an ML-KEM-1024
key file to open the database (section 5.3).

| Item | Linux | Windows |
| --- | --- | --- |
| GUI | `~/.encrypted_password_database/bin/encrypted_password_database` | `encrypted_password_database.exe` (setup: `C:\Program Files\DataReel\Encrypted Password Database`, 32-bit: `C:\Program Files (x86)\...`) |
| Command line tool | `~/.encrypted_password_database/bin/epdb_auth_tool` | `epdb_auth_tool.exe` |
| User folder | `~/.encrypted_password_database` (mode 700; `bin`, `data`, `keys`, `certs`, `backups`, `logs`, `docs`) | `%USERPROFILE%\.encrypted_password_database` |
| GUI settings | `encrypted_password_database.cfg` in the user folder | `encrypted_password_database.ini` in the user folder |
| Database | `name.ehd` (data) and `name.ehx` (index), always together | same |
| During a save | `name.ehd.jnl` (the journal, kept while the database is open for writing and removed when it is closed); `name.change.ehd`/`.ehx` during a command line change | same, plus `name.ehd.lock` (the writer lock) |
| Access log | `logs/access.log` in the user folder, and optionally in the database file | same |
| Smart card list | `smartcard_list.sh` (needs OpenSC) | `smartcard_list.bat` |

Read-only commands (they open no database, or need no credential):

```
epdb_auth_tool --version
epdb_auth_tool --check-version db.ehd     # format v1 or v2; exit 5 for v1
epdb_auth_tool --db-stats db.ehd
epdb_auth_tool --list-users db.ehd        # RSA and smart card users
smartcard_list.sh                          # cards, slots and cert IDs, no PIN
```

Exit codes of `epdb_auth_tool`: 0 ok, 1 usage or other error, 2 the
credential did not open the database, 3 file, record or user not found,
4 locked by another writer, 5 `--check-version` found v1, 6 destroyed by
self-destruct, 7 read or write error.

## 3. Repository layout and building (development work)

| Path | Contents |
| --- | --- |
| `gui/` | the wxWidgets GUI; `gui/linux` (makefile_rhel8/9/10), `gui/msvc` (nmake, the Inno Setup installer in `gui/msvc/installer`) |
| `console/` | `epdb_auth_tool`; `console/linux` (makefiles and the test scripts), `console/msvc` |
| `src/`, `include/` | the shared code (`db_auth.cpp`, `epdb_core.cpp`, `epdb_journal.cpp`) |
| `fcrypt/` | the bundled fcrypt, with DataReel 6.4.1 in `fcrypt/3plibs/datareel`; the build compiles these sources directly |
| `env/` | build settings (`linux.env`, `msvc.env`, `msvc64.env`) |
| `README*.txt` | release notes, Windows OpenSSL, wxWidgets and Visual Studio setup |

Build on RHEL 10: `cd console/linux && make -f makefile_rhel10`, and
`cd gui/linux && make -f makefile_rhel10` (needs gcc-c++, make,
openssl-devel, zlib-devel and a static wxWidgets 3.3.4 build, see
`README_windows_wxwidgets.txt` and `env/linux.env`; RHEL 8 and 9 use
makefile_rhel8/9). Windows: Visual Studio 2026, OpenSSL 3.5 and wxWidgets
3.3.4, `call env\msvc64_vs2026.bat` (or `msvc32_vs2026.bat`) then `nmake`
in `console\msvc` and `gui\msvc` (see `README_VisualStudio_2022.txt`,
`README_windows_openssl.txt`).

Build in a copy of the source, not in the clone you commit from, and stage
files by name: never commit binaries or `.o` files.

Tests (Linux, after the build, in `console/linux`): `run_cli_tests.sh`,
`run_journal_tests.sh` (kill -9 crash tests) and `run_smartcard_tests.sh`
(only with a SoftHSM2 token, rule 7). They make their own test databases
under `~/tmp` and never use any other database.

Every release bumps `version.txt`, `version_str` and `release_date` in
`console/epdb_auth_tool.cpp`, `release_date` in `gui/p_config.cpp`,
`gui/epdb_version.h`, `AppVersion` and `AppVersionFile` in
`gui/msvc/installer/epdb.iss`, and the README.

## 4. New install

Help pages: https://datareel.com/encrypted_password_database/help/install.php

1. Download the package and its `.sha256sum` file from
   https://datareel.com/downloads/encrypted_password_database.php and check
   it: `sha256sum -c <file>.sha256sum` (Windows: `certutil -hashfile <file>
   SHA256` and compare). Stop if it does not match.
2. **RHEL 8, 9, 10:** unpack the `rhel_8_9_10_binaries.tar.gz` and run
   `./rh_install.sh` as the user (no root). It installs the GUI,
   `epdb_auth_tool`, the desktop file and the icons for that user. On
   RHEL 8 (2026.107 and later) it picks the build by the host's FIPS mode
   (`fips-mode-setup --check`): `rhel8` (RHEL 8's FIPS-validated system
   OpenSSL 1.1.1k, no quantum-resistant users) on a FIPS-mode host,
   `rhel8_openssl35` (OpenSSL 3.5 built in, quantum-resistant users)
   otherwise. Never change a host's FIPS mode to make an install work.
3. **Windows:** run the setup (64-bit, or the 32-bit setup on 32-bit
   Windows). It installs the Visual C++ runtime if it is missing and can
   add the program folder to the PATH. Or unzip the binaries package.
4. **Smart cards:** install OpenSC (Linux, or Windows without ActivClient);
   on Windows a DoD CAC uses ActivClient by default. Check the card with
   `smartcard_list.sh` or `smartcard_list.bat` (help page `smart_card.php`).
5. Create the first database in the GUI (File, New Database). The person
   types the password; the agent never sees it (rule 1).

## 5. AI-assisted upgrade to 2026.107

### 5.1 What changes, and what does not

- **Database format:** databases made by 2026.101 and later are format v2
  and open in 2026.107 unchanged. 2026.104 to 2026.107 open each
  other's databases.
- **Format v1 databases** (made by 2025.x and older) still open; the GUI
  and `--check-version` warn. Upgrade them, after a backup, with File,
  Upgrade Database Format in the GUI or `epdb_auth_tool --upgrade-db
  db.ehd` (it keeps a copy of the v1 files). Releases before 2026.101
  cannot open a v2 database: upgrade every computer that opens the same
  database before upgrading the database.
- **Writer lock (2026.105 and later):** releases before 2026.105 do not
  check the lock. Upgrade every computer and script that writes the same
  database, so an older copy cannot write during a newer program's save.
- **Windows UTF-8 (2026.104 and later):** Windows 2026.103 and older show
  non-ASCII text that newer releases write as the wrong characters, and a
  database last written on Windows by 2026.103 or older whose titles have
  accents may need `--rebuild-index` once (the GUI offers it).
- **Kept unchanged:** databases, key files, certificates, the user folder
  and the GUI settings file. The setup and the uninstaller never change
  `%USERPROFILE%\.encrypted_password_database`.

### 5.2 Steps (one computer at a time, confirm each)

1. **Close the GUI and stop scripts** that use the databases on this
   computer.
2. **Back up** the user folder and every database the person uses (rule 3).
3. **Install** the new release (section 4; on Linux `rh_install.sh`
   replaces the programs in `~/.encrypted_password_database/bin`; on
   Windows the setup upgrades in place).
4. **Check:** `epdb_auth_tool --version` says 2026.107;
   `epdb_auth_tool --check-version db.ehd` for each database (exit 0 = v2).
5. **Open each database once** (the person, in the GUI or with
   `--db-stats` and a read-only list they do not paste back) and confirm
   the record count is the same as before.
6. Optional, a separate change with its own OK: `--upgrade-db` for v1
   databases (5.1).

### 5.3 Quantum-resistant users (2026.107 and later)

- Each user makes their own ML-KEM-1024 key on their own computer:
  `epdb_auth_tool --generate-pq-key=user_pq.pem` (it asks for a
  passphrase) and gives only `user_pq.pem.pub.pem` to the database owner.
  The private key file never leaves the user (rule 1). fcrypt 2026.110
  makes and reads the same key files.
- New user: `--add-rsa-key=pub.pem --add-username=user
  --add-pq-public-key=user_pq.pem.pub.pem` (or with
  `--add-smartcard-cert-file=cert.pem`).
- Existing RSA or card users move over with one `--rekey` (dry run
  first, rule 4) whose input has `rsa:user=pub.pem,pq=user_pq.pem.pub.pem`
  or `smartcard:user=cert.pem,pq=...` for every user, and
  `--require-pq` so none is left RSA-only.
- Opening: `--pq-key=user_pq.pem --pq-key-passphrase-file=...` together
  with `--rsa-key` or `--smartcard`.
- **Upgrade every copy that rekeys the database to 2026.107 first.** An
  older release's `--rekey` leaves the quantum-resistant users out (they
  then cannot open the new database); its backup copy still has them.

Roll back: install the previous release again and copy the backed-up
files back. A v1 database upgraded to v2 does not open in releases before
2026.101: keep the v1 copy that `--upgrade-db` made.

## 6. Scripts and services (automation)

Help page: `command_line_tool.php`; README section "the command line tool".

- Read one value: `epdb_auth_tool --password-file=pw.txt --non-interactive
  --get-field='Record name:password' db.ehd`. The value goes to stdout and
  is never logged; the script must not echo it to a log.
- The password file is mode 600 and owned by the service account; with a
  smart card, use `--smartcard-pin-file` the same way, or an RSA key with
  `--rsa-key-passphrase-file`.
- Use `--non-interactive`, check the exit code (section 2), and use
  `--lock-timeout=seconds` where a GUI may be saving at the same time
  (exit 4 means another program is writing).
- The GUI's Command Builder writes these commands for bash, PowerShell and
  cmd.exe without the secret on the command line.

## 7. Forks and contributions

EPDB is free software under the GNU General Public License version 3
(`LICENSE`); the bundled fcrypt and DataReel have their own license files.

- Keep the license and copyright notices, and make the changed source
  available under the same license when you distribute a fork.
- Use your own product name, version numbers and Windows installer
  `AppId` (in `gui/msvc/installer/epdb.iss`), so a fork does not upgrade
  or remove an installed EPDB.
- Do not change the database format or the meaning of existing bytes:
  databases must keep opening in EPDB. Run all three test scripts (section
  3) before and after a change.
- Security issues: report them privately through
  https://datareel.com/contact.php, not in public issue trackers.

## 8. AI-assisted troubleshooting and support

Diagnose read-only first; change nothing until the person agrees on the
cause and the fix (rule 2).

### 8.1 Where to look

| What | Command or file |
| --- | --- |
| Program version | `epdb_auth_tool --version` (also the GUI's Help, About) |
| Database format | `epdb_auth_tool --check-version db.ehd` |
| Users with RSA or smart card access | `epdb_auth_tool --list-users db.ehd` |
| Database statistics | `epdb_auth_tool --db-stats db.ehd` |
| Smart cards, slots, cert IDs, PIN tries | `smartcard_list.sh` / `smartcard_list.bat` (no PIN) |
| Save in progress or interrupted | `.ehd.jnl`, `.change.ehd` or `.ehd.lock` files next to the database |
| Access log | `--show-access-log` and `--verify-access-log` (the person runs them; entries name users and records) |
| Missing libraries (Linux) | `ldd ~/.encrypted_password_database/bin/encrypted_password_database \| grep 'not found'` |

### 8.2 Common problems

- **Exit 2, the credential did not open the database.** The person checks
  the password file (first line only, no extra spaces), the key file, or
  for RSA the user name (`--rsa-key-username`) and passphrase. With
  self-destruct on, every failed open counts: stop after one failure and
  check `--db-info` with the person.
- **"is a quantum-resistant ... user, give the user's ML-KEM key file
  with --pq-key"** or "Cannot read the ML-KEM private key (wrong
  passphrase?)". The person gives their own ML-KEM key file and
  passphrase; these are not counted as failed opens, and no smart card
  PIN was sent.
- **Exit 4, "open in another program".** A GUI or another script is
  writing the database. Close it, or use `--lock-timeout`. Do not delete a
  `.lock` file.
- **"Finished a save that was interrupted".** Normal after a crash, kill
  or power loss: the journal completed or dropped the save. Check the
  records with the person.
- **The integrity check failed (MAC or index mismatch).** The file was
  changed outside EPDB, damaged, or copied while being written. Stop
  (section 8.3).
- **Smart card: card not found, wrong cert ID, or two cards.** Run the card
  list; pick the slot with `--smartcard-slot` and the cert with
  `--smartcard-cert-id` (CAC Key Management is ID 03). Never send the PIN
  again after a failure (rule 7).
- **Windows names with accents look wrong, or a record is not found by
  name.** Upgrade every Windows copy to 2026.104 or later, then
  `--rebuild-index` once (5.1).
- **The GUI does not start on Linux.** Check missing libraries with `ldd`
  and the RHEL version of the binary (rhel8, rhel9 or rhel10).

### 8.3 Stop and escalate at once

Do not try to fix these yourself. Report to the person straight away, with
the summary in 8.4:

- An integrity check failure, unless the person knows why the file changed
  (then `--reseal-db` accepts the current contents; it is their decision).
- A database destroyed by self-destruct (exit 6): only the person's own
  backups can restore it.
- A locked or nearly locked smart card PIN.
- Records missing after a save, or two programs that wrote the same
  database at the same time.
- Anything that needs a password, key or PIN to be shared, or a safeguard
  to be turned off (rule 6).

### 8.4 Support summary

When a problem is not solved, give the person a summary they can send to
their team or to DataReel (https://datareel.com/contact.php). AI-assisted
support ends here: the people take it from this summary.

1. EPDB version (`--version`) and OS, and how the release was installed
   (setup, zip, `rh_install.sh`, source build).
2. `--check-version` and `--db-stats` output for the database.
3. The exact error text and exit code, and the unlock method used
   (password, key file, RSA, smart card), never the credential itself.
4. For smart cards: the `smartcard_list` output and the middleware
   (OpenSC or ActivClient).
5. What happened, when, what changed before it, and every command already
   tried with its result.

Never include passwords, keys, PINs, record values, database files or
exports in the summary. Remove host and user names when the person asks.

EPDB is free software under the GNU General Public License, provided as
is. Check every command an agent proposes before it runs; the person who
runs it is responsible for the changes made.

## 9. Report back

End every task with: the computers and databases touched (names only),
each command and its result (exit code and non-secret output), the backups
made and where, every warning and what was decided, and the final
`--version` and `--check-version` results.
