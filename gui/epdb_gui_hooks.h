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


GUI glue for the 10/27/2026 features: access logs, self-destruct, the
v1 warning and undo/redo. The decisions are made in the toolkit
independent core (src/epdb_core.cpp, src/db_auth.cpp and fcrypt's
fckeymgmt.cpp), shared with epdb_auth_tool. This file only shows the
results in dialogs and wires them to the menus.
*/
// ----------------------------------------------------------- //   
#ifndef __EPDB_GUI_HOOKS_HPP__
#define __EPDB_GUI_HOOKS_HPP__

#include "epdb_core.h"
#include "epdb_journal.h" // 10/31/2026: Crash-safe saves

// Auth method and user of the last open, for log entries
struct GUIAuthInfo {
  gxString auth;  // password, key-file, rsa-key or smartcard
  gxString slot;  // RSA or smart card user name
};
extern GUIAuthInfo gui_last_auth;

// Access log entry for a database file. key is the open database key,
// or null after a failed open.
void GUILog(const char *data_file, const char *action, const gxString &record, int ok,
	    const char *detail, const MemoryBuffer *key);
void GUILog(CryptDBDocument *doc, const char *action, const gxString &record, int ok, const char *detail = 0);

// Self-destruct around the Open dialog. Precheck returns 0 when no
// credential may be tried.
int GUISelfDestructPrecheck(const char *data_file);
void GUIFailedOpen(const char *data_file, const char *what);
void GUIGoodOpen(const char *data_file, const MemoryBuffer &key);

// Shows the v1 warning once per file per session
void GUIWarnV1Once(const char *data_file);

// Undo/redo: per document stacks, see EPDBUndoStack
EPDBUndoStack *GUIUndoStack(CryptDBDocument *doc);
void GUIUndoClear(CryptDBDocument *doc);
// Before key management (add or remove users, upgrade, self-destruct
// settings): asks to clear the undo history. Returns 0 if the user said no.
int GUIConfirmClearUndo(CryptDBDocument *doc, const char *action);
// The snapshot and record pair around a record change
// 10/31/2026: A record change between GUIUndoBefore() and GUIUndoAfter()
// is one crash-safe save (epdb_journal.h), written by GUIUndoAfter() or,
// if the change failed and returned early, when the mark goes away
struct GUIUndoMark {
  GUIUndoMark() { exists = 0; doc = 0; save_db = 0; }
  ~GUIUndoMark() { before.Wipe(); if(save_db) EPDBSaveEnd(save_db); }
  CryptDBDocument *doc;
  EPDBRecord before;
  int exists;
  gxDatabase *save_db;
};
void GUIUndoBefore(CryptDBDocument *doc, const gxString &name, GUIUndoMark &mark);
void GUIUndoAfter(GUIUndoMark &mark, const gxString &after_name, const char *action);
// Name (key member) of a record object
gxString GUIObjectName(INFOHOG &ih);
// Ends an undo group when it goes out of scope, for functions with many returns.
// 10/31/2026: The group (an import or a merge) is also one crash-safe save.
struct GUIUndoGroupGuard {
  GUIUndoGroupGuard(CryptDBDocument *doc, const char *label);
  ~GUIUndoGroupGuard();
  CryptDBDocument *d;
  gxDatabase *save_db;
};
// 10/31/2026: Writes the v2 database MAC of a document in each save,
// see EPDBJournalRegister()
void GUIJournalReseal(void *doc);
// 10/31/2026: Ends a save and shows an error if it could not be written
int GUISaveEnd(gxDatabase *f);
// Undo or redo the last change of the active document
void GUIUndo(CryptDBDocument *doc, int redo);
// Saves one setting in the config file (.cfg on Linux, .ini on Windows)
void GUISaveConfigValue(const char *name, const char *value);

#endif // __EPDB_GUI_HOOKS_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
