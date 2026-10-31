// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 10/31/2026
// Date Last Modified: 10/31/2026
// Copyright (c) 2001-2026 DataReel Software Development
// ----------------------------------------------------------- //
// ------------- Program Description and Details ------------- //
// ----------------------------------------------------------- //
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
USA

Test driver for crash-safe saves (epdb_journal.h), used by
run_journal_tests.sh. Never run it on a real database: the writer
changes the database until it is killed.

  epdb_journal_test writer db.ehd password start
    Opens the database the way the GUI does (recovery, read-write
    open, journal registration with a MAC reseal) and makes saves
    start, start+1, ... until it is killed. Save i adds record J<i>,
    every third save also changes record J<i-1>, every fifth deletes
    J<i-3>, every seventh changes the database config, and every save
    adds an in-file access log entry. Prints i+1 after save i is
    written.

  epdb_journal_test verify db.ehd password acked
    Opens the database (which finishes an interrupted save), checks
    the MAC and that the index lists exactly the records in the data
    file (EPDBVerify(), which walks every data block and index node),
    that the index keys are in order (EPDBIndexInOrder()), decrypts
    every record and checks the records,
    the config and the access log are the state after "acked" or
    "acked + 1" saves, never anything in between. Prints the number
    of saves found.

  epdb_journal_test time db.ehd password count journal
    Average time of "count" saves that each add a record: journal 1
    is this build (journal, MAC in each save), journal 0 writes the
    files directly without a MAC update, as before, journal 2 writes
    the files directly with a MAC update in each save.
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
#include <stdlib.h>
#include <time.h>
#include "epdb_functions.h"
#include "epdb_core.h"
#include "epdb_journal.h"

#if defined (__UNIX__)
#include <sys/time.h>
#endif

const int JT_BASE_RECORDS = 20; // Made by "epdb_test_db create"

static gxString JTName(long i)
{
  char buf[32];
  sprintf(buf, "J%06ld", i);
  return gxString(buf);
}

static gxString JTNotes(long i)
// Notes of varying length, so records are reclaimed and moved
{
  gxString s;
  for(long j = 0; j < (i * 37) % 300; j++) s << (char)('a' + ((i + j) % 26));
  return s;
}

struct JTState {
  // Record J<i>: present, password field
  gxList<long> ids;
  gxList<gxString> pw;
  gxString doc_name;
};

static void JTModel(long n, JTState &st)
// The J records and config after saves 0 .. n-1
{
  const long max_i = n;
  char *present = new char[max_i + 1];
  gxString *pw = new gxString[max_i + 1];
  for(long i = 0; i < max_i; i++) {
    present[i] = 1;
    pw[i] << "pw-" << i;
    if(i % 3 == 2 && i >= 1 && present[i - 1]) { pw[i - 1].Clear(); pw[i - 1] << "upd-" << i; }
    if(i % 5 == 4 && i >= 3) present[i - 3] = 0;
  }
  st.ids.ClearList();
  st.pw.ClearList();
  for(long i = 0; i < max_i; i++) {
    if(!present[i]) continue;
    st.ids.Add(i);
    st.pw.Add(pw[i]);
  }
  st.doc_name = "test_db";
  for(long i = 0; i < max_i; i++) if(i % 7 == 6) { st.doc_name.Clear(); st.doc_name << "doc-" << i; }
  delete[] present;
  delete[] pw;
}

struct JTReseal {
  POD *pod;
  MemoryBuffer *keyset;
};

static void JTResealFunction(void *data)
{
  JTReseal *r = (JTReseal *)data;
  gxString err;
  EPDBSeal(r->pod->OpenDataFile(), *r->keyset, err);
}

static POD *JTOpen(const char *fname, const char *password, MemoryBuffer &keyset, int journal)
{
  gxString err, name = fname;
  POD *pod = OpenEPDB_POD(name, err, gxDBASE_READWRITE); // Recovery and registration
  if(!pod) {
    cerr << "ERROR: " << err.c_str() << "\n";
    return 0;
  }
  if(!journal) EPDBJournalUnregister(pod);
  MemoryBuffer key;
  key.Cat(password, strlen(password));
  if(UnlockEPDB(pod->OpenDataFile(), key, err) != 0) {
    cerr << "ERROR: " << err.c_str() << "\n";
    CloseEPDB_POD(pod);
    return 0;
  }
  keyset = key;
  DBStringConfig::DBStringCaseCompare = 0;
  return pod;
}

static int JTSave(POD *pod, MemoryBuffer &keyset, long i)
{
  EPDBSave save(pod); // As GUIUndoBefore()/GUIUndoAfter() in the GUI
  gxString err;
  EPDBRecordOp op;
  op.op = EPDB_OP_ADD;
  op.name = JTName(i);
  op.rec.value[0] = JTName(i); op.rec.given[0] = 1;
  op.rec.value[1] << "user " << i; op.rec.given[1] = 1;
  op.rec.value[2] << "pw-" << i; op.rec.given[2] = 1;
  op.rec.value[7] = JTNotes(i); op.rec.given[7] = 1;
  if(EPDBApplyRecordOp(pod, keyset, op, 0, err) != EPDB_REC_OK) {
    cerr << "ERROR: add " << op.name.c_str() << ": " << err.c_str() << "\n";
    return -1;
  }
  if(i % 3 == 2 && i >= 1) {
    EPDBRecordOp u;
    u.op = EPDB_OP_UPDATE;
    u.name = JTName(i - 1);
    u.rec.value[2] << "upd-" << i; u.rec.given[2] = 1;
    int rv = EPDBApplyRecordOp(pod, keyset, u, 0, err);
    if(rv != EPDB_REC_OK && rv != EPDB_REC_NOT_FOUND) {
      cerr << "ERROR: update: " << err.c_str() << "\n";
      return -1;
    }
  }
  if(i % 5 == 4 && i >= 3) {
    EPDBRecordOp d;
    d.op = EPDB_OP_DELETE;
    d.name = JTName(i - 3);
    int rv = EPDBApplyRecordOp(pod, keyset, d, 0, err);
    if(rv != EPDB_REC_OK && rv != EPDB_REC_NOT_FOUND) {
      cerr << "ERROR: delete: " << err.c_str() << "\n";
      return -1;
    }
  }
  if(i % 7 == 6) {
    gxDatabaseConfig cfg;
    cfg.ReadConfig(pod->OpenDataFile());
    gxString doc;
    doc << "doc-" << i;
    cfg.SetPrintDocName(doc.c_str());
    if(!cfg.WriteConfig(pod->OpenDataFile())) {
      cerr << "ERROR: config" << "\n";
      return -1;
    }
  }
  FCRingEntry r;
  r.action = FC_action_code("update-record");
  r.auth = FC_auth_code("password");
  r.result = 1;
  if(EPDBRingAppend(pod->OpenDataFile(), keyset, r, "", err) != 0) {
    cerr << "ERROR: log: " << err.c_str() << "\n";
    return -1;
  }
  pod->Flush();
  return save.End();
}

static int Writer(const char *fname, const char *password, long start)
{
  MemoryBuffer keyset;
  POD *pod = JTOpen(fname, password, keyset, 1);
  if(!pod) return 1;
  JTReseal rs;
  rs.pod = pod;
  rs.keyset = &keyset;
  EPDBJournalRegister(pod, fname, JTResealFunction, &rs);
  {
    gxList<FCRingEntry> entries;
    unsigned long long written = 0;
    gxString err;
    if(EPDBRingRead(pod->OpenDataFile(), keyset, entries, written, err) != 0) {
      EPDBSave save(pod);
      if(EPDBRingInit(pod->OpenDataFile(), keyset, err) != 0) {
	cerr << "ERROR: " << err.c_str() << "\n";
	return 1;
      }
    }
  }
  for(long i = start;; i++) {
    if(JTSave(pod, keyset, i) != 0) {
      cerr << "ERROR: save " << i << " failed: " << EPDBJournalLastError().c_str() << "\n";
      return 1;
    }
    cout << (i + 1) << "\n" << flush;
  }
  return 0;
}

static int JTCompare(gxList<EPDBRecord> &recs, long n, const gxString &doc_name, gxString &why)
// Returns 1 if the records and config are the state after n saves
{
  JTState st;
  JTModel(n, st);
  if(doc_name != st.doc_name) {
    why << clear << "config " << doc_name << " is not " << st.doc_name;
    return 0;
  }
  int base = 0;
  gxListNode<long> *pi = st.ids.GetHead();
  gxListNode<gxString> *pp = st.pw.GetHead();
  for(gxListNode<EPDBRecord> *p = recs.GetHead(); p; p = p->next) {
    EPDBRecord &r = p->data;
    if(r.value[0].Find("Entry ") == 0) { base++; continue; }
    if(!pi) {
      why << clear << "extra record " << r.value[0];
      return 0;
    }
    gxString want_pw = pp->data;
    if(r.value[0] != JTName(pi->data) || r.value[2] != want_pw ||
       r.value[7] != JTNotes(pi->data)) {
      why << clear << "record " << r.value[0] << " is not " << JTName(pi->data) << " " << want_pw;
      return 0;
    }
    pi = pi->next;
    pp = pp->next;
  }
  if(pi) {
    why << clear << "missing record " << JTName(pi->data);
    return 0;
  }
  if(base != JT_BASE_RECORDS) {
    why << clear << base << " of the " << JT_BASE_RECORDS << " first records";
    return 0;
  }
  return 1;
}

static int Verify(const char *fname, const char *password, long acked)
{
  MemoryBuffer keyset;
  POD *pod = JTOpen(fname, password, keyset, 1);
  if(!pod) {
    cout << "FAIL open" << "\n";
    return 1;
  }
  gxString err;
  int rv = 0;
  if(EPDBVerify(pod, keyset, err) != 0) {
    cout << "FAIL integrity: " << err.c_str() << "\n";
    rv = 1;
  }
  if(rv == 0 && EPDBIndexInOrder(pod, keyset) != 1) {
    cout << "FAIL the index keys are not in order" << "\n";
    rv = 1;
  }
  gxList<EPDBRecord> recs;
  if(rv == 0 && EPDBReadAllRecords(pod, keyset, recs, err) != EPDB_REC_OK) {
    cout << "FAIL read: " << err.c_str() << "\n";
    rv = 1;
  }
  gxDatabaseConfig cfg;
  cfg.ReadConfig(pod->OpenDataFile());
  char dest[DBStringLength];
  gxString doc_name = cfg.print_doc_name.c_str(dest);
  long found = -1;
  gxString why_a, why_b;
  if(rv == 0) {
    if(JTCompare(recs, acked, doc_name, why_a)) found = acked;
    else if(JTCompare(recs, acked + 1, doc_name, why_b)) found = acked + 1;
    else {
      cout << "FAIL state: after " << acked << " saves: " << why_a.c_str()
	   << "; after " << (acked + 1) << ": " << why_b.c_str() << "\n";
      rv = 1;
    }
  }
  if(rv == 0) {
    gxList<FCRingEntry> entries;
    unsigned long long written = 0;
    if(found > 0 && EPDBRingRead(pod->OpenDataFile(), keyset, entries, written, err) != 0) {
      cout << "FAIL access log: " << err.c_str() << "\n";
      rv = 1;
    }
  }
  EPDBWipeRecords(recs);
  CloseEPDB_POD(pod);
  if(rv == 0 && EPDBRecoveryPending(fname)) {
    cout << "FAIL a journal or swap marker was left" << "\n";
    rv = 1;
  }
  if(rv == 0) cout << "PASS " << found << "\n";
  return rv;
}

static double JTNow()
{
#if defined (__UNIX__)
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1e6;
#else
  return (double)clock() / CLOCKS_PER_SEC;
#endif
}

static int Time(const char *fname, const char *password, long count, int journal)
{
  MemoryBuffer keyset;
  POD *pod = JTOpen(fname, password, keyset, journal == 1);
  if(!pod) return 1;
  JTReseal rs;
  rs.pod = pod;
  rs.keyset = &keyset;
  if(journal == 1) EPDBJournalRegister(pod, fname, JTResealFunction, &rs);
  double t0 = JTNow();
  for(long i = 0; i < count; i++) {
    gxString err;
    EPDBSave save(pod);
    EPDBRecordOp op;
    op.op = EPDB_OP_ADD;
    op.name << "T" << (journal ? "j" : "d") << i;
    op.rec.value[0] = op.name; op.rec.given[0] = 1;
    op.rec.value[2] << "pw-" << i; op.rec.given[2] = 1;
    if(EPDBApplyRecordOp(pod, keyset, op, 0, err) != EPDB_REC_OK) {
      cerr << "ERROR: " << err.c_str() << "\n";
      return 1;
    }
    pod->Flush();
    if(save.End() != 0) return 1;
    if(journal == 2) EPDBSeal(pod->OpenDataFile(), keyset, err);
  }
  double t = JTNow() - t0;
  if(!journal) { // As the GUI did before: the MAC once, at close
    gxString err;
    EPDBSeal(pod->OpenDataFile(), keyset, err);
  }
  CloseEPDB_POD(pod);
  printf("%s: %ld saves, %.3f ms per save\n", journal == 1 ? "journal, MAC in each save" :
	 (journal == 2 ? "direct, MAC in each save" : "direct, no MAC (as before)"), count, t * 1000.0 / count);
  return 0;
}

int main(int argc, char **argv)
{
  if(argc == 5 && strcmp(argv[1], "writer") == 0) return Writer(argv[2], argv[3], atol(argv[4]));
  if(argc == 5 && strcmp(argv[1], "verify") == 0) return Verify(argv[2], argv[3], atol(argv[4]));
  if(argc == 6 && strcmp(argv[1], "time") == 0) return Time(argv[2], argv[3], atol(argv[4]), atoi(argv[5]));
  cerr << "Usage: epdb_journal_test writer|verify db.ehd password n" << "\n"
       << "       epdb_journal_test time db.ehd password count journal" << "\n";
  return 2;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
