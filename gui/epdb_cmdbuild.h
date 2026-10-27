// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File Name: epdb_cmdbuild.h
// Compiler Used: GNU, MSVC
// Produced By: DataReel Software Development Team
// File Creation Date: 10/27/2026
// Date Last Modified: 10/27/2026
// Copyright (c) 2001-2026 DataReel Software Development
// ----------------------------------------------------------- //
// ---------- Include File Description and Details  ---------- //
// ----------------------------------------------------------- //
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
USA

Command builder for the GUI Tools > Command Builder dialog. Builds the
epdb_auth_tool command line for an operation, with the quoting of each
shell. Secrets are never put on the command line: passwords, PINs and
passphrases are read from a file only the user can read or from a pipe
(-fd=0), and record values from --record-file or stdin. No GUI code, so
the quoting is tested on its own (gui/tests/cmdbuild_test.cpp).
*/
// ----------------------------------------------------------- //
#ifndef __EPDB_CMDBUILD_HPP__
#define __EPDB_CMDBUILD_HPP__

#include <string>
#include <vector>

enum EPDBCmdShell {
  EPDB_SHELL_BASH = 0,   // Linux bash or sh
  EPDB_SHELL_POWERSHELL, // Windows PowerShell
  EPDB_SHELL_CMD         // Windows cmd.exe batch file
};

enum EPDBCmdOperation {
  EPDB_OP_ADD_RECORD = 0,
  EPDB_OP_UPDATE_RECORD,
  EPDB_OP_DELETE_RECORD,
  EPDB_OP_SHOW_RECORD,
  EPDB_OP_GET_FIELD,
  EPDB_OP_DB_INFO,
  EPDB_OP_LIST_RECORDS,
  EPDB_OP_EXPORT,
  EPDB_OP_IMPORT,
  EPDB_OP_MERGE_FROM,
  EPDB_OP_CHANGE_PASSWORD,
  EPDB_OP_GENERATE_PASSWORD,
  EPDB_OP_ADD_RSA_USER,
  EPDB_OP_ADD_SMARTCARD_USER,
  EPDB_OP_REMOVE_USER,
  EPDB_OP_LIST_USERS,
  EPDB_OP_REKEY,
  EPDB_OP_CHECK_VERSION,
  EPDB_OP_SHOW_ACCESS_LOG,
  EPDB_OP_VERIFY_ACCESS_LOG,
  EPDB_OP_COUNT
};

enum EPDBCmdAuth {
  EPDB_AUTH_PASSWORD = 0,
  EPDB_AUTH_KEY_FILE,
  EPDB_AUTH_RSA_KEY,
  EPDB_AUTH_SMART_CARD
};

// Where a secret (password, PIN, passphrase) comes from. It is never
// put on the command line.
enum EPDBCmdSecretSource {
  EPDB_SECRET_FILE = 0,   // --xxx-file=path, a file only the user can read
  EPDB_SECRET_PIPE_AWS,   // AWS Secrets Manager piped to --xxx-fd=0
  EPDB_SECRET_PIPE_CMD    // Another command piped to --xxx-fd=0
};

struct EPDBCmdSecret
{
  EPDBCmdSecret() { source = EPDB_SECRET_FILE; }
  int source;
  std::string file;      // EPDB_SECRET_FILE
  std::string aws_id;    // EPDB_SECRET_PIPE_AWS: the secret ID
  std::string command;   // EPDB_SECRET_PIPE_CMD: the command, used as written
};

struct EPDBCmdOptions
{
  EPDBCmdOptions();

  std::string tool;      // epdb_auth_tool path, "epdb_auth_tool" when empty
  std::string database;  // The .ehd file
  int operation;         // EPDBCmdOperation

  // How the database is opened
  int auth;              // EPDBCmdAuth
  EPDBCmdSecret password;     // EPDB_AUTH_PASSWORD
  std::string key_file;       // EPDB_AUTH_KEY_FILE
  std::string rsa_key_file;   // EPDB_AUTH_RSA_KEY
  std::string rsa_username;
  int rsa_has_passphrase;
  EPDBCmdSecret rsa_passphrase;
  std::string sc_username;    // EPDB_AUTH_SMART_CARD
  std::string sc_cert_id;
  std::string sc_slot;
  std::string sc_provider;
  EPDBCmdSecret sc_pin;

  // Operation values
  std::string record_name;    // add, update, delete, show and get-field
  std::string field_name;     // get-field
  int record_from_file;       // 1: --record-file, 0: stdin (or --record-fd=0)
  std::string record_file;
  std::string export_format;  // csv or json
  std::string output_file;    // export, empty for stdout
  std::string input_format;   // import: empty, text, json or csv
  std::string merge_database;
  int merge_auth;             // EPDB_AUTH_PASSWORD or EPDB_AUTH_KEY_FILE
  EPDBCmdSecret merge_password;
  std::string merge_key_file;
  std::string merge_policy;   // fail, skip or replace
  EPDBCmdSecret new_password; // change-password
  int generate_length;
  int generate_symbols;
  std::string user_name;      // add and remove user
  std::string user_key_file;  // add RSA user: public key, add card user: cert file
  int user_from_card;         // add card user: read the cert from the card
  std::string remove_user_type; // rsa, smartcard or empty
  std::string rekey_file;     // rekey input file, empty for stdin

  // Global options
  int non_interactive;
  int json;
  int quiet;
  int dry_run;
  int yes;
  int lock_timeout;           // 0 for the default
};

struct EPDBCmdResult
{
  std::vector<std::string> args; // The arguments, the tool first
  std::string command;           // The command line for the shell
  std::vector<std::string> notes; // How to supply the secrets and input
  std::string input_sample;      // Sample record or rekey input, no secrets
  int supports_dry_run;
  int needs_auth;
};

// Quotes one argument for the shell, only when it needs quoting.
// Returns 0, or -1 if the value cannot be passed safely in that shell
// (cmd.exe cannot pass a double quote or a line break).
int EPDBQuoteArg(const std::string &arg, int shell, std::string &out);

// Builds the command. Returns 0, or -1 with the reason in err.
int EPDBBuildCommand(const EPDBCmdOptions &opt, int shell, EPDBCmdResult &result, std::string &err);

// The full script text: comments, notes and the command.
std::string EPDBCommandScript(const EPDBCmdResult &result, int shell);

// Operation names for menus, and whether the operation supports --dry-run.
const char *EPDBCmdOperationName(int operation);
int EPDBCmdSupportsDryRun(int operation);
int EPDBCmdNeedsAuth(int operation);

// The documented epdb_auth_tool exit codes, one per line.
const char *EPDBCmdExitCodes();

#endif // __EPDB_CMDBUILD_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
