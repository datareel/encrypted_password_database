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

Display panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(DisplayPanel, wxDialog)
  EVT_BUTTON    (ID_DISPLAY_NEXT, DisplayPanel::OnNext)
  EVT_BUTTON    (ID_DISPLAY_PREV, DisplayPanel::OnPrev)
  EVT_BUTTON    (ID_DISPLAY_CLOSE, DisplayPanel::OnClose)
  EVT_BUTTON    (wxID_CANCEL, DisplayPanel::OnClose)
  EVT_BUTTON    (ID_DISPLAY_REMOVE, DisplayPanel::OnRemove)
  EVT_BUTTON    (ID_DISPLAY_CHANGE, DisplayPanel::OnChange)
  EVT_BUTTON    (ID_DISPLAY_EXPORT, DisplayPanel::OnExport)

#ifdef __USE_MSW_PRINTING__  
  EVT_BUTTON    (ID_DISPLAY_PRINT, DisplayPanel::OnPrint)
#endif
END_EVENT_TABLE()

  // Height and width constants for panel objects
int DisplayPanelConfig::key_label_height = 60;
int DisplayPanelConfig::key_label_width = 325;
int DisplayPanelConfig::button_height = 29;
int DisplayPanelConfig::button_width = 65;
int DisplayPanelConfig::text_input_height = 25; 
int DisplayPanelConfig::text_input_width = 305;
int DisplayPanelConfig::label_height = 25;
int DisplayPanelConfig::label_width = 300;
int DisplayPanelConfig::x_start_pos = 17;
int DisplayPanelConfig::y_start_pos = 28;
int DisplayPanelConfig::x_offset = 0;
int DisplayPanelConfig::y_offset_label = 42;
int DisplayPanelConfig::y_offset_textbox = 27;
int DisplayPanelConfig::x_offset_button = 70;
int DisplayPanelConfig::y_offset_button = 39;

  // Panel height and postion
int DisplayPanelConfig::xpos = 50; 
int DisplayPanelConfig::ypos = 50; 
int DisplayPanelConfig::width = 350; 

DisplayPanel::DisplayPanel(wxWindow *parent, wxWindowID id, char *title,
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
  next_btn = prev_btn = close_btn = remove_btn = change_btn = export_btn = 0;
  panel_is_open = 0;
  change_labels = 0;

  child_frame = 0;

#ifdef __USE_MSW_PRINTING__
  print_btn = 0;
#endif
}
  
DisplayPanel::~DisplayPanel()
{
  int i;
  for(i = 0; i < MaxPanelMembers; i++) {
    if(db_input[i]) delete db_input[i];
  }
  for(i = 0; i < MaxPanelMembers; i++) {
    if(db_input_labels[i]) delete db_input_labels[i];
  }

  if(key_name_label) delete key_name_label;
  if(next_btn) delete next_btn;
  if(prev_btn) delete prev_btn;
  if(close_btn) delete close_btn;
  if(remove_btn) delete remove_btn;
  if(change_btn) delete change_btn;
  if(export_btn) delete export_btn;
  
#ifdef __USE_MSW_PRINTING__
  if(print_btn) delete print_btn;
#endif
}

void DisplayPanel::OnClose(wxCommandEvent &WXUNUSED(event))
{
  ClosePanel();
}

void DisplayPanel::OnNext(wxCommandEvent &WXUNUSED(event))
{
  if(!child_frame) {
    ProgramError->Message("The child frame has been closed\n");
    ClosePanel();
    return;
  }

  INFOHOG infohog(child_frame->GetPOD());
  
  if(!dllistptr) return;
  if(!dllist) return;
  if(dllist->IsEmpty()) {
    ClearPanel();
    ProgramError->Message("The display list is empty\n");
    return;
  }
  else {
    dllistptr = dllistptr->next;
  }

  if(dllistptr) {
    if(!infohog.ReadObject(dllistptr->data)) {
      if(!ReReadObject(infohog)) {
	ProgramError->Message("Error processing display list\n");
	return;
      }
    }
    LoadPanel(infohog);
  }
  else {
    // ProgramError->Message("Reached the end of the list\n");
    dllistptr = dllist->GetHead();
    infohog.ReadObject(dllistptr->data);
    LoadPanel(infohog);
  }
}

void DisplayPanel::OnPrev(wxCommandEvent &WXUNUSED(event))
{
  if(!child_frame) {
    ProgramError->Message("The child frame has been closed\n");
    ClosePanel();
    return;
  }

  INFOHOG infohog(child_frame->GetPOD());
  
  if(!dllistptr) return;
  if(!dllist) return;
  if(dllist->IsEmpty()) {
    ClearPanel();
    ProgramError->Message("The display list is empty\n");
    return;
  }
  else {
    dllistptr = dllistptr->prev;
  }

  if(dllistptr) {
    if(!infohog.ReadObject(dllistptr->data)) {
      if(!ReReadObject(infohog)) {
	ProgramError->Message("Error processing display list\n");
	return;
      }
    }
    LoadPanel(infohog);
  }
  else {
    // ProgramError->Message("Reached the beginning of the list\n");
    dllistptr = dllist->GetTail();
    infohog.ReadObject(dllistptr->data);
    LoadPanel(infohog);
  }
}

void DisplayPanel::OnRemove(wxCommandEvent &WXUNUSED(event))
{
  if(!child_frame) {
    ProgramError->Message("The child frame has been closed\n");
    ClosePanel();
    return;
  }

  if(dllist->IsEmpty()) {
    ClearPanel();
    ProgramError->Message("The display list is empty\n");
    return;
  }

  if(!TestDatabase(0, 1, 1)) return;
  
  INFOHOG infohog(child_frame->GetPOD());
  infohog.ReadObject(dllistptr->data);
  char dest[DBStringLength];

  wxString s("Delete item: ");

  // NOTE: This is a safty feature. Using member 0 should always be safe
  // provided the call has initialized the infohog object correctly.
  if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) return;

  INFOHOG_t *dbstring = (INFOHOG_t *)infohog.GetMember(0);

  INFOHOG_t object_name(dbstring->c_str(dest));
  s += object_name.c_str(dest);
  s += "\n";
  int yn = wxMessageBox(s, "Program Message",
			wxYES_NO | wxCENTRE | wxICON_QUESTION, this);
  if(yn == wxNO) return;

  GUIUndoMark undo_mark; // 10/27/2026: Undo and the access log
  gxString undo_name = object_name.c_str(dest);
  GUIUndoBefore(child_frame, undo_name, undo_mark);
  if(!infohog.DeleteObject()) {
    *(frame->statusWin) << "Could not find item: " << object_name.c_str(dest)
			<< " in the database" << "\n";
    return;
  }
  child_frame->GetPOD()->Flush();
  GUIUndoAfter(undo_mark, undo_name, "delete-record");

  *(frame->statusWin) << "Deleted item: " << object_name.c_str(dest) << "\n";


  // Update the grid
  int grid_position = child_frame->GridFrame()->FindGridPosition(infohog);

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Grid Position = " << grid_position << "\n";
#endif
  
  if(grid_position != -1) {
    child_frame->GetGrid()->DeleteRows(grid_position);
    child_frame->GridFrame()->SetResetSkipLines();    
  }
  
  gxListNode<FAU> *curr_ptr = dllistptr;
  dllistptr = dllistptr->next;
  
  dllist->Remove(curr_ptr);
  if(dllist->IsEmpty()) {
    ClearPanel();
    return;
  }

  if(!dllistptr) dllistptr = dllist->GetHead();

  if(!dllistptr) {
    ClearPanel();
    return;
  }
  else {
    infohog.ReadObject(dllistptr->data);
    LoadPanel(infohog);
  }
}

int DisplayPanel::ReReadObject(INFOHOG &infohog)
{
  if(!child_frame) {
    ProgramError->Message("The child frame has been closed\n");
    ClosePanel();
    return 0;
  }

  if(!EPDBFindTitle(infohog, db_input[0]->GetValue())) { 
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Error finding object" << "\n";
#endif
    return 0;
  }
  
  dllistptr->data = infohog.ObjectAddress();
#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Re-reading object: " << (int)dllistptr->data
			<< "\n";
#endif
  if(!infohog.ReadObject(dllistptr->data)) {
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Error re-reading object" << "\n";
#endif
    return 0;
  }

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "No errors reading object" << "\n";
#endif

  return 1;
}

int DisplayPanel::CheckForDeletedBlock(int &is_deleted)
{
  if(!child_frame) {
    ProgramError->Message("The child frame has been closed\n");
    ClosePanel();
    return 0;
  }

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Reading block header: " << (int)dllistptr->data
		      << "\n";
  *(frame->statusWin) << "Object name: " << db_input[0]->GetValue()
		      << "\n";
#endif

  is_deleted = 0;

  gxBlockHeader gx;  // Block Header
  gxDatabase *f = child_frame->DBParms()->pod->OpenDataFile();
  FAU_t addr = dllistptr->data - f->BlockHeaderSize(); 

  if(f->Read(&gx, sizeof(gxBlockHeader), addr) != gxDBASE_NO_ERROR) {
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Error reading database block header" << "\n";
#endif
    return 0;
  }
  char status = (char)gx.block_status & 0xff;
  
  if(status == gxNormalBlock) {
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Block is normal" << "\n";
#endif
    return 1;
  }

  if((status == gxDeletedBlock) || (status == gxRemovedBlock)) {
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Block is removed header" << "\n";
#endif
    is_deleted = 1;
    return 1;
  }

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Block is invalid" << "\n";
#endif

  return 0;
}

void DisplayPanel::OnChange(wxCommandEvent &WXUNUSED(event))
{
  if(!child_frame) {
    ProgramError->Message("The child frame has been closed\n");
    ClosePanel();
    return;
  }

  if(dllist->IsEmpty()) {
    ClearPanel();
    ProgramError->Message("The display list is empty\n");
    return;
  }

  if(!TestDatabase(0, 1, 1)) return;
  
  INFOHOG infohog(child_frame->GetPOD());

  if(dllistptr) {
    int is_deleted;
    if(!CheckForDeletedBlock(is_deleted)) {
      if(!ReReadObject(infohog)) {
	ProgramError->Message("This object could not be modified\n");
	return;
      }
      else {
	frame->cpanel->LoadPanel(infohog);
	frame->cpanel->ShowPanel();
	return;
      }
    }
    if(is_deleted) {
      if(!ReReadObject(infohog)) {
	ProgramError->Message("This object could not be modified\n");
	return;
      }
      else {
	frame->cpanel->LoadPanel(infohog);
	frame->cpanel->ShowPanel();
	return;
      }
    }

    if(!infohog.ReadObject(dllistptr->data)) {
      if(!ReReadObject(infohog)) {
	ProgramError->Message("This object could not be modified\n");
	return;
      }
    }
    
    frame->cpanel->LoadPanel(infohog);
    frame->cpanel->ShowPanel();
  }
  else {
    ProgramError->Message("This object could not be modified\n");
    return;
  }
}

void DisplayPanel::ShowPanel()
{
  child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  char dest[DBStringLength];
  gxString nbuf = child_frame->DBParms()->db_config.database_name.c_str(dest);
  gxString gbuf;
  gbuf << clear << "Displaying " << nbuf;
  SetTitle(gbuf.c_str());

  // 10/27/2026: The column names of this database
  form.SetLabels(child_frame);
  Fit();
  CentreOnParent();
  ShowModal();

  // TODO/NOTE: Must implement in all wxWin 2.6.X code
  // 02/07/2010: Not working on Linux version, so convert to ShowModal() call
  // 02/07/2010: Need to retest in Windows version
  // if wxCHECK_VERSION(2, 6, 0)
  // ShowModal();
  // #else
  // Show(TRUE);
  // #endif

  panel_is_open = 1;
}

void DisplayPanel::ClosePanel()
{
  Show(FALSE);
  panel_is_open = 0;
  child_frame = 0;
}

void DisplayPanel::ClearPanel()
{
  for(int i = 0; i < MaxPanelMembers; i++) {
    if(db_input[i]) {
      db_input[i]->Clear();
    }
  }
}

void DisplayPanel::LoadPanel(INFOHOG &ob)
{
  ClearPanel();
  
  // Load the object
  char dest[DBStringLength];
  for(int i = 0; i < MaxPanelMembers; i++) {
    if(ob.GetMemberLen(i) == sizeof(INFOHOG_t)) {
      INFOHOG_t *dbstring = (INFOHOG_t *)ob.GetMember(i);
      if(!dbstring->is_null()) {
	db_input[i]->SetValue(EPDBFromDB(dbstring->c_str(dest)));
      }
    }
  }

  // Update the display list
  if(dllistptr) dllistptr->data = ob.ObjectAddress();
}

#ifdef __USE_MSW_PRINTING__
void DisplayPanel::OnPrint(wxCommandEvent &event)
{
  if(!child_frame) {
    ProgramError->Message("The child frame has been closed\n");
    ClosePanel();
    return;
  }

  MSWPrintingParameters *print_config = &child_frame->print_config;

  // Record the current pointer in the virtual list
  gxListNode<FAU> *curr_dllistptr = dllistptr;
  print_config->print_list = 1;
  print_config->print_all_entries = 0;
  print_config->print_all_cols = 1;
  wxPrintDialogData printDialogData(print_config->print_data);
  wxPrinter printer(& printDialogData);
  mswPrintout printout(progcfg->ProgramName.c_str());
  
  if(!printer.Print(this, &printout, TRUE)) {
    if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
      wxMessageBox("There was a problem printing.\nPerhaps your current \
printer is not set correctly?", "Printing", wxOK);
    else
      wxMessageBox("Print operation canceled", "Printing", wxOK);
  }
  else {
    print_config->print_data = printer.GetPrintDialogData().GetPrintData();
  }

  // Restore the virtual list pointer
  dllistptr = curr_dllistptr;
}
#endif

void DisplayPanel::OnExport(wxCommandEvent &event)
{
  if(!child_frame) {
    ProgramError->Message("The child frame has been closed\n");
    ClosePanel();
    return;
  }

  // Record the current pointer in the virtual list
  gxListNode<FAU> *curr_dllistptr = dllistptr;
  ExportToASCII(this);
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());

  // Restore the virtual list pointer
  dllistptr = curr_dllistptr;
}

DisplayPanel *InitDisplayPanel(wxWindow *parent)
// 10/27/2026: The record form (record_form.cpp), read-only, with sizers
{
  DisplayPanel *panel = new DisplayPanel(parent, DISPLAY_PANEL_ID, (char *)"Display Records", -1, -1, -1, -1,
					 wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, (char *)"Display Records");
  if(!panel) return 0;
  panel->SetIcons(EPDBAppIcons());
  const int pad = panel->FromDIP(12);
  const int gap = panel->FromDIP(6);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *form = new wxBoxSizer(wxVERTICAL);
  panel->form.Build(panel, form, panel->db_input, panel->db_input_labels, 1);
  top->Add(form, 1, wxEXPAND | wxALL, pad);

  wxBoxSizer *b = new wxBoxSizer(wxHORIZONTAL);
  panel->prev_btn = new wxButton(panel, ID_DISPLAY_PREV, "Prior");
  b->Add(panel->prev_btn, 0);
  panel->next_btn = new wxButton(panel, ID_DISPLAY_NEXT, "Next");
  b->Add(panel->next_btn, 0, wxLEFT, gap);
  b->AddSpacer(panel->FromDIP(16));
  panel->change_btn = new wxButton(panel, ID_DISPLAY_CHANGE, "Edit...");
  panel->change_btn->SetBitmap(EPDBIcon("edit", EPDB_MENU_ICON_DIP));
  b->Add(panel->change_btn, 0, wxLEFT, gap);
  panel->remove_btn = new wxButton(panel, ID_DISPLAY_REMOVE, "Delete");
  panel->remove_btn->SetBitmap(EPDBIcon("delete", EPDB_MENU_ICON_DIP));
  b->Add(panel->remove_btn, 0, wxLEFT, gap);
  panel->export_btn = new wxButton(panel, ID_DISPLAY_EXPORT, "Export...");
  b->Add(panel->export_btn, 0, wxLEFT, gap);
#ifdef __USE_MSW_PRINTING__
  panel->print_btn = new wxButton(panel, ID_DISPLAY_PRINT, "Print...");
  panel->print_btn->SetBitmap(EPDBIcon("print", EPDB_MENU_ICON_DIP));
  b->Add(panel->print_btn, 0, wxLEFT, gap);
#endif
  b->AddStretchSpacer();
  panel->close_btn = new wxButton(panel, wxID_CANCEL, "Close");
  b->Add(panel->close_btn, 0, wxLEFT, gap);
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
