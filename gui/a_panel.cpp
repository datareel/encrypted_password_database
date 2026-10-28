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

Add panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(AddPanel, wxDialog)
  EVT_BUTTON    (ID_ADD_ACCEPT, AddPanel::OnAccept)
  EVT_BUTTON    (ID_ADD_CANCEL, AddPanel::OnCancel)
  EVT_BUTTON    (ID_ADD_CLOSE, AddPanel::OnClose)
  EVT_BUTTON    (ID_ADD_REVERT, AddPanel::OnRevert)
  EVT_BUTTON    (wxID_OK, AddPanel::OnSave)
  EVT_BUTTON    (wxID_CANCEL, AddPanel::OnClose)
END_EVENT_TABLE()


  // Height and width constants for panel objects
int AddPanelConfig::key_label_height = 60;
int AddPanelConfig::key_label_width = 325;
int AddPanelConfig::button_height = 25;
int AddPanelConfig::button_width = 55;
int AddPanelConfig::text_input_height = 25; 
int AddPanelConfig::text_input_width = 305;
int AddPanelConfig::label_height = 25;
int AddPanelConfig::label_width = -1;
int AddPanelConfig::x_offset = 0;
int AddPanelConfig::y_offset_label = 42;
int AddPanelConfig::y_offset_textbox = 27;
int AddPanelConfig::x_offset_button = 66;
int AddPanelConfig::y_offset_button = 39;

// Panel height and position
int AddPanelConfig::xpos = 50; 
int AddPanelConfig::ypos = 50; 
int AddPanelConfig::width = 430; 

AddPanel::AddPanel(wxWindow *parent, wxWindowID id, char *title,
		     int xpos, int ypos, int width, int height,
		     long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
  
{
  int i;
  // Initialize the text input and label arrays
  for(i = 0; i < MaxPanelMembers; i++) db_input[i] = 0;
  for(i = 0; i < MaxPanelMembers; i++) db_input_labels[i] = 0;
  key_name_label = 0;
  accept_btn = close_btn = cancel_btn = revert_btn = 0;

  // Initialize the array of previous inserts
  for(i = 0; i < NumPanelEntries; i++) infohog_a[i] = 0;
  num_entries = count = 0;
  change_labels = 0;

  child_frame = 0;
}
  
AddPanel::~AddPanel()
{
  int i;
  for(i = 0; i < MaxPanelMembers; i++) {
    if(db_input[i]) delete db_input[i];
  }
  for(i = 0; i < MaxPanelMembers; i++) {
    if(db_input_labels[i]) delete db_input_labels[i];
  }

  if(key_name_label) delete key_name_label;
  if(accept_btn) delete accept_btn;
  if(close_btn) delete close_btn;
  if(cancel_btn) delete cancel_btn;
  if(revert_btn) delete revert_btn;

  for(i = 0; i < NumPanelEntries; i++) {
    if(infohog_a[i]) delete infohog_a[i];
  }
}

void AddPanel::OnClose(wxCommandEvent &WXUNUSED(event))
{
  Show(FALSE);
}

void AddPanel::ClearPanel()
{
  for(int i = 0; i < MaxPanelMembers; i++) {
    if(db_input[i]) db_input[i]->Clear();
  }
}

void AddPanel::OnAccept(wxCommandEvent &WXUNUSED(event))
// 10/27/2026: Save and add another, the dialog stays open
{
  AcceptRecord();
}

void AddPanel::OnSave(wxCommandEvent &WXUNUSED(event))
// 10/27/2026: Save and close
{
  if(AcceptRecord()) Show(FALSE);
}

int AddPanel::AcceptRecord()
// Returns 1 if the record was added
{
  INFOHOG infohog(child_frame->GetPOD(), NumDataMembers);

  wxString key_name(db_input[0]->GetValue());
  
  // Trim the leading and trailing spaces from the key name
  key_name.Trim(); key_name.Trim(FALSE); 

  if(key_name == "") {
    ProgramError->Message("You must enter a valid name for this entry\n");
    return 0;
  }
  
  INFOHOG_t key(key_name.c_str());
  infohog.SetMember(&key, sizeof(key), 0);

  // Check to see if the entry already exists in the database
  int exists = infohog.FindObject();
  if(exists) {
    ProgramError->Message("Item: ", key_name.c_str(), 
			  "\nalready exists in the database");
    return 0;
  }
  
  // Load the object
  for(int i = 0; i < MaxPanelMembers; i++) {
    if(!db_input[i]->GetValue().IsNull()) {
      INFOHOG_t ih_members(db_input[i]->GetValue().c_str());
      infohog.SetMember(&ih_members, sizeof(ih_members), i);    
    }
  }

  GUIUndoMark undo_mark; // 10/27/2026: Undo and the access log
  gxString undo_name = (const char *)key_name.c_str();
  GUIUndoBefore(child_frame, undo_name, undo_mark);
  if(!infohog.WriteObject()) {
    ProgramError->Message("Could not add object to the database\n");
    return 0;
  }
  else {
    child_frame->GetPOD()->Flush(); // Flush the POD database following each insert
    GUIUndoAfter(undo_mark, undo_name, "add-record");
    frame->statusWin->WriteText("Added entry ");
    frame->statusWin->WriteText(key_name.c_str());
    frame->statusWin->WriteText(" to the database\n");
  }

  if(!infohog_a[num_entries]) {
    infohog_a[num_entries] = new INFOHOG(child_frame->GetPOD(), NumDataMembers);
  }
  infohog_a[num_entries]->Copy(infohog);
  num_entries++;
  count = num_entries;
  if(num_entries >= NumPanelEntries) num_entries = count = NumPanelEntries;

  child_frame->GridFrame()->InsertCell(infohog);

  ClearPanel();
  db_input[0]->SetFocus();
  return 1;
}

void AddPanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  ClearPanel();
}

void AddPanel::OnRevert(wxCommandEvent &WXUNUSED(event))
{
  if(count <= 0)
    count = num_entries;
  else
    count--;

  if(infohog_a[count]) LoadPanel(*(infohog_a[count]));
}

void AddPanel::LoadPanel(INFOHOG &ob)
{
  ClearPanel();
  char dest[DBStringLength];
  for(int i = 0; i < MaxPanelMembers; i++) {
    if(ob.GetMemberLen(i) == sizeof(INFOHOG_t)) {
      INFOHOG_t *dbstring = (INFOHOG_t *)ob.GetMember(i);
      if(!dbstring->is_null()) {
	db_input[i]->SetValue(EPDBFromDB(dbstring->c_str(dest)));
      }
    }
  }
}

void AddPanel::ShowPanel()
{
  child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  if(frame->dpanel->PanelIsOpen()) {
    frame->dpanel->ClosePanel();
  }

  // 10/27/2026: The column names of this database
  form.SetLabels(child_frame);
  db_input[0]->SetFocus();
  Fit();
  CentreOnParent();
  ShowModal();

}

AddPanel *InitAddPanel(wxWindow *parent)
// 10/27/2026: The record form (record_form.cpp) with sizers
{
  AddPanel *panel = new AddPanel(parent, ADD_PANEL_ID, (char *)"Add Record", -1, -1, -1, -1,
				 wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, (char *)"Add Record");
  if(!panel) return 0;
  panel->SetIcons(EPDBAppIcons());
  const int pad = panel->FromDIP(12);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *form = new wxBoxSizer(wxVERTICAL);
  panel->form.Build(panel, form, panel->db_input, panel->db_input_labels, 0);
  top->Add(form, 1, wxEXPAND | wxALL, pad);

  wxBoxSizer *b = new wxBoxSizer(wxHORIZONTAL);
  panel->cancel_btn = new wxButton(panel, ID_ADD_CANCEL, "Clear");
  b->Add(panel->cancel_btn, 0);
  b->AddStretchSpacer();
  panel->close_btn = new wxButton(panel, wxID_CANCEL, "Cancel");
  b->Add(panel->close_btn, 0, wxLEFT, panel->FromDIP(6));
  panel->accept_btn = new wxButton(panel, ID_ADD_ACCEPT, "Save and Add Another");
  b->Add(panel->accept_btn, 0, wxLEFT, panel->FromDIP(6));
  wxButton *save = new wxButton(panel, wxID_OK, "Save");
  save->SetDefault();
  b->Add(save, 0, wxLEFT, panel->FromDIP(6));
  top->Add(b, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
  panel->SetEscapeId(wxID_CANCEL);
  panel->SetSizerAndFit(top);
  panel->SetMinSize(panel->GetSize());
  panel->Show(FALSE);
  return panel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
