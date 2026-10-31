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

Crash-safe saves, see epdb_journal.h
*/
// ----------------------------------------------------------- //
#include "gxdlcode.h"

#include <stdio.h>
#include <string.h>
#include <vector>
#include "epdb_journal.h"
#include "gxdjrnl.h"
#include "futils.h"
#include "m_globals.h"
#include "fckeymgmt.h"

// Page cache for a save. A save pins the pages it changes, the rest
// is a read cache for the save only.
static const size_t epdb_journal_cache_bytes = 4 * 1024 * 1024;

// Temporary files of the file replacements, see epdb_replace_db()
static const char *epdb_swap_tags[] = {
  ".change", ".v2-upgrade", ".remove-user", ".rekey", 0
};

struct EPDBJournalEntry {
  gxDatabase *data;
  POD *pod; // 0 for a data file opened without its index
  gxBtree *index; // The index attached to the open save
  gxString path; // Absolute path of the data file
  gxString jname;
  EPDBResealFunction reseal;
  void *reseal_data;
  gxdJournal *j;
  int depth;
  unsigned long writes; // Data file writes when the save began
};

static std::vector<EPDBJournalEntry> epdb_journals;
static unsigned long epdb_journaled_saves = 0;
static gxString epdb_journal_error;

gxString EPDBJournalName(const char *data_file)
{
  gxString jname(data_file);
  jname << ".jnl";
  return jname;
}

static gxString epdb_swap_name(const char *data_file, const char *tag, const char *ext)
// "<name><tag><ext>", for example homebill.change.ehd
{
  gxString e(tag);
  e << ext;
  return ReplaceFileExtension(gxString(data_file), e.c_str());
}

static int epdb_journal_has_data(const char *jname)
// A journal holds transactions only while its header has the journal
// magic number (gxdjrnl.cpp). A checkpoint clears it: a journal file
// without it is empty, or is the open journal of a program that has
// the database open between saves, and must be left alone.
{
  FILE *fp = fopen(jname, "rb");
  if(!fp) return 0;
  char magic[8];
  int n = (int)fread(magic, 1, sizeof(magic), fp);
  fclose(fp);
  return n == 8 && memcmp(magic, "GXDWAL01", 8) == 0;
}

int EPDBRecoveryPending(const char *data_file)
{
  gxString jname = EPDBJournalName(data_file);
  if(epdb_journal_has_data(jname.c_str())) return 1;
  for(int i = 0; epdb_swap_tags[i]; i++) {
    gxString marker = epdb_swap_name(data_file, epdb_swap_tags[i], ".ehd");
    marker << ".swap";
    if(futils_exists(marker.c_str())) return 1;
  }
  return 0;
}

static int epdb_finish_swaps(const char *data_file, gxString &msg)
// Finishes a file replacement that was interrupted after its marker
// file was written: the temporary files are complete and synced, the
// index file may or may not have been replaced
{
  gxString dfile(data_file);
  gxString ifile = ReplaceFileExtension(dfile, ".ehx");
  int rv = 0;
  for(int i = 0; epdb_swap_tags[i]; i++) {
    gxString tag(epdb_swap_tags[i]);
    gxString tdfile = epdb_swap_name(data_file, epdb_swap_tags[i], ".ehd");
    gxString tifile = epdb_swap_name(data_file, epdb_swap_tags[i], ".ehx");
    gxString marker = tdfile;
    marker << ".swap";
    if(!futils_exists(marker.c_str())) continue;
    if(!FC_file_writable(data_file) || !FC_file_writable(ifile.c_str())) {
      msg << clear << data_file << ": an interrupted change (" << (tag.c_str() + 1)
	  << ") is waiting to be finished, open the database with write access";
      return 1;
    }
    gxString err;
    if(futils_exists(tifile.c_str()) && FC_replace_file(tifile.c_str(), ifile.c_str(), err) != 0) {
      msg << clear << err;
      return -1;
    }
    if(futils_exists(tdfile.c_str()) && FC_replace_file(tdfile.c_str(), dfile.c_str(), err) != 0) {
      msg << clear << err;
      return -1;
    }
    futils_remove(marker.c_str());
    if(!msg.is_null()) msg << "; ";
    msg << "finished an interrupted change (" << (tag.c_str() + 1) << ") of " << data_file;
  }
  return rv;
}

int EPDBRecoverDatabase(const char *data_file, gxString &msg)
{
  msg.Clear();
  if(!data_file) return -1;
  int rv = epdb_finish_swaps(data_file, msg);
  if(rv != 0) return rv;
  gxString jname = EPDBJournalName(data_file);
  if(!epdb_journal_has_data(jname.c_str())) return 0;
  gxString ifile = ReplaceFileExtension(gxString(data_file), ".ehx");
  if(!FC_file_writable(data_file) || !FC_file_writable(ifile.c_str()) ||
     !FC_file_writable(jname.c_str())) {
    if(!msg.is_null()) msg << "; ";
    msg << data_file << ": a save was interrupted and is waiting to be finished, "
	<< "open the database with write access";
    return 1;
  }
  unsigned long recovered = 0;
  gxDatabaseError err = gxdJournal::Recover(jname.c_str(), &recovered);
  if(err != gxDBASE_NO_ERROR) {
    if(!msg.is_null()) msg << "; ";
    msg << "Cannot finish the interrupted save in " << jname << ": "
	<< gxDatabaseExceptionMessage(err);
    return -1;
  }
  futils_remove(jname.c_str());
  if(recovered) {
    if(!msg.is_null()) msg << "; ";
    msg << "finished a save of " << data_file << " that was interrupted";
  }
  return 0;
}

static gxBtree *epdb_journal_index(EPDBJournalEntry &e)
// The open index of the database now: an index rebuild closes and
// opens it again
{
  if(!e.pod || !e.pod->UsingIndex()) return 0;
  gxBtree *btx = e.pod->Index();
  if(!btx || !btx->gxDatabasePtr() || !btx->gxDatabasePtr()->IsOpen()) return 0;
  return btx;
}

static EPDBJournalEntry *epdb_journal_find(gxDatabase *f)
{
  if(!f) return 0;
  for(size_t i = 0; i < epdb_journals.size(); i++) {
    EPDBJournalEntry &e = epdb_journals[i];
    if(e.data == f) return &e;
    gxBtree *btx = epdb_journal_index(e);
    if(btx && btx->gxDatabasePtr() == f) return &e;
  }
  return 0;
}

static void epdb_journal_register(gxDatabase *f, POD *pod, const char *data_file,
				  EPDBResealFunction reseal, void *reseal_data)
{
  if(!f || !data_file) return;
  EPDBJournalEntry *old = epdb_journal_find(f);
  if(old) EPDBJournalUnregister(f);
  EPDBJournalEntry e;
  e.data = f;
  e.pod = pod;
  e.index = 0;
  e.path = FC_abs_path(data_file);
  e.jname = EPDBJournalName(data_file);
  e.reseal = reseal;
  e.reseal_data = reseal_data;
  e.j = 0;
  e.depth = 0;
  e.writes = 0;
  epdb_journals.push_back(e);
}

static int epdb_journal_open(EPDBJournalEntry &e)
// The journal file is opened once for the time the database is open
// and kept between saves, so its space is reserved once. A clean
// close removes it.
{
  if(e.j) return 0;
  gxdJournal *j = new gxdJournal;
  gxDatabaseError err = j->Open(e.jname.c_str());
  if(err != gxDBASE_NO_ERROR) {
    epdb_journal_error << clear << "Cannot open the journal " << e.jname << ": "
		       << gxDatabaseExceptionMessage(err) << ", the save is not crash-safe";
    delete j;
    return -1;
  }
  // One save is far smaller than this: the journal is emptied by the
  // checkpoint at the end of each save
  j->SetCheckpointSize(256 * 1024 * 1024);
  e.j = j;
  return 0;
}

void EPDBJournalRegister(POD *pod, const char *data_file, EPDBResealFunction reseal, void *reseal_data)
{
  if(!pod) return;
  epdb_journal_register(pod->OpenDataFile(), pod, data_file, reseal, reseal_data);
}

void EPDBJournalRegister(gxDatabase *f, const char *data_file, EPDBResealFunction reseal, void *reseal_data)
{
  epdb_journal_register(f, 0, data_file, reseal, reseal_data);
}

static int epdb_journal_detach(EPDBJournalEntry &e)
// Ends a save: checkpoint (the files are synced and the journal
// emptied), then the page caches are turned off. Returns 0 if
// successful.
{
  int rv = 0;
  if(e.j) {
    if(e.j->InTransaction()) e.j->Rollback();
    if(e.j->Detach() != gxDBASE_NO_ERROR) {
      epdb_journal_error << clear << "Cannot write the save to " << e.path << ": "
			 << gxDatabaseExceptionMessage(e.j->GetJournalError());
      rv = -1;
    }
  }
  if(e.data) e.data->DisablePageCache();
  if(e.index && e.index->gxDatabasePtr()) e.index->gxDatabasePtr()->DisablePageCache();
  e.index = 0;
  return rv;
}

static void epdb_journal_close(EPDBJournalEntry &e)
// Closes the journal, which removes the file
{
  epdb_journal_detach(e);
  if(e.j) {
    e.j->Close();
    delete e.j;
    e.j = 0;
  }
}

void EPDBJournalUnregister(gxDatabase *f)
{
  for(size_t i = 0; i < epdb_journals.size(); i++) {
    EPDBJournalEntry &e = epdb_journals[i];
    if(e.data != f) continue;
    if(e.depth > 0) { // A save left open: write it
      e.depth = 1;
      EPDBSaveEnd(f);
    }
    epdb_journal_close(e);
    epdb_journals.erase(epdb_journals.begin() + i);
    return;
  }
}

void EPDBJournalUnregister(POD *pod)
{
  if(pod) EPDBJournalUnregister(pod->OpenDataFile());
}

gxDatabase *EPDBJournalFind(const char *data_file)
{
  if(!data_file || epdb_journals.empty()) return 0;
  gxString path = FC_abs_path(data_file);
  for(size_t i = 0; i < epdb_journals.size(); i++) {
    if(epdb_journals[i].path == path && epdb_journals[i].data->IsOpen()) return epdb_journals[i].data;
  }
  return 0;
}

int EPDBSaveBegin(gxDatabase *f)
{
  EPDBJournalEntry *e = epdb_journal_find(f);
  if(!e) return 0;
  if(e->depth++ > 0) return 0;
  epdb_journal_error.Clear();
  e->writes = e->data->WriteCount();
  e->index = epdb_journal_index(*e);
  if(epdb_journal_open(*e) != 0) return -1; // The save writes the files directly
  gxdJournal *j = e->j;
  gxDatabaseError err = j->Attach(e->data, epdb_journal_cache_bytes);
  if(err == gxDBASE_NO_ERROR && e->index) err = j->Attach(e->index, epdb_journal_cache_bytes);
  if(err == gxDBASE_NO_ERROR) err = j->Begin();
  if(err != gxDBASE_NO_ERROR) {
    epdb_journal_error << clear << "Cannot use the journal " << e->jname << ": "
		       << gxDatabaseExceptionMessage(err) << ", the save is not crash-safe";
    epdb_journal_detach(*e); // The save writes the files directly
    return -1;
  }
  return 0;
}

int EPDBSaveEnd(gxDatabase *f)
{
  EPDBJournalEntry *e = epdb_journal_find(f);
  if(!e || e->depth == 0) return 0;
  if(e->depth == 1 && e->reseal && e->data->WriteCount() != e->writes) {
    // The v2 MAC is written in the same save as the change it covers.
    // The reseal is a nested save of this database.
    (*e->reseal)(e->reseal_data);
    e = epdb_journal_find(f);
    if(!e) return -1;
  }
  if(--e->depth > 0) return 0;
  if(!e->j || !e->j->InTransaction()) return 0; // The save wrote the files directly
  int rv = 0;
  // Commit(1): the journal is synced before the files are written.
  // Detach(): the files are synced and the journal emptied (checkpoint).
  gxDatabaseError err = e->j->Commit(1);
  if(err != gxDBASE_NO_ERROR) {
    epdb_journal_error << clear << "Cannot save to " << e->path << ": " << gxDatabaseExceptionMessage(err);
    rv = -1;
  }
  if(epdb_journal_detach(*e) != 0) rv = -1;
  if(rv == 0) epdb_journaled_saves++;
  return rv;
}

unsigned long EPDBJournaledSaves()
{
  return epdb_journaled_saves;
}

gxString EPDBJournalLastError()
{
  return epdb_journal_error;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
