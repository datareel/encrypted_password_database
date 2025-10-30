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
{
  INFOHOG infohog(child_frame->GetPOD(), NumDataMembers);

  wxString key_name(db_input[0]->GetValue());
  
  // Trim the leading and trailing spaces from the key name
  key_name.Trim(); key_name.Trim(FALSE); 

  if(key_name == "") {
    ProgramError->Message("You must enter a valid name for this entry\n");
    return;
  }
  
  INFOHOG_t key(key_name.c_str());
  infohog.SetMember(&key, sizeof(key), 0);

  // Check to see if the entry already exists in the database
  int exists = infohog.FindObject();
  if(exists) {
    ProgramError->Message("Item: ", key_name.c_str(), 
			  "\nalready exists in the database");
    return;
  }
  
  // Load the object
  for(int i = 0; i < MaxPanelMembers; i++) {
    if(!db_input[i]->GetValue().IsNull()) {
      INFOHOG_t ih_members(db_input[i]->GetValue().c_str());
      infohog.SetMember(&ih_members, sizeof(ih_members), i);    
    }
  }

  if(!infohog.WriteObject()) {
    ProgramError->Message("Could not add object to the database\n");
    return;
  }
  else {
    child_frame->GetPOD()->Flush(); // Flush the POD database following each insert
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
	db_input[i]->SetValue(dbstring->c_str(dest));
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

  int i;
  if(change_labels) {
    delete key_name_label;
    key_name_label = new wxStaticBox(this, -1,
				     progcfg->global_dbparms.dbgrid_labels[0].c_str(),
				     wxPoint(9, 4),
				     wxSize(AddPanelConfig::key_label_width, 
					    AddPanelConfig::key_label_height));
    int start_x = 17;
    int start_y = 28;
    for(i = 1; i < MaxPanelMembers; i++) {
      start_x += AddPanelConfig::x_offset ;
      start_y += AddPanelConfig::y_offset_label;
      delete db_input_labels[i-1];
      db_input_labels[i-1] = new wxStaticText(this, -1,
					      progcfg->global_dbparms.dbgrid_labels[i].c_str(),
					      wxPoint(start_x, start_y), 
					      wxSize(AddPanelConfig::label_width, 
						     AddPanelConfig::label_height));
      start_y += AddPanelConfig::y_offset_textbox;
    }
    change_labels = 0;
  }
  
  char dest[DBStringLength];
  gxString key_label = child_frame->DBParms()->db_config.GetColName(0, dest);  

  if(key_label != progcfg->global_dbparms.dbgrid_labels[0]) {
    delete key_name_label;
    key_name_label = new wxStaticBox(this, -1,
				     key_label.c_str(),
				     wxPoint(9, 4),
				     wxSize(AddPanelConfig::key_label_width, 
					    AddPanelConfig::key_label_height));
    change_labels++;
  }
  
  int start_x = 17;
  int start_y = 28;
  for(i = 1; i < MaxPanelMembers; i++) {
    start_x += AddPanelConfig::x_offset ;
    start_y += AddPanelConfig::y_offset_label;
    gxString label = child_frame->DBParms()->db_config.GetColName(i, dest);  
    if(label != progcfg->global_dbparms.dbgrid_labels[i]) {
      delete db_input_labels[i-1];
      db_input_labels[i-1] = new wxStaticText(this, -1,
					      label.c_str(),
					      wxPoint(start_x, start_y), 
					      wxSize(AddPanelConfig::label_width, 
						     AddPanelConfig::label_height));
      change_labels++;
    }
    start_y += AddPanelConfig::y_offset_textbox;
  }

  ShowModal();

}

AddPanel *InitAddPanel(wxWindow *parent)
{
  // Calculate the height of the panel
  int height = MaxPanelMembers * (AddPanelConfig::text_input_height + AddPanelConfig::label_height);
  height += (AddPanelConfig::key_label_height + AddPanelConfig::button_height);
  height += height/2;
  height += 20;
  AddPanel *panel = new AddPanel(parent, ADD_PANEL_ID,
				 (char *)"Add Database Object",
				 AddPanelConfig::xpos, AddPanelConfig::ypos, 
				 AddPanelConfig::width, height,
				 // TODO: Is this deprciated
				 // wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
				 wxRAISED_BORDER|wxCAPTION|wxSYSTEM_MENU,
				 (char *)"Add Database Object");

  if(!panel) return 0;
  
  panel->key_name_label = new wxStaticBox(panel, -1,
					  progcfg->global_dbparms.dbgrid_labels[0].c_str(),
					  wxPoint(9, 4),
					  wxSize(AddPanelConfig::key_label_width, 
						 AddPanelConfig::key_label_height));

  panel->db_input[0] = new wxTextCtrl(panel, -1, "",
			       wxPoint(17, 28),
			       wxSize(AddPanelConfig::text_input_width, 
				      AddPanelConfig::text_input_height));

  int start_x = 17;
  int start_y = 28;
  for(int i = 1; i < MaxPanelMembers; i++) {
    start_x += AddPanelConfig::x_offset;
    start_y += AddPanelConfig::y_offset_label;
    panel->db_input_labels[i-1] = new wxStaticText(panel, -1,
		   progcfg->global_dbparms.dbgrid_labels[i].c_str(),
					   wxPoint(start_x, start_y), 
					   wxSize(AddPanelConfig::label_width, 
						  AddPanelConfig::label_height));
    start_y += AddPanelConfig::y_offset_textbox;
    panel->db_input[i] = new wxTextCtrl(panel, -1, "",
				wxPoint(start_x, start_y),
				wxSize(AddPanelConfig::text_input_width, 
				       AddPanelConfig::text_input_height));
  }

  start_y += AddPanelConfig::y_offset_button;
  panel->accept_btn = new wxButton(panel, ID_ADD_ACCEPT, "Add",
				   wxPoint(start_x, start_y),
				   wxSize(AddPanelConfig::button_width, 
					  AddPanelConfig::button_height));

  start_x += AddPanelConfig::x_offset_button;
  panel->cancel_btn = new wxButton(panel, ID_ADD_CANCEL, "Clear",
				   wxPoint(start_x, start_y),
				   wxSize(AddPanelConfig::button_width, 
					  AddPanelConfig::button_height));
  start_x += AddPanelConfig::x_offset_button;
  panel->close_btn = new wxButton(panel, ID_ADD_CLOSE, "Close",
				  wxPoint(start_x, start_y),
				  wxSize(AddPanelConfig::button_width, 
					 AddPanelConfig::button_height));

  start_x += AddPanelConfig::x_offset_button;
  panel->revert_btn = new wxButton(panel, ID_ADD_REVERT, "Redo",
				   wxPoint(start_x, start_y),
				   wxSize(AddPanelConfig::button_width, 
					  AddPanelConfig::button_height));

  // TODO: remove
  //  panel->ShowModal(); 

  panel->Show(FALSE);

  return panel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
