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
modify it under the terms of the GNU Lesser General Public
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

10/27/2026: Record details pane, see details_pane.h
*/
// ----------------------------------------------------------- //
#include "app_defs.h"

enum {
  ID_DETAILS_COPY_USER = 6301,
  ID_DETAILS_COPY_PASSWORD,
  ID_DETAILS_OPEN_LINK,
  ID_DETAILS_EDIT,
  ID_DETAILS_COMMAND
};

const int EPDB_FIELD_TITLE = 0;
const int EPDB_FIELD_USERNAME = 1;
const int EPDB_FIELD_PASSWORD = 2;
const int EPDB_FIELD_PREVIOUS_PASSWORD = 3;
const int EPDB_FIELD_LINK = 6;

wxTextCtrl *DetailsPane::ReadOnlyText(int multiline)
{
  long style = wxTE_READONLY | (multiline ? (wxTE_MULTILINE | wxTE_NO_VSCROLL) : 0);
  wxTextCtrl *t = new wxTextCtrl(content, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, style);
  if(multiline) t->SetMinSize(wxSize(-1, t->GetCharHeight() * 3 + FromDIP(8)));
  return t;
}

DetailsPane::DetailsPane(wxWindow *parent, CryptDBDocument *d)
  : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxBORDER_NONE)
{
  doc = d;
  row = -1;
  SetScrollRate(0, FromDIP(10));
  for(int i = 0; i < NumDataMembers; i++) {
    labels[i] = 0;
    values[i] = 0;
  }

  const int pad = FromDIP(12);
  wxBoxSizer *outer = new wxBoxSizer(wxVERTICAL);
  empty_note = EPDBNote(this, "Select a record to see its details.");
  outer->Add(empty_note, 0, wxALL, pad);

  content = new wxPanel(this);
  body = new wxBoxSizer(wxVERTICAL);
  title = EPDBHeading(content, "");
  body->Add(title, 0, wxEXPAND | wxBOTTOM, FromDIP(8));

  for(int i = 1; i < NumDataMembers; i++) {
    labels[i] = new wxStaticText(content, wxID_ANY, "");
    labels[i]->SetForegroundColour(EPDBMutedTextColour());
    body->Add(labels[i], 0, wxTOP, FromDIP(6));
    wxBoxSizer *r = new wxBoxSizer(wxHORIZONTAL);
    if(i == EPDB_FIELD_PASSWORD || i == EPDB_FIELD_PREVIOUS_PASSWORD) {
      PasswordEntry *p = new PasswordEntry(content, wxID_ANY);
      p->SetEditable(false);
      if(i == EPDB_FIELD_PASSWORD) password = p;
      else previous_password = p;
      r->Add(p, 1, wxALIGN_CENTER_VERTICAL);
    }
    else {
      int multi = (i == 4 || i == 5 || i == 7); // Security questions, account information, notes
      values[i] = ReadOnlyText(multi);
      r->Add(values[i], 1, wxALIGN_CENTER_VERTICAL);
    }
    if(i == EPDB_FIELD_USERNAME || i == EPDB_FIELD_PASSWORD || i == EPDB_FIELD_LINK) {
      int id = (i == EPDB_FIELD_USERNAME) ? ID_DETAILS_COPY_USER : (i == EPDB_FIELD_PASSWORD ? ID_DETAILS_COPY_PASSWORD : ID_DETAILS_OPEN_LINK);
      wxBitmapButton *b = new wxBitmapButton(content, id, EPDBIcon(i == EPDB_FIELD_LINK ? "open_link" : "copy", EPDB_MENU_ICON_DIP));
      b->SetToolTip(i == EPDB_FIELD_LINK ? "Open the link in the web browser" : "Copy to the clipboard");
      r->Add(b, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, FromDIP(4));
    }
    body->Add(r, 0, wxEXPAND | wxTOP, FromDIP(2));
  }

  clip_note = EPDBNote(content, "");
  body->Add(clip_note, 0, wxEXPAND | wxTOP, FromDIP(10));

  wxBoxSizer *btns = new wxBoxSizer(wxHORIZONTAL);
  wxButton *edit = new wxButton(content, ID_DETAILS_EDIT, "Edit...");
  edit->SetBitmap(EPDBIcon("edit", EPDB_MENU_ICON_DIP));
  wxButton *cmd = new wxButton(content, ID_DETAILS_COMMAND, "Copy as command...");
  cmd->SetBitmap(EPDBIcon("terminal", EPDB_MENU_ICON_DIP));
  cmd->SetToolTip("Build the epdb_auth_tool command that reads this record");
  btns->Add(edit, 0);
  btns->Add(cmd, 0, wxLEFT, FromDIP(6));
  body->Add(btns, 0, wxTOP, FromDIP(12));
  content->SetSizer(body);
  outer->Add(content, 1, wxEXPAND | wxALL, pad);
  content->Hide();
  SetSizer(outer);

  Bind(wxEVT_BUTTON, &DetailsPane::OnCopyUser, this, ID_DETAILS_COPY_USER);
  Bind(wxEVT_BUTTON, &DetailsPane::OnCopyPassword, this, ID_DETAILS_COPY_PASSWORD);
  Bind(wxEVT_BUTTON, &DetailsPane::OnOpenLink, this, ID_DETAILS_OPEN_LINK);
  Bind(wxEVT_BUTTON, &DetailsPane::OnEdit, this, ID_DETAILS_EDIT);
  Bind(wxEVT_BUTTON, &DetailsPane::OnCommand, this, ID_DETAILS_COMMAND);
  UpdateLabels();
  UpdateClipboardNote();
}

void DetailsPane::UpdateLabels()
{
  char dest[DBStringLength];
  for(int i = 1; i < NumDataMembers; i++) {
    gxString name = (doc && doc->GridFrame() && doc->DBParms()->pod) ? gxString(doc->DBParms()->db_config.GetColName(i, dest)) :
      progcfg->global_dbparms.dbgrid_labels[i];
    if(name.is_null()) name = progcfg->global_dbparms.dbgrid_labels[i];
    labels[i]->SetLabel(wxString::FromUTF8(name.c_str()));
  }
}

void DetailsPane::UpdateClipboardNote()
{
  if(progcfg->clipboard_clear_seconds > 0) {
    clip_note->SetLabel(wxString::Format("Copied values are cleared from the clipboard after %d seconds.",
					 progcfg->clipboard_clear_seconds));
  }
  else {
    clip_note->SetLabel("Copied values stay on the clipboard (set a clear time in Preferences).");
  }
  clip_note->Wrap(FromDIP(300));
}

void DetailsPane::ShowRow(int r)
{
  row = -1;
  wxGrid *g = (doc && doc->GridFrame()) ? doc->GetGrid() : 0;
  if(g && r >= 0 && r < g->GetNumberRows() && !g->GetCellValue(r, 0).IsEmpty()) row = r;
  password->Reveal(0);
  previous_password->Reveal(0);
  if(row < 0) {
    title->SetLabel("");
    for(int i = 1; i < NumDataMembers; i++) if(values[i]) values[i]->ChangeValue("");
    password->Clear();
    previous_password->Clear();
    content->Hide();
    empty_note->Show();
  }
  else {
    title->SetLabel(g->GetCellValue(row, EPDB_FIELD_TITLE));
    for(int i = 1; i < NumDataMembers; i++) {
      if(values[i]) values[i]->ChangeValue(g->GetCellValue(row, i));
    }
    password->SetValue(g->GetCellValue(row, EPDB_FIELD_PASSWORD));
    previous_password->SetValue(g->GetCellValue(row, EPDB_FIELD_PREVIOUS_PASSWORD));
    empty_note->Hide();
    content->Show();
  }
  UpdateClipboardNote();
  Layout();
  FitInside();
}

void DetailsPane::OnCopyUser(wxCommandEvent &WXUNUSED(event))
{
  if(row >= 0) frame->CopyToClipboard(values[EPDB_FIELD_USERNAME]->GetValue());
}

void DetailsPane::OnCopyPassword(wxCommandEvent &WXUNUSED(event))
{
  if(row < 0) return;
  wxGrid *g = doc->GetGrid();
  wxString pw = g->GetCellValue(row, EPDB_FIELD_PASSWORD);
  if(frame->CopyToClipboard(pw)) frame->SetStatusText("Password copied to the clipboard");
}

void DetailsPane::OnOpenLink(wxCommandEvent &WXUNUSED(event))
{
  if(row < 0) return;
  wxString url = values[EPDB_FIELD_LINK]->GetValue();
  url.Trim().Trim(false);
  if(url.IsEmpty()) return;
  gxString u = (const char *)url.utf8_str();
  LaunchURL(u, this);
}

void DetailsPane::OnEdit(wxCommandEvent &WXUNUSED(event))
{
  wxCommandEvent e(wxEVT_MENU, WXAPPFW_DATABASE_CHANGE);
  frame->GetEventHandler()->ProcessEvent(e);
}

void DetailsPane::OnCommand(wxCommandEvent &WXUNUSED(event))
{
  wxCommandEvent e(wxEVT_MENU, WXAPPFW_TOOLS_CMDBUILD);
  e.SetInt(1); // Prefill the selected record
  frame->GetEventHandler()->ProcessEvent(e);
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
