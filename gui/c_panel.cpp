// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 1999-2023 DataReel Software Development
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

Change panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(ChangePanel, wxDialog)
  EVT_BUTTON    (ID_CHANGE_ACCEPT, ChangePanel::OnAccept)
  EVT_BUTTON    (ID_CHANGE_CANCEL, ChangePanel::OnCancel)
  EVT_BUTTON    (ID_CHANGE_CLOSE, ChangePanel::OnClose)
  EVT_BUTTON    (ID_CHANGE_REVERT, ChangePanel::OnRevert)
END_EVENT_TABLE()

  // Height and width constants for panel objects
int ChangePanelConfig::key_label_height = 60;
int ChangePanelConfig::key_label_width = 325;
int ChangePanelConfig::button_height = 25;
int ChangePanelConfig::button_width = 60;
int ChangePanelConfig::text_input_height = 25; 
int ChangePanelConfig::text_input_width = 305;
int ChangePanelConfig::label_height = 25;
int ChangePanelConfig::label_width = -1;
int ChangePanelConfig::x_offset = 0;
int ChangePanelConfig::y_offset_label = 42;
int ChangePanelConfig::y_offset_textbox = 27;
int ChangePanelConfig::x_offset_button = 69;
int ChangePanelConfig::y_offset_button = 39;

// Panel height and postion
int ChangePanelConfig::xpos = 50; 
int ChangePanelConfig::ypos = 50; 
int ChangePanelConfig::width = 350; 

ChangePanel::ChangePanel(wxWindow *parent, wxWindowID id, char *title,
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
  
ChangePanel::~ChangePanel()
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

void ChangePanel::OnClose(wxCommandEvent &WXUNUSED(event))
{
  LoadCurrObject();

  int has_changed = 0;
  for(int i = 0; i < MaxPanelMembers; i++) {
    if((curr_object.GetMemberLen(i) == sizeof(INFOHOG_t)) &&
       (prev_object.GetMemberLen(i) == sizeof(INFOHOG_t))) {
      char dest1[DBStringLength];
      char dest2[DBStringLength];
      INFOHOG_t *dbstring1 = (INFOHOG_t *)curr_object.GetMember(i);
      INFOHOG_t *dbstring2 = (INFOHOG_t *)prev_object.GetMember(i);
      if(CaseICmp(dbstring1->c_str(dest1), dbstring2->c_str(dest2)) != 0) {
	has_changed = 1;
	break;
      }
    }
  }
  if(has_changed) {
#ifdef __APP_DEBUG_VERSION__
    char dest[DBStringLength];
    if(curr_object.GetMemberLen(0) == sizeof(INFOHOG_t)) {
      INFOHOG_t *dbstring = (INFOHOG_t *)curr_object.GetMember(0);
      *(frame->statusWin) << "Curr_ob m0 = " << dbstring->c_str(dest)
			  << "\n";
      *(frame->statusWin) << "Curr_ob OID = " << (long)curr_object.GetObjectID()
			  << "\n";
    }
    if(prev_object.GetMemberLen(0) == sizeof(INFOHOG_t)) {
      INFOHOG_t *dbstring = (INFOHOG_t *)prev_object.GetMember(0);
      *(frame->statusWin) << "Prev_ob m0 = " << dbstring->c_str(dest)
			  << "\n";
      *(frame->statusWin) << "Prev_ob OID = " << (long)prev_object.GetObjectID()
			  << "\n";
    }
#endif

    int yn = wxMessageBox("This object has been modifed!\nAccept these \
changes?\n",
			  "Program Message",
			  wxYES_NO | wxCENTRE | wxICON_QUESTION, this);
    if(yn == wxYES) CommitChanges();
  }
  Show(FALSE);
}

void ChangePanel::LoadCurrObject()
// Loads the "curr_object" variable with the current panel values.
{
  for(int i = 0; i < MaxPanelMembers; i++) {
    INFOHOG_t ih_members(db_input[i]->GetValue().c_str());
    curr_object.SetMember(&ih_members, sizeof(ih_members), i);    
  }
}

void ChangePanel::CommitChanges()
// Commits changes to the database. NOTE: This function should only be
// called following a LoadCurrObject() call and assumes the object
// has been modifed.
{
  int change_grid_name = 0;
  char dest[DBStringLength];
  char dest1[DBStringLength];
  char dest2[DBStringLength];

  // NOTE: This is a safty feature. Using member 0 should always be safe
  // provided the call has initialized the infohog object correctly.
  if(curr_object.GetMemberLen(0) != sizeof(INFOHOG_t)) return;
  if(prev_object.GetMemberLen(0) != sizeof(INFOHOG_t)) return;

  INFOHOG_t *dbstring1 = (INFOHOG_t *)curr_object.GetMember(0);
  INFOHOG_t *dbstring2 = (INFOHOG_t *)prev_object.GetMember(0);
  INFOHOG_t curr_name(dbstring1->c_str(dest));
  INFOHOG_t prev_name(dbstring2->c_str(dest));
  if(CaseICmp(dbstring1->c_str(dest1), dbstring2->c_str(dest2)) != 0) {
    change_grid_name = 1;
    wxString name = curr_name.c_str(dest);
    if(child_frame->GridFrame()->FindGridPosition(name) != -1) {
      gxString gbuf;
      gbuf << clear << "Error changing " << prev_name.c_str(dest) << "\n"
	   << "The database " << curr_name.c_str(dest) 
	   << " object already exists" << "\n";
      ProgramError->Message(gbuf.c_str());
      LoadPanel(prev_object);
      return;
    }
  }
  if(!prev_object.ChangeObject(curr_object, 0)) {
    *(frame->statusWin) << "Could not update item: "
			<< db_input[0]->GetValue()
			<< " in the database" << "\n";

    gxString gbuf;
    gbuf << clear << "Error updating " << prev_name.c_str(dest) << "\n"
	 << "Database object was not changed" << "\n";
    ProgramError->Message(gbuf.c_str());
    LoadPanel(prev_object);
    return;
  }
  else {
    *(frame->statusWin) << "Updated item: " << db_input[0]->GetValue() << "\n";
    // Flush the POD database following each change operation
    child_frame->GetPOD()->Flush(); 
  }

  if(change_grid_name) {
    wxString name = prev_name.c_str(dest);
    int row_number = child_frame->GridFrame()->FindGridPosition(name);
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Prev Grid Name = " << prev_name.c_str(dest) << "\n";
    *(frame->statusWin) << "Prev Grid Position = " << row_number << "\n";
#endif
    child_frame->GridFrame()->MoveCell(curr_object, row_number);
  }
  else {
    wxString name = curr_name.c_str(dest);
    int row_number = child_frame->GridFrame()->FindGridPosition(name);
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Curr Grid Name = " << curr_name.c_str(dest) << "\n";
    *(frame->statusWin) << "Curr Grid Position = " << row_number << "\n";
#endif
    child_frame->GridFrame()->LoadCell(curr_object, row_number);
  }

  if(frame->dpanel->PanelIsOpen()) {
   frame->dpanel->LoadPanel(prev_object);
  }
}

void ChangePanel::OnAccept(wxCommandEvent &WXUNUSED(event))
{
  LoadCurrObject();
  
  if(curr_object == prev_object) {
    ProgramError->Message("There are no changes to commit\n");
    return;
  }

  CommitChanges();
  
  if(!infohog_a[num_entries]) infohog_a[num_entries] = \
	new INFOHOG(child_frame->GetPOD(), NumDataMembers);
  infohog_a[num_entries]->Copy(curr_object);
  num_entries++;
  count = num_entries;
  if(num_entries >= NumPanelEntries) num_entries = count = NumPanelEntries;

  ClearPanel();
  Show(FALSE);
}

void ChangePanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  LoadPanel(prev_object);
}

void ChangePanel::OnRevert(wxCommandEvent &WXUNUSED(event))
{
  if(count <= 0)
    count = num_entries;
  else
    count--;

  if(infohog_a[count]) {
    ClearPanel();
    char dest[DBStringLength];
    
    // NOTE: This is a safty feature. Using member 0 should always be safe
    // provided the call has initialized the infohog object correctly.
    if(prev_object.GetMemberLen(0) != sizeof(INFOHOG_t)) return;

    INFOHOG_t *dbstring = (INFOHOG_t *)prev_object.GetMember(0);
    db_input[0]->SetValue(dbstring->c_str(dest));
    for(int i = 1; i < MaxPanelMembers; i++) {
      if(infohog_a[count]->GetMemberLen(i) != sizeof(INFOHOG_t)) {
	dbstring = (INFOHOG_t *)infohog_a[count]->GetMember(i);
	db_input[i]->SetValue(dbstring->c_str(dest));
      }
    }
  }
}

void ChangePanel::ClearPanel()
{
  for(int i = 0; i < MaxPanelMembers; i++) {
    if(db_input[i]) db_input[i]->Clear();
  }
}

void ChangePanel::LoadPanel(INFOHOG &ob)
{
  ClearPanel();
  curr_object.Copy(ob);
  prev_object.Copy(ob);

  // Load the object
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

void ChangePanel::ShowPanel()
{
  child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  int i;
  if(change_labels) {
    delete key_name_label;
    key_name_label = new wxStaticBox(this, -1,
				     progcfg->global_dbparms.dbgrid_labels[0].c_str(),
				     wxPoint(9, 4),
				     wxSize(ChangePanelConfig::key_label_width, 
					    ChangePanelConfig::key_label_height));
    int start_x = 17;
    int start_y = 28;
    for(i = 1; i < MaxPanelMembers; i++) {
      start_x += ChangePanelConfig::x_offset;
      start_y += ChangePanelConfig::y_offset_label;
      delete db_input_labels[i-1];
      db_input_labels[i-1] = new wxStaticText(this, -1,
					      progcfg->global_dbparms.dbgrid_labels[i].c_str(),
					      wxPoint(start_x, start_y), 
					      wxSize(ChangePanelConfig::label_width,
						     ChangePanelConfig::label_height));
      
      start_y += ChangePanelConfig::y_offset_textbox;
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
				     wxSize(ChangePanelConfig::key_label_width, 
					    ChangePanelConfig::key_label_height));
    change_labels++;
  }
  
  int start_x = 17;
  int start_y = 28;
  for(i = 1; i < MaxPanelMembers; i++) {
    start_x += ChangePanelConfig::x_offset;
    start_y += ChangePanelConfig::y_offset_label;
    gxString label = child_frame->DBParms()->db_config.GetColName(i, dest);  
    if(label != progcfg->global_dbparms.dbgrid_labels[i]) {
      delete db_input_labels[i-1];
      db_input_labels[i-1] = new wxStaticText(this, -1,
					      label.c_str(),
					      wxPoint(start_x, start_y), 
					      wxSize(ChangePanelConfig::label_width,
						     ChangePanelConfig::label_height));

      change_labels++;
    }
    start_y += ChangePanelConfig::y_offset_textbox;
  }

  ShowModal();

  // TODO/NOTE: Must implement in all wxWin 2.6.X code
  // 02/07/2010: Not working on Linux version, so convert to ShowModal() call
  // 02/07/2010: Need to retest in Windows version
  // if wxCHECK_VERSION(2, 6, 0)
  // ShowModal();
  // #else
  // Show(TRUE);
  // #endif
}

ChangePanel *InitChangePanel(wxWindow *parent)
{
  // Calculate the height of the panel
  int height = MaxPanelMembers * (ChangePanelConfig::text_input_height + ChangePanelConfig::label_height);
  height += (ChangePanelConfig::key_label_height + ChangePanelConfig::button_height);
  height += height/2;
  height += 30;

  ChangePanel *panel = new ChangePanel(parent, CHANGE_PANEL_ID,
				       (char *)"Change Database Object",
				       ChangePanelConfig::xpos, ChangePanelConfig::ypos, 
				       ChangePanelConfig::width, height,
		  wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
				       (char *)"Change Database Object");

  if(!panel) return 0;
  
  panel->key_name_label = new wxStaticBox(panel, -1,
					  progcfg->global_dbparms.dbgrid_labels[0].c_str(),
					  wxPoint(9, 4),
					  wxSize(ChangePanelConfig::key_label_width, 
						 ChangePanelConfig::key_label_height));

  panel->db_input[0] = new wxTextCtrl(panel, -1, "",
			       wxPoint(17, 28),
			       wxSize(ChangePanelConfig::text_input_width, 
				      ChangePanelConfig::text_input_height));

  int start_x = 17;
  int start_y = 28;
  for(int i = 1; i < MaxPanelMembers; i++) {
    start_x += ChangePanelConfig::x_offset;
    start_y += ChangePanelConfig::y_offset_label;
    panel->db_input_labels[i-1] = new wxStaticText(panel, -1,
						   progcfg->global_dbparms.dbgrid_labels[i].c_str(),
						   wxPoint(start_x, start_y), 
						   wxSize(ChangePanelConfig::label_width, 
							  ChangePanelConfig::label_height));
    start_y += ChangePanelConfig::y_offset_textbox;
    panel->db_input[i] = new wxTextCtrl(panel, -1, "",
				wxPoint(start_x, start_y),
				wxSize(ChangePanelConfig::text_input_width, 
				       ChangePanelConfig::text_input_height));
  }

  start_y += ChangePanelConfig::y_offset_button;
  panel->accept_btn = new wxButton(panel, ID_CHANGE_ACCEPT, "Accept",
				   wxPoint(start_x, start_y),
				   wxSize(ChangePanelConfig::button_width, 
					  ChangePanelConfig::button_height));

  start_x += ChangePanelConfig::x_offset_button;
  panel->cancel_btn = new wxButton(panel, ID_CHANGE_CANCEL, "Undo",
				   wxPoint(start_x, start_y),
				   wxSize(ChangePanelConfig::button_width, 
					  ChangePanelConfig::button_height));
  start_x += ChangePanelConfig::x_offset_button;
  panel->close_btn = new wxButton(panel, ID_CHANGE_CLOSE, "Close",
				  wxPoint(start_x, start_y),
				  wxSize(ChangePanelConfig::button_width, 
					 ChangePanelConfig::button_height));

  start_x += ChangePanelConfig::x_offset_button;
  panel->revert_btn = new wxButton(panel, ID_CHANGE_REVERT, "Redo",
				   wxPoint(start_x, start_y),
				   wxSize(ChangePanelConfig::button_width, 
					  ChangePanelConfig::button_height));
  // TODO: remove
  //  panel->ShowModal(); 
  panel->Show(FALSE);

  return panel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
