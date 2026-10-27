// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
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

Encrypted Password Database core functions that do not depend on a
GUI toolkit, used by epdb_auth_tool and the GUI: record changes,
atomic record batches, JSON and CSV conversion, database info, merge,
the password generator and the undo/redo model. Key management, the
logs and self-destruct are in db_auth.h and fcrypt's fckeymgmt.h.

RECORD JSON (the --json output and the JSON input use the same shape)

  {"title": "My Bank", "username": "alice", "password": "...",
   "previous-password": "", "security-questions": "",
   "account-information": "", "website-link": "", "notes": ""}

Every key is optional except "title". An array of such objects is a
batch, applied all or nothing. Unknown keys, duplicate keys and values
that are not strings are refused.
*/
// ----------------------------------------------------------- //
#ifndef __EPDB_CORE_HPP__
#define __EPDB_CORE_HPP__

#include "m_globals.h"

// Field names, in record member order
extern const char *epdb_field_names[NumDataMembers];

// Field number for a field name: the names above, field1 to field8, or
// the database's own column name (spaces, case and _ do not matter).
// Returns -1 if not found.
int EPDBFieldIndex(const gxString &name, gxDatabaseConfig *cfg);

// The values of one record. given[i] is set for each value supplied (an
// update changes only those, an empty value clears the field).
struct EPDBRecord {
  EPDBRecord() { Clear(); }
  ~EPDBRecord() { Wipe(); }
  EPDBRecord(const EPDBRecord &ob) { Copy(ob); }
  EPDBRecord &operator=(const EPDBRecord &ob) { if(&ob != this) Copy(ob); return *this; }
  void Copy(const EPDBRecord &ob);
  void Wipe();
  void Clear() { for(int i = 0; i < NumDataMembers; i++) given[i] = 0; }
  gxString value[NumDataMembers];
  int given[NumDataMembers];
};

const int EPDB_OP_ADD = 1;
const int EPDB_OP_UPDATE = 2;
const int EPDB_OP_DELETE = 3;
const int EPDB_OP_PUT = 4;    // Add, or replace every field when the record exists (merge, undo)
const int EPDB_OP_ADD_SKIP = 5; // Add unless the record exists (merge)

struct EPDBRecordOp {
  EPDBRecordOp() { op = 0; result = 0; }
  int op;
  int result;        // Set by EPDBRecordBatch(): 1 added, 2 replaced or changed, 3 deleted, 4 skipped
  gxString name;     // Record to change (title before the change)
  EPDBRecord rec;    // Values for add, update and put
};

// Error codes returned by the record functions
const int EPDB_REC_OK = 0;
const int EPDB_REC_ERROR = -1;
const int EPDB_REC_NOT_FOUND = -3;
const int EPDB_REC_EXISTS = -4;

// Reads a record by name from an open database (DBStringConfig::crypt_key
// is not used, key is). Returns EPDB_REC_OK or EPDB_REC_NOT_FOUND.
int EPDBReadRecord(POD *pod, const MemoryBuffer &key, const gxString &name, EPDBRecord &rec, gxString &err);
// Decrypts the fields of a record read with INFOHOG::ReadObject()
int EPDBLoadRecord(INFOHOG &ih, const MemoryBuffer &key, EPDBRecord &rec, gxString &err);
// Every record, in index order
int EPDBReadAllRecords(POD *pod, const MemoryBuffer &key, gxList<EPDBRecord> &recs, gxString &err);
void EPDBWipeRecords(gxList<EPDBRecord> &recs);
// Applies one change to an open database in place. The caller flushes
// and re-seals. before is set to the record before the change (for
// undo). Returns an EPDB_REC_* code.
int EPDBApplyRecordOp(POD *pod, const MemoryBuffer &key, const EPDBRecordOp &op, EPDBRecord *before, gxString &err);
// Applies a batch atomically: the database is copied to temporary
// files, every change is made there, the copy is re-sealed and checked
// and then renamed over the original. If any change fails nothing is
// changed. With dry_run the copy is removed instead. Returns an
// EPDB_REC_* code and the number of changes made in num_done.
int EPDBRecordBatch(const char *data_file, const MemoryBuffer &key, gxList<EPDBRecordOp> &ops, int dry_run,
		    unsigned &num_done, gxString &err);

// JSON
gxString EPDBRecordJSON(const EPDBRecord &rec);
// Turns JSON input (one object or an array) into records. Returns 0 if successful.
int EPDBParseJSONRecords(const MemoryBuffer &data, gxList<EPDBRecord> &recs, gxString &err);
// name=value input into one record. Returns 0 if successful.
int EPDBNameValuesToRecord(gxList<FCNameValue> &list, gxDatabaseConfig *cfg, EPDBRecord &rec, gxString &err);

// CSV in the format of the GUI's Export To CSV File: a line of column
// names, an empty line, then one line per record
gxString EPDBCSVHeader(gxDatabaseConfig &cfg, const MemoryBuffer &key);
gxString EPDBCSVRow(const EPDBRecord &rec);
// Parses that format (also without the header). Returns 0 if successful.
int EPDBParseCSV(const MemoryBuffer &data, gxList<EPDBRecord> &recs, gxString &err);

// Database info, read without a key except the record count
struct EPDBInfo {
  EPDBInfo() { format = 0; records = -1; rsa_users = smartcard_users = 0; flags = key_generation = 0; rekey_time = 0;
    sd_on = 0; sd_limit = sd_failed = 0; destroyed = 0; }
  int format;           // 1 or 2
  long records;         // -1 if not counted
  gxString id;          // Database ID (v2)
  unsigned rsa_users, smartcard_users;
  unsigned flags, key_generation;
  unsigned long long rekey_time;
  int sd_on;
  unsigned sd_limit, sd_failed;
  int destroyed;
  gxList<gxString> users; // "name (RSA key)" or "name (Smart Card)"
};
int EPDBGetInfo(const char *data_file, POD *pod, EPDBInfo &info, gxString &err);
gxString EPDBInfoJSON(const char *data_file, const EPDBInfo &info);

// Merge: every record of the source is added to the destination. A
// record that exists in both is kept (skip), replaced by the source
// (replace) or makes the merge fail (fail). Applied as one atomic batch.
const int EPDB_MERGE_SKIP = 0;
const int EPDB_MERGE_REPLACE = 1;
const int EPDB_MERGE_FAIL = 2;
int EPDBMergeDatabases(const char *dst_file, const MemoryBuffer &dst_key, gxList<EPDBRecord> &src_records,
		       int policy, int dry_run, unsigned &added, unsigned &replaced, unsigned &skipped, gxString &err);

// Password generator: length 8 to 256, from upper and lower case
// letters, digits and (with symbols) punctuation, at least one of each
// class, with the OpenSSL random generator. Returns 0 if successful.
int EPDBGeneratePassword(unsigned length, int symbols, gxString &password);

// ----------------------------------------------------------- //
// Access log and self-destruct decisions, shared by the CLI and GUI
// ----------------------------------------------------------- //
struct EPDBLogContext {
  EPDBLogContext() { per_user = 0; warned = 0; }
  int per_user;          // --access-log or the GUI setting
  gxString log_file;     // Empty for the default per-user log
  gxString tool;         // epdb_auth_tool or the GUI name
  gxString auth;         // password, key-file, rsa-key or smartcard
  int warned;            // Set after the first log warning
  gxString warning;      // The first log failure, for the caller to show once
};
// Writes an entry to the per-user log (when on) and, when the database
// has the in-file log on, to the database file. If the in-file log
// cannot be written (read-only file, or no key after a failed open) the
// entry goes to the per-user log even when it is off, marked as a
// fallback. Never fails the caller: problems end up in ctx.warning.
void EPDBLog(EPDBLogContext &ctx, const char *data_file, const char *action, const gxString &slot,
	     const gxString &record, int ok, const char *detail, const MemoryBuffer *key, int force_per_user = 0);
// Per-user log entry only, for actions the in-file log does not keep
void EPDBPerUserLog(EPDBLogContext &ctx, const char *data_file, const char *action, const gxString &slot,
		    const gxString &record, int ok, const char *detail, const MemoryBuffer *key);
// Returns 1 if the database has the in-file log on
int EPDBInFileLogOn(const char *data_file);

// Self-destruct, before any credential is tried. Returns 0 to go on
// (msg may hold the attempts used so far), FC_EXIT_DESTROYED for a
// destroyed database or FC_EXIT_IO for a file that cannot be written
// while self-destruct is on.
int EPDBSelfDestructPrecheck(const char *data_file, gxString &msg);
// Counts a failed open in the file (flushed to disk) and destroys the
// database at the limit. msg is the warning to show; last_try is set
// when one attempt is left; destroyed when it was destroyed now.
// Returns 0 if self-destruct is off or the count was written.
int EPDBSelfDestructFailedOpen(const char *data_file, const char *what, gxString &msg,
			       int &last_try, int &destroyed);
// After a good open: resets the counter and restores the control block
// from the authenticated header copy if it was changed (msg says so).
void EPDBSelfDestructReset(const char *data_file, const MemoryBuffer &key, gxString &msg);

// ----------------------------------------------------------- //
// Undo and redo of record changes, for any GUI. In memory only: the
// steps hold record values, so they are wiped when cleared, never
// written to disk or a log, and the depth is capped.
// ----------------------------------------------------------- //
const unsigned EPDB_UNDO_MAX_DEPTH = 100;

struct EPDBUndoStep {
  // One user action: the ops that undo it (applied in list order) and
  // the ops that redo it
  gxString label;               // "Add My Bank" for menus
  gxList<EPDBRecordOp> undo_ops;
  gxList<EPDBRecordOp> redo_ops;
};

class EPDBUndoStack
{
public:
  EPDBUndoStack() { depth = EPDB_UNDO_MAX_DEPTH; group = 0; }
  ~EPDBUndoStack() { Clear(); }
  // Record a change that was just made. before is the record before
  // (for update and delete), after the record after (for add and update).
  void RecordAdd(const EPDBRecord &after);
  void RecordUpdate(const EPDBRecord &before, const EPDBRecord &after);
  void RecordDelete(const EPDBRecord &before);
  // For GUIs with many write paths: Snapshot() reads a record before a
  // change (returns 1 if it exists), Changed() reads it after the change
  // and records an add, update or delete from the two states.
  static int Snapshot(POD *pod, const MemoryBuffer &key, const gxString &name, EPDBRecord &rec);
  void Changed(POD *pod, const MemoryBuffer &key, int before_exists, const EPDBRecord &before,
	       const gxString &after_name);
  // Several changes undone in one step, such as an import or merge
  void BeginGroup(const char *label);
  void EndGroup();
  int CanUndo() const { return undo.GetHead() != 0; }
  int CanRedo() const { return redo.GetHead() != 0; }
  gxString UndoLabel();
  gxString RedoLabel();
  // Applies the step to an open database. The caller flushes, re-seals
  // and refreshes its view. Returns an EPDB_REC_* code; on failure the
  // step stays where it was.
  int Undo(POD *pod, const MemoryBuffer &key, gxString &err);
  int Redo(POD *pod, const MemoryBuffer &key, gxString &err);
  // Wipes every step (lock, close, exit, key management)
  void Clear();
  unsigned Depth() const;
  void SetDepth(unsigned d) { depth = d ? d : 1; }

private:
  void Push(EPDBUndoStep *step);
  int Apply(POD *pod, const MemoryBuffer &key, gxList<EPDBRecordOp> &ops, gxString &err);
  static void FreeSteps(gxList<EPDBUndoStep *> &l);
  EPDBUndoStep *NewStep(const char *label);
  void Add(EPDBUndoStep *s);
  gxList<EPDBUndoStep *> undo; // Head is the newest
  gxList<EPDBUndoStep *> redo;
  EPDBUndoStep *group;
  unsigned depth;
};

#endif // __EPDB_CORE_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
