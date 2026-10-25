// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Source Code File Name: epdb_test_db.cpp
// Compiler Used: GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 10/25/2026
// Date Last Modified: 10/25/2026
// Copyright (c) 2003-2026 DataReel Software Development
// ----------------------------------------------------------- //
// ------------- Program Description and Details ------------- //
// ----------------------------------------------------------- //
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

Test helper for the regression tests in console/linux/run_cli_tests.sh.
Creates an encrypted password database with known records and dumps
every record of a database in a fixed escaped format, so tests can
compare databases without the GUI.

Usage:
  epdb_test_db create dbfile.ehd password num_records
  epdb_test_db dump dbfile.ehd password
*/
// ----------------------------------------------------------- //
#include "gxdlcode.h"

#if defined (__USE_ANSI_CPP__) // Use the ANSI Standard C++ library
#include <iostream>
using namespace std; // Use unqualified names for Standard C++ library
#else // Use the old iostream library by default
#include <iostream.h>
#endif // __USE_ANSI_CPP__

#include <stdio.h>
#include "epdb_functions.h"

// Value of record r, field i. Covers empty fields, delimiters, quotes,
// newlines, leading formula characters and long values.
static gxString TestValue(int r, int i)
{
  gxString s;
  switch(i) {
    case 0: s << "Entry " << (r < 10 ? "00" : (r < 100 ? "0" : "")) << r; break;
    case 1: s << "user" << r; break;
    case 2: s << "P@ss\tw0rd," << r << "\"q"; break;
    case 3: if(r % 3) s << "old-" << r; break; // Some empty fields
    case 4: s << "Q1? \"blue\"\nQ2? 42"; break;
    case 5: if(r % 2) s << "=HYPERLINK(\"http://example.com\")"; else s << "+1-555-0100"; break;
    case 6: s << "https://example.com/" << r; break;
    case 7:
      for(int j = 0; j < (r % 7) * 40; j++) s << (char)('a' + ((j + r) % 26));
      break;
  }
  return s;
}

static int Create(const char *fname, const char *password, int num)
{
  DBStringConfig::crypt_key.Clear(1);
  DBStringConfig::crypt_key.Cat(password, strlen(password));

  gxString data_file = fname;
  gxString index_file = fname;
  data_file.DeleteAfterLastIncluding(".");
  data_file += ".ehd";
  index_file.DeleteAfterLastIncluding(".");
  index_file += ".ehx";
  if(futils_exists(data_file.c_str()) || futils_exists(index_file.c_str())) {
    cerr << "ERROR: " << data_file.c_str() << " or " << index_file.c_str() << " already exists" << "\n";
    return 1;
  }

  POD pod;
  InfoHogKey<DBString> key_type;
  FAU_t static_data_size = (FAU_t)(DB_CONFIG_STATIC_AREA_SIZE + DB_AUTH_STATIC_AREA_SIZE);
  gxDatabaseError err = pod.Open(data_file.c_str(), index_file.c_str(), key_type,
				 InfoHogNodeOrder, gxDBASE_READWRITE,
				 InfoHogUseIndexFile, static_data_size,
				 InfoHogNumTrees, 'E', 'E');
  if(err != gxDBASE_NO_ERROR) {
    cerr << "ERROR: " << gxDatabaseExceptionMessage(err) << "\n";
    return 1;
  }

  // Same steps as OpenDatabase() in the GUI for a new file
  unsigned char *static_data = new unsigned char[static_data_size];
  AES_fillrand(static_data, static_data_size);
  err = pod.OpenDataFile()->Write(static_data, static_data_size, pod.OpenDataFile()->FileHeaderSize());
  delete[] static_data;
  if(err != gxDBASE_NO_ERROR) {
    cerr << "ERROR: " << gxDatabaseExceptionMessage(err) << "\n";
    return 1;
  }
  gxDatabaseConfig db_config;
  db_config.SetDatabaseName("test_db");
  db_config.SetPrintDocName("test_db");
  if(!db_config.WriteConfig(pod.OpenDataFile())) {
    cerr << "ERROR: Cannot write the database config" << "\n";
    return 1;
  }

  for(int r = 0; r < num; r++) {
    INFOHOG infohog(&pod, NumDataMembers);
    for(int i = 0; i < NumDataMembers; i++) {
      gxString v = TestValue(r, i);
      if(v.is_null()) continue;
      INFOHOG_t ob(v.c_str());
      infohog.SetMember(&ob, sizeof(ob), i);
    }
    if(!infohog.WriteObject()) {
      cerr << "ERROR: Cannot write record " << r << "\n";
      return 1;
    }
  }
  pod.Flush();
  return 0;
}

static void PrintEscaped(const char *s)
{
  for(; *s; s++) {
    unsigned char c = (unsigned char)*s;
    if(c == '\\') cout << "\\\\";
    else if(c == '|') cout << "\\|";
    else if(c < 32 || c > 126) {
      char buf[8];
      sprintf(buf, "\\x%02x", c);
      cout << buf;
    }
    else cout << (char)c;
  }
}

static int Dump(const char *fname, const char *password)
{
  gxString err_string;
  MemoryBuffer key;
  key.Cat(password, strlen(password));
  gxDatabase *f = OpenEPDB(fname, err_string);
  if(!f) {
    cerr << "ERROR: " << err_string.c_str() << "\n";
    return 1;
  }
  if(UnlockEPDB(f, key, err_string) != 0) {
    cerr << "ERROR: " << err_string.c_str() << "\n";
    f->Close();
    delete f;
    return 1;
  }
  f->Close();
  delete f;

  gxString name = fname;
  POD *pod = OpenEPDB_POD(name, err_string);
  if(!pod) {
    cerr << "ERROR: " << err_string.c_str() << "\n";
    return 1;
  }
  int num = BuildVirtualEPDB(pod);
  int rv = 0;
  dllistptr = dllist->GetHead();
  while(dllistptr) {
    INFOHOG infohog(pod);
    if(!infohog.ReadObject(dllistptr->data)) {
      cerr << "ERROR: Cannot read record" << "\n";
      rv = 1;
      break;
    }
    for(int i = 0; i < NumDataMembers; i++) {
      if(i > 0) cout << "|";
      if(infohog.GetMemberLen(i) != sizeof(INFOHOG_t)) {
	cout << "<none>";
	continue;
      }
      int error_level = 0;
      char *s = ((DBString *)infohog.GetMember(i))->GetString(DBStringConfig::crypt_key, error_level);
      if(!s || error_level) {
	cout << "<error>";
	rv = 1;
	continue;
      }
      PrintEscaped(s);
      if(s != DBStringNULLPtr::DBStringNUllStr) delete[] s;
    }
    cout << "\n";
    dllistptr = dllistptr->next;
  }
  cout << flush;
  delete pod;
  if(num == 0) cerr << "INFO: No records" << "\n";
  return rv;
}

int main(int argc, char **argv)
{
  AES_openssl_init();
  if(argc == 5 && strcmp(argv[1], "create") == 0) {
    return Create(argv[2], argv[3], atoi(argv[4]));
  }
  if(argc == 4 && strcmp(argv[1], "dump") == 0) {
    return Dump(argv[2], argv[3]);
  }
  cerr << "Usage: " << argv[0] << " create dbfile.ehd password num_records" << "\n";
  cerr << "       " << argv[0] << " dump dbfile.ehd password" << "\n";
  return 1;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
