// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 10/31/2026
// Date Last Modified: 10/31/2026
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

Crash-safe saves for the Encrypted Password Database, with the
DataReel 6.3 write-ahead journal (gxdjrnl.h).

A database opened read-write by this program is registered with
EPDBJournalRegister(), which opens its journal file "<name>.ehd.jnl"
(kept while the database is open, removed by a clean close). Every
save of a registered database runs between EPDBSaveBegin() and
EPDBSaveEnd(), usually through an EPDBSave object: the data and index
files are attached to the journal, the changes are kept in memory,
and EPDBSaveEnd() writes them all or none:

  1. The changed pages go to the journal, which is synced.
  2. The pages are written to the .ehd and .ehx files, which are
     synced, and the journal is emptied (a checkpoint).

A crash or power loss during 1 leaves the files as they were before
the save. A crash during 2 leaves the save in the journal, and the
next open of the database (EPDBRecoverDatabase()) writes the whole
save again. Between saves the files are not cached and the journal
holds nothing to recover.

Saves nest: only the outermost EPDBSaveEnd() writes. Before it does,
it calls the database's reseal function, if one was registered and
the save changed the data file, so the v2 database MAC is always
written in the same save as the change it covers.

The journal holds encrypted records and index keys only (the records
and keys are encrypted before they reach DataReel), the database
settings and auth area, like the .ehd file. It is created owner-only
(mode 0600 on Linux, a protected DACL for the current user on
Windows), emptied after each save and removed when the database is
closed.

The journal names the files recovery writes, so a journal is only
replayed if no other user could have written it (10/31/2026): on Linux
the journal must belong to this user and neither it nor its directory
may be writable by the group or others, and on every system it may only
name this database's .ehd and .ehx files. Otherwise the database opens
read-only with a warning.

Read-only opens never create a journal. EPDBRecoverDatabase() is
called before any open and needs write access only when there is an
interrupted save or file replacement to finish.

Only one program may change a database at a time (10/31/2026). A
read-write open takes an exclusive, non-blocking writer lock on the
.ehd file (flock() on Linux; on Windows LockFileEx() on a byte far
past byte 0 of "<name>.ehd.lock", the file epdb_auth_tool's
--lock-timeout lock uses, because Windows cannot rename a file over one
that has an open handle), held until the database is closed; the
operating system releases it if the program dies, also after kill -9.
A second read-write open is refused with "The database ... is open in
another program" (the GUI then opens it read-only). Read-only opens
never take the lock. Recovery runs only while holding the lock, so it
never replays the journal of a program that is still writing, and the
file replacements (rekey, remove user, upgrade, epdb_auth_tool record
changes, index rebuild) hold it too. The access log, failed-open
counter and self-destruct writes by file name do not wait for the
lock: they write as before, so holding a database open cannot stop
the failed-open counter.

The files of an interrupted file replacement (rekey, remove user,
format upgrade, epdb_auth_tool record changes, see epdb_replace_db()
in db_auth.cpp) are also finished by EPDBRecoverDatabase(): a marker
file "<temp data file>.swap" exists while the temporary files replace
the originals.
*/
// ----------------------------------------------------------- //
#ifndef __EPDB_JOURNAL_HPP__
#define __EPDB_JOURNAL_HPP__

#include "gxdlcode.h"
#include "gxdbase.h"
#include "gxbtree.h"
#include "pod.h"
#include "gxstring.h"

// Called before a registered database's outermost save is written,
// see EPDBJournalRegister()
typedef void (*EPDBResealFunction)(void *data);

// 10/31/2026: Writer lock. Returns 0 if the lock is held (counted: a
// program that holds it already gets it again), 1 if another program
// holds it ("err" says the database is open in another program), or -1
// on an error. Every successful EPDBLockDatabase() needs an
// EPDBUnlockDatabase().
int EPDBLockDatabase(const char *data_file, gxString &err);
void EPDBUnlockDatabase(const char *data_file);
int EPDBDatabaseLocked(const char *data_file); // Held by this program

class EPDBLockScope
// Holds the writer lock while the object exists
{
public:
  EPDBLockScope(const char *data_file) {
    file = data_file;
    rv = EPDBLockDatabase(data_file, err);
  }
  ~EPDBLockScope() { if(rv == 0) EPDBUnlockDatabase(file.c_str()); }
  int Held() const { return rv == 0; }
  gxString err;

private:
  EPDBLockScope(const EPDBLockScope &) { }
  void operator=(const EPDBLockScope &) { }

private:
  gxString file;
  int rv;
};

// The journal file of a database: "<data_file>.jnl"
gxString EPDBJournalName(const char *data_file);

// Returns 1 if a crash left an unfinished save or file replacement
int EPDBRecoveryPending(const char *data_file);

// Finishes a save or file replacement that a crash interrupted. Call
// it before the database files are opened. Returns 0 if there was
// nothing to do or it was done ("msg" then says what was done, if
// anything), 1 if work is pending but the files cannot be written
// ("msg" says so), or -1 on an error.
int EPDBRecoverDatabase(const char *data_file, gxString &msg);

// Registers a database this program opened read-write, so its saves
// are journaled. It holds the writer lock until it is unregistered
// (take the lock before the open, see EPDBLockDatabase()). "reseal" (optional) is called with "reseal_data"
// before each outermost save is written, when the save changed the
// data file. Unregister before the files are closed: it ends a save
// left open.
void EPDBJournalRegister(POD *pod, const char *data_file,
			 EPDBResealFunction reseal = 0, void *reseal_data = 0);
void EPDBJournalRegister(gxDatabase *f, const char *data_file,
			 EPDBResealFunction reseal = 0, void *reseal_data = 0);
void EPDBJournalUnregister(POD *pod);
void EPDBJournalUnregister(gxDatabase *f);

// The registered, open data file of "data_file", or 0
gxDatabase *EPDBJournalFind(const char *data_file);

// Start and end a save of the registered database that "f" (its data
// or index file) belongs to. They do nothing for a file that is not
// registered. EPDBSaveBegin() returns 0, or -1 if the journal cannot
// be used (the save then writes the files directly, as before).
// EPDBSaveEnd() returns 0, or -1 if the save could not be written.
int EPDBSaveBegin(gxDatabase *f);
int EPDBSaveEnd(gxDatabase *f);

// Number of saves written through the journal, and the last journal
// error, for tests and diagnostics
unsigned long EPDBJournaledSaves();
gxString EPDBJournalLastError();

class EPDBSave
// One save, ended when the object goes out of scope
{
public:
  EPDBSave(gxDatabase *f) { db = f; EPDBSaveBegin(db); }
  EPDBSave(POD *pod) { db = pod ? pod->OpenDataFile() : 0; EPDBSaveBegin(db); }
  ~EPDBSave() { End(); }
  int End() { int rv = 0; if(db) rv = EPDBSaveEnd(db); db = 0; return rv; }

private:
  EPDBSave(const EPDBSave &) { }
  void operator=(const EPDBSave &) { }

private:
  gxDatabase *db;
};

#endif // __EPDB_JOURNAL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
