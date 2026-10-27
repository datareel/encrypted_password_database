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


GUI glue for the access logs, self-destruct, the v1 warning and
undo/redo, see epdb_gui_hooks.h
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

GUIAuthInfo gui_last_auth;
static EPDBLogContext gui_log_ctx;
static gxList<gxString> gui_v1_warned;

static void GUIShowLogWarning()
{
  if(gui_log_ctx.warning.is_null()) return;
  gxString m;
  m << clear << "Access log: " << gui_log_ctx.warning << "\n\nThe database is used without the access log.";
  gui_log_ctx.warning.Clear();
  if(frame) *(frame->statusWin) << "WARNING: " << m.c_str() << "\n";
  wxMessageBox(m.c_str(), "Access Log", wxOK | wxICON_WARNING);
}

void GUILog(const char *data_file, const char *action, const gxString &record, int ok,
	    const char *detail, const MemoryBuffer *key)
{
  gui_log_ctx.per_user = progcfg->access_log;
  gui_log_ctx.log_file = progcfg->access_log_file;
  gui_log_ctx.tool = "encrypted_password_database";
  gui_log_ctx.auth = gui_last_auth.auth.is_null() ? gxString("password") : gui_last_auth.auth;
  EPDBLog(gui_log_ctx, data_file, action, gui_last_auth.slot, record, ok, detail, key);
  GUIShowLogWarning();
}

void GUILog(CryptDBDocument *doc, const char *action, const gxString &record, int ok, const char *detail)
{
  if(!doc) return;
  GUILog(doc->DBParms()->data_file.c_str(), action, record, ok, detail, &doc->DBParms()->crypt_key);
}

int GUISelfDestructPrecheck(const char *data_file)
{
  gxString msg;
  int rv = EPDBSelfDestructPrecheck(data_file, msg);
  if(rv != 0) {
    wxMessageBox(msg.c_str(), rv == FC_EXIT_DESTROYED ? "Database Destroyed" : "Self-Destruct",
		 wxOK | wxICON_ERROR);
    GUILog(data_file, "open", gxString(), 0, "refused before trying a credential", 0);
    return 0;
  }
  if(!msg.is_null() && frame) *(frame->statusWin) << "WARNING: " << msg.c_str() << "\n";
  return 1;
}

void GUIFailedOpen(const char *data_file, const char *what)
{
  GUILog(data_file, "open", gxString(), 0, what, 0);
  gxString msg;
  int last_try = 0, destroyed = 0;
  EPDBSelfDestructFailedOpen(data_file, what, msg, last_try, destroyed);
  if(msg.is_null()) return;
  if(frame) *(frame->statusWin) << (destroyed ? "ERROR: " : "WARNING: ") << msg.c_str() << "\n";
  wxMessageBox(msg.c_str(), destroyed ? "Database Destroyed" : (last_try ? "ONE ATTEMPT LEFT" : "Self-Destruct"),
	       wxOK | (destroyed || last_try ? wxICON_ERROR : wxICON_WARNING));
  if(destroyed) {
    gui_log_ctx.tool = "encrypted_password_database";
    EPDBPerUserLog(gui_log_ctx, data_file, "self-destruct", gxString(), gxString(), 1,
		   "destroyed after too many failed opens", 0);
  }
}

void GUIGoodOpen(const char *data_file, const MemoryBuffer &key)
{
  gxString msg;
  EPDBSelfDestructReset(data_file, key, msg);
  if(!msg.is_null()) wxMessageBox(msg.c_str(), "Self-Destruct", wxOK | wxICON_WARNING);
  GUILog(data_file, "open", gxString(), 1, 0, &key);
  GUIWarnV1Once(data_file);
}

void GUIWarnV1Once(const char *data_file)
{
  gxString f = FC_abs_path(data_file);
  for(gxListNode<gxString> *p = gui_v1_warned.GetHead(); p; p = p->next) if(p->data == f) return;
  FCVersionInfo vi;
  gxString err;
  if(FC_check_version(data_file, vi, err) != 0 || !vi.is_v1) return;
  gui_v1_warned.Add(f);
  gxString m;
  m << data_file << " uses " << vi.version << ", which has a weaker key derivation and no "
    << "integrity check over the whole file.\n\nUpgrade it with File > Upgrade Database Format, or with:\n"
    << vi.upgrade << "\n\nThe v1 files are kept as a backup.";
  if(frame) *(frame->statusWin) << "WARNING: " << data_file << " is a v1 database, upgrade it with File > Upgrade Database Format" << "\n";
  wxMessageBox(m.c_str(), "Old Database Format", wxOK | wxICON_WARNING);
}

EPDBUndoStack *GUIUndoStack(CryptDBDocument *doc)
{
  return doc ? &doc->undo_stack : 0;
}

void GUIUndoClear(CryptDBDocument *doc)
{
  if(doc) doc->undo_stack.Clear();
}

int GUIConfirmClearUndo(CryptDBDocument *doc, const char *action)
{
  if(!doc || (!doc->undo_stack.CanUndo() && !doc->undo_stack.CanRedo())) return 1;
  gxString m;
  m << action << " cannot be undone, and it clears the undo history of this database.\n\nContinue?";
  if(wxMessageBox(m.c_str(), "Undo History", wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES) return 0;
  doc->undo_stack.Clear();
  return 1;
}

void GUIUndoBefore(CryptDBDocument *doc, const gxString &name, GUIUndoMark &mark)
{
  mark.doc = doc;
  mark.exists = 0;
  if(!doc || !doc->GetPOD()) return;
  mark.exists = EPDBUndoStack::Snapshot(doc->GetPOD(), doc->DBParms()->crypt_key, name, mark.before);
}

void GUIUndoAfter(GUIUndoMark &mark, const gxString &after_name, const char *action)
{
  CryptDBDocument *doc = mark.doc;
  if(!doc || !doc->GetPOD()) return;
  doc->undo_stack.Changed(doc->GetPOD(), doc->DBParms()->crypt_key, mark.exists, mark.before, after_name);
  mark.before.Wipe();
  if(action) GUILog(doc, action, after_name, 1);
}

gxString GUIObjectName(INFOHOG &ih)
{
  char dest[DBStringLength];
  if(ih.GetMemberLen(0) != sizeof(INFOHOG_t)) return gxString();
  gxString n = ((INFOHOG_t *)ih.GetMember(0))->c_str(dest);
  OPENSSL_cleanse(dest, sizeof(dest));
  return n;
}

GUIUndoGroupGuard::GUIUndoGroupGuard(CryptDBDocument *doc, const char *label)
{
  d = doc;
  if(d) d->undo_stack.BeginGroup(label);
}

GUIUndoGroupGuard::~GUIUndoGroupGuard()
{
  if(d) d->undo_stack.EndGroup();
}

void GUIUndo(CryptDBDocument *doc, int redo)
{
  if(!doc || !doc->GetPOD()) return;
  EPDBUndoStack &u = doc->undo_stack;
  gxString label = redo ? u.RedoLabel() : u.UndoLabel();
  if(label.is_null()) return;
  gxString err;
  int rv = redo ? u.Redo(doc->GetPOD(), doc->DBParms()->crypt_key, err) :
    u.Undo(doc->GetPOD(), doc->DBParms()->crypt_key, err);
  doc->GetPOD()->Flush();
  ResealDocument(doc);
  gxString detail;
  detail << label;
  GUILog(doc, redo ? "redo" : "undo", gxString(), rv == EPDB_REC_OK, detail.c_str());
  if(rv != EPDB_REC_OK) {
    gxString m;
    m << clear << (redo ? "Redo" : "Undo") << " of \"" << label << "\" failed: " << err;
    ProgramError->Message(m.c_str());
  }
  else if(frame) {
    *(frame->statusWin) << (redo ? "Redo: " : "Undo: ") << label.c_str() << "\n";
  }
  if(doc->GridFrame()) doc->GridFrame()->ReloadGrid();
}

void GUISaveConfigValue(const char *name, const char *value)
{
  gxConfig cfg(progcfg->cfgFile.c_str());
  if(cfg.GetStrValue(name)) cfg.ChangeConfigValue(name, value);
  else cfg.WriteConfigLine(name, value);
}
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
