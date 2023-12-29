// ------------------------------- //
// -------- start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, BCC32, GCC, HPUX aCC, SOLARIS CC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
// Date Last Modified: 12/27/2023
// Copyright (c) 2001-2024 DataReel Software Development
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
#include "c_thread.h"

#ifdef __MSVC_DEBUG__
#include "leaktest.h"
#endif

// Global vars
gxString program_name = "Encrypted Password Database Auth Tool";
gxString version_str = "2023.107";
gxString copyright = "Copyright (c) Datareel Open Source";
gxString copyright_dates = "2003-2023";
gxString produced_by = "Datareel Open Source";
gxString default_url = "https://datareel.com";
gxString release_date = "12/28/2023";
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
int add_smart_card = 0;
int use_smartcard_cert_file = 0;
gxString smartcard_cert_file;
SmartCardOB sc;
gxString smartcard_username;
int use_smartcard = 0;
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
int db_find_all = 0;
gxString db_find_key_name;

// Functions
void DisplayVersion();
void HelpMessage();
int ProcessDashDashArg(gxString &arg);
int ProcessArgs(char *arg);
int DEBUG_m(char *message, int level = 1, int rv = 0);
int ExitProgram(int rv = 0, char *exit_message = 0);
int check_if_file_exists(gxString &fname);
int check_if_file_exists(char *fname);
void set_delimiter_arg(const gxString &equal_arg, char &delimiter_char);

int main(int argc, char **argv)
{
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
      if (arg[0] != '\0') {
	if (arg[0] == '-') { // Look for command line arguments
	  if(!ProcessArgs(arg)) return ExitProgram(1); // Exit if argument is not valid
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

  if(num_files == 0) {
    cerr << "ERROR: No file name specified with command line args" << "\n";
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
      cout << "RSA key passphrase: " << flush;
      if(!consoleGetString(rsa_key_passphrase, 1)) {
	rsa_key_passphrase.Clear(1);
	cout << "\n" << flush;
	cerr << "Invalid entry!" << "\n" << flush;
	return ExitProgram(1);
      }
      cout << "\n" << flush;
    }
  }
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
      cerr << "Smart card engine ID = " << sc.engine_ID << "\n" << flush;
      cerr << "Smart card engine path = " << sc.enginePath << "\n" << flush;
      cerr << "Smart card module path = " << sc.modulePath << "\n" << flush;
      cerr << "Smart card cert ID = " << sc.cert_id << "\n"  << flush;
    }

    if(!check_if_file_exists(sc.enginePath)) {
      cerr << "ERROR: Smart card engine " << sc.enginePath << " does not exist or cannot be read" <<  "\n" << flush;
      return ExitProgram(1);
    }
    if(!futils_exists(sc.modulePath)) {
      cerr << "ERROR: Smart card provider " << sc.modulePath << " does not exist or cannot be read" <<  "\n" << flush;
      return ExitProgram(1);
    }

    if(debug_mode) sc.verbose_mode = 1;
    gxString pin_buf;
    if(sc.pin[0] == 0) {
      cout << "Smart card PIN: " << flush;
      if(!consoleGetString(pin_buf, 1)) {
	pin_buf.Clear(1);
	cout << "\n" << flush;
	cerr << "Invalid entry!" << "\n" << flush;
	return ExitProgram(1);
      }
      sc.SetPin(pin_buf.c_str());
      pin_buf.Clear(1);
      cout << "\n" << flush;
    }
  }
  else if(use_password) {
    if(password.is_null()) {
      cout << "Password: " << flush;
      if(!consoleGetString(password, 1)) {
	cout << "\n" << flush;
	password.Clear(1);
	cerr << "Invalid entry!" << "\n" << flush;
	return ExitProgram(1);
      }
      cout << "\n" << flush;
    }
    aes_file_decrypt_secret.Clear(1);
    aes_file_decrypt_secret.Cat(password.GetSPtr(), password.length());
    DBStringConfig::crypt_key = aes_file_decrypt_secret;
  }
  else if(list_users || db_stats) {
    DEBUG_m("No auth operation");
  }
  else {
    if(clientcfg->verbose_mode) cout << "No decryption method specifed, defaulting to password" << "\n" << flush;
    cout << "Password: " << flush;
    if(!consoleGetString(password, 1)) {
      cout << "\n" << flush;
      password.Clear(1);
      cerr << "Invalid entry!" << "\n" << flush;
      return ExitProgram(1);
    }
    cout << "\n" << flush;
    aes_file_decrypt_secret.Clear(1);
    aes_file_decrypt_secret.Cat(password.GetSPtr(), password.length());
    DBStringConfig::crypt_key = aes_file_decrypt_secret;
  }

  while(ptr) {
    fname = ptr->data;
    if(!check_if_file_exists(fname)) {
      ptr = ptr->next;
      continue; 
    }

    if(list_users) {
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
	ERROR_LEVEL++;
	f->Close();
	delete f;
	ptr = ptr->next;
	continue; 
      }
      aes_file_decrypt_secret = DBStringConfig::crypt_key;
      f->Close();
      delete f;
    }
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
      if(db_auth.DecryptWithSmartcard(&sc, smartcard_username) != 0) {
	cerr << "ERROR: " << db_auth.err.c_str() << "\n";
	ERROR_LEVEL++;
	f->Close();
	delete f;
	ptr = ptr->next;
	continue; 
      }
      aes_file_decrypt_secret = DBStringConfig::crypt_key;
      f->Close();
      delete f;
    }
    
    if(display_db_config) {
      if(PrintDBConfig(fname.c_str(), aes_file_decrypt_secret) != 0) ERROR_LEVEL++;
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
      gxDatabase *f = OpenEPDB(fname.c_str(), err_string);
      if(!f) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      db_auth.f = f;
      if(db_auth.AddRSAKeyToStaticArea(aes_file_decrypt_secret, public_key, public_key_len, add_username) != 0) {
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
    if(add_smart_card) {
      num_operations++;
      if(add_username.is_null()) {
	cerr << "ERROR: --add-smartcard-cert requires --add-username" << "\n" << flush;
	return ExitProgram(1);
      }
      cerr << "Adding smartcard cert for user " << add_username.c_str() << " to encrypted file " << ptr->data.c_str() << "\n" << flush;
      DatabaseUserAuth db_auth;
      gxDatabase *f = OpenEPDB(fname.c_str(), err_string);
      if(!f) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      db_auth.f = f;
      
      if(db_auth.AddSmartCardCertToStaticArea(&sc, use_smartcard_cert_file, aes_file_decrypt_secret, add_username) != 0){
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

    if(db_list) {
      num_operations++;
      int admin_rights;
      POD *pod = OpenEPDB(fname, admin_rights, err_string);
      if(!pod) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      if(!ExportEPDB(pod, delimiter, filter_char, replace_char, replace_char_with)) {
	cout << "No objects exported from the encrypted database" << "\n" << flush;
	ERROR_LEVEL++;
      }
      delete pod;
    }

    if(db_find_key) {
      num_operations++;
      int admin_rights;
      POD *pod = OpenEPDB(fname, admin_rights, err_string);
      if(!pod) {
	cerr << "ERROR: " << err_string.c_str() << "\n";
	ERROR_LEVEL++;
	ptr = ptr->next;
	continue; 
      }
      INFOHOG_t key(db_find_key_name.c_str());
      SearchEPDB(pod->Index(), 0, pod, key, db_find_all);
      if(!ExportListObjectsEPDB(pod, delimiter, filter_char, replace_char, replace_char_with)) {
	cout << "No objects found matching " << db_find_key_name.c_str() << " key name" << "\n" << flush;
	ERROR_LEVEL++;
      }
      delete pod;
    }
    
    if(num_operations == 0) {
      cerr << "INFO: File " << ptr->data.c_str() << " opened, no operations performed" << "\n" << flush;
    }
    ptr = ptr->next;
  }

  return ExitProgram(ERROR_LEVEL);
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

  if(!debug_message.is_null()) DEBUG_m(debug_message.c_str(), debug_level);

  if(debug_mode) {
    cerr << "Smart card: " << sc.err_string << "\n" << flush;
    char err[1024];
    memset(err, 0, sizeof(err));
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    cerr << "Openssl " << err << "\n" << flush;
  }

  if(!exit_message) {
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
  cout << "Smart card enabled for " << SC_get_default_engine_ID() << "\n" << flush;
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
  cout << "          --password (Use a password for symmetric file decryption)" << "\n" << flush;
  cout << "          --rsa-key (Use a private RSA key file for decryption)" << "\n" << flush;
  cout << "          --rsa-key-passphrase (Passphrase for private RSA key)" << "\n" << flush;
  cout << "          --rsa-key-username=name (Username that owns the private RSA key, defaults to current user)" << "\n" << flush;
  cout << "          --smartcard (Use a smart card for decryption)" << "\n" << flush;
  cout << "          --smartcard-pin=pin (Supply smart card PIN on the command line for scripting, use with caution)" << "\n" << flush;
  cout << "          --smartcard-username=name (Username assigned to the smart card cert, defaults to current user)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "Smart card settings:\n" << flush;
  cout << "          --smartcard-cert-id=" << SC_get_default_cert_id() << " (Set the ID number for the smartcard cert)" << "\n" << flush;
  cout << "          --smartcard-engine=" << SC_get_default_enginePath() << " (Set the smartcard engine path)" << "\n" << flush;
  cout << "          --smartcard-provider=" << SC_get_default_modulePath() << " (Set the smartcard provider)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB add user functions:\n" << flush;
  cout << "          --add-rsa-key (Add access to database for another users public RSA key)" << "\n" << flush;
  cout << "          --add-smartcard-cert (Add access to database for another users smart card)" << "\n" << flush;
  cout << "          --add-username (Username being added for public RSA key file or exported smart card cert file)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB no-auth stat functions:\n" << flush;
  cout << "          --db-stats (Display database stats and exit)" << "\n" << flush;
  cout << "          --list-users (List the users with RSA key of Smart Card cert access and exit)" << "\n" << flush;
  cout << "\n" << flush;
  cout << "DB authenticated functions:\n" << flush;
  cout << "          --db-config (Display database config and exit)" << "\n" << flush;
  cout << "          --db-list (List database stdout, defaults to tab delimited output)" << "\n" << flush;
  cout << "          --db-find-key=name (Search the database for a key name and list output to stdout)" << "\n" << flush;
  cout << "          --db-find-all-key=name (Search for all matching key names and list output to stdout)" << "\n" << flush;
  cout << "          --delimiter=\",\" (Set the list delimiter to comma or other value)" << "\n" << flush;
  cout << "          --filter-char (Set a character to filter from list output)" << "\n" << flush;
  cout << "          --replace-char (Set a character to replace in list output)" << "\n" << flush;
  cout << "          --replace-char-with (Set the replacement character to replace in list output)" << "\n" << flush;
  cout << "\n" << flush; // End of list
}

int ProcessDashDashArg(gxString &arg)
{
  gxString sbuf, equal_arg, ebuf;
  int has_valid_args = 0;
  int rv = 0;
  
  if(arg.Find("=") != -1) {
    // Look for equal arguments
    // --log-file="/var/log/my_service.log"
    equal_arg = arg;
    equal_arg.DeleteBeforeIncluding("=");
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
    if(equal_arg.is_null()) {
      cerr << "ERROR: --rsa-key-passphrase requires an input argument" << "\n" << flush;
      return 0;
    }
    rsa_key_passphrase = equal_arg;
    has_valid_args = 1;
  }

  if(arg == "password") {
    if(!equal_arg.is_null()) {
      password = equal_arg;
    }
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
    sc.SetCertID(equal_arg.c_str());
    has_valid_args = 1;
  }

  if(arg == "smartcard-engine") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --smartcard-engine missing path: --smartcard-engine=" <<  SC_get_default_enginePath() << "\n" << flush;
      return 0;
    }
    sc.SetEnginePath(equal_arg.c_str());
    if(!check_if_file_exists(sc.enginePath)) {
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
    sc.SetModulePath(equal_arg.c_str());
    if(!check_if_file_exists(sc.modulePath)) {
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
    sc.SetPin(equal_arg.c_str());
    has_valid_args = 1;
  }

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
    rv = RSA_read_key_file(public_rsa_key_file.c_str(), public_key, sizeof(public_key), &public_key_len, &has_passphrase);
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

  if(arg == "add-smartcard-cert") {
    if(equal_arg.is_null()) {
      cerr << "ERROR: --add-smartcard-cert missing filename: --add-smartcard-cert=${HOME}/certs/certfile.pem" << "\n" << flush;
      return 0;
    }
    add_smart_card = 1;
    use_smartcard_cert_file = 1;
    smartcard_cert_file = equal_arg;
    if(!check_if_file_exists(smartcard_cert_file.c_str())) {
      cerr << "ERROR: Smart card cert file " << smartcard_cert_file.c_str() << " does not exist or cannot be read" <<  "\n" << flush;
      return 0;
    }
    if(SC_read_cert_file(&sc, smartcard_cert_file.c_str()) != 0) {
      cerr << "ERROR: " << sc.err_string << "\n" << flush;
      return 0;
    }
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
  arg[0] = '\0';

  return 1; // All command line arguments were valid
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
