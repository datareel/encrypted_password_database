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
#include "epdb_core.h"

#if defined (__UNIX__)
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#endif

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

static int Create(const char *fname, const char *password, int num, int v2)
{
  DBStringConfig::crypt_key.Clear(1);
  DBStringConfig::crypt_key.Cat(password, strlen(password));
  DBStringConfig::DBStringCaseCompare = 0; // As the GUI builds the index

  gxString data_file = ReplaceFileExtension(fname, ".ehd");
  gxString index_file = ReplaceFileExtension(fname, ".ehx");
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
  if(v2) { // Database format v2, as the GUI creates new databases
    MemoryBuffer keyset;
    gxString err;
    if(EPDBInitV2(pod.OpenDataFile(), DBStringConfig::crypt_key, keyset, err) != 0) {
      cerr << "ERROR: " << err.c_str() << "\n";
      return 1;
    }
    DBStringConfig::crypt_key = keyset;
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
  if(v2) {
    gxString err;
    if(EPDBSeal(pod.OpenDataFile(), DBStringConfig::crypt_key, err) != 0) {
      cerr << "ERROR: " << err.c_str() << "\n";
      return 1;
    }
  }
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
  if(EPDBIsKeyset(DBStringConfig::crypt_key) && EPDBVerify(pod, DBStringConfig::crypt_key, err_string) != 0) {
    cerr << "ERROR: " << err_string.c_str() << "\n";
    delete pod;
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

static int SelfTest(const char *dir)
// Tests the helpers in m_globals.cpp the GUI uses for links, CSV export
// and private files, and the process setup.
{
  int fails = 0;
  struct { const char *s; int link; } links[] = {
    { "https://example.com/x", 1 }, { "HTTP://example.com", 1 }, { "http://", 0 },
    { "\\\\server\\share\\x.exe", 0 }, { "C:\\Windows\\x.exe", 0 },
    { "file:///etc/passwd", 0 }, { "javascript://x", 0 }, { "mailto:a@b.c", 0 },
    { "see https://example.com", 0 }, { "ftp://x.y", 0 }, { 0, 0 } };
  for(int i = 0; links[i].s; i++) {
    if(IsHyperlink(gxString(links[i].s)) != links[i].link) {
      cerr << "FAIL: IsHyperlink(" << links[i].s << ")" << "\n"; fails++;
    }
  }
  const char *values[] = { "=cmd|' /C calc'!A0", "+1", "-1", "@SUM(A1)", "'=x", "''+y",
			   "plain", "'quoted", "", "a=b", 0 };
  for(int i = 0; values[i]; i++) {
    gxString v(values[i]);
    EscapeCSVFormula(v);
    if(CSVFormulaPrefixLength(gxString(values[i])) >= 0 && v[0] != '\'') {
      cerr << "FAIL: EscapeCSVFormula(" << values[i] << ") not escaped" << "\n"; fails++;
    }
    UnescapeCSVFormula(v);
    if(v != gxString(values[i])) {
      cerr << "FAIL: CSV formula escaping does not round trip for " << values[i] << "\n"; fails++;
    }
  }
  gxString out;
  AppendCSVField(out, gxString("a,b\nc"), ',', '\t');
  if(out != gxString("\"a,b c\"")) { cerr << "FAIL: AppendCSVField " << out.c_str() << "\n"; fails++; }

  gxString fname, err;
  fname << dir << "/private_file.txt";
  if(WritePrivateFile(fname.c_str(), "secret\n", 7, err) != 0) {
    cerr << "FAIL: WritePrivateFile " << err.c_str() << "\n"; fails++;
  }
#if defined (__UNIX__)
  else {
    struct stat st;
    if(lstat(fname.c_str(), &st) != 0 || (st.st_mode & 0777) != 0600) {
      cerr << "FAIL: WritePrivateFile mode is not 0600" << "\n"; fails++;
    }
  }
  gxString target, link;
  target << dir << "/link_target.txt";
  link << dir << "/link.txt";
  unlink(link.c_str());
  if(symlink(target.c_str(), link.c_str()) == 0) {
    // An existing link is removed and replaced, the target is not written
    if(WritePrivateFile(link.c_str(), "x", 1, err) != 0 || futils_exists(target.c_str())) {
      cerr << "FAIL: WritePrivateFile followed a symbolic link" << "\n"; fails++;
    }
  }
  SecureProcessSetup();
  mode_t old_mask = umask(077);
  if(old_mask != 077) { cerr << "FAIL: SecureProcessSetup umask" << "\n"; fails++; }
  struct rlimit rl;
  if(getrlimit(RLIMIT_CORE, &rl) != 0 || rl.rlim_cur != 0) {
    cerr << "FAIL: SecureProcessSetup core size limit" << "\n"; fails++;
  }
#endif
  cout << (fails ? "SELFTEST FAIL" : "SELFTEST PASS") << "\n";
  return fails ? 1 : 0;
}

static int AddRSA(const char *fname, const char *password, const char *pubkey_file,
		  const char *username, int use_oaep)
// Adds an RSA user block with PKCS#1 v1.5 or OAEP padding
{
  char public_key[RSA_max_keybuf_len];
  unsigned public_key_len = 0;
  if(RSA_read_key_file(pubkey_file, public_key, sizeof(public_key), &public_key_len) != RSA_NO_ERROR) {
    cerr << "ERROR: Cannot read " << pubkey_file << "\n";
    return 1;
  }
  gxString err_string;
  gxDatabase *f = OpenEPDB(fname, err_string, gxDBASE_READWRITE);
  if(!f) {
    cerr << "ERROR: " << err_string.c_str() << "\n";
    return 1;
  }
  MemoryBuffer secret;
  secret.Cat(password, strlen(password));
  DatabaseUserAuth db_auth;
  db_auth.f = f;
  int rv = db_auth.AddRSAKeyToStaticArea(secret, public_key, public_key_len, gxString(username), 0, use_oaep);
  if(rv != 0) cerr << "ERROR: " << db_auth.err.c_str() << "\n";
  f->Close();
  delete f;
  return rv != 0;
}

static int UndoTest(const char *fname, const char *password)
// 10/27/2026: Tests the toolkit independent undo/redo model the GUI uses
{
  gxString err, name = fname;
  MemoryBuffer key;
  key.Cat(password, strlen(password));
  gxDatabase *f = OpenEPDB(fname, err);
  if(!f || UnlockEPDB(f, key, err) != 0) {
    cerr << "ERROR: " << err.c_str() << "\n";
    return 1;
  }
  f->Close();
  delete f;
  MemoryBuffer ks = DBStringConfig::crypt_key;
  POD *pod = OpenEPDB_POD(name, err, gxDBASE_READWRITE);
  if(!pod) {
    cerr << "ERROR: " << err.c_str() << "\n";
    return 1;
  }
  int fails = 0;
  EPDBUndoStack u;
  EPDBRecord b, a, chk;
  // A change made the way a GUI does: snapshot, write, record
  EPDBRecordOp op;
  op.op = EPDB_OP_ADD; op.name = "U1"; op.rec.value[1] = "user1"; op.rec.given[1] = 1;
  int ex = EPDBUndoStack::Snapshot(pod, ks, "U1", b);
  EPDBApplyRecordOp(pod, ks, op, 0, err);
  u.Changed(pod, ks, ex, b, "U1");
  op.op = EPDB_OP_UPDATE; op.rec.Clear(); op.rec.value[0] = "U2"; op.rec.given[0] = 1;
  op.rec.value[2] = "pw2"; op.rec.given[2] = 1;
  ex = EPDBUndoStack::Snapshot(pod, ks, "U1", b);
  EPDBApplyRecordOp(pod, ks, op, 0, err);
  u.Changed(pod, ks, ex, b, "U2");
  if(u.Depth() != 2 || u.UndoLabel() != gxString("Change U1")) { cerr << "FAIL: two steps recorded" << "\n"; fails++; }
  // Undo the rename and update, then the add
  if(u.Undo(pod, ks, err) != EPDB_REC_OK || EPDBReadRecord(pod, ks, "U1", chk, err) != EPDB_REC_OK ||
     chk.value[1] != gxString("user1") || !chk.value[2].is_null() || EPDBReadRecord(pod, ks, "U2", chk, err) == EPDB_REC_OK) {
    cerr << "FAIL: undo of the change restores U1" << "\n"; fails++;
  }
  if(u.Undo(pod, ks, err) != EPDB_REC_OK || EPDBReadRecord(pod, ks, "U1", chk, err) == EPDB_REC_OK) {
    cerr << "FAIL: undo of the add removes U1" << "\n"; fails++;
  }
  if(u.CanUndo() || !u.CanRedo()) { cerr << "FAIL: nothing left to undo, redo available" << "\n"; fails++; }
  // Redo both
  if(u.Redo(pod, ks, err) != EPDB_REC_OK || u.Redo(pod, ks, err) != EPDB_REC_OK ||
     EPDBReadRecord(pod, ks, "U2", chk, err) != EPDB_REC_OK || chk.value[2] != gxString("pw2")) {
    cerr << "FAIL: redo twice gives U2 with pw2" << "\n"; fails++;
  }
  // Delete and undo
  ex = EPDBUndoStack::Snapshot(pod, ks, "U2", b);
  op.op = EPDB_OP_DELETE; op.name = "U2"; op.rec.Clear();
  EPDBApplyRecordOp(pod, ks, op, 0, err);
  u.Changed(pod, ks, ex, b, "U2");
  if(u.CanRedo()) { cerr << "FAIL: a new change clears redo" << "\n"; fails++; }
  if(u.Undo(pod, ks, err) != EPDB_REC_OK || EPDBReadRecord(pod, ks, "U2", chk, err) != EPDB_REC_OK ||
     chk.value[1] != gxString("user1")) {
    cerr << "FAIL: undo of the delete restores U2" << "\n"; fails++;
  }
  // A group (import) is one step
  u.BeginGroup("Import");
  for(int i = 0; i < 5; i++) {
    gxString n;
    n << "G" << i;
    ex = EPDBUndoStack::Snapshot(pod, ks, n, b);
    op.op = EPDB_OP_ADD; op.name = n; op.rec.Clear();
    EPDBApplyRecordOp(pod, ks, op, 0, err);
    u.Changed(pod, ks, ex, b, n);
  }
  u.EndGroup();
  if(u.UndoLabel() != gxString("Import") || u.Undo(pod, ks, err) != EPDB_REC_OK ||
     EPDBReadRecord(pod, ks, "G3", chk, err) == EPDB_REC_OK) {
    cerr << "FAIL: undo of a group removes every record of the group" << "\n"; fails++;
  }
  // The depth is capped
  u.Clear();
  u.SetDepth(3);
  for(int i = 0; i < 5; i++) {
    gxString n;
    n << "D" << i;
    ex = EPDBUndoStack::Snapshot(pod, ks, n, b);
    op.op = EPDB_OP_ADD; op.name = n; op.rec.Clear();
    EPDBApplyRecordOp(pod, ks, op, 0, err);
    u.Changed(pod, ks, ex, b, n);
  }
  if(u.Depth() != 3) { cerr << "FAIL: depth capped at 3" << "\n"; fails++; }
  u.Clear();
  if(u.CanUndo() || u.CanRedo()) { cerr << "FAIL: Clear() empties both stacks" << "\n"; fails++; }
  pod->Flush();
  if(EPDBIsKeyset(ks) && EPDBSeal(pod->OpenDataFile(), ks, err) != 0) { cerr << "FAIL: seal" << "\n"; fails++; }
  delete pod;
  cout << (fails ? "UNDOTEST FAIL" : "UNDOTEST PASS") << "\n";
  return fails ? 1 : 0;
}

int main(int argc, char **argv)
{
  AES_openssl_init();
  if(argc == 7 && strcmp(argv[1], "addrsa") == 0) {
    return AddRSA(argv[2], argv[3], argv[4], argv[5], strcmp(argv[6], "oaep") == 0);
  }
  if(argc == 4 && strcmp(argv[1], "undotest") == 0) {
    return UndoTest(argv[2], argv[3]);
  }
  if(argc == 3 && strcmp(argv[1], "selftest") == 0) {
    return SelfTest(argv[2]);
  }
  if(argc == 5 && strcmp(argv[1], "create") == 0) {
    return Create(argv[2], argv[3], atoi(argv[4]), 1);
  }
  if(argc == 5 && strcmp(argv[1], "create-v1") == 0) {
    return Create(argv[2], argv[3], atoi(argv[4]), 0);
  }
  if(argc == 4 && strcmp(argv[1], "dump") == 0) {
    return Dump(argv[2], argv[3]);
  }
  cerr << "Usage: " << argv[0] << " create dbfile.ehd password num_records (format v2)" << "\n";
  cerr << "       " << argv[0] << " create-v1 dbfile.ehd password num_records" << "\n";
  cerr << "       " << argv[0] << " dump dbfile.ehd password" << "\n";
  cerr << "       " << argv[0] << " selftest dir" << "\n";
  cerr << "       " << argv[0] << " undotest dbfile.ehd password (undo/redo model test)" << "\n";
  cerr << "       " << argv[0] << " addrsa dbfile.ehd password public_key.pem username oaep|pkcs1" << "\n";
  return 1;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
