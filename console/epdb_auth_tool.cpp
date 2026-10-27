// ------------------------------- //
// -------- start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, BCC32, GCC, HPUX aCC, SOLARIS CC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
// Date Last Modified: 10/25/2026
// Copyright (c) 2001-2026 DataReel Software Development
// ----------------------------------------------------------- // 
// ------------- Program Description and Details ------------- // 
// ----------------------------------------------------------- // 
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
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

Encrypted password database auth tool 
*/
// ----------------------------------------------------------- // 
#include "gxdlcode.h"

#if defined (__USE_ANSI_CPP__) // Use the ANSI Standard C++ library
#include <iostream>
using namespace std; // Use unqualified names for Standard C++ library
#else // Use the old iostream library by default
#include <iostream.h>
#endif // __USE_ANSI_CPP__

#include "epdb_functions.h"
#include "fcfile.h"
#include "fckeymgmt.h"
#include "epdb_core.h"
#if defined (__UNIX__)
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#endif
#include "c_thread.h"

#ifdef __MSVC_DEBUG__
#include "leaktest.h"
#endif

// Global vars
gxString program_name = "Encrypted Password Database Auth Tool";
gxString version_str = "2026.103";
gxString copyright = "Copyright (c) Datareel Open Source";
gxString copyright_dates = "2003-2026";
gxString produced_by = "Datareel Open Source";
gxString default_url = "https://datareel.com";
gxString release_date = "10/27/2026";
gxString debug_message;
int debug_mode = 0;
int debug_level = 1;
int verbose_mode = 0;
gxList<gxString> file_list;
MemoryBuffer key;
gxString password;
gxString key_file;
gxString public_rsa_key_file;
int add_rsa_key = 0;
char public_key[RSA_max_keybuf_len];
int use_private_rsa_key = 0;
gxString private_rsa_key_file;
gxString rsa_key_username;
unsigned public_key_len = 0;
char private_key[RSA_max_keybuf_len];
unsigned private_key_len = 0;
gxString rsa_key_passphrase;
int has_passphrase = 0;
int ERROR_LEVEL = 0;
gxString executable_name;
gxString HOMEdir;
gxString USERNAME;

#ifdef __ENABLE_SMART_CARD__
int add_smart_card = 0;
int use_smartcard_cert_file = 0;
gxString smartcard_cert_file;
SmartCardOB sc;
gxString smartcard_username;
int use_smartcard = 0;
#endif // __ENABLE_SMART_CARD__

gxString input_arg_key_file;
int use_password = 0;
int use_key_file = 0;
MemoryBuffer aes_file_decrypt_secret;
int list_users = 0;
int db_stats = 0;
int display_db_config = 0;
gxString add_username;
int db_list = 0;
char delimiter = '\t';
char filter_char = 0;
char replace_char = 0;
char replace_char_with = 0;
int db_find_key = 0;
int upgrade_db = 0; // 10/25/2026: Upgrade to database format v2
int reseal_db = 0;  // 10/25/2026: Accept the current contents of a v2 database
int db_find_all = 0;
gxString db_find_key_name;
// 10/27/2026: Key management, records, access log, version check and self-destruct
int remove_user = 0;
gxString remove_username;
gxUINT32 remove_user_type = 0; // 0 = the only block of the user
int record_op = 0;              // EPDB_RECORD_ADD, _UPDATE or _DELETE
gxString record_name;
gxString record_input_fd;
gxString record_input_file;
int assume_yes = 0;
int rekey = 0;
gxString rekey_input_fd;
gxString rekey_input_file;
gxString rekey_new_password_fd;
gxString rekey_new_password_file;
int access_log = 0;
gxString access_log_file;
int access_log_warned = 0;
int verify_access_log = 0;
int show_access_log = 0;
int access_log_in_file = -1;    // --access-log-in-file=on|off
int check_version = 0;
int self_destruct = -1;         // --self-destruct=on|off
unsigned self_destruct_attempts = 0;
gxList<FCNameValue> record_fields;
FCRekeyInput rekey_in;
// 10/27/2026: Automation and GUI features from the command line
int exit_code = 0;              // FC_EXIT_* of the first failure
int non_interactive = 0;        // --non-interactive, --batch
int quiet_mode = 0;             // --quiet
int json_output = 0;            // --json
int dry_run = 0;                // --dry-run
unsigned lock_timeout = FC_DEF_LOCK_TIMEOUT;
gxString get_field;             // --get-field=record:field
gxString output_file;           // --output-file
gxString show_record_name;      // --show-record
int db_info = 0;                // --db-info
int export_db = 0;              // --export[=csv|json]
int export_json = 0;
int import_db = 0;              // --import
gxString input_format;          // --input-format=text|json|csv
gxString merge_from;            // --merge-from=src.ehd
gxString merge_secret;          // Credential of the merge source
int merge_policy = EPDB_MERGE_FAIL;
int merge_policy_set = 0;
int change_password = 0;        // --change-password
gxString new_password;          // --new-password-fd or --new-password-file
unsigned generate_password = 0; // --generate-password[=length]
int password_symbols = 0;
gxList<EPDBRecord> input_records; // JSON or CSV records from the input
#define NOTE if(!quiet_mode) cerr
void SetExit(int code) { if(!exit_code) exit_code = code; }

// Functions
void DisplayVersion();
void HelpMessage();
int ProcessDashDashArg(gxString &arg);
int ProcessArgs(char *arg);
void WarnSecretOnCommandLine(const char *option, const char *alternatives);
int ReadSecretArg(const gxString &arg, gxString &value, gxString &secret);
int ConsoleIsInteractive();
void WipeSecretArg(char *arg);
int DEBUG_m(char *message, int level = 1, int rv = 0);
int ExitProgram(int rv = 0, char *exit_message = 0);
int check_if_file_exists(gxString &fname);
int check_if_file_exists(char *fname);
void set_delimiter_arg(const gxString &equal_arg, char &delimiter_char);
int KeyManagementOps(const gxString &fname, MemoryBuffer &file_key, int is_v2);
int ExtraOps(const gxString &fname, MemoryBuffer &file_key, int is_v2);
int WillWrite(const gxString &fname);
int DestroyedNow(const gxString &fname);
static gxString AuthSlotName();
int ReadOperationInput();
int CheckVersions();
int SelfDestructPrecheck(const gxString &fname);
void CountFailedOpen(const gxString &fname, const char *what);
void ResetFailedOpens(const gxString &fname, const MemoryBuffer &file_key);
void LogEvent(const gxString &fname, const char *action, const gxString &slot, const gxString &record,
	      int ok, const char *detail, const MemoryBuffer *file_key);

int main(int argc, char **argv)
{
  SecureProcessSetup(); // 10/25/2026: No core dumps with decrypted data

  HOMEdir = getenv("HOME");
  USERNAME = getenv("USER");
  
  executable_name = argv[0];

  if(argc < 2) {
    cerr << "ERROR: You must supply an encrypted password database file name" << "\n" << flush;
    HelpMessage();
    return ExitProgram(1);
  }

  int narg;
  char *arg = argv[narg = 1];
  gxString fn;
  int num_files = 0;
  int num_operations = 0;
  
  if(argc >= 2) {
    while (narg < argc) {
      if (arg[0] == '\0') { // 10/25/2026: An empty argument looped forever
	arg = argv[++narg];
	continue;
      }
      if (arg[0] != '\0') {
	if (arg[0] == '-') { // Look for command line arguments
	  int arg_ok = ProcessArgs(arg);
	  WipeSecretArg(argv[narg]); // Remove secrets from the process arguments
	  if(!arg_ok) return ExitProgram(1); // Exit if argument is not valid
	}
	else { 
	  fn = arg;
	  if(futils_isdirectory(fn.c_str())) {
	    cerr << "Encountered fatal error processing file names" << "\n";
	    cerr << fn.c_str() << "is a directory name" << "\n";
	    return ExitProgram(1);
	  }
	  else {
	    num_files++;
	    file_list.Add(fn);
	  }
	}
	arg = argv[++narg];
      }
    }
  }

  // 10/27/2026: The password generator needs no database
  if(generate_password) {
    gxString pw;
    if(EPDBGeneratePassword(generate_password, password_symbols, pw) != 0) {
      cerr << "ERROR: Cannot generate a password" << "\n" << flush;
      return ExitProgram(FC_EXIT_USAGE);
    }
    int rv = 0;
    if(!output_file.is_null()) {
      gxString werr;
      if(FC_write_secret_file(output_file.c_str(), pw, werr) != 0) {
	cerr << "ERROR: " << werr.c_str() << "\n" << flush;
	rv = FC_EXIT_IO;
      }
    }
    else if(json_output) cout << "{\"password\": " << FC_json_string(pw).c_str() << "}" << "\n" << flush;
    else cout << pw.c_str() << "\n" << flush;
    SecureWipe(pw);
    if(num_files == 0) return ExitProgram(rv);
  }

  if(num_files == 0) {
    cerr << "ERROR: No file name specified with command line args" << "\n";
    return ExitProgram(1);
  }

  if(epdb_output_null && epdb_output_mode != EPDB_OUTPUT_FIELD) {
    cerr << "ERROR: The --null arg requires --field" << "\n";
    return ExitProgram(1);
  }
  if(replace_char != 0 && replace_char_with == 0) {
    cerr << "ERROR: The --replace-char arg requires --replace-char-with" << "\n";
    return ExitProgram(1);
  }
  if(replace_char == 0 && replace_char_with != 0) {
    cerr << "ERROR: The --replace-char-with arg requires --replace-char" << "\n";
    return ExitProgram(1);
  }
  
  // 10/27/2026: No credential and no smart card for the version check
  if(check_version) return ExitProgram(CheckVersions());
  if(access_log && access_log_file.is_null()) access_log_file = FC_default_log_file(".encrypted_password_database");
  if(verify_access_log && show_access_log) {
    cerr << "ERROR: Use --show-access-log and --verify-access-log in separate runs" << "\n";
    return ExitProgram(1);
  }
  // 10/27/2026: --change-password is a rekey with a new password
  int change_password_reads_input = 0;
  if(change_password) {
    rekey = 1;
    change_password_reads_input = !rekey_input_fd.is_null() || !rekey_input_file.is_null() || !FC_stdin_is_terminal();
  }
  // Record fields and rekey input are read once, before any prompt
  if((!change_password || change_password_reads_input) && !ReadOperationInput()) return ExitProgram(1);
  if(change_password) {
    if(new_password.is_null()) {
      if(non_interactive || !ConsoleIsInteractive()) {
	cerr << "ERROR: --change-password needs the new password from --new-password-fd or --new-password-file" << "\n" << flush;
	return ExitProgram(FC_EXIT_USAGE);
      }
      gxString again;
      cerr << "New password: " << flush;
      int ok = consoleGetString(new_password, 1);
      cerr << "\n" << "Retype the new password: " << flush;
      ok = ok && consoleGetString(again, 1);
      cerr << "\n" << flush;
      if(!ok || new_password != again) {
	SecureWipe(again);
	cerr << "ERROR: The passwords do not match" << "\n" << flush;
	return ExitProgram(FC_EXIT_USAGE);
      }
      SecureWipe(again);
    }
    rekey_in.new_password = new_password;
    SecureWipe(new_password);
  }

  gxString sbuf;
  gxString err_string;
  gxString fname;
  gxListNode<gxString> *ptr = file_list.GetHead();

  ERROR_LEVEL = 0;

  if(use_key_file) {
    if(clientcfg->verbose_mode) cerr << "Using key file for decryption" << "\n" << flush;
    aes_file_decrypt_secret.Clear(1);
    aes_file_decrypt_secret = key;
    DBStringConfig::crypt_key = aes_file_decrypt_secret;
  }
  else if(use_private_rsa_key) {
    if(rsa_key_username.is_null()) {
      rsa_key_username = USERNAME;
      if(rsa_key_username.is_null()) {
	cerr << "ERROR: --rsa-key requires --rsa-key-username" << "\n" << flush;
	return ExitProgram(1);
      }
      else {
	cerr << "INFO: No --rsa-key-username was supplied, using " << USERNAME.c_str() << "\n" << flush;
      }
    }
    if(clientcfg->verbose_mode) cerr << "Using private RSA key file for decryption" << "\n" << flush;
    if(has_passphrase && rsa_key_passphrase.is_null()) {
      if(non_interactive) {
	cerr << "ERROR: The RSA key has a passphrase, give it with --rsa-key-passphrase-fd or --rsa-key-passphrase-file (--non-interactive)" << "\n" << flush;
	return ExitProgram(FC_EXIT_USAGE);
      }
      cerr << "RSA key passphrase: " << flush;
      if(!consoleGetString(rsa_key_passphrase, 1)) {
	rsa_key_passphrase.Clear(1);
	cerr << "\n" << flush;
	cerr << "Invalid entry!" << "\n" << flush;
	return ExitProgram(1);
      }
      cerr << "\n" << flush;
    }
  }
#ifdef __ENABLE_SMART_CARD__
  else if(use_smartcard) {
    if(smartcard_username.is_null()) {
      smartcard_username = USERNAME;
      if(smartcard_username.is_null()) {
	cerr << "ERROR: --smartcard-cert requires --smartcard-username" << "\n" << flush;
	return ExitProgram(1);
      }
      else {
	cerr << "INFO: No --smartcard-username was supplied, using " << USERNAME.c_str() << "\n" << flush;
      }
    }
    if(clientcfg->verbose_mode) cerr << "Using smart card cert for decryption" << "\n" << flush;

    if(clientcfg->verbose_mode) {
      // 10/26/2026: OpenSSL 3 builds (RHEL 9, RHEL 10 and Windows) load the
      // PKCS#11 module directly and have no engine
      if(SC_uses_engine()) {
	cerr << "Smart card engine ID = " << sc.engine_ID << "\n" << flush;
	cerr << "Smart card engine path = " << sc.enginePath << "\n" << flush;
      }
      cerr << "Smart card module path = " << sc.modulePath << "\n" << flush;
      cerr << "Smart card cert ID = " << sc.cert_id << "\n"  << flush;
      if(sc.has_slot) cerr << "Smart card slot = " << sc.slot_id << "\n"  << flush;
    }

    if(SC_uses_engine() && (!futils_exists(sc.enginePath) || !futils_isfile(sc.enginePath))) {
      cerr << "ERROR: Smart card engine " << sc.enginePath << " does not exist or cannot be read" <<  "\n" << flush;
      return ExitProgram(1);
    }
    if(!futils_exists(sc.modulePath) || !futils_isfile(sc.modulePath)) {
      cerr << "ERROR: Smart card provider " << sc.modulePath << " does not exist or cannot be read" <<  "\n" << flush;
      return ExitProgram(1);
    }

    if(debug_mode) sc.verbose_mode = 1;
    gxString pin_buf;
    // 10/26/2026: A PIN that does not fit is refused, never cut short
    if(sc.pin_too_long) {
      cerr << "ERROR: The smart card PIN is longer than " << (SC_max_pin_size-1) << " characters, the PIN was not sent" << "\n" << flush;
      return ExitProgram(1);
    }
    if(sc.pin[0] == 0) {
      if(non_interactive) {
	cerr << "ERROR: Give the smart card PIN with --smartcard-pin-fd or --smartcard-pin-file (--non-interactive), the PIN was not sent" << "\n" << flush;
	return ExitProgram(FC_EXIT_USAGE);
      }
      cerr << "Smart card PIN: " << flush;
      if(!consoleGetString(pin_buf, 1)) {
	pin_buf.Clear(1);
	cerr << "\n" << flush;
	cerr << "Invalid entry!" << "\n" << flush;
	return ExitProgram(1);
      }
      cerr << "\n" << flush;
      // 10/26/2026: Never send an empty or cut short PIN, it uses up a try
      if(pin_buf.is_null() || pin_buf.length() >= SC_max_pin_size) {
	pin_buf.Clear(1);
	cerr << "ERROR: The smart card PIN is empty or too long, the PIN was not sent" << "\n" << flush;
	return ExitProgram(1);
      }
      sc.SetPin(pin_buf.c_str());
      pin_buf.Clear(1);
    }
  }
#endif // __ENABLE_SMART_CARD__
  else if(use_password) {
    if(password.is_null()) {
      if(non_interactive) {
	cerr << "ERROR: Give the password with --password-fd or --password-file (--non-interactive)" << "\n" << flush;
	return ExitProgram(FC_EXIT_USAGE);
      }
      cerr << "Password: " << flush;
      if(!consoleGetString(password, 1)) {
	cerr << "\n" << flush;
	password.Clear(1);
	cerr << "Invalid entry!" << "\n" << flush;
	return ExitProgram(1);
      }
      cerr << "\n" << flush;
    }
    aes_file_decrypt_secret.Clear(1);
    aes_file_decrypt_secret.Cat(password.GetSPtr(), password.length());
    DBStringConfig::crypt_key = aes_file_decrypt_secret;
  }
  else if(list_users || db_stats) {
    DEBUG_m("No auth operation");
  }
  else {
    if(clientcfg->verbose_mode) cerr << "No decryption method specifed, defaulting to password" << "\n" << flush;
    if(non_interactive) {
      cerr << "ERROR: No credential given: use --password-fd, --password-file, --key, --rsa-key or --smartcard (--non-interactive)" << "\n" << flush;
      return ExitProgram(FC_EXIT_USAGE);
    }
    cerr << "Password: " << flush;
    if(!consoleGetString(password, 1)) {
      cerr << "\n" << flush;
      password.Clear(1);
      cerr << "Invalid entry!" << "\n" << flush;
      return ExitProgram(1);
    }
    cerr << "\n" << flush;
    aes_file_decrypt_secret.Clear(1);
    aes_file_decrypt_secret.Cat(password.GetSPtr(), password.length());
    DBStringConfig::crypt_key = aes_file_decrypt_secret;
  }

  while(ptr) {
    fname = ptr->data;
    MemoryBuffer file_key; // 10/25/2026: Key for this file, a v2 keyset or the v1 secret
    if(!check_if_file_exists(fname)) {
      // 10/25/2026: A missing file is an error, scripts must not see success
      cerr << "ERROR: Encrypted password database file " << fname.c_str() << " does not exist or cannot be read" << "\n" << flush;
      SetExit(FC_EXIT_NOT_FOUND);
      ERROR_LEVEL++;
      ptr = ptr->next;
      continue; 
    }

    if(list_users && json_output) { // 10/27/2026
      EPDBInfo info;
      gxString ierr;
      if(EPDBGetInfo(fname.c_str(), 0, info, ierr) != 0) {
	cerr << "ERROR: " << ierr.c_str() << "\n" << flush;
	SetExit(FC_EXIT_IO);
	ERROR_LEVEL++;
      }
      else cout << EPDBInfoJSON(fname.c_str(), info).c_str() << "\n" << flush;
    }
    else if(list_users) {
      DEBUG_m("No auth list users operation");
      if(ListUsers(fname.c_str()) != 0) ERROR_LEVEL++;
    }
    if(db_stats) {
      DEBUG_m("No auth display DB stats operation");
      if(PrintDBStats(fname.c_str()) != 0) ERROR_LEVEL++;
    }
    if(list_users || db_stats) { // complete all non-auth ops first and exit program 
      ptr = ptr->next;
      continue; 
    }

    // 10/27/2026: A destroyed database, or self-destruct on a file that
    // cannot be written, is refused before any credential is tried
    // 10/27/2026: Writers take the advisory lock for the whole file
    // operation, including the failed open counter of self-destruct
    FCLock db_lock;
    if(WillWrite(fname)) {
      gxString lerr;
      int lrv = FC_lock_file(fname.c_str(), lock_timeout, db_lock, lerr);
      if(lrv != 0) {
	cerr << "ERROR: " << lerr.c_str() << "\n" << flush;
	SetExit(lrv == 1 ? FC_EXIT_LOCKED : FC_EXIT_IO);
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue;
      }
    }
    if(!SelfDestructPrecheck(fname)) {
      LogEvent(fname, "open", AuthSlotName(), gxString(), 0, "refused before trying a credential", 0);
      ERROR_LEVEL++;
      ptr = ptr->next;
      continue;
    }
    {
      FCVersionInfo vi; // 10/27/2026: Warn about v1 databases still in use
      gxString verr;
      if(FC_check_version(fname.c_str(), vi, verr) == 0 && vi.is_v1 && !upgrade_db) {
	NOTE << FC_v1_warning(fname.c_str(), vi).c_str() << "\n" << flush;
      }
    }

    if(use_private_rsa_key) {
      DatabaseUserAuth db_auth;
      gxDatabase *f = OpenEPDB(fname.c_str(), err_string);
      if(!f) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      db_auth.f = f;
      char *passphrase = 0;
      if(!rsa_key_passphrase.is_null()) passphrase = (char *)rsa_key_passphrase.GetSPtr();
      if(db_auth.DecryptWithRSAKey(private_key, private_key_len, rsa_key_username, passphrase) != 0) {
	cerr << "ERROR: " << db_auth.err.c_str() << "\n";
	f->Close();
	delete f;
	f = 0;
	LogEvent(fname, "open", AuthSlotName(), gxString(), 0, "RSA key did not open the database", 0);
	CountFailedOpen(fname, "The RSA key did not open the database");
	SetExit(DestroyedNow(fname) ? FC_EXIT_DESTROYED : FC_EXIT_AUTH);
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue;
      }
      file_key = DBStringConfig::crypt_key;
      f->Close();
      delete f;
    }

#ifdef __ENABLE_SMART_CARD__
    if(use_smartcard) {
      DatabaseUserAuth db_auth;
      gxDatabase *f = OpenEPDB(fname.c_str(), err_string);
      if(!f) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      db_auth.f = f;
      int sc_rv = db_auth.DecryptWithSmartcard(&sc, smartcard_username);
      if(sc_rv != 0 && sc.error_level == SC_ERROR_PIN_COUNT_LOW && !sc.allow_pin_count_low) {
	// 10/26/2026: The PIN was not sent. Ask before sending it to a card
	// that reports low PIN tries, only when a person is at the console.
	cerr << "WARNING: " << sc.err_string << "\n" << flush;
	if(ConsoleIsInteractive() && !non_interactive) {
	  cerr << "Check the PIN tries left (for example in ActivClient, open the card and view its smart card info) and be sure of the PIN." << "\n" << flush;
	  cerr << "Send the PIN to the card? [y/N]: " << flush;
	  gxString answer;
	  if(consoleGetString(answer, 0) && (answer == "y" || answer == "Y" || answer == "yes")) {
	    sc.allow_pin_count_low = 1;
	    sc_rv = db_auth.DecryptWithSmartcard(&sc, smartcard_username);
	  }
	  cerr << "\n" << flush;
	}
	else {
	  cerr << "Not interactive, use --smartcard-allow-pin-count-low to send the PIN" << "\n" << flush;
	}
      }
      if(sc_rv != 0) {
	cerr << "ERROR: " << db_auth.err.c_str() << "\n";
	LogEvent(fname, "open", AuthSlotName(), gxString(), 0, "smart card did not open the database", 0);
	// 10/27/2026: A wrong PIN, or a card key that does not open the
	// database, is one failed open. A PIN that was not sent is not.
	if(sc.error_level == SC_ERROR_PIN_INCORRECT || !SC_is_pin_error(sc.error_level)) {
	  if(sc.error_level != SC_ERROR || db_auth.err.Find("does not hold") != -1 ||
	     db_auth.err.Find("authentication failed") != -1) {
	    CountFailedOpen(fname, "The smart card did not open the database");
	  }
	}
	if(sc.error_level == SC_ERROR_PIN_FINAL_TRY) {
	  cerr << "Use --smartcard-allow-final-try only if you are sure of the PIN, a wrong PIN locks the card" << "\n" << flush;
	}
	SetExit(DestroyedNow(fname) ? FC_EXIT_DESTROYED : FC_EXIT_AUTH);
	ERROR_LEVEL++;
	f->Close();
	delete f;
	// 10/26/2026: Stop here, the PIN may be wrong and CAC and PIV
	// cards lock after a few wrong PINs. Never try it on another file.
	OPENSSL_cleanse(sc.pin, sizeof(sc.pin));
	break;
      }
      file_key = DBStringConfig::crypt_key;
      f->Close();
      delete f;
    }
 #endif // __ENABLE_SMART_CARD__

    // 10/25/2026: Check the key against the database before using it. A
    // wrong key decrypted every field to an empty string and the search
    // matched every record. For a v2 database the password or key is
    // turned into the database keyset here, and the database MAC and
    // index are checked.
    int is_v2 = 0;
    {
      gxDatabase *f = OpenEPDB(fname.c_str(), err_string);
      if(!f) {
	cerr << "ERROR: " << err_string.c_str() << "\n" << flush;
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      MemoryBuffer unlocked;
      int aes_error = 0;
      int uses_block = use_private_rsa_key;
#ifdef __ENABLE_SMART_CARD__
      if(use_smartcard) uses_block = 1;
#endif
      if(EPDBUnlock(f, uses_block ? file_key : aes_file_decrypt_secret, unlocked, err_string, aes_error) != 0) {
	cerr << "ERROR: " << err_string.c_str() << "\n" << flush;
	f->Close();
	delete f;
	f = 0;
	LogEvent(fname, "open", AuthSlotName(), gxString(), 0, "credential did not open the database", 0);
	if(aes_error) CountFailedOpen(fname, use_key_file ? "The key file did not open the database" : "The password did not open the database");
	SetExit(DestroyedNow(fname) ? FC_EXIT_DESTROYED : (aes_error ? FC_EXIT_AUTH : FC_EXIT_IO));
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue;
      }
      file_key = unlocked;
      unlocked.Clear(1);
      DBStringConfig::crypt_key = file_key;
      is_v2 = EPDBIsKeyset(file_key);
      f->Close();
      delete f;
    }

    if(upgrade_db) {
      num_operations++;
      if(is_v2) {
	cerr << "ERROR: " << fname.c_str() << " is already a v2 database" << "\n" << flush;
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue;
      }
      cerr << "Upgrading " << fname.c_str() << " to database format v2..." << "\n" << flush;
      gxString backup_file;
      if(EPDBUpgrade(fname.c_str(), file_key, backup_file, err_string) != 0) {
	cerr << "ERROR: Upgrade failed, the database was not changed: " << err_string.c_str() << "\n" << flush;
	LogEvent(fname, "upgrade", AuthSlotName(), gxString(), 0, "not changed", 0);
	ERROR_LEVEL++;
      }
      else {
	LogEvent(fname, "upgrade", AuthSlotName(), gxString(), 1, "v1 to v2", 0);
	cerr << "Upgraded " << fname.c_str() << " to database format v2" << "\n" << flush;
	cerr << "The v1 database was saved to " << backup_file.c_str() << " and "
	     << ReplaceFileExtension(backup_file, ".ehx").c_str() << "\n" << flush;
      }
      ptr = ptr->next;
      continue;
    }

    if(reseal_db) {
      num_operations++;
      if(!is_v2) {
	cerr << "ERROR: --reseal-db only applies to v2 databases" << "\n" << flush;
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue;
      }
      gxDatabase *f = OpenEPDB(fname.c_str(), err_string, gxDBASE_READWRITE);
      if(!f || EPDBSeal(f, file_key, err_string) != 0) {
	cerr << "ERROR: Cannot reseal " << fname.c_str() << " " << err_string.c_str() << "\n" << flush;
	ERROR_LEVEL++;
      }
      else {
	cerr << "WARNING: Resealed " << fname.c_str() << ". Its current contents, including any changes made "
	     << "outside this program, are now accepted as valid." << "\n" << flush;
      }
      if(f) {
	f->Close();
	delete f;
      }
      ptr = ptr->next;
      continue;
    }

    if(is_v2) {
      POD *pod = OpenEPDB_POD(fname, err_string);
      int rv = pod ? EPDBVerify(pod, file_key, err_string) : -1;
      if(pod) delete pod;
      if(rv != 0) {
	cerr << "ERROR: " << fname.c_str() << ": " << err_string.c_str() << "\n" << flush;
	cerr << "If you know why the file changed, for example this program stopped while writing, "
	     << "--reseal-db accepts the current contents." << "\n" << flush;
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue;
      }
    }

    // 10/27/2026: The open worked, reset the failed open counter and log it
    ResetFailedOpens(fname, file_key);
    LogEvent(fname, "open", AuthSlotName(), gxString(), 1, 0, &file_key);
    {
      int n = KeyManagementOps(fname, file_key, is_v2);
      if(n < 0) {
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue;
      }
      num_operations += n;
      n = ExtraOps(fname, file_key, is_v2);
      if(n < 0) {
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue;
      }
      num_operations += n;
    }

    if(display_db_config) {
      if(PrintDBConfig(fname.c_str(), file_key) != 0) ERROR_LEVEL++;
      ptr = ptr->next;
      continue; // Display the db config for all files and exit program 
    }

    if(add_rsa_key) {
      num_operations++;
      if(add_username.is_null()) {
	cerr << "ERROR: --add-rsa-key requires --add-username" << "\n" << flush;
	return ExitProgram(1);
      }
      cerr << "Adding RSA key for user " << add_username.c_str() << " to encrypted file " << ptr->data.c_str() << "\n" << flush;
      DatabaseUserAuth db_auth;
      gxDatabase *f = OpenEPDB(fname.c_str(), err_string, gxDBASE_READWRITE);
      if(!f) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      db_auth.f = f;
      if(db_auth.AddRSAKeyToStaticArea(file_key, public_key, public_key_len, add_username) != 0) {
	cerr << "ERROR: Cannot add public RSA key " << db_auth.err.c_str() << "\n" << flush;
	ERROR_LEVEL++;
	f->Close();
	delete f;
	ptr = ptr->next;
	continue; 
      }
      cerr << "Public RSA key for user " << add_username.c_str() << " added to " << ptr->data.c_str() << "\n" << flush;
      f->Close();
      delete f;
    }

 #ifdef __ENABLE_SMART_CARD__
    if(add_smart_card) {
      num_operations++;
      if(add_username.is_null()) {
	cerr << "ERROR: --add-smartcard-cert requires --add-username" << "\n" << flush;
	return ExitProgram(1);
      }
      if(!use_smartcard_cert_file && (!futils_exists(sc.modulePath) || !futils_isfile(sc.modulePath))) {
	cerr << "ERROR: Smart card provider " << sc.modulePath << " does not exist or cannot be read" <<  "\n" << flush;
	return ExitProgram(1);
      }
      cerr << "Adding smartcard cert for user " << add_username.c_str() << " to encrypted file " << ptr->data.c_str() << "\n" << flush;
      if(!use_smartcard_cert_file && clientcfg->verbose_mode) {
	cerr << "Reading the public key for cert ID " << sc.cert_id << " from the smart card with " << sc.modulePath << "\n" << flush;
      }
      DatabaseUserAuth db_auth;
      gxDatabase *f = OpenEPDB(fname.c_str(), err_string, gxDBASE_READWRITE);
      if(!f) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      db_auth.f = f;
      
      if(db_auth.AddSmartCardCertToStaticArea(&sc, use_smartcard_cert_file, file_key, add_username) != 0){
	cerr << "ERROR: Cannot add smart card cert " << db_auth.err.c_str() << "\n";
	ERROR_LEVEL++;
	f->Close();
	delete f;
	ptr = ptr->next;
	continue; 
      }
      cerr << "Smart card cert for user " << add_username.c_str() << " added to " << ptr->data.c_str() << "\n" << flush;
      f->Close();
      delete f;
    }
#endif // __ENABLE_SMART_CARD__

    if(db_list && !json_output) {
      num_operations++;
      int admin_rights;
      POD *pod = OpenEPDB_POD(fname, err_string);
      if(!pod) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      int read_errors = 0;
      if(ExportEPDB(pod, delimiter, filter_char, replace_char, replace_char_with, &read_errors) <= 0) {
	cerr << "No objects exported from the encrypted database" << "\n" << flush;
	ERROR_LEVEL++;
      }
      if(read_errors || epdb_btree_errors) ERROR_LEVEL++;
      delete pod;
    }

    if(db_find_key && !json_output) {
      num_operations++;
      int admin_rights;
      POD *pod = OpenEPDB_POD(fname, err_string);
      if(!pod) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      INFOHOG_t key(db_find_key_name.c_str());
      SearchEPDB(pod->Index(), 0, pod, key, db_find_all);
      int read_errors = 0;
      if(epdb_btree_errors) {
	ERROR_LEVEL++;
      }
      else if(ExportListObjectsEPDB(pod, delimiter, filter_char, replace_char, replace_char_with, &read_errors) <= 0) {
	cerr << "No objects found matching " << db_find_key_name.c_str() << " key name" << "\n" << flush;
	ERROR_LEVEL++;
      }
      if(read_errors) ERROR_LEVEL++;
      delete pod;
    }
    
    if(num_operations == 0) {
      NOTE << "INFO: File " << ptr->data.c_str() << " opened, no operations performed" << "\n" << flush;
    }
    ptr = ptr->next;
  }

  // 10/25/2026: Exit with 0 or 1. The error count wrapped to 0 at 256.
  // 10/27/2026: The FC_EXIT_* code of the first failure
  return ExitProgram(exit_code ? exit_code : (ERROR_LEVEL ? 1 : 0));
}

int ExitProgram(int rv, char *exit_message)
{
  // Clear and destory all global buffers
  aes_file_decrypt_secret.Clear(1);
  key.Clear(1);
  password.Clear(1);
  memset(public_key, 0, sizeof(public_key));
  memset(private_key, 0, sizeof(private_key));
  rsa_key_passphrase.Clear(1);
  FC_wipe_name_values(record_fields); // 10/27/2026: Record values
  rekey_in.Wipe();

  if(!debug_message.is_null()) DEBUG_m(debug_message.c_str(), debug_level);

  if(debug_mode) {
#ifdef __ENABLE_SMART_CARD__
    cerr << "Smart card: " << sc.err_string << "\n" << flush;
#endif // __ENABLE_SMART_CARD__
    char err[1024];
    memset(err, 0, sizeof(err));
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    cerr << "Openssl " << err << "\n" << flush;
  }

  if(exit_message) { // 10/25/2026: Was if(!exit_message)
    cerr << exit_message << "\n" << flush;
  }
  return rv;
}

void DisplayVersion()
{
  cout << "\n" << flush;
  cout << program_name.c_str() 
       << " version " << version_str.c_str();
  cout << "\n" << flush;
  cout << copyright.c_str() << " " 
       << copyright_dates.c_str() << "\n" << flush;
  cout << "Produced by: " << produced_by.c_str() << "\n" << flush;
  cout << default_url.c_str() << "\n" << flush;
  cout << "Release date: " << release_date.c_str() << "\n" << flush;
  cout << "\n" << flush;
  cout << "Encryption engine: Openssl" << "\n" << flush;
  cout << "Version string: " << OPENSSL_VERSION_TEXT << "\n" << flush;
  cout << "Version number: 0x" << hex << OPENSSL_VERSION_NUMBER << "\n" << flush;
  cout << "\n" << flush;
#ifdef __ENABLE_SMART_CARD__
  if(SC_uses_engine()) {
    cout << "Smart card enabled for " << SC_get_default_engine_ID() << "\n" << flush;
  }
  else {
    cout << "Smart card enabled for PKCS#11" << "\n" << flush;
  }
#endif // __ENABLE_SMART_CARD__
}

void HelpMessage() 
{
  DisplayVersion();
  cout << "\n" << flush;
  cout << "Usage: " << executable_name.c_str() << " [switches] " << "dbfile.ehd" << "\n" << flush;
  cout << "Switches: -?  Display this help message and exit." << "\n" << flush;
  cout << "          -d  Enable debugging output" << "\n" << flush;
  cout << "          -h  Display this help message and exit." << "\n" << flush;
  cout << "          -v  Enable verbose messages to the console" << "\n" << flush;
  cout << "\n" << flush;
  cout << "          --debug (Turn on debugging and set optional level)" << "\n" << flush;
  cout << "          --verbose (Turn on verbose output)" << "\n" << flush;
  cout << "          --version (Display program version number)" << "\n" << flush;
  cout << "          --help (Display this help message and exit." << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB authentication methods:\n" << flush;
  cout << "          --key=aes_key (Use a key file for symmetric file decryption)" << "\n" << flush;
  cout << "          --password (Use a password for symmetric file decryption, prompts for the password)" << "\n" << flush;
  cout << "          --password-file=file (Read the password from the first line of a file only you can read)" << "\n" << flush;
  cout << "          --password-fd=n (Read the password from the first line of open file descriptor n)" << "\n" << flush;
  cout << "          --rsa-key (Use a private RSA key file for decryption)" << "\n" << flush;
  cout << "          --rsa-key-passphrase=pass (Passphrase for private RSA key, visible to other users, not recommended)" << "\n" << flush;
  cout << "          --rsa-key-passphrase-file=file (Read the private RSA key passphrase from a file only you can read)" << "\n" << flush;
  cout << "          --rsa-key-passphrase-fd=n (Read the private RSA key passphrase from open file descriptor n)" << "\n" << flush;
  cout << "          --rsa-key-username=name (Username that owns the private RSA key, defaults to current user)" << "\n" << flush;

#ifdef __ENABLE_SMART_CARD__
  cout << "          --smartcard (Use a smart card for decryption)" << "\n" << flush;
  cout << "          --smartcard-pin=pin (Smart card PIN, visible to other users, not recommended)" << "\n" << flush;
  cout << "          --smartcard-pin-file=file (Read the smart card PIN from a file only you can read)" << "\n" << flush;
  cout << "          --smartcard-pin-fd=n (Read the smart card PIN from open file descriptor n)" << "\n" << flush;
  cout << "          --smartcard-username=name (Username assigned to the smart card cert, defaults to current user)" << "\n" << flush;
  cout << "          --smartcard-allow-pin-count-low (Send the PIN when the card reports low PIN tries, asks first if interactive)" << "\n" << flush;
  cout << "          --smartcard-allow-final-try (Send the PIN when the card reports one try left, a wrong PIN locks the card)" << "\n" << flush;
  cout << "                 The PIN is sent at most once per run, it is never retried after a failure" << "\n" << flush;
  cout << "                 List the cards, slots and cert IDs without a PIN with smartcard_list.sh or smartcard_list.bat" << "\n" << flush;
  cout << "\n" << flush;
  cout << "Smart card settings:\n" << flush;
  cout << "          --smartcard-cert-id=" << SC_get_default_cert_id() << " (Set the ID number for the smartcard cert)" << "\n" << flush;
  cout << "                 Also id_01, label_name, slot_N-id_01, slot_N-label_name or \"pkcs11:id=%01\"" << "\n" << flush;
  if(SC_uses_engine()) cout << "          --smartcard-engine=" << SC_get_default_enginePath() << " (Set the smartcard engine path)" << "\n" << flush;
  cout << "          --smartcard-provider=" << SC_get_default_modulePath() << " (Set the smartcard PKCS#11 module)" << "\n" << flush;
  cout << "          --smartcard-slot=N (Use only the card in PKCS#11 slot N, decimal or 0x hex, see pkcs11-tool --list-slots)" << "\n" << flush;
#endif // __ENABLE_SMART_CARD__

  cout << "\n" << flush;
  cout << "DB add user functions:\n" << flush;
  cout << "          --add-rsa-key (Add access to database for another users public RSA key)" << "\n" << flush;
#ifdef __ENABLE_SMART_CARD__
  cout << "          --add-smartcard-cert (Add access to database for the smart card in the reader, uses --smartcard-cert-id)" << "\n" << flush;
  cout << "          --add-smartcard-cert-file=file.pem (Add access to database for another users exported smart card cert)" << "\n" << flush;
  cout << "          --add-username (Username being added for public RSA key file or smart card cert)" << "\n" << flush;
#else
  cout << "          --add-username (Username being added for public RSA key file)" << "\n" << flush;
#endif // __ENABLE_SMART_CARD__
  cout << "\n" << flush;
  cout << "DB remove user and rekey functions (v2 databases):\n" << flush;
  cout << "          --remove-user=name (Remove the RSA key or smart card access of a user, a backup copy is kept)" << "\n" << flush;
  cout << "          --remove-user-type=rsa|smartcard (Which access to remove when the user has both)" << "\n" << flush;
  cout << "                 Removing a user does not revoke a database key the user already recovered, run --rekey after." << "\n" << flush;
  cout << "                 The last RSA or smart card user can only be removed after opening with the password or key file." << "\n" << flush;
  cout << "          --rekey (New master key: every field is encrypted again and every kept user is rewrapped)" << "\n" << flush;
  cout << "          --rekey-file=file (Read the rekey input from a file only you can read, default stdin)" << "\n" << flush;
  cout << "          --rekey-fd=n (Read the rekey input from open file descriptor n)" << "\n" << flush;
  cout << "                 Rekey input, one name=value per line:" << "\n" << flush;
  cout << "                   password=current password (implied when you open with --password or --key)" << "\n" << flush;
  cout << "                   new-password=new password, or drop-password=yes (only RSA and smart card users can open)" << "\n" << flush;
  cout << "                   rsa:user=public_key.pem, smartcard:user=cert.pem|cert.cer or smartcard:user=card (no PIN)" << "\n" << flush;
  cout << "                   drop-rsa:user=yes, drop-smartcard:user=yes" << "\n" << flush;
  cout << "                 Every RSA and smart card user needs a line, the rekey lists what is missing and changes nothing." << "\n" << flush;
  cout << "          --rekey-new-password-file=file (Generate a new database password and write it to a new file)" << "\n" << flush;
  cout << "          --rekey-new-password-fd=n (Generate a new database password and write it to open file descriptor n)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB record functions (field values are read from stdin, never the command line):\n" << flush;
  cout << "          --add-record=name (Add a record, the fields are read as name=value lines)" << "\n" << flush;
  cout << "          --update-record=name (Change only the fields given, an empty value clears a field, title= renames)" << "\n" << flush;
  cout << "          --delete-record=name (Delete a record, asks first at a terminal)" << "\n" << flush;
  cout << "          --yes (Delete without asking)" << "\n" << flush;
  cout << "          --record-file=file (Read the fields from a file only you can read instead of stdin)" << "\n" << flush;
  cout << "          --record-fd=n (Read the fields from open file descriptor n instead of stdin)" << "\n" << flush;
  cout << "                 Fields: title, username, password, previous-password, security-questions," << "\n" << flush;
  cout << "                 account-information, website-link, notes (or field1 to field8, or the column name)." << "\n" << flush;
  cout << "                 In values \\n is a newline, \\t a tab and \\\\ a backslash. Lines starting with # are skipped." << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB access log functions:\n" << flush;
  cout << "          --access-log[=file] (Log opens and changes, default " << FC_default_log_file(".encrypted_password_database").c_str() << ")" << "\n" << flush;
  cout << "          --no-access-log (Do not log to the per-user log, the default)" << "\n" << flush;
  cout << "          --access-log-in-file=on|off (Also keep the last " << EPDB_RING_ENTRIES << " opens and changes, encrypted, in the database file)" << "\n" << flush;
  cout << "          --show-access-log (List the in-file log and the per-user log entries of the database)" << "\n" << flush;
  cout << "          --verify-access-log (Check both logs for changed, removed or inserted entries)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB self-destruct (off by default, v2 databases):\n" << flush;
  cout << "          --self-destruct=on|off (Destroy the records after too many failed opens)" << "\n" << flush;
  cout << "          --self-destruct-attempts=n (Failed opens allowed, 1 to " << FC_SD_MAX_ATTEMPTS << ", default " << FC_SD_DEFAULT_ATTEMPTS << ")" << "\n" << flush;
  cout << "                 Only guesses made through this program and the GUI are counted. Anyone with a copy of" << "\n" << flush;
  cout << "                 the file can guess offline or reset the counter. A destroyed database cannot be recovered" << "\n" << flush;
  cout << "                 except from your own backups. With self-destruct on, a read-only file is not opened." << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB records and GUI features from the command line:\n" << flush;
  cout << "          --show-record=name (Show one record, all fields)" << "\n" << flush;
  cout << "          --get-field=record:field (Write only the value of one field, for scripts, never logged)" << "\n" << flush;
  cout << "          --db-info (Format, records, users, key generation, logging and self-destruct settings)" << "\n" << flush;
  cout << "          --export[=csv|json] (Write every record, CSV as the GUI writes it, to stdout or --output-file)" << "\n" << flush;
  cout << "          --import (Add the JSON or CSV records read from stdin, --record-fd or --record-file, all or nothing)" << "\n" << flush;
  cout << "          --merge-from=other.ehd (Add the records of another database, all or nothing)" << "\n" << flush;
  cout << "          --merge-password-file=file, --merge-password-fd=n, --merge-key=file (Credential of the other database)" << "\n" << flush;
  cout << "          --merge-policy=fail|skip|replace (A record in both: stop, keep this one or take the other, default fail)" << "\n" << flush;
  cout << "          --input-format=text|json|csv (Record input format, JSON is detected by a leading { or [)" << "\n" << flush;
  cout << "          --change-password (New database password from --new-password-fd or --new-password-file, a rekey)" << "\n" << flush;
  cout << "          --generate-password[=length] (Print a random password, 8 to 256 characters, default 20)" << "\n" << flush;
  cout << "          --password-symbols (Use punctuation in generated passwords)" << "\n" << flush;
  cout << "          --output-file=file (Write the requested output to a new file only you can read)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "Automation:\n" << flush;
  cout << "          --non-interactive, --batch (Never prompt, a missing password, PIN or input is an error)" << "\n" << flush;
  cout << "          --yes (Confirm a delete without asking)" << "\n" << flush;
  cout << "          --quiet (Only errors and the requested output)" << "\n" << flush;
  cout << "          --json (JSON output for --db-list, --db-find-key, --show-record, --db-info, --list-users," << "\n" << flush;
  cout << "                 --check-version, --show-access-log, --export, --import and --merge-from)" << "\n" << flush;
  cout << "          --dry-run (Do and check --remove-user, --rekey, --delete-record, --add-record, --update-record," << "\n" << flush;
  cout << "                 --import and --merge-from on a copy, then remove it, the database is not changed)" << "\n" << flush;
  cout << "          --lock-timeout=seconds (Wait this long for another writer, default " << FC_DEF_LOCK_TIMEOUT << ")" << "\n" << flush;
  cout << "          Errors go to stderr, the requested output to stdout." << "\n" << flush;
  cout << "          Exit codes: 0 ok, 1 usage or other error, 2 the credential did not open the database," << "\n" << flush;
  cout << "          3 file, record or user not found, 4 locked by another writer, 5 --check-version found v1," << "\n" << flush;
  cout << "          6 destroyed by self-destruct, 7 read or write error (also a read-only file with self-destruct on)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB format check (no authentication, never uses a smart card):\n" << flush;
  cout << "          --check-version (Report v1 or v2 for each database, exit 1 if any is v1)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB no-auth stat functions:\n" << flush;
  cout << "          --db-stats (Display database stats and exit)" << "\n" << flush;

#ifdef __ENABLE_SMART_CARD__
  cout << "          --list-users (List the users with RSA key of Smart Card cert access and exit)" << "\n" << flush;
#else
  cout << "          --list-users (List the users with RSA key access and exit)" << "\n" << flush;
#endif // __ENABLE_SMART_CARD__
  cout << "\n" << flush;
  cout << "DB format functions:\n" << flush;
  cout << "          --upgrade-db (Upgrade a v1 database to format v2, after a backup copy)" << "\n" << flush;
  cout << "          --reseal-db (Accept the current contents of a v2 database that fails its integrity check)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB authenticated functions:\n" << flush;
  cout << "          --db-config (Display database config and exit)" << "\n" << flush;
  cout << "          --db-list (List database stdout, defaults to tab delimited output)" << "\n" << flush;
  cout << "          --db-find-key=name (Search the database for a key name and list output to stdout)" << "\n" << flush;
  cout << "          --db-find-all-key=name (Search for all matching key names and list output to stdout)" << "\n" << flush;
  cout << "          --delimiter=\",\" (Set the list delimiter to comma or other value)" << "\n" << flush;
  cout << "          --csv (List in RFC 4180 CSV format, comma delimited unless --delimiter is given)" << "\n" << flush;
  cout << "          --field=n (List only field n of each record, 1 is the key name, values are not quoted)" << "\n" << flush;
  cout << "          --null (With --field, end each value with a NUL character instead of a newline)" << "\n" << flush;
  cout << "          --filter-char (Set a character to filter from list output)" << "\n" << flush;
  cout << "          --replace-char (Set a character to replace in list output)" << "\n" << flush;
  cout << "          --replace-char-with (Set the replacement character to replace in list output)" << "\n" << flush;
  cout << "\n" << flush; // End of list
}

void WarnSecretOnCommandLine(const char *option, const char *alternatives)
// 10/25/2026: Secrets on the command line can be read by other users
// with ps and end up in shell history.
{
  cerr << "WARNING: " << option << "=value puts the secret on the command line, where other users can read it. "
       << "Use " << alternatives << " instead." << "\n" << flush;
}

int ReadSecretArg(const gxString &arg, gxString &value, gxString &secret)
// Read a secret for a --xxx-file or --xxx-fd argument. Returns 0 if
// successful or 1 after printing an error.
{
  gxString err;
  int rv;
  if(arg.Find("-fd") != -1) {
    rv = ReadSecretFD(value.c_str(), secret, err);
  }
  else {
    check_if_file_exists(value); // Expand ${HOME} and ~/
    rv = ReadSecretFile(value.c_str(), secret, err);
  }
  if(rv != 0) cerr << "ERROR: " << err.c_str() << "\n" << flush;
  return rv;
}

int ProcessDashDashArg(gxString &arg)
{
  gxString sbuf, equal_arg, ebuf;
  int has_valid_args = 0;
  int rv = 0;
  
  gxString raw_equal_arg; // 10/25/2026: Untrimmed value for secrets
  if(arg.Find("=") != -1) {
    // Look for equal arguments
    // --log-file="/var/log/my_service.log"
    equal_arg = arg;
    equal_arg.DeleteBeforeIncluding("=");
    raw_equal_arg = equal_arg;
    arg.DeleteAfterIncluding("=");
    equal_arg.TrimLeading(' '); equal_arg.TrimTrailing(' ');
    equal_arg.TrimLeading('\"'); equal_arg.TrimTrailing('\"');
    equal_arg.TrimLeading('\''); equal_arg.TrimTrailing('\'');
  }

  arg.ToLower();

  // Process all -- arguments here
  if(arg == "help") {
    HelpMessage();
    return 0; // Signal program to exit
  }
  if(arg == "?") {
    HelpMessage();
    return 0; // Signal program to exit
  }
  if((arg == "version") || (arg == "ver")) {
    DisplayVersion();
    return 0; // Signal program to exit
  }

  if(arg == "debug") {
    if(!equal_arg.is_null()) {
      if(equal_arg.Atoi() <= 0) {
	cerr << "ERROR: Invalid value passed to --debug" << "\n" << flush;
	return 0;
      }
      debug_level = equal_arg.Atoi();
      verbose_mode = 1;
      debug_mode= 1;
    }
    has_valid_args = 1;
  }

  if(arg == "verbose") {
    verbose_mode = 1;
    has_valid_args = 1;
  }

  if(arg == "list-users" ) {
    list_users = 1;
    has_valid_args = 1;
  }

  if(arg == "db-stats" ) {
    db_stats = 1;
    has_valid_args = 1;
  }

  if(arg == "db-config" ) {
    display_db_config = 1;
    has_valid_args = 1;
  }

  if(arg == "upgrade-db") {
    upgrade_db = 1;
    has_valid_args = 1;
  }

  if(arg == "reseal-db") {
    reseal_db = 1;
    has_valid_args = 1;
  }

  if(arg == "db-list" ) {
    db_list = 1;
    has_valid_args = 1;
  }

  if(arg == "delimiter" ) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --delimiter requires an input argument" << "\n" << flush;
      return 0;
    }
    delimiter = equal_arg[0];
    set_delimiter_arg(equal_arg, delimiter);
    has_valid_args = 1;
  }

  // 10/25/2026: Output formats for scripts
  if(arg == "csv") {
    epdb_output_mode = EPDB_OUTPUT_CSV;
    if(delimiter == '\t') delimiter = ',';
    has_valid_args = 1;
  }

  if(arg == "field") {
    if(equal_arg.is_null() || equal_arg.Atoi() < 1 || equal_arg.Atoi() > NumDataMembers) {
      cerr << "ERROR: --field requires a field number from 1 to " << NumDataMembers << "\n" << flush;
      return 0;
    }
    epdb_output_mode = EPDB_OUTPUT_FIELD;
    epdb_output_field = (unsigned)(equal_arg.Atoi() - 1);
    has_valid_args = 1;
  }

  if(arg == "null") {
    epdb_output_null = 1;
    has_valid_args = 1;
  }

  if(arg == "filter-char" ) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --filter-char requires an input argument" << "\n" << flush;
      return 0;
    }
    filter_char = equal_arg[0];
    set_delimiter_arg(equal_arg, filter_char);
    has_valid_args = 1;
  }

  if(arg == "replace-char" ) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --replace-char requires an input argument" << "\n" << flush;
      return 0;
    }
    replace_char = equal_arg[0];
    set_delimiter_arg(equal_arg, replace_char);
    has_valid_args = 1;
  }
  
  if(arg == "replace-char-with" ) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --replace-char-with requires an input argument" << "\n" << flush;
      return 0;
    }
    replace_char_with = equal_arg[0];
    set_delimiter_arg(equal_arg, replace_char_with);
    has_valid_args = 1;
  }
  
  if(arg == "db-find-key" ) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --db-find-key requires an input argument" << "\n" << flush;
      return 0;
    }
    db_find_key = 1;
    db_find_key_name = equal_arg;
    has_valid_args = 1;
  }

  if(arg == "db-find-all-key" ) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --db-find-all-key requires an input argument" << "\n" << flush;
      return 0;
    }
    db_find_key = 1;
    db_find_all = 1;
    db_find_key_name = equal_arg;
    has_valid_args = 1;
  }

  if(arg == "rsa-key-passphrase") {
    if(raw_equal_arg.is_null()) {
      cerr << "ERROR: --rsa-key-passphrase requires an input argument" << "\n" << flush;
      return 0;
    }
    WarnSecretOnCommandLine("--rsa-key-passphrase", "--rsa-key-passphrase-file or --rsa-key-passphrase-fd");
    rsa_key_passphrase = raw_equal_arg;
    has_valid_args = 1;
  }

  // 10/25/2026: Read secrets from a file or file descriptor
  if((arg == "rsa-key-passphrase-file") || (arg == "rsa-key-passphrase-fd")) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --" << arg.c_str() << " requires an input argument" << "\n" << flush;
      return 0;
    }
    if(ReadSecretArg(arg, equal_arg, rsa_key_passphrase) != 0) return 0;
    has_valid_args = 1;
  }

  if(arg == "password") {
    if(!raw_equal_arg.is_null()) {
      WarnSecretOnCommandLine("--password", "--password-file or --password-fd");
      password = raw_equal_arg;
    }
    use_password  = 1;
    has_valid_args = 1;
  }

  if((arg == "password-file") || (arg == "password-fd")) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --" << arg.c_str() << " requires an input argument" << "\n" << flush;
      return 0;
    }
    if(ReadSecretArg(arg, equal_arg, password) != 0) return 0;
    use_password  = 1;
    has_valid_args = 1;
  }
  
  if(arg == "key") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --key requires an input argument" << "\n" << flush;
      return 0;
    }
    input_arg_key_file = equal_arg;
    if(!check_if_file_exists(input_arg_key_file.c_str())) {
      cerr << "ERROR: Key file " << input_arg_key_file.c_str() << " does not exist or cannot be read" <<  "\n" << flush;
      return 0;
    }
    if(CheckPrivateKeyFile(input_arg_key_file.c_str(), ebuf) != 0) { // 10/25/2026
      cerr << "ERROR: " << ebuf.c_str() << "\n" << flush;
      return 0;
    }
    DEBUG_m("Reading symmetric key file");
    if(read_key_file(input_arg_key_file.c_str(), key, ebuf) != 0) {
      cerr << "ERROR: " << ebuf.c_str() << "\n" << flush;
      return 0;
    }
    use_key_file  = 1;
    has_valid_args = 1;
  }
  
  if(arg == "rsa-key") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --rsa-key missing filename: --rsa-key=/$HOME/keys/rsa_key.pem" << "\n" << flush;
      return 0;
    }
    private_rsa_key_file = equal_arg;
    if(!check_if_file_exists(private_rsa_key_file.c_str())) {
      cerr << "ERROR: Private RSA key file " << private_rsa_key_file.c_str() << " does not exist or cannot be read" <<  "\n" << flush;
      return 0;
    }
    if(CheckPrivateKeyFile(private_rsa_key_file.c_str(), ebuf) != 0) { // 10/25/2026
      cerr << "ERROR: " << ebuf.c_str() << "\n" << flush;
      return 0;
    }
    DEBUG_m("Reading RSA key file");
    rv = RSA_read_key_file(private_rsa_key_file.c_str(), private_key, sizeof(private_key), &private_key_len, &has_passphrase);
    if(rv != RSA_NO_ERROR) {
      cerr << "ERROR: " << RSA_err_string(rv) << "\n" << flush;
      return 0;
    }
    use_private_rsa_key = 1;
    has_valid_args = 1;
  }

  if(arg == "rsa-key-username") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --rsa-key-username missing name: --rsa-key-username=$(whoami)" << "\n" << flush;
      return 0;
    }
    rsa_key_username = equal_arg;
    has_valid_args = 1;
  }
  
#ifdef __ENABLE_SMART_CARD__
  if(arg == "smartcard") {
    use_smartcard = 1;
    has_valid_args = 1;
  }
  
  if(arg == "smartcard-username") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --smartcard-username missing name: --smartcard-username=$(whoami)" << "\n" << flush;
      return 0;
    }
    smartcard_username = equal_arg;
    has_valid_args = 1;
  }

  if(arg == "smartcard-cert-id") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --smartcard-cert-id missing ID: --smartcard-cert-id=01" << "\n" << flush;
      return 0;
    }
    // 10/26/2026: Refuse an ID that does not fit, it was cut short
    if(equal_arg.length() >= SC_max_cert_id_size) {
      cerr << "ERROR: --smartcard-cert-id is longer than " << (SC_max_cert_id_size-1) << " characters" << "\n" << flush;
      return 0;
    }
    sc.SetCertID(equal_arg.c_str());
    has_valid_args = 1;
  }

  if(arg == "smartcard-engine") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --smartcard-engine missing path: --smartcard-engine=" <<  SC_get_default_enginePath() << "\n" << flush;
      return 0;
    }
    sc.SetEnginePath(equal_arg.c_str());
    if(!SC_uses_engine()) {
      cerr << "NOTE: --smartcard-engine is ignored, this build loads the PKCS#11 provider module directly" << "\n" << flush;
    }
    if(SC_uses_engine() && !check_if_file_exists(sc.enginePath)) {
      cerr << "ERROR: Smart card engine " << sc.enginePath << " does not exist or cannot be read" <<  "\n" << flush;
      return 0;
    }
    has_valid_args = 1;
  }

  if(arg == "smartcard-provider") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --smartcard-provider missing path: --smartcard-provider=" <<  SC_get_default_modulePath() << "\n" << flush;
      return 0;
    }
    if(equal_arg.length() >= SC_max_path_size) {
      cerr << "ERROR: --smartcard-provider path is longer than " << (SC_max_path_size-1) << " characters" << "\n" << flush;
      return 0;
    }
    sc.SetModulePath(equal_arg.c_str());
    if(!futils_exists(sc.modulePath) || !futils_isfile(sc.modulePath)) {
      cerr << "ERROR: Smart card provider " << sc.modulePath << " does not exist or cannot be read" <<  "\n" << flush;
      return 0;
    }
    has_valid_args = 1;
  }

  if(arg == "smartcard-pin") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --smartcard-pin missing pin number: --smartcard-pin=12345" << "\n" << flush;
      return 0;
    }
    WarnSecretOnCommandLine("--smartcard-pin", "--smartcard-pin-file or --smartcard-pin-fd");
    sc.SetPin(equal_arg.c_str());
    has_valid_args = 1;
  }

  // 10/26/2026: Use only the card in this PKCS#11 slot, for cert lookup,
  // adding a card user and the login. No other slot is opened.
  if(arg == "smartcard-slot") {
    unsigned long slot_id = 0;
    if(!SC_parse_slot(equal_arg.c_str(), &slot_id)) {
      cerr << "ERROR: --smartcard-slot needs a slot ID: --smartcard-slot=1 or --smartcard-slot=0x4" << "\n" << flush;
      return 0;
    }
    sc.SetSlot(slot_id);
    has_valid_args = 1;
  }

  // 10/26/2026: Send the PIN to a card that reports low PIN tries or one
  // try left. Some modules, such as ActivClient may, do not report them.
  if(arg == "smartcard-allow-pin-count-low") {
    sc.allow_pin_count_low = 1;
    has_valid_args = 1;
  }
  if(arg == "smartcard-allow-final-try") {
    sc.allow_pin_count_low = 1;
    sc.allow_pin_final_try = 1;
    has_valid_args = 1;
  }

  if((arg == "smartcard-pin-file") || (arg == "smartcard-pin-fd")) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --" << arg.c_str() << " requires an input argument" << "\n" << flush;
      return 0;
    }
    gxString pin_buf;
    if(ReadSecretArg(arg, equal_arg, pin_buf) != 0) return 0;
    sc.SetPin(pin_buf.c_str());
    pin_buf.Clear(1);
    has_valid_args = 1;
  }
#endif // __ENABLE_SMART_CARD__
  
  if(arg == "add-rsa-key") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --add-rsa-key missing filename: --add-rsa-key=/$HOME/keys/rsa_pubkey.pem" << "\n" << flush;
      return 0;
    }
    public_rsa_key_file = equal_arg;
    
    DEBUG_m("Reading public RSA key file");
    if(!check_if_file_exists(public_rsa_key_file.c_str())) {
      cerr << "ERROR: Public RSA key file " << public_rsa_key_file.c_str() << " does not exist or cannot be read" <<  "\n" << flush;
      return 0;
    }
    // 10/25/2026: Do not overwrite has_passphrase, which belongs to --rsa-key
    int public_key_has_passphrase = 0;
    rv = RSA_read_key_file(public_rsa_key_file.c_str(), public_key, sizeof(public_key), &public_key_len, &public_key_has_passphrase);
    if(rv != RSA_NO_ERROR) {
      std::cerr << "ERROR: " << RSA_err_string(rv) << "\n" << std::flush;
      return 0;
    }
    add_rsa_key = 1;
    has_valid_args = 1;
  }

  if(arg == "add-username") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --add-username missing name: --add-username=$(whoami)" << "\n" << flush;
      return 0;
    }
    add_username = equal_arg;
    has_valid_args = 1;
  }

#ifdef __ENABLE_SMART_CARD__
  // 10/26/2026: --add-smartcard-cert with no file reads the public key
  // from the card in the reader (no PIN needed), as fcrypt does.
  // --add-smartcard-cert=file is kept for older scripts.
  if(arg == "add-smartcard-cert" && equal_arg.is_null()) {
    add_smart_card = 1;
    use_smartcard_cert_file = 0;
    has_valid_args = 1;
  }
  else if((arg == "add-smartcard-cert") || (arg == "add-smartcard-cert-file")) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --add-smartcard-cert-file missing filename: --add-smartcard-cert-file=${HOME}/certs/certfile.pem" << "\n" << flush;
      return 0;
    }
    add_smart_card = 1;
    use_smartcard_cert_file = 1;
    smartcard_cert_file = equal_arg;
    if(!check_if_file_exists(smartcard_cert_file.c_str())) {
      cerr << "ERROR: Smart card cert file " << smartcard_cert_file.c_str() << " does not exist or cannot be read" <<  "\n" << flush;
      return 0;
    }
    // 10/26/2026: PEM or DER (Windows .cer) cert files
    gxString cert_err;
    if(EPDBReadSmartCardCertFile(&sc, smartcard_cert_file.c_str(), cert_err) != 0) {
      cerr << "ERROR: " << cert_err.c_str() << "\n" << flush;
      return 0;
    }
    has_valid_args = 1;
  }
#endif // __ENABLE_SMART_CARD__ 

  // 10/27/2026: Key management, records, access log, version check and self-destruct
  if(arg == "remove-user") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --remove-user missing name: --remove-user=name" << "\n" << flush;
      return 0;
    }
    remove_username = equal_arg;
    remove_user = 1;
    has_valid_args = 1;
  }
  if(arg == "remove-user-type") {
    gxString t = equal_arg;
    t.ToLower();
    if(t == "rsa") remove_user_type = 1;
    else if(t == "smartcard") remove_user_type = 2;
    else {
      cerr << "ERROR: --remove-user-type must be rsa or smartcard" << "\n" << flush;
      return 0;
    }
    has_valid_args = 1;
  }
  if((arg == "add-record") || (arg == "update-record") || (arg == "delete-record")) {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --" << arg.c_str() << " requires the record name: --" << arg.c_str() << "=\"Name\"" << "\n" << flush;
      return 0;
    }
    if(record_op) {
      cerr << "ERROR: Give only one of --add-record, --update-record and --delete-record" << "\n" << flush;
      return 0;
    }
    record_op = (arg == "add-record") ? EPDB_RECORD_ADD : ((arg == "update-record") ? EPDB_RECORD_UPDATE : EPDB_RECORD_DELETE);
    record_name = equal_arg;
    has_valid_args = 1;
  }
  if(arg == "record-fd" || arg == "record-file" || arg == "rekey-fd" || arg == "rekey-file" ||
     arg == "rekey-new-password-fd" || arg == "rekey-new-password-file") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --" << arg.c_str() << " requires an input argument" << "\n" << flush;
      return 0;
    }
    if(arg == "record-fd") record_input_fd = equal_arg;
    if(arg == "record-file") { check_if_file_exists(equal_arg); record_input_file = equal_arg; }
    if(arg == "rekey-fd") rekey_input_fd = equal_arg;
    if(arg == "rekey-file") { check_if_file_exists(equal_arg); rekey_input_file = equal_arg; }
    if(arg == "rekey-new-password-fd") rekey_new_password_fd = equal_arg;
    if(arg == "rekey-new-password-file") rekey_new_password_file = equal_arg;
    has_valid_args = 1;
  }
  if(arg == "yes") {
    assume_yes = 1;
    has_valid_args = 1;
  }
  if(arg == "rekey") {
    rekey = 1;
    has_valid_args = 1;
  }
  if(arg == "access-log") {
    access_log = 1;
    if(!equal_arg.is_null()) access_log_file = equal_arg;
    has_valid_args = 1;
  }
  if(arg == "no-access-log") {
    access_log = 0;
    has_valid_args = 1;
  }
  if(arg == "verify-access-log") {
    verify_access_log = 1;
    has_valid_args = 1;
  }
  if(arg == "show-access-log") {
    show_access_log = 1;
    has_valid_args = 1;
  }
  if(arg == "access-log-in-file" || arg == "self-destruct") {
    gxString v = equal_arg;
    v.ToLower();
    int on = (v == "on" || v == "yes" || v == "1") ? 1 : ((v == "off" || v == "no" || v == "0") ? 0 : -1);
    if(on < 0) {
      cerr << "ERROR: --" << arg.c_str() << " must be on or off" << "\n" << flush;
      return 0;
    }
    if(arg == "access-log-in-file") access_log_in_file = on;
    else self_destruct = on;
    has_valid_args = 1;
  }
  if(arg == "self-destruct-attempts") {
    if(equal_arg.Atoi() < 1 || equal_arg.Atoi() > (int)FC_SD_MAX_ATTEMPTS) {
      cerr << "ERROR: --self-destruct-attempts must be 1 to " << FC_SD_MAX_ATTEMPTS << "\n" << flush;
      return 0;
    }
    self_destruct_attempts = (unsigned)equal_arg.Atoi();
    has_valid_args = 1;
  }
  if(arg == "check-version") {
    check_version = 1;
    has_valid_args = 1;
  }

  // 10/27/2026: Automation and GUI features from the command line
  if(arg == "non-interactive" || arg == "batch") {
    non_interactive = 1;
    has_valid_args = 1;
  }
  if(arg == "quiet") {
    quiet_mode = 1;
    has_valid_args = 1;
  }
  if(arg == "json") {
    json_output = 1;
    has_valid_args = 1;
  }
  if(arg == "dry-run") {
    dry_run = 1;
    has_valid_args = 1;
  }
  if(arg == "lock-timeout") {
    if(equal_arg.is_null() || equal_arg.Atoi() < 0 || equal_arg.Atoi() > 3600) {
      cerr << "ERROR: --lock-timeout must be 0 to 3600 seconds" << "\n" << flush;
      return 0;
    }
    lock_timeout = (unsigned)equal_arg.Atoi();
    has_valid_args = 1;
  }
  if(arg == "get-field") {
    if(equal_arg.Find(":") == -1) {
      cerr << "ERROR: --get-field needs record:field, for example --get-field=\"My Bank:password\"" << "\n" << flush;
      return 0;
    }
    get_field = equal_arg;
    has_valid_args = 1;
  }
  if(arg == "output-file") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --output-file requires a file name" << "\n" << flush;
      return 0;
    }
    output_file = equal_arg;
    has_valid_args = 1;
  }
  if(arg == "show-record") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --show-record requires the record name" << "\n" << flush;
      return 0;
    }
    show_record_name = equal_arg;
    has_valid_args = 1;
  }
  if(arg == "db-info") {
    db_info = 1;
    has_valid_args = 1;
  }
  if(arg == "export") {
    gxString f = equal_arg;
    f.ToLower();
    if(!f.is_null() && f != "csv" && f != "json") {
      cerr << "ERROR: --export must be csv or json" << "\n" << flush;
      return 0;
    }
    export_db = 1;
    export_json = (f == "json");
    has_valid_args = 1;
  }
  if(arg == "import") {
    import_db = 1;
    has_valid_args = 1;
  }
  if(arg == "input-format") {
    gxString f = equal_arg;
    f.ToLower();
    if(f != "text" && f != "json" && f != "csv") {
      cerr << "ERROR: --input-format must be text, json or csv" << "\n" << flush;
      return 0;
    }
    input_format = f;
    has_valid_args = 1;
  }
  if(arg == "merge-from") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --merge-from requires the database to merge from" << "\n" << flush;
      return 0;
    }
    merge_from = equal_arg;
    has_valid_args = 1;
  }
  if((arg == "merge-password-file") || (arg == "merge-password-fd")) {
    if(equal_arg.is_null() || ReadSecretArg(arg, equal_arg, merge_secret) != 0) return 0;
    has_valid_args = 1;
  }
  if(arg == "merge-key") {
    MemoryBuffer mk;
    if(equal_arg.is_null() || !check_if_file_exists(equal_arg) || CheckPrivateKeyFile(equal_arg.c_str(), ebuf) != 0 ||
       read_key_file(equal_arg.c_str(), mk, ebuf) != 0) {
      cerr << "ERROR: --merge-key: " << ebuf.c_str() << "\n" << flush;
      return 0;
    }
    merge_secret.SetString((const char *)mk.m_buf(), mk.length());
    mk.Clear(1);
    has_valid_args = 1;
  }
  if(arg == "merge-policy") {
    gxString f = equal_arg;
    f.ToLower();
    if(f == "skip") merge_policy = EPDB_MERGE_SKIP;
    else if(f == "replace") merge_policy = EPDB_MERGE_REPLACE;
    else if(f == "fail") merge_policy = EPDB_MERGE_FAIL;
    else {
      cerr << "ERROR: --merge-policy must be skip, replace or fail" << "\n" << flush;
      return 0;
    }
    merge_policy_set = 1;
    has_valid_args = 1;
  }
  if(arg == "change-password") {
    change_password = 1;
    has_valid_args = 1;
  }
  if((arg == "new-password-file") || (arg == "new-password-fd")) {
    if(equal_arg.is_null() || ReadSecretArg(arg, equal_arg, new_password) != 0) return 0;
    has_valid_args = 1;
  }
  if(arg == "generate-password") {
    generate_password = equal_arg.is_null() ? 20 : (unsigned)equal_arg.Atoi();
    if(generate_password < 8 || generate_password > 256) {
      cerr << "ERROR: --generate-password length must be 8 to 256" << "\n" << flush;
      return 0;
    }
    has_valid_args = 1;
  }
  if(arg == "password-symbols") {
    password_symbols = 1;
    has_valid_args = 1;
  }

  if(!has_valid_args) {
    cerr << "Unknown or invalid --" << arg.c_str() << "\n" << flush;
    cerr << "Exiting..." << "\n" << flush;
  }

  arg.Clear();
  return has_valid_args;
}

int ProcessArgs(char *arg)
{
  gxString sbuf;
  gxString ebuf;
  
  switch(arg[1]) {
    case 'v':
      verbose_mode = 1;
      break;

    case 'd':
      verbose_mode = 1;
      debug_mode = 1;
      break;

    case 'h': case 'H': case '?':
      HelpMessage();
      return 0;

    case '-':
      sbuf = arg+2; 
      // Add all -- prepend filters here
      sbuf.TrimLeading('-');
      if(!ProcessDashDashArg(sbuf)) return 0;
      break;

    default:
      cerr << "ERROR: Unknown switch " << arg << "\n" << flush;
      cerr << "Exiting..." << "\n" << flush;
      return 0;
  }
  return 1; // All command line arguments were valid
}

void WipeSecretArg(char *arg)
// 10/25/2026: Overwrite the value of --password=, --rsa-key-passphrase=
// and --smartcard-pin= in argv, so it is no longer shown by ps or in
// /proc/<pid>/cmdline after the arguments are read. It is still visible
// until then, use the -file or -fd arguments for secrets.
{
  if(!arg) return;
  const char *secret_args[] = { "password=", "rsa-key-passphrase=", "smartcard-pin=", 0 };
  char *p = arg;
  while(*p == '-') p++;
  for(int i = 0; secret_args[i]; i++) {
    unsigned len = strlen(secret_args[i]);
    if(CaseICmp(gxString(p, len), secret_args[i]) == 0) {
      char *v = p + len;
      memset(v, 'x', strlen(v));
      return;
    }
  }
}

int DEBUG_m(char *message, int level, int rv)
{
  if(debug_mode && debug_level >= level) {
    if(message) cerr << "DEBUG" << debug_level << ": " << message << "\n" << flush; 
    if(!debug_message.is_null()) cerr << debug_message << "\n" << flush;
  }
  return rv; 
}

int check_if_file_exists(char *fname)
{
  if(!futils_exists(fname) || !futils_isfile(fname)) {
    cerr << "ERROR: File " << fname << " does not exist or cannot be read" <<  "\n" << flush;
    return 0;
  }

  return 1;
}

int check_if_file_exists(gxString &fname)
{
  HOMEdir = getenv("HOME");
  gxString sbuf;

  fname.ReplaceString("${HOME}", HOMEdir.c_str());
  sbuf << clear <<  HOMEdir << "/";
  fname.ReplaceString("~/", sbuf.c_str());

  if(!futils_exists(fname.c_str()) || !futils_isfile(fname.c_str())) {
    cerr << "ERROR: File " << fname.c_str() << " does not exist or cannot be read" <<  "\n" << flush;
    return 0;
  }

  return 1;
}

void set_delimiter_arg(const gxString &equal_arg, char &delimiter_char)
{
  if(equal_arg == "\\t") delimiter_char = '\t';
  if(equal_arg == "tab") delimiter_char = '\t';
  if(equal_arg == "comma") delimiter_char = ',';
  if(equal_arg == "pipe") delimiter_char = '|';
}
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

int ConsoleIsInteractive()
// 10/26/2026: Returns 1 if stdin and stderr are a terminal
{
#if defined (__UNIX__)
  return isatty(0) && isatty(2);
#else
  return _isatty(_fileno(stdin)) && _isatty(_fileno(stderr));
#endif
}

// ----------------------------------------------------------- //
// 10/27/2026: Key management, records, access log, version check and
// self-destruct
// ----------------------------------------------------------- //
static const char *AuthName()
{
  if(use_private_rsa_key) return "rsa-key";
#ifdef __ENABLE_SMART_CARD__
  if(use_smartcard) return "smartcard";
#endif
  if(use_key_file) return "key-file";
  if(use_password) return "password";
  return "password";
}

static int AuthCode()
{
  return FC_auth_code(AuthName());
}

static gxString AuthSlotName()
{
  if(use_private_rsa_key) return rsa_key_username;
#ifdef __ENABLE_SMART_CARD__
  if(use_smartcard) return smartcard_username;
#endif
  return gxString();
}

static EPDBLogContext log_ctx;

static void SetupLog()
{
  log_ctx.per_user = access_log;
  log_ctx.log_file = access_log_file;
  log_ctx.tool = "epdb_auth_tool";
  log_ctx.auth = AuthName();
}

static void ShowLogWarning()
{
  if(!log_ctx.warning.is_null()) {
    cerr << "WARNING: " << log_ctx.warning.c_str() << "\n" << flush;
    log_ctx.warning.Clear();
  }
}

static void PerUserLog(const gxString &fname, const char *action, const gxString &slot, const gxString &record,
		       int ok, const char *detail, const MemoryBuffer *file_key, int force)
// Per-user log only, for actions the in-file log does not keep
{
  SetupLog();
  if(!access_log && !force) return;
  EPDBPerUserLog(log_ctx, fname.c_str(), action, slot, record, ok, detail, file_key);
  ShowLogWarning();
}

void LogEvent(const gxString &fname, const char *action, const gxString &slot, const gxString &record,
	      int ok, const char *detail, const MemoryBuffer *file_key)
// The core's EPDBLog(): per-user log when on, the in-file log when the
// database has it on, with the per-user fallback. Never fails.
{
  SetupLog();
  EPDBLog(log_ctx, fname.c_str(), action, slot, record, ok, detail, file_key);
  ShowLogWarning();
}

int CheckVersions()
// Reports v1 or v2 for each file without a credential or smart card.
// Returns FC_EXIT_V1 if any file is v1, FC_EXIT_DESTROYED if any was
// destroyed, FC_EXIT_IO if any cannot be read, else 0.
{
  int any_v1 = 0, any_error = 0, any_destroyed = 0, n = 0;
  if(json_output) cout << "[";
  gxListNode<gxString> *ptr = file_list.GetHead();
  while(ptr) {
    FCVersionInfo vi;
    gxString err, state, detail;
    if(FC_check_version(ptr->data.c_str(), vi, err) != 0) {
      state = "unknown";
      detail = err;
      any_error = 1;
    }
    else if(vi.is_v1) {
      state = "v1";
      detail << vi.version << ", upgrade with: " << vi.upgrade;
      any_v1 = 1;
    }
    else {
      FCControlBlock c;
      if(vi.kind == FC_KIND_EPDB && EPDBReadControl(ptr->data.c_str(), c) && c.destroyed) {
	state = "destroyed";
	detail = "the records were destroyed after too many failed opens";
	any_destroyed = 1;
      }
      else {
	state = "v2";
	detail = vi.version;
      }
    }
    if(json_output) {
      if(n++) cout << ", ";
      cout << "{\"file\": " << FC_json_string(ptr->data).c_str() << ", \"state\": \"" << state.c_str()
	   << "\", \"detail\": " << FC_json_string(detail).c_str();
      if(vi.is_v1) cout << ", \"upgrade\": " << FC_json_string(vi.upgrade).c_str();
      cout << "}";
    }
    else {
      cout << ptr->data.c_str() << ": " << state.c_str() << ": " << detail.c_str() << "\n";
    }
    ptr = ptr->next;
  }
  if(json_output) cout << "]" << "\n";
  cout << flush;
  if(any_v1) return FC_EXIT_V1;
  if(any_destroyed) return FC_EXIT_DESTROYED;
  return any_error ? FC_EXIT_IO : 0;
}

int ReadOperationInput()
// Reads the record fields, records to import or rekey input once,
// before any prompt. Record input is name=value lines, JSON (detected
// by a leading { or [, or --input-format=json) or CSV for --import.
{
  int wants_records = (record_op == EPDB_RECORD_ADD || record_op == EPDB_RECORD_UPDATE || import_db);
  if(!wants_records && !rekey) return 1;
  gxString &fd = rekey ? rekey_input_fd : record_input_fd;
  gxString &file = rekey ? rekey_input_file : record_input_file;
  MemoryBuffer data;
  gxString err;
  int rv = 0;
  if(!file.is_null()) rv = FC_read_input_file(file.c_str(), data, err);
  else if(!fd.is_null()) rv = FC_read_input_fd(fd.c_str(), data, err);
  else {
    if(FC_stdin_is_terminal()) {
      if(non_interactive) {
	cerr << "ERROR: --non-interactive needs the input on stdin, --record-fd/--rekey-fd or --record-file/--rekey-file" << "\n" << flush;
	return 0;
      }
      cerr << "Enter " << (rekey ? "the rekey input" : "the record fields") << ", one name=value per line, end with "
#if defined (__UNIX__)
	   << "Ctrl-D"
#else
	   << "Ctrl-Z and Enter"
#endif
	   << ":" << "\n" << flush;
    }
    rv = FC_read_input_fd("0", data, err);
  }
  if(rv == 0 && rekey) {
    rv = FC_parse_name_values(data, record_fields, err);
    if(rv == 0) rv = FC_parse_rekey_input(record_fields, rekey_in, err);
    FC_wipe_name_values(record_fields);
  }
  else if(rv == 0) {
    int is_json = (input_format == "json") || (input_format.is_null() && FC_input_is_json(data));
    if(is_json) {
      rv = EPDBParseJSONRecords(data, input_records, err);
      if(rv == 0 && !import_db) {
	unsigned n = 0;
	for(gxListNode<EPDBRecord> *p = input_records.GetHead(); p; p = p->next) n++;
	if(n != 1) {
	  err << clear << "--add-record and --update-record take one JSON object, use --import for an array of records";
	  rv = 1;
	}
      }
    }
    else if(import_db && input_format != "text") {
      rv = EPDBParseCSV(data, input_records, err);
    }
    else if(import_db) {
      err << clear << "--import reads JSON or CSV records";
      rv = 1;
    }
    else {
      rv = FC_parse_name_values(data, record_fields, err);
    }
  }
  data.Clear(1);
  if(rv != 0) {
    cerr << "ERROR: " << (rekey ? "Rekey" : "Record") << " input: " << err.c_str() << ", nothing was changed" << "\n" << flush;
    SetExit(FC_EXIT_USAGE);
    return 0;
  }
  return 1;
}

int WillWrite(const gxString &fname)
// Returns 1 if this run may change the file, so it takes the lock
{
  if(record_op || remove_user || rekey || upgrade_db || reseal_db || add_rsa_key || import_db ||
     !merge_from.is_null() || access_log_in_file >= 0 || self_destruct >= 0 || self_destruct_attempts) return 1;
#ifdef __ENABLE_SMART_CARD__
  if(add_smart_card) return 1;
#endif
  FCControlBlock c; // The failed open counter is written
  return EPDBReadControl(fname.c_str(), c) && c.SelfDestruct();
}

int DestroyedNow(const gxString &fname)
{
  FCControlBlock c;
  return EPDBReadControl(fname.c_str(), c) && c.destroyed;
}

int SelfDestructPrecheck(const gxString &fname)
// Before any credential is tried, see EPDBSelfDestructPrecheck()
{
  gxString msg;
  int rv = EPDBSelfDestructPrecheck(fname.c_str(), msg);
  if(rv != 0) {
    cerr << "ERROR: " << msg.c_str() << "\n" << flush;
    SetExit(rv);
    return 0;
  }
  if(!msg.is_null()) cerr << "WARNING: " << msg.c_str() << "\n" << flush;
  return 1;
}

void CountFailedOpen(const gxString &fname, const char *what)
// Counts a failed open in the file before the error is returned, and
// destroys the database at the limit
{
  gxString msg;
  int last_try = 0, destroyed = 0;
  EPDBSelfDestructFailedOpen(fname.c_str(), what, msg, last_try, destroyed);
  if(msg.is_null()) return;
  cerr << (destroyed ? "ERROR: " : "WARNING: ") << msg.c_str() << "\n" << flush;
  if(destroyed) PerUserLog(fname, "self-destruct", gxString(), gxString(), 1, "destroyed after too many failed opens", 0, 1);
}

void ResetFailedOpens(const gxString &fname, const MemoryBuffer &file_key)
{
  gxString msg;
  EPDBSelfDestructReset(fname.c_str(), file_key, msg);
  if(!msg.is_null()) cerr << "WARNING: " << msg.c_str() << "\n" << flush;
}

static int ConfirmAtTerminal(const char *question)
{
  if(assume_yes) return 1;
  if(!ConsoleIsInteractive() || non_interactive) {
    cerr << "ERROR: Not at a terminal, use --yes to " << question << "\n" << flush;
    return 0;
  }
  cerr << question << "? [y/N]: " << flush;
  gxString answer;
  int yes = consoleGetString(answer, 0) && (answer == "y" || answer == "Y" || answer == "yes");
  cerr << "\n" << flush;
  return yes;
}

static void ListLoadedUsers(const gxString &fname, gxList<FCRekeySlot> &users)
{
  gxString err;
  gxDatabase *f = OpenEPDB(fname.c_str(), err);
  if(!f) return;
  DatabaseUserAuth db_auth;
  db_auth.f = f;
  if(db_auth.LoadStaticDataBlocks() == 0) {
    for(gxListNode<StaticDataBlock> *p = db_auth.static_block_list.GetHead(); p; p = p->next) {
      if(p->data.block_header.block_type != 1 && p->data.block_header.block_type != 2) continue;
      FCRekeySlot r;
      r.type = (p->data.block_header.block_type == 1) ? FC_SLOT_RSA : FC_SLOT_SMARTCARD;
      r.username = p->data.username;
      users.Add(r);
    }
  }
  f->Close();
  delete f;
}

static int ShowAccessLog(const gxString &fname, MemoryBuffer &file_key, int verify)
// Returns 0 if everything verified
{
  int rv = 0;
  gxString err;
  gxString id = EPDBIDHex(fname.c_str());
  // In-file log
  gxDatabase *f = OpenEPDB(fname.c_str(), err);
  EPDBHeaderExt x;
  if(f && EPDBReadExt(f, x) && (x.flags & EPDB_EXT_LOG_IN_FILE)) {
    gxList<FCRingEntry> entries;
    unsigned long long written = 0;
    int r = EPDBRingRead(f, file_key, entries, written, err);
    // Names for the keyed slot hashes: the current users and this user
    gxList<FCRekeySlot> users;
    ListLoadedUsers(fname, users);
    FCRekeySlot me;
    me.username = AuthSlotName();
    if(!me.username.is_null()) users.Add(me);
    cout << "In-file access log of " << fname.c_str() << ": " << written << " entries written, the last "
	 << EPDB_RING_ENTRIES << " are kept" << "\n" << flush;
    for(gxListNode<FCRingEntry> *p = entries.GetHead(); p; p = p->next) {
      char stamp[32];
      time_t t = (time_t)p->data.when;
      strftime(stamp, sizeof(stamp), "%Y-%m-%dT%H:%M:%SZ", gmtime(&t));
      gxString who = "-";
      unsigned char zero[8];
      memset(zero, 0, sizeof(zero));
      if(memcmp(p->data.slot_hash, zero, 8) != 0) {
	who << clear << "user#" << FC_hex(p->data.slot_hash, 8);
	for(gxListNode<FCRekeySlot> *u = users.GetHead(); u; u = u->next) {
	  unsigned char h[8];
	  FC_ring_slot_hash(EPDBKeysetKey(file_key, EPDB_KEY_MASTER), EPDBKeysetID(file_key), EPDB_V2_ID_LEN,
			    u->data.username.c_str(), h);
	  if(memcmp(h, p->data.slot_hash, 8) == 0) { who = u->data.username; break; }
	}
	if(p->data.flags & 0x80) who << clear << "(before rekey)";
      }
      if(!verify) {
	cout << p->data.seq << "\t" << stamp << "\t" << FC_action_name(p->data.action) << "\t"
	     << FC_auth_name(p->data.auth) << "\t" << who.c_str() << "\t" << (p->data.result ? "ok" : "fail") << "\n" << flush;
      }
    }
    if(r != 0) {
      cout << "TAMPERED: in-file access log: " << err.c_str() << "\n" << flush;
      rv = 1;
    }
    else if(verify) cout << "OK: in-file access log verified" << "\n" << flush;
  }
  else {
    cout << "The in-file access log is off for " << fname.c_str() << "\n" << flush;
  }
  if(f) {
    f->Close();
    delete f;
  }
  // Per-user log
  gxString log_file = access_log_file;
  if(log_file.is_null()) log_file = FC_default_log_file(".encrypted_password_database");
  if(!futils_exists(log_file.c_str())) {
    cout << "No per-user access log at " << log_file.c_str() << "\n" << flush;
    return rv;
  }
  if(!verify) {
    FILE *fp = fopen(log_file.c_str(), "rb");
    char line[8192];
    gxString key;
    key << "\tid=" << id << "\t";
    cout << "Per-user access log " << log_file.c_str() << " entries for this database:" << "\n" << flush;
    while(fp && fgets(line, sizeof(line), fp)) {
      if(!id.is_null() && strstr(line, key.c_str())) cout << line;
    }
    if(fp) fclose(fp);
    cout << flush;
    return rv;
  }
  unsigned char lk[FC_LOG_KEY_LEN];
  FC_log_key(EPDBKeysetKey(file_key, EPDB_KEY_MASTER), EPDB_V2_KEY_LEN, EPDBKeysetID(file_key), EPDB_V2_ID_LEN, lk);
  FCLogVerifyResult r;
  int lrv = FC_log_verify(log_file.c_str(), id.c_str(), lk, r, err);
  OPENSSL_cleanse(lk, sizeof(lk));
  if(lrv < 0) {
    cout << "ERROR: " << err.c_str() << "\n" << flush;
    return 1;
  }
  cout << "Per-user access log " << log_file.c_str() << ": " << r.id_lines << " entries, " << r.tagged
       << " tags verified, " << r.untagged_covered << " untagged entries covered";
  if(r.trailing_untagged) cout << ", " << r.trailing_untagged << " untagged entries after the last tag (not checked)";
  cout << "\n" << flush;
  if(lrv != 0) {
    cout << "TAMPERED: per-user access log: " << r.bad_lines << " problem(s), first at line " << r.first_bad_line
	 << ": " << r.first_bad_reason.c_str() << "\n" << flush;
    return 1;
  }
  cout << "OK: per-user access log verified" << "\n" << flush;
  return rv;
}

static int SetDatabaseOptions(const gxString &fname, MemoryBuffer &file_key)
// --access-log-in-file and --self-destruct. Returns 0 if successful.
{
  gxString err;
  gxDatabase *f = OpenEPDB(fname.c_str(), err, gxDBASE_READWRITE);
  if(!f) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    return -1;
  }
  EPDBHeaderExt x;
  EPDBReadExt(f, x);
  int rv = 0;
  if(access_log_in_file == 1) {
    gxList<FCRingEntry> entries;
    unsigned long long written = 0;
    gxString rerr;
    if(EPDBRingRead(f, file_key, entries, written, rerr) != 0) rv = EPDBRingInit(f, file_key, err);
    x.flags |= EPDB_EXT_LOG_IN_FILE;
  }
  if(access_log_in_file == 0) x.flags &= ~EPDB_EXT_LOG_IN_FILE;
  FCControlBlock c;
  EPDBReadControl(f, c);
  if(self_destruct == 1 || (self_destruct < 0 && self_destruct_attempts && c.SelfDestruct())) {
    c.flags |= FC_CTL_SELF_DESTRUCT;
    if(self_destruct_attempts) c.limit = self_destruct_attempts;
    else if(!c.limit) c.limit = FC_SD_DEFAULT_ATTEMPTS;
    c.failed = 0;
    c.destroyed = 0;
    x.flags |= EPDB_EXT_SELF_DESTRUCT;
    x.sd_limit = c.limit;
  }
  else if(self_destruct == 0) {
    c.flags &= ~FC_CTL_SELF_DESTRUCT;
    c.failed = 0;
    x.flags &= ~EPDB_EXT_SELF_DESTRUCT;
  }
  else if(self_destruct_attempts) {
    cerr << "ERROR: --self-destruct-attempts needs --self-destruct=on" << "\n" << flush;
    rv = -1;
  }
  if(rv == 0 && (self_destruct >= 0 || self_destruct_attempts) && EPDBWriteControl(f, c, err) != 0) rv = -1;
  if(rv == 0 && EPDBWriteExt(f, x, file_key, err) != 0) rv = -1;
  f->Close();
  delete f;
  if(rv == 0 && FC_fsync_file(fname.c_str()) != 0) {
    err << clear << "Cannot flush " << fname.c_str();
    rv = -1;
  }
  if(rv != 0) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    return -1;
  }
  if(access_log_in_file == 1) {
    cerr << "The in-file access log is on for " << fname.c_str() << " (last " << EPDB_RING_ENTRIES << " entries)" << "\n" << flush;
    LogEvent(fname, "access-log-on", AuthSlotName(), gxString(), 1, 0, &file_key);
  }
  if(access_log_in_file == 0) {
    PerUserLog(fname, "access-log-off", AuthSlotName(), gxString(), 1, 0, &file_key, 1);
    cerr << "The in-file access log is off for " << fname.c_str() << "\n" << flush;
  }
  if(c.SelfDestruct() && (self_destruct == 1 || self_destruct_attempts)) {
    cerr << "Self-destruct is on for " << fname.c_str() << ": the records are destroyed after " << c.limit << " failed opens." << "\n"
	 << "This only counts guesses made through this program and the GUI: anyone with a copy of the file can guess offline "
	 << "or reset the counter. A destroyed database cannot be recovered except from your own backups. With self-destruct "
	 << "on, a read-only copy is not opened, and --remove-user and --rekey keep no backup copy." << "\n" << flush;
  }
  if(self_destruct == 0) cerr << "Self-destruct is off for " << fname.c_str() << "\n" << flush;
  return 0;
}

static int SelfDestructOn(const gxString &fname)
{
  FCControlBlock c;
  return EPDBReadControl(fname.c_str(), c) && c.SelfDestruct();
}

static int RemoveUser(const gxString &fname, MemoryBuffer &file_key)
{
  gxList<FCRekeySlot> users;
  ListLoadedUsers(fname, users);
  int has_rsa = 0, has_sc = 0, count = 0;
  for(gxListNode<FCRekeySlot> *p = users.GetHead(); p; p = p->next) {
    count++;
    if(p->data.username == remove_username) {
      if(p->data.type == FC_SLOT_RSA) has_rsa = 1;
      else has_sc = 1;
    }
  }
  if(!has_rsa && !has_sc) {
    cerr << "ERROR: User " << remove_username.c_str() << " has no RSA key or smart card access to " << fname.c_str() << "\n" << flush;
    SetExit(FC_EXIT_NOT_FOUND);
    return -1;
  }
  gxUINT32 type = remove_user_type;
  if(!type) {
    if(has_rsa && has_sc) {
      cerr << "ERROR: User " << remove_username.c_str() << " has RSA key and smart card access to " << fname.c_str()
	   << ", give --remove-user-type=rsa or --remove-user-type=smartcard" << "\n" << flush;
      return -1;
    }
    type = has_sc ? 2 : 1;
  }
  // A database made with RSA keys or smart cards only has a random
  // password nobody knows, so the last such user is only removed when
  // the password or key file was used to open it.
  if(count == 1 && ((type == 1 && has_rsa) || (type == 2 && has_sc)) && !use_password && !use_key_file) {
    cerr << "ERROR: " << remove_username.c_str() << " is the last RSA key or smart card user of " << fname.c_str()
	 << ". The database may have no password anyone knows, so removing it could leave a database nothing can open. "
	 << "Open it with the database password or key file to remove this user." << "\n" << flush;
    LogEvent(fname, "remove-user", remove_username, gxString(), 0, "refused, last user", &file_key);
    SetExit(FC_EXIT_USAGE);
    return -1;
  }
  int keep_backup = !SelfDestructOn(fname);
  gxString backup, err;
  if(EPDBRemoveUser(fname.c_str(), file_key, type, remove_username, keep_backup, backup, err, dry_run) != 0) {
    cerr << "ERROR: The database was not changed: " << err.c_str() << "\n" << flush;
    SetExit(FC_EXIT_NOT_FOUND);
    LogEvent(fname, "remove-user", remove_username, gxString(), 0, "not changed", &file_key);
    return -1;
  }
  if(dry_run) {
    NOTE << "Dry run: removing " << (type == 1 ? "RSA key" : "smart card") << " access for user " << remove_username.c_str()
	 << " would work, " << fname.c_str() << " was not changed" << "\n" << flush;
    return 0;
  }
  NOTE << "Removed " << (type == 1 ? "RSA key" : "smart card") << " access for user " << remove_username.c_str()
       << " from " << fname.c_str() << "\n" << flush;
  if(keep_backup) {
    cerr << "The original database was saved to " << backup.c_str() << " and " << ReplaceFileExtension(backup, ".ehx").c_str()
	 << ", delete them when you no longer need them, the removed user can still open them" << "\n" << flush;
  }
  else {
    cerr << "No backup copy was kept because self-destruct is on" << "\n" << flush;
  }
  cerr << "NOTE: Removing a user does not revoke a database key the user may already have recovered, or copies "
       << "of the database made before. Run epdb_auth_tool --rekey to give the database a new master key." << "\n" << flush;
  LogEvent(fname, "remove-user", remove_username, gxString(), 1, 0, &file_key);
  return 0;
}

static int RekeyDatabase(const gxString &fname, MemoryBuffer &file_key)
{
  gxList<FCRekeySlot> existing, keep;
  ListLoadedUsers(fname, existing);
  int password_supplied = use_password || use_key_file;
  int generate = !rekey_new_password_file.is_null() || !rekey_new_password_fd.is_null();
  int pw_action = 0;
  gxString missing, err;
  int rv = FC_rekey_plan(existing, 1, password_supplied, generate, rekey_in, keep, pw_action, missing, err);
  if(rv == 1) {
    cerr << "ERROR: The rekey of " << fname.c_str() << " needs a choice for every user and the database password, "
	 << "nothing was changed. Missing from the rekey input:" << "\n" << missing.c_str() << flush;
    SetExit(FC_EXIT_USAGE);
    return -1;
  }
  if(rv != 0) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    return -1;
  }
  MemoryBuffer new_secret;
  gxString generated;
  if(pw_action == FC_REKEY_PW_KEEP) {
    if(password_supplied) new_secret = aes_file_decrypt_secret;
    else {
      new_secret.Cat(rekey_in.password.GetSPtr(), rekey_in.password.length());
      gxDatabase *f = OpenEPDB(fname.c_str(), err);
      MemoryBuffer ks;
      int aes_error = 0;
      int ok = f && EPDBUnlock(f, new_secret, ks, err, aes_error) == 0;
      ks.Clear(1);
      if(f) { f->Close(); delete f; }
      if(!ok) {
	cerr << "ERROR: The password= in the rekey input is not the database password of " << fname.c_str() << "\n" << flush;
	new_secret.Clear(1);
	return -1;
      }
    }
  }
  else if(pw_action == FC_REKEY_PW_NEW) {
    if((int)rekey_in.new_password.length() < AES_MIN_SECRET_LEN) {
      cerr << "ERROR: The new password must be at least " << AES_MIN_SECRET_LEN << " characters long" << "\n" << flush;
      return -1;
    }
    new_secret.Cat(rekey_in.new_password.GetSPtr(), rekey_in.new_password.length());
  }
  else if(pw_action == FC_REKEY_PW_GENERATE) {
    if(!FC_generate_secret(generated)) {
      cerr << "ERROR: Cannot generate a password" << "\n" << flush;
      return -1;
    }
    gxString werr;
    int wrv = !rekey_new_password_file.is_null() ?
      FC_write_secret_file(rekey_new_password_file.c_str(), generated, werr) :
      FC_write_secret_fd(rekey_new_password_fd.c_str(), generated, werr);
    if(wrv != 0) {
      cerr << "ERROR: " << werr.c_str() << ", the database was not changed" << "\n" << flush;
      generated.Clear(1);
      return -1;
    }
    new_secret.Cat(generated.GetSPtr(), generated.length());
    generated.Clear(1);
  }
  gxList<EPDBRekeyUser> users;
  for(gxListNode<FCRekeySlot> *p = keep.GetHead(); p; p = p->next) {
    EPDBRekeyUser u;
    u.block_type = (p->data.type == FC_SLOT_RSA) ? 1 : 2;
    u.username = p->data.username;
    u.source = p->data.source;
    users.Add(u);
  }
  int keep_backup = !SelfDestructOn(fname);
  MemoryBuffer new_keyset;
  gxString backup;
  gxString old_id = EPDBIDHex(fname.c_str());
  unsigned char old_lk[FC_LOG_KEY_LEN];
  FC_log_key(EPDBKeysetKey(file_key, EPDB_KEY_MASTER), EPDB_V2_KEY_LEN, EPDBKeysetID(file_key), EPDB_V2_ID_LEN, old_lk);
#ifdef __ENABLE_SMART_CARD__
  SmartCardOB *scp = &sc;
#else
  SmartCardOB *scp = 0;
#endif
  if(dry_run && pw_action == FC_REKEY_PW_GENERATE) {
    // Nothing is kept, remove the generated password again
    if(!rekey_new_password_file.is_null()) futils_remove(rekey_new_password_file.c_str());
  }
  rv = EPDBRekey(fname.c_str(), file_key, pw_action == FC_REKEY_PW_NONE ? 0 : &new_secret, users, scp,
		 keep_backup, new_keyset, backup, err, dry_run);
  new_secret.Clear(1);
  if(rv != 0) {
    cerr << "ERROR: Rekey of " << fname.c_str() << " failed, the database was not changed: " << err.c_str() << "\n" << flush;
    if(pw_action == FC_REKEY_PW_GENERATE) {
      if(!rekey_new_password_file.is_null()) futils_remove(rekey_new_password_file.c_str());
      cerr << "The generated password was not used" << "\n" << flush;
    }
    LogEvent(fname, "rekey", AuthSlotName(), gxString(), 0, "not changed", &file_key);
    OPENSSL_cleanse(old_lk, sizeof(old_lk));
    SetExit(FC_EXIT_USAGE);
    return -1;
  }
  if(dry_run) {
    NOTE << "Dry run: the rekey of " << fname.c_str() << " would work (the new database was written and checked, then removed), "
	 << "the database was not changed" << "\n" << flush;
    new_keyset.Clear(1);
    OPENSSL_cleanse(old_lk, sizeof(old_lk));
    return 0;
  }
  NOTE << "Rekeyed " << fname.c_str() << ": new master key, every field encrypted again" << "\n" << flush;
  for(gxListNode<FCRekeySlot> *p = keep.GetHead(); p; p = p->next) {
    NOTE << "  kept " << (p->data.type == FC_SLOT_RSA ? "RSA key" : "smart card") << " user " << p->data.username.c_str() << "\n" << flush;
  }
  if(pw_action == FC_REKEY_PW_KEEP) NOTE << "  the database password is the same (the master key is new: it is derived from the password with a new salt)" << "\n" << flush;
  if(pw_action == FC_REKEY_PW_NEW) NOTE << "  the database has the new password" << "\n" << flush;
  if(pw_action == FC_REKEY_PW_GENERATE) {
    cerr << "  the database has a generated password, written to "
	 << (rekey_new_password_file.is_null() ? "the file descriptor" : rekey_new_password_file.c_str()) << "\n" << flush;
  }
  if(pw_action == FC_REKEY_PW_NONE) NOTE << "  the database has no password anyone knows, only the RSA key and smart card users above can open it" << "\n" << flush;
  if(keep_backup) {
    cerr << "The original database was saved to " << backup.c_str() << " and " << ReplaceFileExtension(backup, ".ehx").c_str()
	 << ", delete them when you no longer need them, they open with the old credentials" << "\n" << flush;
  }
  else {
    cerr << "No backup copy was kept because self-destruct is on" << "\n" << flush;
  }
  // Access log: close the old chain, then carry it to the new key
  PerUserLog(fname, "rekey", AuthSlotName(), gxString(), 1, 0, &file_key, 0);
  if(access_log) {
    gxString log_file = access_log_file;
    if(log_file.is_null()) log_file = FC_default_log_file(".encrypted_password_database");
    unsigned char new_lk[FC_LOG_KEY_LEN];
    FC_log_key(EPDBKeysetKey(new_keyset, EPDB_KEY_MASTER), EPDB_V2_KEY_LEN, EPDBKeysetID(new_keyset), EPDB_V2_ID_LEN, new_lk);
    gxString lbackup, lerr;
    int lrv = FC_log_rekey(log_file.c_str(), old_id.c_str(), old_lk, FC_hex(EPDBKeysetID(new_keyset), EPDB_V2_ID_LEN).c_str(),
			   new_lk, lbackup, lerr);
    if(lrv == 1) cerr << "WARNING: " << lerr.c_str() << ". The log was kept as " << lbackup.c_str() << " and a new log started" << "\n" << flush;
    else if(lrv < 0) cerr << "WARNING: " << lerr.c_str() << "\n" << flush;
    OPENSSL_cleanse(new_lk, sizeof(new_lk));
  }
  file_key = new_keyset;
  DBStringConfig::crypt_key = new_keyset;
  new_keyset.Clear(1);
  LogEvent(fname, "rekey", AuthSlotName(), gxString(), 1, "logs carried over to the new key", &file_key);
  OPENSSL_cleanse(old_lk, sizeof(old_lk));
  return 0;
}

static int ReadConfig(const gxString &fname, MemoryBuffer &file_key, gxDatabaseConfig &cfg)
{
  gxString err;
  gxDatabase *f = OpenEPDB(fname.c_str(), err);
  if(!f) return 0;
  MemoryBuffer saved = DBStringConfig::crypt_key;
  DBStringConfig::crypt_key = file_key;
  int ok = cfg.ReadConfig(f);
  DBStringConfig::crypt_key = saved;
  f->Close();
  delete f;
  return ok;
}

static int RecordChange(const gxString &fname, MemoryBuffer &file_key)
// Through the core's atomic record batch: a copy is changed, checked
// and renamed over the database
{
  const char *action = (record_op == EPDB_RECORD_ADD) ? "add-record" :
    ((record_op == EPDB_RECORD_UPDATE) ? "update-record" : "delete-record");
  if(record_op == EPDB_RECORD_DELETE && !dry_run) {
    gxString q;
    q << "Delete record " << record_name << " from " << fname;
    if(!ConfirmAtTerminal(q.c_str())) {
      cerr << "The record was not deleted" << "\n" << flush;
      SetExit(FC_EXIT_USAGE);
      return -1;
    }
  }
  gxString err;
  EPDBRecordOp op;
  op.op = (record_op == EPDB_RECORD_ADD) ? EPDB_OP_ADD : ((record_op == EPDB_RECORD_UPDATE) ? EPDB_OP_UPDATE : EPDB_OP_DELETE);
  op.name = record_name;
  if(input_records.GetHead()) op.rec = input_records.GetHead()->data;
  else if(record_fields.GetHead()) {
    gxDatabaseConfig cfg;
    int have_cfg = ReadConfig(fname, file_key, cfg);
    if(EPDBNameValuesToRecord(record_fields, have_cfg ? &cfg : 0, op.rec, err) != 0) {
      cerr << "ERROR: " << err.c_str() << ", nothing was changed" << "\n" << flush;
      SetExit(FC_EXIT_USAGE);
      return -1;
    }
  }
  if(op.op == EPDB_OP_UPDATE) {
    int any = 0;
    for(int i = 0; i < NumDataMembers; i++) any |= op.rec.given[i];
    if(!any) {
      cerr << "ERROR: No fields to update were given on the input" << "\n" << flush;
      SetExit(FC_EXIT_USAGE);
      return -1;
    }
  }
  gxString new_name = op.rec.given[0] ? op.rec.value[0] : record_name;
  gxList<EPDBRecordOp> ops;
  ops.Add(op);
  op.rec.Wipe();
  unsigned done = 0;
  int rv = EPDBRecordBatch(fname.c_str(), file_key, ops, dry_run, done, err);
  ops.GetHead()->data.rec.Wipe();
  if(rv != EPDB_REC_OK) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    SetExit(rv == EPDB_REC_NOT_FOUND ? FC_EXIT_NOT_FOUND : FC_EXIT_USAGE);
    if(!dry_run) LogEvent(fname, action, AuthSlotName(), record_name, 0, "not changed", &file_key);
    return -1;
  }
  if(dry_run) {
    NOTE << "Dry run: the " << action << " of " << record_name.c_str() << " would work, " << fname.c_str() << " was not changed" << "\n" << flush;
    return 0;
  }
  if(record_op == EPDB_RECORD_ADD) NOTE << "Added record " << record_name.c_str() << " to " << fname.c_str() << "\n" << flush;
  if(record_op == EPDB_RECORD_DELETE) NOTE << "Deleted record " << record_name.c_str() << " from " << fname.c_str() << "\n" << flush;
  if(record_op == EPDB_RECORD_UPDATE) {
    NOTE << "Updated record " << record_name.c_str();
    if(new_name != record_name) NOTE << " (now " << new_name.c_str() << ")";
    NOTE << " in " << fname.c_str() << "\n" << flush;
  }
  gxString detail;
  if(new_name != record_name) detail << "renamed to " << new_name;
  LogEvent(fname, action, AuthSlotName(), record_name, 1, detail.is_null() ? 0 : detail.c_str(), &file_key);
  return 0;
}

static int WriteOutput(const gxString &data, int is_secret)
// Requested output to --output-file (a new file only you can read) or
// stdout. Clear text records at a terminal get a warning on stderr.
{
  if(!output_file.is_null()) {
    gxString err;
    if(WritePrivateFile(output_file.c_str(), data.c_str(), data.length(), err) != 0) {
      cerr << "ERROR: " << err.c_str() << "\n" << flush;
      SetExit(FC_EXIT_IO);
      return -1;
    }
    return 0;
  }
#if defined (__UNIX__)
  int tty = isatty(1);
#else
  int tty = _isatty(_fileno(stdout));
#endif
  if(is_secret && tty) {
    NOTE << "WARNING: Writing clear text passwords to the terminal. Use --output-file=file to write a file only you can read." << "\n" << flush;
  }
#if !defined (__UNIX__)
  fflush(stdout);
  _setmode(_fileno(stdout), _O_BINARY); // The same bytes as on Linux
#endif
  fwrite(data.c_str(), 1, data.length(), stdout);
  fflush(stdout);
  return 0;
}

static int OpenRead(const gxString &fname, POD **pod)
{
  gxString err, name = fname;
  *pod = OpenEPDB_POD(name, err);
  if(!*pod) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    SetExit(FC_EXIT_IO);
    return 0;
  }
  return 1;
}

static int ShowRecord(const gxString &fname, MemoryBuffer &file_key)
{
  POD *pod;
  if(!OpenRead(fname, &pod)) return -1;
  EPDBRecord rec;
  gxString err;
  int rv = EPDBReadRecord(pod, file_key, show_record_name, rec, err);
  gxDatabaseConfig cfg;
  int have_cfg = ReadConfig(fname, file_key, cfg);
  delete pod;
  if(rv != EPDB_REC_OK) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    SetExit(rv == EPDB_REC_NOT_FOUND ? FC_EXIT_NOT_FOUND : FC_EXIT_IO);
    return -1;
  }
  gxString out;
  if(json_output) out << EPDBRecordJSON(rec) << "\n";
  else {
    for(int i = 0; i < NumDataMembers; i++) {
      char sbuf[DBStringLength];
      memset(sbuf, 0, sizeof(sbuf));
      MemoryBuffer saved = DBStringConfig::crypt_key;
      DBStringConfig::crypt_key = file_key;
      char *col = have_cfg ? cfg.GetColName(i, sbuf) : 0;
      DBStringConfig::crypt_key = saved;
      out << ((col && *col) ? col : epdb_field_names[i]) << ": ";
      gxString v = rec.value[i];
      v.ReplaceString("\n", "\n    ");
      out << v << "\n";
      SecureWipe(v);
    }
  }
  rv = WriteOutput(out, 1);
  SecureWipe(out);
  PerUserLog(fname, "show-record", AuthSlotName(), show_record_name, rv == 0, 0, &file_key, 0);
  return rv;
}

static int GetField(const gxString &fname, MemoryBuffer &file_key)
// Writes only the value, for scripts. The value is never logged.
{
  int colon = FC_find_last(get_field.c_str(), ":");
  gxString name, field;
  if(colon > 0) name.SetString(get_field.c_str(), (unsigned)colon);
  field = get_field.c_str() + colon + 1;
  gxDatabaseConfig cfg;
  int have_cfg = ReadConfig(fname, file_key, cfg);
  int i = EPDBFieldIndex(field, have_cfg ? &cfg : 0);
  if(i < 0) {
    cerr << "ERROR: Unknown field " << field.c_str() << "\n" << flush;
    SetExit(FC_EXIT_USAGE);
    return -1;
  }
  POD *pod;
  if(!OpenRead(fname, &pod)) return -1;
  EPDBRecord rec;
  gxString err;
  int rv = EPDBReadRecord(pod, file_key, name, rec, err);
  delete pod;
  if(rv != EPDB_REC_OK) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    SetExit(rv == EPDB_REC_NOT_FOUND ? FC_EXIT_NOT_FOUND : FC_EXIT_IO);
    return -1;
  }
  rv = WriteOutput(rec.value[i], 0);
  gxString detail;
  detail << "field=" << epdb_field_names[i];
  PerUserLog(fname, "get-field", AuthSlotName(), name, rv == 0, detail.c_str(), &file_key, 0);
  return rv;
}

static int ListRecordsJSON(const gxString &fname, MemoryBuffer &file_key, const gxString *find, int find_all)
{
  POD *pod;
  if(!OpenRead(fname, &pod)) return -1;
  gxList<EPDBRecord> recs;
  gxString err;
  int rv = EPDBReadAllRecords(pod, file_key, recs, err);
  delete pod;
  if(rv != EPDB_REC_OK) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    SetExit(FC_EXIT_IO);
    return -1;
  }
  gxString out;
  out << "[";
  int n = 0;
  for(gxListNode<EPDBRecord> *p = recs.GetHead(); p; p = p->next) {
    if(find) {
      int match = find_all ? (IFindMatch(p->data.value[0].c_str(), find->c_str(), 0) != -1)
	: (CaseICmp(p->data.value[0], *find) == 0);
      if(!match) continue;
    }
    if(n++) out << ",\n ";
    out << EPDBRecordJSON(p->data);
  }
  out << "]\n";
  EPDBWipeRecords(recs);
  rv = WriteOutput(out, 1);
  SecureWipe(out);
  if(find && n == 0) {
    cerr << "No objects found matching " << find->c_str() << " key name" << "\n" << flush;
    SetExit(FC_EXIT_NOT_FOUND);
    return -1;
  }
  return rv;
}

static int ExportDatabase(const gxString &fname, MemoryBuffer &file_key)
{
  POD *pod;
  if(!OpenRead(fname, &pod)) return -1;
  gxList<EPDBRecord> recs;
  gxString err;
  int rv = EPDBReadAllRecords(pod, file_key, recs, err);
  delete pod;
  if(rv != EPDB_REC_OK) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    SetExit(FC_EXIT_IO);
    return -1;
  }
  gxString out;
  unsigned n = 0;
  if(export_json) {
    out << "[";
    for(gxListNode<EPDBRecord> *p = recs.GetHead(); p; p = p->next) {
      if(n++) out << ",\n ";
      out << EPDBRecordJSON(p->data);
    }
    out << "]\n";
  }
  else {
    gxDatabaseConfig cfg;
    if(ReadConfig(fname, file_key, cfg)) out << EPDBCSVHeader(cfg, file_key);
    for(gxListNode<EPDBRecord> *p = recs.GetHead(); p; p = p->next) {
      out << EPDBCSVRow(p->data);
      n++;
    }
  }
  EPDBWipeRecords(recs);
  rv = WriteOutput(out, 1);
  SecureWipe(out);
  if(rv == 0) NOTE << "Exported " << n << " records from " << fname.c_str() << "\n" << flush;
  PerUserLog(fname, "export", AuthSlotName(), gxString(), rv == 0, export_json ? "json" : "csv", &file_key, 0);
  return rv;
}

static int ImportOrMerge(const gxString &fname, MemoryBuffer &file_key, gxList<EPDBRecord> &recs, const char *action)
{
  unsigned added = 0, replaced = 0, skipped = 0;
  gxString err;
  int policy = merge_policy;
  int rv = EPDBMergeDatabases(fname.c_str(), file_key, recs, policy, dry_run, added, replaced, skipped, err);
  if(rv != EPDB_REC_OK) {
    cerr << "ERROR: " << err.c_str() << ". Nothing was changed." << "\n" << flush;
    SetExit(FC_EXIT_USAGE);
    if(!dry_run) LogEvent(fname, action, AuthSlotName(), gxString(), 0, "not changed", &file_key);
    return -1;
  }
  gxString detail;
  detail << added << " added, " << replaced << " replaced, " << skipped << " skipped";
  if(json_output) {
    cout << "{\"file\": " << FC_json_string(fname).c_str() << ", \"action\": \"" << action << "\", \"dry_run\": "
	 << (dry_run ? "true" : "false") << ", \"added\": " << added << ", \"replaced\": " << replaced
	 << ", \"skipped\": " << skipped << "}" << "\n" << flush;
  }
  NOTE << (dry_run ? "Dry run, nothing was changed: " : "") << action << " " << fname.c_str() << ": " << detail.c_str() << "\n" << flush;
  if(!dry_run) LogEvent(fname, action, AuthSlotName(), gxString(), 1, detail.c_str(), &file_key);
  return 0;
}

static int MergeDatabase(const gxString &fname, MemoryBuffer &file_key)
{
  gxString src = merge_from;
  if(!check_if_file_exists(src)) {
    SetExit(FC_EXIT_NOT_FOUND);
    return -1;
  }
  // The source opens with --merge-password-fd/-file or --merge-key, else
  // with the password or key file used for the destination
  MemoryBuffer secret;
  if(!merge_secret.is_null()) secret.Cat(merge_secret.GetSPtr(), merge_secret.length());
  else if(use_password || use_key_file) secret = aes_file_decrypt_secret;
  else {
    cerr << "ERROR: Give the password of " << src.c_str() << " with --merge-password-fd, --merge-password-file or --merge-key" << "\n" << flush;
    SetExit(FC_EXIT_USAGE);
    return -1;
  }
  gxString err;
  gxDatabase *f = OpenEPDB(src.c_str(), err);
  MemoryBuffer src_key;
  int aes_error = 0;
  int ok = f && EPDBUnlock(f, secret, src_key, err, aes_error) == 0;
  if(f) { f->Close(); delete f; }
  secret.Clear(1);
  if(!ok) {
    cerr << "ERROR: Cannot open " << src.c_str() << ": " << err.c_str() << "\n" << flush;
    SetExit(FC_EXIT_AUTH);
    return -1;
  }
  POD *pod;
  if(!OpenRead(src, &pod)) return -1;
  if(EPDBIsKeyset(src_key) && EPDBVerify(pod, src_key, err) != 0) {
    delete pod;
    cerr << "ERROR: " << src.c_str() << ": " << err.c_str() << "\n" << flush;
    SetExit(FC_EXIT_IO);
    return -1;
  }
  gxList<EPDBRecord> recs;
  int rv = EPDBReadAllRecords(pod, src_key, recs, err);
  delete pod;
  src_key.Clear(1);
  DBStringConfig::crypt_key = file_key;
  if(rv != EPDB_REC_OK) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    SetExit(FC_EXIT_IO);
    return -1;
  }
  rv = ImportOrMerge(fname, file_key, recs, "merge");
  EPDBWipeRecords(recs);
  return rv;
}

static int DatabaseInfo(const gxString &fname, MemoryBuffer &file_key)
{
  POD *pod;
  if(!OpenRead(fname, &pod)) return -1;
  EPDBInfo info;
  gxString err;
  int rv = EPDBGetInfo(fname.c_str(), pod, info, err);
  delete pod;
  if(rv != 0) {
    cerr << "ERROR: " << err.c_str() << "\n" << flush;
    SetExit(FC_EXIT_IO);
    return -1;
  }
  if(json_output) {
    cout << EPDBInfoJSON(fname.c_str(), info).c_str() << "\n" << flush;
    return 0;
  }
  cout << "Database: " << fname.c_str() << "\n";
  cout << "Format: v" << info.format << "\n";
  cout << "Records: " << info.records << "\n";
  if(info.format == 2) {
    cout << "Database ID: " << info.id.c_str() << "\n";
    cout << "Key generation: " << info.key_generation;
    if(info.rekey_time) {
      char stamp[32];
      time_t t = (time_t)info.rekey_time;
      strftime(stamp, sizeof(stamp), "%Y-%m-%dT%H:%M:%SZ", gmtime(&t));
      cout << " (last rekey " << stamp << ")";
    }
    cout << "\n";
    cout << "In-file access log: " << ((info.flags & EPDB_EXT_LOG_IN_FILE) ? "on" : "off") << "\n";
    cout << "Self-destruct: " << (info.sd_on ? "on" : "off");
    if(info.sd_on) cout << ", " << FC_sd_message(info.sd_failed, info.sd_limit).c_str();
    cout << "\n";
  }
  cout << "RSA key users: " << info.rsa_users << ", smart card users: " << info.smartcard_users << "\n";
  for(gxListNode<gxString> *p = info.users.GetHead(); p; p = p->next) cout << "  " << p->data.c_str() << "\n";
  cout << flush;
  return 0;
}

int ExtraOps(const gxString &fname, MemoryBuffer &file_key, int is_v2)
// The GUI features available from the command line. Returns the number
// of operations, or -1 if one failed.
{
  int n = 0;
  if(import_db) {
    n++;
    if(ImportOrMerge(fname, file_key, input_records, "import") != 0) return -1;
  }
  if(!merge_from.is_null()) {
    n++;
    if(MergeDatabase(fname, file_key) != 0) return -1;
  }
  if(!show_record_name.is_null()) {
    n++;
    if(ShowRecord(fname, file_key) != 0) return -1;
  }
  if(!get_field.is_null()) {
    n++;
    if(GetField(fname, file_key) != 0) return -1;
  }
  if(db_info) {
    n++;
    if(DatabaseInfo(fname, file_key) != 0) return -1;
  }
  if(export_db) {
    n++;
    if(ExportDatabase(fname, file_key) != 0) return -1;
  }
  if(json_output && db_list) {
    n++;
    if(ListRecordsJSON(fname, file_key, 0, 0) != 0) return -1;
  }
  if(json_output && db_find_key) {
    n++;
    if(ListRecordsJSON(fname, file_key, &db_find_key_name, db_find_all) != 0) return -1;
  }
  return n;
}

int KeyManagementOps(const gxString &fname, MemoryBuffer &file_key, int is_v2)
// Runs the 10/27/2026 operations on an unlocked and verified database.
// Returns the number of operations, or -1 if one failed.
{
  int n = 0;
  int needs_v2 = remove_user || rekey || verify_access_log || show_access_log ||
    access_log_in_file >= 0 || self_destruct >= 0 || self_destruct_attempts;
  if(needs_v2 && !is_v2) {
    cerr << "ERROR: " << fname.c_str() << " is a v1 database. --remove-user, --rekey, the access log options and "
	 << "self-destruct need a v2 database, upgrade it first with: epdb_auth_tool --upgrade-db " << fname.c_str() << "\n" << flush;
    return -1;
  }
  if(access_log_in_file >= 0 || self_destruct >= 0 || self_destruct_attempts) {
    n++;
    if(SetDatabaseOptions(fname, file_key) != 0) return -1;
  }
  if(remove_user) {
    n++;
    if(RemoveUser(fname, file_key) != 0) return -1;
  }
  if(rekey) {
    n++;
    if(RekeyDatabase(fname, file_key) != 0) return -1;
  }
  if(record_op) {
    n++;
    if(RecordChange(fname, file_key) != 0) return -1;
  }
  if(show_access_log || verify_access_log) {
    n++;
    if(ShowAccessLog(fname, file_key, verify_access_log) != 0) return -1;
  }
  return n;
}
