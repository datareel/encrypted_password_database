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
Tests for the command builder (gui/epdb_cmdbuild.cpp).

The quoting tests build a command that runs this program with
--print-args, run it through the real shell (bash on Linux, PowerShell
and cmd.exe batch files on Windows) and check every argument arrives
unchanged: paths with spaces, quotes, $, backticks, %, &, ^, ! and UTF-8.
The other tests check that secrets never appear on the command line and
that each operation has the expected options.

Linux:   g++ -o cmdbuild_test cmdbuild_test.cpp ../epdb_cmdbuild.cpp && ./cmdbuild_test
Windows: cl /EHsc cmdbuild_test.cpp ..\epdb_cmdbuild.cpp && cmdbuild_test.exe
*/
// ----------------------------------------------------------- //
#include "../epdb_cmdbuild.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#if defined (_WIN32)
#include <windows.h>
#define popen _popen
#define pclose _pclose
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

static int passed = 0;
static int failed = 0;
static std::string self_path;

static void Check(const char *name, int ok, const std::string &detail = std::string())
{
  if(ok) {
    passed++;
    printf("Passed: %s\n", name);
  }
  else {
    failed++;
    printf("FAILED: %s %s\n", name, detail.c_str());
  }
}

static std::string Hex(const std::string &s)
{
  static const char hexd[] = "0123456789abcdef";
  std::string h;
  for(size_t i = 0; i < s.size(); i++) {
    h += hexd[((unsigned char)s[i] >> 4) & 0xF];
    h += hexd[(unsigned char)s[i] & 0xF];
  }
  return h.empty() ? std::string("-") : h;
}

static std::string RunCapture(const std::string &cmd)
{
  std::string out;
  FILE *fp = popen(cmd.c_str(), "r");
  if(!fp) return out;
  char buf[4096];
  size_t n;
  while((n = fread(buf, 1, sizeof(buf), fp)) > 0) out.append(buf, n);
  pclose(fp);
  return out;
}

static std::string WriteTemp(const std::string &name, const std::string &text)
{
#if defined (_WIN32)
  char dir[MAX_PATH];
  GetTempPathA(sizeof(dir), dir);
  std::string path = std::string(dir) + name;
#else
  std::string path = std::string("/tmp/cmdbuild_test_") + std::to_string((long)getpid()) + "_" + name;
#endif
  FILE *fp = fopen(path.c_str(), "wb");
  if(fp) {
    fwrite(text.data(), 1, text.size(), fp);
    fclose(fp);
  }
  return path;
}

// Runs the quoted command through the shell and returns the arguments
// the program got, one hex line each
static std::string ShellRoundTrip(const std::string &command, int shell)
{
  std::string out;
#if defined (_WIN32)
  if(shell == EPDB_SHELL_POWERSHELL) {
    // A UTF-8 BOM, or Windows PowerShell 5.1 reads the script in the ANSI code page
    std::string ps = WriteTemp("cmdbuild_test.ps1", "\xef\xbb\xbf" + command + "\r\n");
    out = RunCapture("powershell -NoProfile -ExecutionPolicy Bypass -File \"" + ps + "\"");
    remove(ps.c_str());
  }
  else if(shell == EPDB_SHELL_CMD) {
    std::string bat = WriteTemp("cmdbuild_test.bat", "@echo off\r\n" + command + "\r\n");
    out = RunCapture("cmd /c \"" + bat + "\"");
    remove(bat.c_str());
  }
#else
  if(shell == EPDB_SHELL_BASH) {
    std::string sh = WriteTemp("cmdbuild_test.sh", command + "\n");
    out = RunCapture("bash " + sh);
    remove(sh.c_str());
  }
#endif
  return out;
}

static void QuotingTests(int shell, const char *shell_name)
{
  const char *values[] = {
    "plain",
    "/home/me/My Databases/work db.ehd",
    "C:\\Users\\Me\\My Documents\\work db.ehd",
    "C:\\path with trailing\\",
    "it's",
    "two  spaces",
    "dollar $HOME and ${PATH}",
    "back`tick`",
    "semi;colon && pipe | redirect > < file",
    "percent %PATH% and 100%",
    "caret ^ bang ! paren ( )",
    "star * question ? [abc]",
    "hash # tilde ~",
    "at @array",
    "comma,equals=plus+",
    "unicode \xc3\xa9t\xc3\xa9 \xe2\x82\xac",
    "curly \xe2\x80\x98quotes\xe2\x80\x99",
    "",
    "-",
    "--option=value with space",
    "double \"quote\"",
    0
  };
  std::vector<std::string> args;
  for(int i = 0; values[i]; i++) {
    if(shell != EPDB_SHELL_BASH && strchr(values[i], '"')) continue; // Refused, tested below
    if(shell == EPDB_SHELL_POWERSHELL && (!values[i][0] || values[i][strlen(values[i])-1] == '\\')) continue;
    // Windows programs get their arguments in the system code page, a
    // UTF-8 value cannot arrive unchanged (the builder warns, tested below)
    int wide = 0;
    for(const char *c = values[i]; *c; c++) if((unsigned char)*c >= 0x80) wide = 1;
    if(shell != EPDB_SHELL_BASH && wide) continue;
    args.push_back(values[i]);
  }

  std::string cmd, expect;
  std::string q;
  EPDBQuoteArg(self_path, shell, q);
  if(shell == EPDB_SHELL_POWERSHELL && q[0] == '\'') cmd = "& ";
  cmd += q + " --print-args";
  for(size_t i = 0; i < args.size(); i++) {
    if(EPDBQuoteArg(args[i], shell, q) != 0) {
      Check("quote a value", 0, args[i]);
      continue;
    }
    cmd += " " + q;
    expect += Hex(args[i]) + "\n";
  }
  std::string got = ShellRoundTrip(cmd, shell);
  // PowerShell may change line endings
  std::string clean;
  for(size_t i = 0; i < got.size(); i++) if(got[i] != '\r') clean += got[i];
  std::string name = std::string(shell_name) + ": every special value reaches the program unchanged";
  Check(name.c_str(), clean == expect, "\ncommand: " + cmd + "\ngot:\n" + clean + "expected:\n" + expect);

  if(shell == EPDB_SHELL_POWERSHELL) {
    name = std::string(shell_name) + ": a quoted value with a trailing backslash is refused";
    Check(name.c_str(), EPDBQuoteArg("C:\\path with trailing\\", shell, q) != 0);
    name = std::string(shell_name) + ": an empty value is refused (Windows PowerShell 5.1 drops it)";
    Check(name.c_str(), EPDBQuoteArg("", shell, q) != 0);
  }
  if(shell != EPDB_SHELL_BASH) {
    name = std::string(shell_name) + ": a value with a double quote is refused";
    Check(name.c_str(), EPDBQuoteArg("double \"quote\"", shell, q) != 0);
  }
}

static int Contains(const std::string &s, const char *part) { return s.find(part) != std::string::npos; }

static void CommandTests()
{
  EPDBCmdOptions o;
  EPDBCmdResult r;
  std::string err;
  o.tool = "/opt/epdb/epdb_auth_tool";
  o.database = "/home/me/My DBs/work db.ehd";

  o.operation = EPDB_OP_DB_INFO;
  o.password.file = "/home/me/.secrets/pw.txt";
  Check("db-info with a password file", EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0 &&
	r.command == "/opt/epdb/epdb_auth_tool --password-file=/home/me/.secrets/pw.txt --db-info "
	"--non-interactive '/home/me/My DBs/work db.ehd'", r.command + err);

  o.password.source = EPDB_SECRET_PIPE_AWS;
  o.password.aws_id = "epdb/pw";
  Check("password piped from AWS Secrets Manager", EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0 &&
	r.command.find("aws secretsmanager get-secret-value --secret-id epdb/pw --query SecretString --output text | "
		       "/opt/epdb/epdb_auth_tool --password-fd=0 ") == 0, r.command + err);

  o.operation = EPDB_OP_ADD_RECORD;
  o.record_name = "My Bank";
  o.record_from_file = 0;
  Check("record values and a piped password cannot both use stdin",
	EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) != 0 && Contains(err, "stdin"));
  o.record_from_file = 1;
  o.record_file = "/home/me/rec.txt";
  o.dry_run = 1;
  Check("add-record reads the fields from --record-file, --dry-run added",
	EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0 && Contains(r.command, "'--add-record=My Bank'") &&
	Contains(r.command, "--record-file=/home/me/rec.txt") && Contains(r.command, "--dry-run"), r.command + err);
  Check("the sample input has no real password", Contains(r.input_sample, "(the password)"));
  // 10/27/2026: A line break in a value points to the record file
  o.record_name = "Two\nlines";
  Check("cmd: a line break is refused with a pointer to --record-file",
	EPDBBuildCommand(o, EPDB_SHELL_CMD, r, err) != 0 && Contains(err, "--record-file") && Contains(err, "cmd.exe"), err);
  o.record_name = "My Bank";

  o.operation = EPDB_OP_SHOW_RECORD;
  Check("--dry-run is left out for operations that do not support it",
	EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0 && !Contains(r.command, "--dry-run"), r.command);
  o.dry_run = 0;

  o.operation = EPDB_OP_GET_FIELD;
  o.field_name = "password";
  Check("get-field joins record and field", EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0 &&
	Contains(r.command, "'--get-field=My Bank:password'"), r.command);

  o.auth = EPDB_AUTH_SMART_CARD;
  o.sc_username = "dgaer";
  o.sc_cert_id = "03";
  o.sc_slot = "0x4";
  o.sc_pin.source = EPDB_SECRET_FILE;
  o.sc_pin.file = "/home/me/.secrets/pin.txt";
  Check("smart card: PIN from a file, slot and cert ID", EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0 &&
	Contains(r.command, "--smartcard --smartcard-username=dgaer --smartcard-cert-id=03 --smartcard-slot=0x4 "
		 "--smartcard-pin-file=/home/me/.secrets/pin.txt"), r.command + err);
  Check("smart card: no --smartcard-pin= on the command line", !Contains(r.command, "--smartcard-pin="));

  o.auth = EPDB_AUTH_RSA_KEY;
  o.rsa_key_file = "C:\\Keys\\my key.pem";
  o.rsa_username = "dgaer";
  o.rsa_has_passphrase = 1;
  o.rsa_passphrase.source = EPDB_SECRET_PIPE_CMD;
  o.rsa_passphrase.command = "Get-Content -Raw C:\\Secrets\\pass.txt";
  o.tool = "C:\\Program Files\\Encrypted Password Database\\epdb_auth_tool.exe";
  o.database = "C:\\Users\\Me\\Documents\\work db.ehd";
  o.operation = EPDB_OP_DB_INFO;
  Check("PowerShell: quoted tool path is run with &, passphrase piped",
	EPDBBuildCommand(o, EPDB_SHELL_POWERSHELL, r, err) == 0 &&
	r.command == "Get-Content -Raw C:\\Secrets\\pass.txt | & 'C:\\Program Files\\Encrypted Password Database\\epdb_auth_tool.exe' "
	"'--rsa-key=C:\\Keys\\my key.pem' --rsa-key-username=dgaer --rsa-key-passphrase-fd=0 --db-info --non-interactive "
	"'C:\\Users\\Me\\Documents\\work db.ehd'", r.command + err);
  o.rsa_passphrase.source = EPDB_SECRET_FILE;
  o.rsa_passphrase.file = "C:\\Secrets\\100% pass.txt";
  Check("cmd: double quotes and %% for a batch file", EPDBBuildCommand(o, EPDB_SHELL_CMD, r, err) == 0 &&
	r.command == "\"C:\\Program Files\\Encrypted Password Database\\epdb_auth_tool.exe\" "
	"\"--rsa-key=C:\\Keys\\my key.pem\" --rsa-key-username=dgaer \"--rsa-key-passphrase-file=C:\\Secrets\\100%% pass.txt\" "
	"--db-info --non-interactive \"C:\\Users\\Me\\Documents\\work db.ehd\"", r.command + err);

  o.database = "C:\\Users\\Me\\Documents\\caf\xc3\xa9.ehd";
  int warned = 0;
  if(EPDBBuildCommand(o, EPDB_SHELL_CMD, r, err) == 0) {
    for(size_t i = 0; i < r.notes.size(); i++) if(Contains(r.notes[i], "plain ASCII")) warned = 1;
  }
  Check("Windows shells warn about a non-ASCII file name", warned);
  warned = 0;
  if(EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0) {
    for(size_t i = 0; i < r.notes.size(); i++) if(Contains(r.notes[i], "plain ASCII")) warned = 1;
  }
  Check("bash does not warn about UTF-8", !warned);

  o.auth = EPDB_AUTH_PASSWORD;
  o.password.source = EPDB_SECRET_FILE;
  o.password.file = "/home/me/pw.txt";
  o.operation = EPDB_OP_CHANGE_PASSWORD;
  o.new_password.source = EPDB_SECRET_FILE;
  o.new_password.file = "";
  Check("change-password needs a new password file", EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) != 0);
  o.new_password.file = "/home/me/newpw.txt";
  Check("change-password uses --new-password-file", EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0 &&
	Contains(r.command, "--change-password --new-password-file=/home/me/newpw.txt"), r.command);

  o.operation = EPDB_OP_GENERATE_PASSWORD;
  o.database = "";
  o.generate_length = 32;
  o.generate_symbols = 1;
  Check("generate-password needs no database or password", EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0 &&
	!Contains(r.command, "--password-file") && Contains(r.command, "--generate-password=32 --password-symbols"), r.command + err);

  o.database = "/db.ehd";
  o.operation = EPDB_OP_CHECK_VERSION;
  Check("check-version needs no password", EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) == 0 &&
	!Contains(r.command, "--password"), r.command);

  for(int op = 0; op < EPDB_OP_COUNT; op++) {
    EPDBCmdOptions a;
    a.database = "/db.ehd";
    a.password.file = "/pw.txt";
    a.record_name = "R";
    a.field_name = "notes";
    a.record_file = "/r.txt";
    a.merge_database = "/other.ehd";
    a.merge_password.file = "/pw2.txt";
    a.new_password.file = "/pw3.txt";
    a.user_name = "u";
    a.user_key_file = "/u.pem";
    a.rekey_file = "/rk.txt";
    std::string script;
    int ok = (EPDBBuildCommand(a, EPDB_SHELL_BASH, r, err) == 0);
    if(ok) script = EPDBCommandScript(r, EPDB_SHELL_BASH);
    std::string name = std::string("operation builds: ") + EPDBCmdOperationName(op);
    a.operation = op;
    ok = (EPDBBuildCommand(a, EPDB_SHELL_BASH, r, err) == 0) && !r.command.empty() && Contains(EPDBCommandScript(r, EPDB_SHELL_BASH), "exit $?");
    Check(name.c_str(), ok, err);
  }
}

#if !defined (_WIN32)
static int LiveRun(const EPDBCmdOptions &o, const char *name, int want_rc, std::string &out)
// Builds the bash command, runs it and checks the exit code
{
  EPDBCmdResult r;
  std::string err;
  if(EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) != 0) {
    Check(name, 0, err);
    return -1;
  }
  std::string sh = WriteTemp("live.sh", EPDBCommandScript(r, EPDB_SHELL_BASH));
  std::string cmd = "bash " + sh + " 2>&1; echo \"rc=$?\"";
  out = RunCapture(cmd);
  remove(sh.c_str());
  char want[32];
  snprintf(want, sizeof(want), "rc=%d\n", want_rc);
  int ok = out.size() >= strlen(want) && out.compare(out.size() - strlen(want), strlen(want), want) == 0;
  Check(name, ok, "\n" + r.command + "\n" + out);
  return ok ? 0 : -1;
}

static void LiveTests(const char *tool, const char *db, const char *pwfile, const char *dir)
// Runs the commands the builder makes against a throwaway database
{
  EPDBCmdOptions o;
  std::string out;
  o.tool = tool;
  o.database = db;
  o.password.file = pwfile;
  o.operation = EPDB_OP_DB_INFO;
  LiveRun(o, "live: db-info", 0, out);
  std::string rec = std::string(dir) + "/record with spaces.txt";
  FILE *fp = fopen(rec.c_str(), "w");
  if(fp) {
    fprintf(fp, "title=Live Test 'Bank' $HOME\nusername=jdoe\npassword=Pa$$ w0rd `x`\nnotes=one\\ntwo\n");
    fclose(fp);
  }
  chmod(rec.c_str(), 0600);
  o.operation = EPDB_OP_ADD_RECORD;
  o.record_name = "Live Test 'Bank' $HOME";
  o.record_file = rec;
  o.dry_run = 1;
  LiveRun(o, "live: add-record --dry-run leaves the database unchanged", 0, out);
  o.dry_run = 0;
  LiveRun(o, "live: add-record with a quoted title and a values file", 0, out);
  o.operation = EPDB_OP_GET_FIELD;
  o.field_name = "password";
  o.quiet = 1;
  if(LiveRun(o, "live: get-field", 0, out) == 0) {
    Check("live: get-field returns the stored password", out.find("Pa$$ w0rd `x`") == 0, out);
  }
  o.quiet = 0;
  o.operation = EPDB_OP_SHOW_RECORD;
  if(LiveRun(o, "live: show-record", 0, out) == 0) {
    Check("live: show-record shows the two note lines", out.find("one") != std::string::npos && out.find("two") != std::string::npos, out);
  }
  o.operation = EPDB_OP_EXPORT;
  o.export_format = "json";
  o.output_file = std::string(dir) + "/export out.json";
  remove(o.output_file.c_str());
  LiveRun(o, "live: export json to a file", 0, out);
  o.operation = EPDB_OP_LIST_RECORDS;
  LiveRun(o, "live: db-list", 0, out);
  o.operation = EPDB_OP_LIST_USERS;
  LiveRun(o, "live: list-users", 0, out);
  o.operation = EPDB_OP_CHECK_VERSION;
  LiveRun(o, "live: check-version (v2 database)", 0, out);
  o.operation = EPDB_OP_GENERATE_PASSWORD;
  o.generate_length = 24;
  if(LiveRun(o, "live: generate-password", 0, out) == 0) {
    Check("live: the generated password has 24 characters", out.size() > 24 && out[24] == '\n', out);
  }
  o.operation = EPDB_OP_DELETE_RECORD;
  o.dry_run = 1;
  LiveRun(o, "live: delete-record --dry-run", 0, out);
  o.dry_run = 0;
  LiveRun(o, "live: delete-record --yes", 0, out);
  o.operation = EPDB_OP_GET_FIELD;
  LiveRun(o, "live: the deleted record is not found (exit 3)", 3, out);
  o.operation = EPDB_OP_DB_INFO;
  o.password.file = std::string(dir) + "/no such password file";
  LiveRun(o, "live: a missing password file is an error (exit 1)", 1, out);
  remove(rec.c_str());
}
#endif

int main(int argc, char **argv)
{
  if(argc > 1 && strcmp(argv[1], "--print-args") == 0) {
    for(int i = 2; i < argc; i++) printf("%s\n", Hex(argv[i]).c_str());
    return 0;
  }
#if defined (_WIN32)
  char buf[MAX_PATH];
  GetModuleFileNameA(0, buf, sizeof(buf));
  self_path = buf;
  QuotingTests(EPDB_SHELL_POWERSHELL, "PowerShell");
  QuotingTests(EPDB_SHELL_CMD, "cmd.exe");
#else
  if(argc == 6 && strcmp(argv[1], "--live") == 0) {
    // cmdbuild_test --live tool database password_file work_dir
    LiveTests(argv[2], argv[3], argv[4], argv[5]);
    printf("\n%d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
  }
  char buf[4096];
  ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf)-1);
  if(n > 0) buf[n] = 0;
  self_path = (n > 0) ? buf : argv[0];
  QuotingTests(EPDB_SHELL_BASH, "bash");
#endif
  CommandTests();
  printf("\n%d passed, %d failed\n", passed, failed);
  return failed ? 1 : 0;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
