// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Source Code File
// Compiler Used: MSVC, GCC
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

Command builder for the epdb_auth_tool command line, see epdb_cmdbuild.h
*/
// ----------------------------------------------------------- //
#include "epdb_cmdbuild.h"
#include <stdio.h>

EPDBCmdOptions::EPDBCmdOptions()
{
  operation = EPDB_OP_DB_INFO;
  auth = EPDB_AUTH_PASSWORD;
  rsa_has_passphrase = 0;
  record_from_file = 1;
  export_format = "csv";
  merge_auth = EPDB_AUTH_PASSWORD;
  merge_policy = "fail";
  generate_length = 20;
  generate_symbols = 0;
  user_from_card = 1;
  non_interactive = 1;
  json = 0;
  quiet = 0;
  dry_run = 0;
  yes = 0;
  lock_timeout = 0;
}

struct EPDBCmdOpInfo
{
  const char *name;
  int dry_run;
  int auth;
};

static const EPDBCmdOpInfo epdb_cmd_ops[EPDB_OP_COUNT] = {
  { "Add a record", 1, 1 },
  { "Update a record", 1, 1 },
  { "Delete a record", 1, 1 },
  { "Show a record", 0, 1 },
  { "Get one field", 0, 1 },
  { "Database information", 0, 1 },
  { "List the records", 0, 1 },
  { "Export the records", 0, 1 },
  { "Import records", 1, 1 },
  { "Merge from another database", 1, 1 },
  { "Change the password", 0, 1 },
  { "Generate a password", 0, 0 },
  { "Add an RSA key user", 0, 1 },
  { "Add a smart card user", 0, 1 },
  { "Remove a user", 1, 1 },
  { "List the users", 0, 0 },
  { "Rekey (new master key)", 1, 1 },
  { "Check the format version", 0, 0 },
  { "Show the access log", 0, 1 },
  { "Verify the access log", 0, 1 }
};

const char *EPDBCmdOperationName(int operation)
{
  if(operation < 0 || operation >= EPDB_OP_COUNT) return "";
  return epdb_cmd_ops[operation].name;
}

int EPDBCmdSupportsDryRun(int operation)
{
  if(operation < 0 || operation >= EPDB_OP_COUNT) return 0;
  return epdb_cmd_ops[operation].dry_run;
}

int EPDBCmdNeedsAuth(int operation)
{
  if(operation < 0 || operation >= EPDB_OP_COUNT) return 0;
  return epdb_cmd_ops[operation].auth;
}

const char *EPDBCmdExitCodes()
{
  return "0  OK\n"
    "1  Usage or other error\n"
    "2  The credential did not open the database\n"
    "3  File, record or user not found\n"
    "4  Locked by another writer (--lock-timeout)\n"
    "5  --check-version found a v1 database\n"
    "6  Destroyed by self-destruct\n"
    "7  Read or write error (also a read-only file with self-destruct on)\n";
}

// ----------------------------------------------------------- //
// Quoting
// ----------------------------------------------------------- //
static int EPDBNeedsQuotes(const std::string &s, int shell)
{
  if(s.empty()) return 1;
  for(size_t i = 0; i < s.size(); i++) {
    unsigned char c = (unsigned char)s[i];
    if(c >= 0x80) return 1; // UTF-8, quote to be safe
    if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) continue;
    switch(c) {
      case '-': case '_': case '.': case '/': case '=': case ',': case '+':
	continue;
      case ':':
	continue;
      case '\\':
	if(shell != EPDB_SHELL_BASH) continue; // A path separator on Windows
	return 1;
      case '@':
	if(shell == EPDB_SHELL_POWERSHELL) return 1; // @ starts a splat or array
	continue;
      default:
	return 1;
    }
  }
  return 0;
}

int EPDBQuoteArg(const std::string &arg, int shell, std::string &out)
{
  out.clear();
  if(!EPDBNeedsQuotes(arg, shell)) {
    out = arg;
    return 0;
  }
  if(shell == EPDB_SHELL_BASH) {
    // Single quotes keep every character, a single quote is ended,
    // escaped and started again
    out = "'";
    for(size_t i = 0; i < arg.size(); i++) {
      if(arg[i] == '\'') out += "'\\''";
      else out += arg[i];
    }
    out += "'";
    return 0;
  }
  if(shell == EPDB_SHELL_POWERSHELL) {
    // Single quoted strings keep every character, a single quote is
    // doubled. PowerShell 7.3 and later pass embedded double quotes to
    // the program; Windows PowerShell 5.1 does not, so they are refused.
    // 5.1 also turns a trailing backslash of a quoted value into an
    // escaped quote, and 7.3 does not, so that is refused too.
    for(size_t i = 0; i < arg.size(); i++) {
      if(arg[i] == '"' || arg[i] == '\n' || arg[i] == '\r') return -1;
    }
    // and drops an empty argument
    if(arg.empty() || arg[arg.size()-1] == '\\') return -1;
    out = "'";
    for(size_t i = 0; i < arg.size(); i++) {
      unsigned char c = (unsigned char)arg[i];
      if(c == '\'') out += "''";
      else if(c == 0xE2 && i + 2 < arg.size() && (unsigned char)arg[i+1] == 0x80 &&
	      ((unsigned char)arg[i+2] >= 0x98 && (unsigned char)arg[i+2] <= 0x9B)) {
	// Curly single quotes also end a PowerShell string, double them
	out += arg.substr(i, 3);
	out += arg.substr(i, 3);
	i += 2;
      }
      else out += arg[i];
    }
    out += "'";
    return 0;
  }
  // cmd.exe batch file: double quotes, which cannot contain a double
  // quote or a line break. A % is doubled so a batch file keeps it.
  for(size_t i = 0; i < arg.size(); i++) {
    if(arg[i] == '"' || arg[i] == '\n' || arg[i] == '\r') return -1;
  }
  out = "\"";
  for(size_t i = 0; i < arg.size(); i++) {
    if(arg[i] == '%') out += "%%";
    else out += arg[i];
  }
  // A trailing backslash would escape the closing quote for the program
  if(!arg.empty() && arg[arg.size()-1] == '\\') out += "\\";
  out += "\"";
  return 0;
}

// ----------------------------------------------------------- //
// Command building
// ----------------------------------------------------------- //
static std::string EPDBCmdComment(int shell)
{
  return (shell == EPDB_SHELL_CMD) ? "REM " : "# ";
}

struct EPDBCmdBuilder
{
  EPDBCmdBuilder(int s) { shell = s; stdin_used = 0; }
  int shell;
  int stdin_used;           // 1 once a pipe feeds stdin
  std::string pipe;         // The command piped into the tool
  std::vector<std::string> args;
  std::vector<std::string> notes;

  void Arg(const std::string &a) { args.push_back(a); }
  void Opt(const char *name, const std::string &value) { args.push_back(std::string(name) + "=" + value); }

  int Secret(const char *what, const char *opt_prefix, const EPDBCmdSecret &s, std::string &err) {
    // opt_prefix is "--password" and so on: --password-file=, --password-fd=0
    if(s.source == EPDB_SECRET_FILE) {
      if(s.file.empty()) {
	err = std::string("Enter the file that holds the ") + what;
	return -1;
      }
      Opt((std::string(opt_prefix) + "-file").c_str(), s.file);
      if(shell == EPDB_SHELL_BASH) {
	notes.push_back(std::string("The ") + what + " is read from the first line of " + s.file +
			", a file only you can read (chmod 600). It is not put on the command line.");
      }
      else {
	notes.push_back(std::string("The ") + what + " is read from the first line of " + s.file +
			", a file only you can read. It is not put on the command line.");
      }
      return 0;
    }
    if(stdin_used) {
      err = std::string("Only one secret or input can come from a pipe, read the ") + what + " from a file";
      return -1;
    }
    stdin_used = 1;
    Opt((std::string(opt_prefix) + "-fd").c_str(), "0");
    if(s.source == EPDB_SECRET_PIPE_AWS) {
      if(s.aws_id.empty()) {
	err = std::string("Enter the AWS Secrets Manager secret ID for the ") + what;
	return -1;
      }
      std::string q;
      if(EPDBQuoteArg(s.aws_id, shell, q) != 0) {
	err = "The secret ID cannot be quoted for this shell";
	return -1;
      }
      pipe = "aws secretsmanager get-secret-value --secret-id " + q + " --query SecretString --output text";
      notes.push_back(std::string("The ") + what + " is read from AWS Secrets Manager and piped in on stdin (-fd=0). "
		      "It is not put on the command line or in the script.");
    }
    else {
      if(s.command.empty()) {
	err = std::string("Enter the command that prints the ") + what;
	return -1;
      }
      pipe = s.command;
      notes.push_back(std::string("The ") + what + " is printed by the command before the | and piped in on stdin (-fd=0).");
    }
    return 0;
  }
};

static int EPDBCmdAddAuth(const EPDBCmdOptions &opt, EPDBCmdBuilder &b, std::string &err)
{
  switch(opt.auth) {
    case EPDB_AUTH_PASSWORD:
      return b.Secret("database password", "--password", opt.password, err);
    case EPDB_AUTH_KEY_FILE:
      if(opt.key_file.empty()) {
	err = "Enter the key file";
	return -1;
      }
      b.Opt("--key", opt.key_file);
      return 0;
    case EPDB_AUTH_RSA_KEY:
      if(opt.rsa_key_file.empty()) {
	err = "Enter the private RSA key file";
	return -1;
      }
      b.Opt("--rsa-key", opt.rsa_key_file);
      if(!opt.rsa_username.empty()) b.Opt("--rsa-key-username", opt.rsa_username);
      if(opt.rsa_has_passphrase) return b.Secret("private key passphrase", "--rsa-key-passphrase", opt.rsa_passphrase, err);
      return 0;
    case EPDB_AUTH_SMART_CARD:
      b.Arg("--smartcard");
      if(!opt.sc_username.empty()) b.Opt("--smartcard-username", opt.sc_username);
      if(!opt.sc_cert_id.empty()) b.Opt("--smartcard-cert-id", opt.sc_cert_id);
      if(!opt.sc_slot.empty()) b.Opt("--smartcard-slot", opt.sc_slot);
      if(!opt.sc_provider.empty()) b.Opt("--smartcard-provider", opt.sc_provider);
      if(b.Secret("smart card PIN", "--smartcard-pin", opt.sc_pin, err) != 0) return -1;
      b.notes.push_back("The PIN is sent to the card at most once per run. A wrong PIN is not retried; "
			"check the PIN tries left before running again.");
      return 0;
  }
  err = "Unknown unlock method";
  return -1;
}

static int EPDBCmdRecordInput(const EPDBCmdOptions &opt, EPDBCmdBuilder &b, std::string &err)
{
  if(opt.record_from_file) {
    if(opt.record_file.empty()) {
      err = "Enter the file that holds the record fields";
      return -1;
    }
    b.Opt("--record-file", opt.record_file);
    b.notes.push_back("The record fields are read from " + opt.record_file +
		      ", one name=value per line, a file only you can read. Delete it when you are done.");
    return 0;
  }
  if(b.stdin_used) {
    err = "stdin already brings a secret, read the record fields from a file";
    return -1;
  }
  b.stdin_used = 1;
  b.notes.push_back("The record fields are read from stdin, one name=value per line (or JSON).");
  return 0;
}

int EPDBBuildCommand(const EPDBCmdOptions &opt, int shell, EPDBCmdResult &result, std::string &err)
{
  err.clear();
  result.args.clear();
  result.notes.clear();
  result.command.clear();
  result.input_sample.clear();
  result.supports_dry_run = EPDBCmdSupportsDryRun(opt.operation);
  result.needs_auth = EPDBCmdNeedsAuth(opt.operation);

  EPDBCmdBuilder b(shell);
  b.Arg(opt.tool.empty() ? std::string("epdb_auth_tool") : opt.tool);

  int needs_db = (opt.operation != EPDB_OP_GENERATE_PASSWORD);
  if(needs_db && opt.database.empty()) {
    err = "Enter the database file";
    return -1;
  }
  if(result.needs_auth && EPDBCmdAddAuth(opt, b, err) != 0) return -1;

  switch(opt.operation) {
    case EPDB_OP_ADD_RECORD:
    case EPDB_OP_UPDATE_RECORD:
      if(opt.record_name.empty()) {
	err = "Enter the record title";
	return -1;
      }
      b.Opt(opt.operation == EPDB_OP_ADD_RECORD ? "--add-record" : "--update-record", opt.record_name);
      if(EPDBCmdRecordInput(opt, b, err) != 0) return -1;
      if(opt.operation == EPDB_OP_ADD_RECORD) {
	result.input_sample = "title=" + opt.record_name + "\nusername=jdoe\npassword=(the password)\n"
	  "website-link=https://example.com\nnotes=Line one\\nLine two\n";
      }
      else {
	result.input_sample = "password=(the new password)\nprevious-password=(the old password)\n";
      }
      break;
    case EPDB_OP_DELETE_RECORD:
      if(opt.record_name.empty()) {
	err = "Enter the record title";
	return -1;
      }
      b.Opt("--delete-record", opt.record_name);
      if(opt.yes || opt.non_interactive) b.Arg("--yes");
      break;
    case EPDB_OP_SHOW_RECORD:
      if(opt.record_name.empty()) {
	err = "Enter the record title";
	return -1;
      }
      b.Opt("--show-record", opt.record_name);
      b.notes.push_back("The record is written to stdout in clear text, including the password.");
      break;
    case EPDB_OP_GET_FIELD:
      if(opt.record_name.empty() || opt.field_name.empty()) {
	err = "Enter the record title and the field";
	return -1;
      }
      b.Opt("--get-field", opt.record_name + ":" + opt.field_name);
      b.notes.push_back("Only the field value is written to stdout, for use in scripts. It is never logged.");
      break;
    case EPDB_OP_DB_INFO:
      b.Arg("--db-info");
      break;
    case EPDB_OP_LIST_RECORDS:
      b.Arg("--db-list");
      break;
    case EPDB_OP_EXPORT:
      b.Opt("--export", opt.export_format.empty() ? std::string("csv") : opt.export_format);
      if(!opt.output_file.empty()) b.Opt("--output-file", opt.output_file);
      b.notes.push_back(opt.output_file.empty() ? "The records are written to stdout in clear text." :
			"The records are written in clear text to a new file only you can read.");
      break;
    case EPDB_OP_IMPORT:
      b.Arg("--import");
      if(!opt.input_format.empty()) b.Opt("--input-format", opt.input_format);
      if(EPDBCmdRecordInput(opt, b, err) != 0) return -1;
      result.input_sample = "[{\"title\": \"My Bank\", \"username\": \"jdoe\", \"password\": \"(the password)\"}]\n";
      break;
    case EPDB_OP_MERGE_FROM:
      if(opt.merge_database.empty()) {
	err = "Enter the database to merge from";
	return -1;
      }
      b.Opt("--merge-from", opt.merge_database);
      if(opt.merge_auth == EPDB_AUTH_KEY_FILE) {
	if(opt.merge_key_file.empty()) {
	  err = "Enter the key file of the other database";
	  return -1;
	}
	b.Opt("--merge-key", opt.merge_key_file);
      }
      else if(b.Secret("password of the other database", "--merge-password", opt.merge_password, err) != 0) {
	return -1;
      }
      if(!opt.merge_policy.empty()) b.Opt("--merge-policy", opt.merge_policy);
      break;
    case EPDB_OP_CHANGE_PASSWORD:
      b.Arg("--change-password");
      if(b.Secret("new database password", "--new-password", opt.new_password, err) != 0) return -1;
      break;
    case EPDB_OP_GENERATE_PASSWORD: {
      char buf[32];
      snprintf(buf, sizeof(buf), "%d", opt.generate_length > 0 ? opt.generate_length : 20);
      b.Opt("--generate-password", buf);
      if(opt.generate_symbols) b.Arg("--password-symbols");
      b.notes.push_back("The password is written to stdout. Redirect it to a file only you can read.");
      break;
    }
    case EPDB_OP_ADD_RSA_USER:
      if(opt.user_name.empty() || opt.user_key_file.empty()) {
	err = "Enter the username and the public RSA key file";
	return -1;
      }
      b.Opt("--add-rsa-key", opt.user_key_file);
      b.Opt("--add-username", opt.user_name);
      break;
    case EPDB_OP_ADD_SMARTCARD_USER:
      if(opt.user_name.empty()) {
	err = "Enter the username";
	return -1;
      }
      if(opt.user_from_card) {
	b.Arg("--add-smartcard-cert");
	if(opt.auth != EPDB_AUTH_SMART_CARD) {
	  if(!opt.sc_cert_id.empty()) b.Opt("--smartcard-cert-id", opt.sc_cert_id);
	  if(!opt.sc_slot.empty()) b.Opt("--smartcard-slot", opt.sc_slot);
	  if(!opt.sc_provider.empty()) b.Opt("--smartcard-provider", opt.sc_provider);
	}
	b.notes.push_back("The cert is read from the card in the reader. No PIN is needed to add a card user.");
      }
      else {
	if(opt.user_key_file.empty()) {
	  err = "Enter the exported cert file";
	  return -1;
	}
	b.Opt("--add-smartcard-cert-file", opt.user_key_file);
      }
      b.Opt("--add-username", opt.user_name);
      break;
    case EPDB_OP_REMOVE_USER:
      if(opt.user_name.empty()) {
	err = "Enter the username";
	return -1;
      }
      b.Opt("--remove-user", opt.user_name);
      if(!opt.remove_user_type.empty()) b.Opt("--remove-user-type", opt.remove_user_type);
      b.notes.push_back("Removing a user does not revoke a key the user already recovered. Run a rekey after.");
      break;
    case EPDB_OP_LIST_USERS:
      b.Arg("--list-users");
      break;
    case EPDB_OP_REKEY:
      b.Arg("--rekey");
      if(!opt.rekey_file.empty()) {
	b.Opt("--rekey-file", opt.rekey_file);
	b.notes.push_back("The rekey input is read from " + opt.rekey_file + ", a file only you can read.");
      }
      else {
	if(b.stdin_used) {
	  err = "stdin already brings a secret, read the rekey input from a file";
	  return -1;
	}
	b.stdin_used = 1;
	b.notes.push_back("The rekey input is read from stdin.");
      }
      result.input_sample = "new-password=(the new password)\nrsa:alice=/path/alice_public.pem\nsmartcard:bob=card\n";
      break;
    case EPDB_OP_CHECK_VERSION:
      b.Arg("--check-version");
      break;
    case EPDB_OP_SHOW_ACCESS_LOG:
      b.Arg("--show-access-log");
      break;
    case EPDB_OP_VERIFY_ACCESS_LOG:
      b.Arg("--verify-access-log");
      break;
    default:
      err = "Unknown operation";
      return -1;
  }

  if(opt.non_interactive) b.Arg("--non-interactive");
  if(opt.json) b.Arg("--json");
  if(opt.quiet) b.Arg("--quiet");
  if(opt.dry_run && result.supports_dry_run) b.Arg("--dry-run");
  if(opt.lock_timeout > 0) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", opt.lock_timeout);
    b.Opt("--lock-timeout", buf);
  }
  if(needs_db) b.Arg(opt.database);

  // The command line
  std::string cmd;
  if(!b.pipe.empty()) cmd = b.pipe + " | ";
  for(size_t i = 0; i < b.args.size(); i++) {
    std::string q;
    if(EPDBQuoteArg(b.args[i], shell, q) != 0) {
      err = "A value has a double quote, a line break or (PowerShell) a trailing backslash, which this shell "
	"cannot pass safely: " + b.args[i];
      return -1;
    }
    if(i == 0 && shell == EPDB_SHELL_POWERSHELL && q[0] == '\'') cmd += "& "; // Run a quoted path
    cmd += q;
    if(i + 1 < b.args.size()) cmd += " ";
  }
  if(shell != EPDB_SHELL_BASH) {
    // 10/27/2026: Windows programs read their arguments in the system
    // code page, so characters outside it do not arrive as typed
    for(size_t i = 0; i < b.args.size(); i++) {
      int wide = 0;
      for(size_t k = 0; k < b.args[i].size(); k++) if((unsigned char)b.args[i][k] >= 0x80) wide = 1;
      if(wide) {
	b.notes.push_back("A file name or value has characters outside plain ASCII. epdb_auth_tool on Windows "
			  "reads its arguments in the system code page, rename the file or use a plain ASCII name "
			  "if the command cannot find it.");
	break;
      }
    }
  }
  result.args = b.args;
  result.command = cmd;
  result.notes = b.notes;
  if(!b.stdin_used && opt.non_interactive) {
    // Nothing else reads stdin
  }
  return 0;
}

std::string EPDBCommandScript(const EPDBCmdResult &result, int shell)
{
  std::string c = EPDBCmdComment(shell);
  std::string s;
  if(shell == EPDB_SHELL_BASH) s += "#!/bin/bash\n";
  if(shell == EPDB_SHELL_CMD) s += "@echo off\n";
  s += c + "Made by Encrypted Password Database, Tools > Command Builder\n";
  for(size_t i = 0; i < result.notes.size(); i++) s += c + result.notes[i] + "\n";
  if(!result.input_sample.empty()) {
    s += c + "Sample input (no real secrets):\n";
    size_t p = 0;
    while(p < result.input_sample.size()) {
      size_t e = result.input_sample.find('\n', p);
      if(e == std::string::npos) e = result.input_sample.size();
      s += c + "  " + result.input_sample.substr(p, e - p) + "\n";
      p = e + 1;
    }
  }
  s += result.command + "\n";
  if(shell == EPDB_SHELL_BASH) s += "exit $?\n";
  if(shell == EPDB_SHELL_POWERSHELL) s += "exit $LASTEXITCODE\n";
  if(shell == EPDB_SHELL_CMD) s += "exit /b %ERRORLEVEL%\n";
  return s;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
