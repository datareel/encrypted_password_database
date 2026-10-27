// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
// Copyright (c) 2001-2025 DataReel Software Development
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

Edit panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

enum {
  ID_EDITTABPANEL_DEFAULT = 991,
  ID_EDITTABPANEL_DIRECTORY
};

BEGIN_EVENT_TABLE(EditTabPanel, wxDialog)
  EVT_CLOSE(EditTabPanel::OnCloseWindow)
  EVT_BUTTON(wxID_OK, EditTabPanel::OnAccept)
  EVT_BUTTON(wxID_CANCEL, EditTabPanel::OnCancel)
  EVT_BUTTON(ID_EDITTABPANEL_DEFAULT, EditTabPanel::OnDefault)
  EVT_BUTTON(ID_EDITTABPANEL_DIRECTORY, EditTabPanel::OnBrowseDirectory)
END_EVENT_TABLE()

static wxBoxSizer *EPDBPrefRow(wxWindow *p, const wxString &label, wxWindow *ctrl, const wxString &after)
{
  wxBoxSizer *r = new wxBoxSizer(wxHORIZONTAL);
  r->Add(new wxStaticText(p, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, p->FromDIP(8));
  r->Add(ctrl, 0, wxALIGN_CENTER_VERTICAL);
  if(!after.IsEmpty()) r->Add(new wxStaticText(p, wxID_ANY, after), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, p->FromDIP(6));
  return r;
}

EditTabPanel::EditTabPanel(wxWindow *parent)
  : wxDialog(parent, EDIT_TAB_PANEL_ID, "Preferences", wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  SetIcons(EPDBAppIcons());
  const int pad = FromDIP(12);
  const int gap = FromDIP(8);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  notebook = new wxNotebook(this, wxID_ANY);

  // File locations
  wxPanel *p1 = new wxPanel(notebook);
  wxBoxSizer *s1 = new wxBoxSizer(wxVERTICAL);
  s1->Add(new wxStaticText(p1, wxID_ANY, "Default folder for databases"), 0, wxBOTTOM, FromDIP(3));
  wxBoxSizer *dr = new wxBoxSizer(wxHORIZONTAL);
  directory_input = new wxTextCtrl(p1, wxID_ANY, progcfg->docDir.c_str());
  directory_input->SetMinSize(wxSize(FromDIP(320), -1));
  dr->Add(directory_input, 1, wxALIGN_CENTER_VERTICAL);
  dr->Add(new wxButton(p1, ID_EDITTABPANEL_DIRECTORY, "Browse..."), 0, wxLEFT | wxALIGN_CENTER_VERTICAL, FromDIP(6));
  s1->Add(dr, 0, wxEXPAND);
  s1->Add(EPDBNote(p1, "New databases are created here and the Open and Browse dialogs start here.", 380), 0, wxTOP, FromDIP(4));
  wxBoxSizer *b1 = new wxBoxSizer(wxVERTICAL);
  b1->Add(s1, 1, wxEXPAND | wxALL, pad);
  p1->SetSizer(b1);
  notebook->AddPage(p1, "File Locations", true);

  // Security
  wxPanel *p2 = new wxPanel(notebook);
  wxBoxSizer *s2 = new wxBoxSizer(wxVERTICAL);
  static const wxString choices[3] = { "128-bit", "192-bit", "256-bit" };
  mode_radio_box = new wxRadioBox(p2, wxID_ANY, "Encryption strength of new v1 records", wxDefaultPosition, wxDefaultSize,
				  3, choices, 3, wxRA_SPECIFY_COLS);
  s2->Add(mode_radio_box, 0, wxEXPAND);
  clipboard_seconds = new wxSpinCtrl(p2, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 3600, 30);
  s2->Add(EPDBPrefRow(p2, "Clear copied values from the clipboard after", clipboard_seconds, "seconds"), 0, wxTOP, gap * 2);
  lock_minutes = new wxSpinCtrl(p2, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1440, 15);
  s2->Add(EPDBPrefRow(p2, "Lock the open databases after", lock_minutes, "minutes with no activity"), 0, wxTOP, gap);
  s2->Add(EPDBNote(p2, "0 turns clearing or locking off.", 0), 0, wxTOP, FromDIP(3));
  access_log = new wxCheckBox(p2, wxID_ANY, "Keep a per-user access log of opens and changes");
  s2->Add(access_log, 0, wxTOP, gap * 2);
  wxBoxSizer *b2 = new wxBoxSizer(wxVERTICAL);
  b2->Add(s2, 1, wxEXPAND | wxALL, pad);
  p2->SetSizer(b2);
  notebook->AddPage(p2, "Security");

  // Appearance
  wxPanel *p3 = new wxPanel(notebook);
  wxBoxSizer *s3 = new wxBoxSizer(wxVERTICAL);
  show_passwords = new wxCheckBox(p3, wxID_ANY, "Show passwords in the records grid");
  show_details = new wxCheckBox(p3, wxID_ANY, "Show the record details pane");
  show_toolbar = new wxCheckBox(p3, wxID_ANY, "Show the toolbar");
  s3->Add(show_details, 0);
  s3->Add(show_toolbar, 0, wxTOP, gap);
  s3->Add(show_passwords, 0, wxTOP, gap);
  s3->Add(EPDBNote(p3, "Grid colors and fonts are set for each database in View > Grid Appearance. "
		   "The light or dark look follows the system setting.", 380), 0, wxTOP, gap * 2);
  wxBoxSizer *b3 = new wxBoxSizer(wxVERTICAL);
  b3->Add(s3, 1, wxEXPAND | wxALL, pad);
  p3->SetSizer(b3);
  notebook->AddPage(p3, "Appearance");

  top->Add(notebook, 1, wxEXPAND | wxALL, pad);
  wxBoxSizer *bs = new wxBoxSizer(wxHORIZONTAL);
  bs->Add(new wxButton(this, ID_EDITTABPANEL_DEFAULT, "Restore Defaults"), 0);
  bs->AddStretchSpacer();
  wxStdDialogButtonSizer *std = new wxStdDialogButtonSizer();
  wxButton *ok = new wxButton(this, wxID_OK);
  ok->SetDefault();
  std->AddButton(ok);
  std->AddButton(new wxButton(this, wxID_CANCEL));
  std->Realize();
  bs->Add(std, 0);
  top->Add(bs, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
  SetEscapeId(wxID_CANCEL);
  SetSizerAndFit(top);
  SetMinSize(GetSize());
}

EditTabPanel::~EditTabPanel()
{
}

void EditTabPanel::LoadPanel()
{
  directory_input->ChangeValue(progcfg->docDir.c_str());
  int mode = (int)DBStringConfig::mode - 1;
  if(mode < 0 || mode > 2) mode = 2;
  mode_radio_box->SetSelection(mode);
  clipboard_seconds->SetValue(progcfg->clipboard_clear_seconds);
  lock_minutes->SetValue(progcfg->auto_lock_minutes);
  access_log->SetValue(progcfg->access_log != 0);
  show_passwords->SetValue(progcfg->show_passwords != 0);
  show_details->SetValue(progcfg->show_details != 0);
  show_toolbar->SetValue(frame->GetToolBar() != 0);
}

void EditTabPanel::ShowPanel()
{
  LoadPanel();
  CentreOnParent();
  ShowModal();
}

static void EPDBMenuToggle(int id, int on)
// Runs the menu command as if its check item was clicked
{
  wxMenuBar *mb = frame->GetMenuBar();
  if(mb && mb->FindItem(id) && mb->FindItem(id)->IsCheckable()) mb->Check(id, on != 0);
  wxCommandEvent e(wxEVT_MENU, id);
  e.SetInt(on);
  frame->GetEventHandler()->ProcessEvent(e);
}

int EditTabPanel::CommitChanges()
{
  gxConfig CfgData(progcfg->cfgFile.c_str());

  gxString docDir((const char *)directory_input->GetValue().utf8_str());
  int mode = (mode_radio_box->GetSelection() + 1);

  // Write any changes to the config file
  if(docDir != progcfg->docDir) {
    FixHomeDirID(docDir);
    if(!::wxDirExists(docDir.c_str())) {
      if(futils_mkdir(docDir.c_str()) != 0) {
	ProgramError->Message("Cannot make new document directory");
	return 0;
      }
    }
    // Set the new names for the doc directory and database files
    progcfg->docDir = docDir;
    CfgData.ChangeConfigValue("DocumentDirectory", (const char *)docDir.c_str());
  }

  if((char)mode != DBStringConfig::mode) {
    DBStringConfig::mode = mode;
    CfgData.ChangeConfigValue("EncryptionMode", mode);
  }

  // 10/27/2026: Clipboard clearing and auto-lock
  if(clipboard_seconds->GetValue() != progcfg->clipboard_clear_seconds) {
    progcfg->clipboard_clear_seconds = clipboard_seconds->GetValue();
    GUISaveConfigValue("ClipboardClearSeconds", wxString::Format("%d", progcfg->clipboard_clear_seconds).utf8_str());
  }
  if(lock_minutes->GetValue() != progcfg->auto_lock_minutes) {
    progcfg->auto_lock_minutes = lock_minutes->GetValue();
    GUISaveConfigValue("AutoLockMinutes", wxString::Format("%d", progcfg->auto_lock_minutes).utf8_str());
    frame->NoteUserActivity();
    if(frame->idle_timer) {
      if(progcfg->auto_lock_minutes > 0) frame->idle_timer->Start(15000);
      else frame->idle_timer->Stop();
    }
  }
  if((access_log->GetValue() ? 1 : 0) != progcfg->access_log) EPDBMenuToggle(WXAPPFW_SECURITY_ACCESS_LOG, access_log->GetValue());
  if((show_passwords->GetValue() ? 1 : 0) != progcfg->show_passwords) EPDBMenuToggle(WXAPPFW_VIEW_SHOW_PASSWORDS, show_passwords->GetValue());
  if((show_details->GetValue() ? 1 : 0) != progcfg->show_details) EPDBMenuToggle(WXAPPFW_VIEW_DETAILS, show_details->GetValue());
  if(show_toolbar->GetValue() != (frame->GetToolBar() != 0)) EPDBMenuToggle(WXAPPFW_VIEW_TOOLBAR, show_toolbar->GetValue());
  for(gxListNode<CryptDBDocument *> *p = progcfg->child_frame_list.GetHead(); p; p = p->next) {
    if(p->data->details) p->data->details->UpdateClipboardNote();
  }
  return 1;
}

void EditTabPanel::OnAccept(wxCommandEvent &WXUNUSED(event))
{
  if(CommitChanges()) EndModal(wxID_OK);
}

void EditTabPanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  EndModal(wxID_CANCEL);
}

void EditTabPanel::OnCloseWindow(wxCloseEvent &WXUNUSED(event))
// The window X cancels, nothing is saved
{
  EndModal(wxID_CANCEL);
}

void EditTabPanel::OnDefault(wxCommandEvent &WXUNUSED(event))
// The program defaults, saved only with OK
{
  mode_radio_box->SetSelection(2);
  clipboard_seconds->SetValue(30);
  lock_minutes->SetValue(15);
  access_log->SetValue(false);
  show_passwords->SetValue(false);
  show_details->SetValue(true);
  show_toolbar->SetValue(true);
}

void EditTabPanel::OnBrowseDirectory(wxCommandEvent &WXUNUSED(event))
{
  wxDirDialog dialog(this, "Folder for database files", directory_input->GetValue());
  if(dialog.ShowModal() == wxID_OK) directory_input->SetValue(dialog.GetPath());
}

EditTabPanel *InitEditTabPanel(wxWindow *parent)
{
  return new EditTabPanel(parent);
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
