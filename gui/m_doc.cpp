// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
// Copyright (c) 2001-2024 DataReel Software Development
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

Document controls
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

// Note that MDI_NEW_WINDOW and MDI_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.
BEGIN_EVENT_TABLE(CryptDBDocument, wxMDIChildFrame)
  EVT_MENU(WXAPPFW_MDI_CHILD_QUIT, CryptDBDocument::OnQuit)
  EVT_MENU(WXAPPFW_MDI_REFRESH, CryptDBDocument::OnRefresh)
  EVT_MENU(WXAPPFW_MDI_CHANGE_POSITION, CryptDBDocument::OnChangePosition)
  EVT_MENU(WXAPPFW_MDI_CHANGE_SIZE, CryptDBDocument::OnChangeSize)
  EVT_SIZE(CryptDBDocument::OnSize)
  EVT_MOVE(CryptDBDocument::OnMove)
  EVT_CLOSE(CryptDBDocument::OnClose)
  
  // Grid events
  EVT_GRID_CELL_LEFT_CLICK(CryptDBDocument::OnCellLeftClick)
  EVT_GRID_CELL_RIGHT_CLICK(CryptDBDocument::OnCellRightClick)
  EVT_GRID_CELL_LEFT_DCLICK(CryptDBDocument::OnCellLeftDClick)
  EVT_GRID_CELL_RIGHT_DCLICK(CryptDBDocument::OnCellRightDClick)
  EVT_GRID_LABEL_LEFT_CLICK(CryptDBDocument::OnLabelLeftClick)
  EVT_GRID_LABEL_RIGHT_CLICK(CryptDBDocument::OnLabelRightClick)
  // EVT_GRID_LABEL_LEFT_DCLICK(CryptDBDocument::OnLabelLeftDClick)	
  // EVT_GRID_LABEL_RIGHT_DCLICK(CryptDBDocument::OnLabelRightDClick)	
  EVT_GRID_CELL_CHANGE(CryptDBDocument::OnCellValueChanged)
  EVT_GRID_SELECT_CELL(CryptDBDocument::OnSelectCell)
  EVT_GRID_EDITOR_HIDDEN(CryptDBDocument::OnEditorHidden)	
  EVT_GRID_EDITOR_SHOWN(CryptDBDocument::OnEditorShown)	
  EVT_GRID_ROW_SIZE(CryptDBDocument::OnRowSize)
  EVT_GRID_COL_SIZE(CryptDBDocument::OnColSize)
  EVT_GRID_RANGE_SELECT(CryptDBDocument::OnRangeSelected)
  // EVT_GRID_EDITOR_CREATED(CryptDBDocument::OnEditorCreated)	
END_EVENT_TABLE()

CryptDBDocument::CryptDBDocument(wxMDIParentFrame *parent, 
				 const wxString& title,
				 const wxPoint& pos, const wxSize& size,
				 const long style)
  : wxMDIChildFrame(parent, CHILD_FRAME_ID, title, pos, size,
		    style | wxNO_FULL_REPAINT_ON_RESIZE)
{
  grid_frame =(CryptDBGrid *)0;

  // TODO: Not working in linux version
  CryptDBDocument *p = (CryptDBDocument *)this;
  progcfg->child_frame_list.Add(p);
  
  // this should work for MDI frames as well as for normal ones
  SetSizeHints(100, 100);
}

CryptDBDocument::~CryptDBDocument()
{
  GetPOD()->Close(); // Close the open database

  if(progcfg->active_child_frame == this) {
    progcfg->active_child_frame = 0;
  }

  gxListNode<CryptDBDocument *> *ptr = progcfg->child_frame_list.GetHead();
  while(ptr) {
    if(ptr->data == (CryptDBDocument *)this) {
      progcfg->child_frame_list.Remove(ptr);
      break;
    }
    ptr = ptr->next;
  } 

}

void CryptDBDocument::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(TRUE);
}

void CryptDBDocument::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
  if(grid_frame) grid_frame->Refresh();
}

void CryptDBDocument::OnChangePosition(wxCommandEvent& WXUNUSED(event))
{
  Move(10, 10);
}

void CryptDBDocument::OnChangeSize(wxCommandEvent& WXUNUSED(event))
{
  SetClientSize(100, 100);
}

void CryptDBDocument::OnActivate(wxActivateEvent& event)
{
  if(event.GetActive() && grid_frame) grid_frame->SetFocus();
}

void CryptDBDocument::OnMove(wxMoveEvent& event)
{
  // NOTE: Here everything is totally wrong under MSW, the positions are
  //       different and both wrong (pos2 is off by 2 pixels for me which seems
  //       to be the width of the MDI grid_frame border)
  // wxPoint pos1 = event.GetPosition(),
  // pos2 = GetPosition();
  // wxLogStatus(wxT("position from event: (%d, %d), from frame (%d, %d)"),
  //	         pos1.x, pos1.y, pos2.x, pos2.y);
    event.Skip();
}

void CryptDBDocument::OnSize(wxSizeEvent& event)
{
  // NOTE: under MSW the size event carries the client size (quite
  //       unexpectedly) *except* for the very first one which has the full
  //       size... what should it really be? Need to check under wxGTK
  // wxSize size1 = event.GetSize(),
  // size2 = GetSize(),
  // size3 = GetClientSize();
  // wxLogStatus(wxT("size from event: %dx%d, from frame %dx%d, client %dx%d"),
  //          size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);
  
  event.Skip();
}

void CryptDBDocument::OnClose(wxCloseEvent& event)
{
  // TODO: Use to track grid changes
  // because there are no files to save.
  // if(grid_frame && grid_frame->IsDirty()) {
  //   if ( wxMessageBox("Really close?", "Please confirm",
  //		      wxICON_QUESTION | wxYES_NO) != wxYES ) {
  //    event.Veto();
  //    return;
  //  }
  // }

  if(frame->dpanel->PanelIsOpen()) {
    if(!frame->dpanel->child_frame) {
      frame->dpanel->ClosePanel();
    }
    else {
      if(frame->dpanel->child_frame == this) {
	frame->dpanel->ClosePanel();
      }
    }
  }

  if(progcfg->num_child_frames > 0) progcfg->num_child_frames--;
  event.Skip();
}

int CryptDBDocument::LoadGridObject(INFOHOG &infohog)
// Load an InfoHog object from the current row. Return true
// if the col 0 is not null.
{
  if(grid_frame->m_grid->GetCellValue(DBParms()->current_row, 0) == ""){
    *(frame->statusWin) << "Error loading grid object at: " << DBParms()->current_row << ",0" 
			<< "\n";
    return 0;
  }
  for(int i = 0; i < NumDataMembers; i++) {
    wxString mbuf = grid_frame->m_grid->GetCellValue(DBParms()->current_row, i);
    if(!mbuf.IsNull()) {
      INFOHOG_t ih_members(mbuf.c_str());
      infohog.SetMember(&ih_members, sizeof(ih_members), i);    
    }
  }
  return 1;
}

void CryptDBDocument::OnLabelLeftClick(wxGridEvent& event)
{
  // Sort order the grid
  if(event.GetCol() != -1) {
    if(event.GetCol() == 0) {
      grid_frame->ReloadGrid(0);
    }
  }

#ifdef __APP_DEBUG_VERSION__
  if(event.GetRow() != -1 ) {
    *(frame->statusWin) << "Left click on row label " << event.GetRow() << "\n";
  }
  else if(event.GetCol() != -1) {
    *(frame->statusWin) << "Left click on col label " << event.GetCol() << "\n";
  }
  else {
    *(frame->statusWin) << "Left click on corner label" << "\n";
  }
#endif
  
  return;
  // NOTE: Do not process the default behavior
  // event.Skip() will highlight the column
}

void CryptDBDocument::OnLabelRightClick(wxGridEvent& event)
{
  char dest[DBStringLength];

#ifdef __APP_DEBUG_VERSION__
  if(event.GetRow() != -1 ) {
    *(frame->statusWin) << "Right click on row label " << event.GetRow() << "\n";
  }
  else if(event.GetCol() != -1) {
    *(frame->statusWin) << "Right click on col label " << event.GetCol() << "\n";
  }
  else {
    *(frame->statusWin) << "Right click on corner label" << "\n";
  }
#endif

  if(event.GetCol() != -1) {
    int col = (int)event.GetCol();
    char *p = DBParms()->db_config.GetColName(col, dest);
    wxTextEntryDialog dialog(this,
			     "Enter a new column name\n",
			     "Changing column name",
			     p, wxOK | wxCANCEL);

    if(dialog.ShowModal() == wxID_OK) {
      if(!TestDatabase(0, 1, 1)) {
	event.Skip();
	return;
      }
      if(!DBParms()->db_config.WriteColName(DBParms()->pod->OpenDataFile(),
					    col, dialog.GetValue().c_str())) {
#ifdef __APP_DEBUG_VERSION__
	*(frame->statusWin) << "Error writing DB config value" << "\n";
	CheckError(DBParms()->pod->OpenDataFile(), frame->statusWin);
#endif
      }

      p = DBParms()->db_config.GetColName(col, dest);

#ifdef __WXWIN2_GRID__
      grid_frame->m_grid->SetLabelValue(wxHORIZONTAL, p, col);
#else
      grid_frame->m_grid->SetColLabelValue(col, p);
#endif
    }
  }
  event.Skip();
}

void CryptDBDocument::OnCellLeftClick(wxGridEvent& event)
{
  DBParms()->current_row = event.GetRow();
  DBParms()->current_col = event.GetCol();
  DBParms()->current_name = grid_frame->m_grid->GetCellValue(DBParms()->current_row, 0);


#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Left click at row " << event.GetRow()
		      << " col " << event.GetCol() << "\n";
  *(frame->statusWin) << "Curr name = " <<  DBParms()->current_name << "\n";
#endif
  
  event.Skip(); // Process the default behavior
}

void CryptDBDocument::OnCellRightClick(wxGridEvent& event)
{
#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Right click at row " << event.GetRow()
		      << " col " << event.GetCol() << "\n";
#endif
  
  event.Skip(); // Process the default behavior
}

void CryptDBDocument::OnCellRightDClick(wxGridEvent& event)
{
#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Right double click at row " << event.GetRow()
		      << " col " << event.GetCol() << "\n";
#endif
  
  event.Skip(); // Process the default behavior
}

void CryptDBDocument::OnCellLeftDClick(wxGridEvent& event)
{
  DBParms()->current_row = event.GetRow();
  DBParms()->current_col = event.GetCol();
  DBParms()->current_name = grid_frame->m_grid->GetCellValue(DBParms()->current_row, 0);

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Left double click at row " << event.GetRow()
		      << " col " << event.GetCol() << "\n";
#endif


  if(DBParms()->current_name == "") { // This cell is empty
    if(!TestDatabase(0, 1, 1)) return;
    frame->apanel->ShowPanel();
    event.Skip(); // Process the default behavior if the table is empty
    return;
  }
  
  gxString cell_str = (const char *)grid_frame->m_grid->GetCellValue(DBParms()->current_row,
								     DBParms()->current_col).c_str();

  if(IsHyperlink(cell_str)) { // Start the hyperlink
    StartDocument(cell_str, progcfg->web_browser);
    event.Skip(); // Process the default behavior and return
    return;
  }

  // NOTE: The new design will allow your to edit the object's key member on the 
  // grid or with the change panel by double left clicking on the cell.
  if(DBParms()->current_col == 0) { // Update the name of this object
    INFOHOG infohog(DBParms()->pod, NumDataMembers);
    wxString sbuf;

    INFOHOG_t key(grid_frame->m_grid->GetCellValue(DBParms()->current_row, 0));
    infohog.SetMember(&key, sizeof(key), 0);
    if(!infohog.FindObject()) {
      if(DBParms()->display_status) {
	*(frame->statusWin) << "Could not find item: " 
			    << grid_frame->m_grid->GetCellValue(DBParms()->current_row, 0)
			    << " in the database" << "\n";
      }
      event.Skip(); // Process the default behavior
      return;
    }

    if(!infohog.ReadObject()) {
      if(DBParms()->display_status) {
	  char dest[DBStringLength];
	  if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) {
	    event.Skip();
	    return;
	  }
	  INFOHOG_t *dbstring = (INFOHOG_t *)infohog.GetMember(0);
	*(frame->statusWin) << "Error reading item: " 
			    << dbstring->c_str(dest)
			    << " in the database" << "\n";
      }
      event.Skip();
      return;
    }
    frame->cpanel->LoadPanel(infohog);
    frame->cpanel->ShowPanel();
  }

  event.Skip(); // Process the default behavior
}

void CryptDBDocument::OnSelectCell(wxGridEvent& event)
{
  if(event.Selecting()) {
    DBParms()->current_row = event.GetRow();
    DBParms()->current_col = event.GetCol();
    DBParms()->current_name = grid_frame->m_grid->GetCellValue(DBParms()->current_row, 0);
  }

#ifdef __APP_DEBUG_VERSION__
  if(event.Selecting()) {
    *(frame->statusWin) << "Select cell at row " << event.GetRow()
			<< " col " << event.GetCol() << "\n";
    *(frame->statusWin) << "Curr name = " <<  DBParms()->current_name << "\n";
  }
  else {
    *(frame->statusWin) << "Deselect cell at row " << event.GetRow()
			<< " col " << event.GetCol() << "\n";
  }
#endif
  
  event.Skip();
}

void CryptDBDocument::OnRangeSelected(wxGridRangeSelectEvent& event)
{
#ifdef __APP_DEBUG_VERSION__
  if(event.Selecting())
    *(frame->statusWin) << "Range Selected " << "\n";
  else
    *(frame->statusWin) << "Range Deselected " << "\n";
#endif

  event.Skip();
}

void CryptDBDocument::OnRowSize(wxGridSizeEvent& event)
{
#ifdef __APP_DEBUG_VERSION__
  int current_row = event.GetRowOrCol();
  *(frame->statusWin) << "Resized row number " << current_row << "\n";
#endif
  
  event.Skip();
}

void CryptDBDocument::OnColSize(wxGridSizeEvent& event)
{
  int current_col = event.GetRowOrCol();
#ifdef __WXWIN2_GRID__
  int col_width = grid_frame->m_grid->GetColumnWidth(current_col);
#else
  int col_width = grid_frame->m_grid->GetColSize(current_col);
#endif
  
  if(!TestDatabase(0, 1, 0)) {
    event.Skip();
    return;
  }
  
  if(!DBParms()->db_config.WriteColSize(DBParms()->pod->OpenDataFile(),
					current_col, col_width)) {
    *(frame->statusWin) << "Error writing colsize DB config value" << "\n";
#ifdef __APP_DEBUG_VERSION__
    CheckError(DBParms()->pod->OpenDataFile(), frame->statusWin);
#endif
  }
  
#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Resized column number " << current_col << " to "
	       << col_width << "\n";
#endif

  event.Skip();
}

POD *CryptDBDocument::GetPOD() 
{
  if(grid_frame) return DBParms()->pod;
  return (POD *)0;
}

CryptDBGrid *CryptDBDocument::GridFrame()
{
  return grid_frame;
}

gxDatabaseParms *CryptDBDocument::DBParms()
{
  return &grid_frame->dbparms;
}

wxGrid *CryptDBDocument::GetGrid()
{
  return grid_frame->m_grid;
}


int CryptDBDocument::change_cell_value(wxGridEvent& event)
{
  char dest[DBStringLength];
  char *p;
  gxString gbuf;
  DBParms()->current_name = grid_frame->m_grid->GetCellValue(DBParms()->current_row,0);
  DBParms()->current_row = event.GetRow();
  DBParms()->current_col = event.GetCol();
  wxFont font;
  wxColor color;

  // Unique copies
  wxString my_current_name = DBParms()->current_name;
  int my_current_row = DBParms()->current_row;
  int my_current_col = DBParms()->current_col;

  if(grid_frame->m_grid->GetCellValue(my_current_row, 0) == "") {
    p = DBParms()->db_config.GetColName(0, dest);
    gbuf << clear << "Please enter the object's " 
	 << p << " field";
    ProgramError->Message(gbuf.c_str());
    grid_frame->m_grid->SetCellValue(my_current_row, 
				     my_current_col, "");
    return 0;
  }

  INFOHOG infohog(DBParms()->pod, NumDataMembers);
  wxString sbuf;

  if(my_current_col == 0) {
    wxString curr_val, find_val;
    curr_val = grid_frame->m_grid->GetCellValue(my_current_row, 0);
    curr_val.MakeUpper();

#ifdef __WXWIN2_GRID__
    int rows = grid_frame->m_grid->GetRows();
#else
    int rows = grid_frame->m_grid->GetNumberRows();
#endif

    int pos = 0; // Start at row zero
      
    // Find the row excluding row zero
    while(rows != 0) {
      find_val = grid_frame->m_grid->GetCellValue(pos, 0);
      find_val.MakeUpper();
      if(find_val == curr_val) break;
      pos++; // Go to the next row
      rows--;
    }
    
    if(my_current_row != pos) {
      p = DBParms()->db_config.GetColName(0, dest);
      gbuf << clear << p << ": " 
	   <<  (const char *)curr_val.c_str() << "\n" << "already exists in the database";
      ProgramError->Message(gbuf.c_str());
      
      if(DBParms()->adding_cell_keyname == 1) {
	grid_frame->m_grid->SetCellValue(my_current_row, 0, "");
      }
      else {
	grid_frame->m_grid->SetCellValue(my_current_row, 0, DBParms()->prev_name);
      }
      return 0;
    }

    if(DBParms()->adding_cell_keyname == 1) {
      *(frame->statusWin) << "Adding object to the database: " << my_current_name.c_str() 
			  << "\n";

      if(frame->dpanel->PanelIsOpen()) {
	if(!frame->dpanel->child_frame) {
	  frame->dpanel->ClosePanel();
	}
	else {
	  if(frame->dpanel->child_frame == this) {
	    frame->dpanel->ClosePanel();
	  }
	}
      }
      
      // This is a new object so add it to the database and return
      if(!LoadGridObject(infohog)) {
	ProgramError->Message("Could not add new object to the database\n");
	return 0;
      }
      if(!infohog.WriteObject()) {
	ProgramError->Message("Could not write new object to the database\n");
	return 0;
      }
      else {
	DBParms()->pod->Flush();
	*(frame->statusWin) << "Added new object : " 
			    << curr_val
			    << " to the database" << "\n";
      }
      DBParms()->current_name = curr_val;
      gbuf = (const char *)curr_val.c_str();
      if(IsHyperlink(gbuf)) {
	if(DBParms()->db_config.GetHyperlinkColor(&color)) {
	  grid_frame->m_grid->SetCellTextColour(my_current_row, 0, 
						color);
	}
	else { 
	  grid_frame->m_grid->SetCellTextColour(my_current_row, 0, 
						*frame->hyperlinkColor);
	}
	if(DBParms()->db_config.GetHyperlinkFont(&font)) {
#ifdef __WXWIN2_GRID__
	  grid_frame->m_grid->SetCellTextFont(font, my_current_row, 0);
#else
	  grid_frame->m_grid->SetCellFont(my_current_row, 0, font);
#endif
	}
	else {
#ifdef __WXWIN2_GRID__
	  grid_frame->m_grid->SetCellTextFont(*frame->hyperlinkFont, my_current_row, 0);
#else
	  grid_frame->m_grid->SetCellFont(my_current_row, 0, *frame->hyperlinkFont);
#endif
	  
	}
      }
      return 1;
    }
    else if(DBParms()->changing_cell_keyname == 1) {
      *(frame->statusWin) << "Changing key member: " << DBParms()->prev_name.c_str() << "\n";
      
      if(frame->dpanel->PanelIsOpen()) {
	if(!frame->dpanel->child_frame) {
	  frame->dpanel->ClosePanel();
	}
	else {
	  if(frame->dpanel->child_frame == this) {
	    frame->dpanel->ClosePanel();
	  }
	}
      }

      INFOHOG_t search_key2(DBParms()->prev_name.c_str());
      infohog.SetMember(&search_key2, sizeof(search_key2), 0);
      if(!infohog.FindObject()) { 
	*(frame->statusWin) << "Could not change object " << DBParms()->prev_name.c_str()
			    << "\n" << "Object not found in the database" 
			    << "\n";
	
	return 0;
      }
      INFOHOG curr_object(DBParms()->pod, NumDataMembers);
      LoadGridObject(curr_object);
      INFOHOG_t *dbstring = (INFOHOG_t *)curr_object.GetMember(0);
      
      if(!infohog.ChangeObject(curr_object, 0)) {
	*(frame->statusWin) << "Could not change " << DBParms()->prev_name
			    << " to " << dbstring->c_str(dest)
			    << " in the database" << "\n";

	grid_frame->m_grid->SetCellValue(my_current_row, 0, 
					 DBParms()->prev_name);
	return 0;
      }
      else {
	gbuf = (const char *)DBParms()->prev_name.c_str();
	if(IsHyperlink(gbuf)) { // Reset the font it the previous name is a hyperlink
	  if(DBParms()->db_config.GetGridTextColor(&color)) {
	    grid_frame->m_grid->SetCellTextColour(my_current_row, 0, color);
	  }
	  else {
	    grid_frame->m_grid->SetCellTextColour(my_current_row, 0, *frame->cellFontColor);
	  }
	  if(DBParms()->db_config.GetGridTextFont(&font)) {
#ifdef __WXWIN2_GRID__
	    grid_frame->m_grid->SetCellTextFont(font, my_current_row, 0);
#else
	    grid_frame->m_grid->SetCellFont(my_current_row, 0, font);
#endif

	  }
	  else {
#ifdef __WXWIN2_GRID__
	    grid_frame->m_grid->SetCellTextFont(*frame->itemFont, my_current_row, 0);
#else
	    grid_frame->m_grid->SetCellFont(my_current_row, 0, *frame->itemFont);
#endif
	  }
	}
	*(frame->statusWin) << "Changed " << DBParms()->prev_name
			    << " to " <<  dbstring->c_str(dest) << "\n";
	// Flush the POD database following each change operation
	DBParms()->pod->Flush();
	gbuf = dbstring->c_str(dest);
	if(IsHyperlink(gbuf)) {
	  if(DBParms()->db_config.GetHyperlinkColor(&color)) {
	    grid_frame->m_grid->SetCellTextColour(my_current_row, 0, 
						  color);
	  }
	  else {
	    grid_frame->m_grid->SetCellTextColour(my_current_row, 0, 
						  *frame->hyperlinkColor);
	  }
	  if(DBParms()->db_config.GetHyperlinkFont(&font)) {
#ifdef __WXWIN2_GRID__
	    grid_frame->m_grid->SetCellTextFont(font, my_current_row, 0);
#else
	    grid_frame->m_grid->SetCellFont(my_current_row, 0, font);
#endif
	  }
	  else {
#ifdef __WXWIN2_GRID__
	    grid_frame->m_grid->SetCellTextFont(*frame->hyperlinkFont, my_current_row, 0);
#else
	    grid_frame->m_grid->SetCellFont(my_current_row, 0, *frame->hyperlinkFont);
#endif
	  }
	}
	return 1;
      }
    }
    else {
      *(frame->statusWin) << "Unknown status" << "\n";
      return 0;
    }
  }
  
  if(my_current_col >= 1) { // Update the object member
    if(my_current_name != DBParms()->prev_name) {
      my_current_name = DBParms()->prev_name;
      my_current_col = DBParms()->prev_col;
      my_current_row = DBParms()->prev_row;
    }

    if(frame->dpanel->PanelIsOpen()) {
      if(!frame->dpanel->child_frame) {
	frame->dpanel->ClosePanel();
      }
      else {
	if(frame->dpanel->child_frame == this) {
	  frame->dpanel->ClosePanel();
	}
      }
    }

    INFOHOG_t key(my_current_name.c_str());
    infohog.SetMember(&key, sizeof(key), 0);
    if(!infohog.FindObject()) { 
      *(frame->statusWin) << "Could not change object " << my_current_name
			  << "\n" << "Object not found in the database" 
			  << "\n";
      
      return 0;
    }
    INFOHOG_t member;
    sbuf = grid_frame->m_grid->GetCellValue(my_current_row, my_current_col);
    gxString ssbuf = (const char *)sbuf.c_str();
    member = ssbuf.c_str();

    if(!infohog.ChangeMember(&member, sizeof(member), my_current_col)) {
      *(frame->statusWin) << "Could not update item: "
			  << grid_frame->m_grid->GetCellValue(my_current_row, 
							      0)
			  << " in the database" << "\n";
      return 0;
    }
    else {
      // NOTE: Resetting the grid font and color
      if(DBParms()->db_config.GetGridTextColor(&color)) {
	grid_frame->m_grid->SetCellTextColour(my_current_row, 
					      my_current_col, 
					      color);
      }
      else {
	grid_frame->m_grid->SetCellTextColour(my_current_row, 
					      my_current_col, 
					      *frame->cellFontColor);
      }
      if(DBParms()->db_config.GetGridTextFont(&font)) {
#ifdef __WXWIN2_GRID__
	grid_frame->m_grid->SetCellTextFont(font, my_current_row, my_current_col);
#else
	grid_frame->m_grid->SetCellFont(my_current_row, my_current_col, font);
#endif
      }
      else {
#ifdef __WXWIN2_GRID__
	grid_frame->m_grid->SetCellTextFont(*frame->itemFont, my_current_row, my_current_col);
#else
	grid_frame->m_grid->SetCellFont(my_current_row, my_current_col, *frame->itemFont);
#endif

      }

      *(frame->statusWin) << "Updated item: " 
			  << grid_frame->m_grid->GetCellValue(my_current_row, 
							      0)
			  << "\n";
      DBParms()->pod->Flush();
      gbuf = (const char *)grid_frame->m_grid->GetCellValue(my_current_row, my_current_col).c_str();
      if(IsHyperlink(gbuf)) {
	if(DBParms()->db_config.GetHyperlinkColor(&color)) {
	  grid_frame->m_grid->SetCellTextColour(my_current_row, 
						my_current_col, 
						color);
	}
	else {
	  grid_frame->m_grid->SetCellTextColour(my_current_row, 
						my_current_col, 
					      *frame->hyperlinkColor);
	}
	if(DBParms()->db_config.GetHyperlinkFont(&font)) {
#ifdef __WXWIN2_GRID__
	  grid_frame->m_grid->SetCellTextFont(font, my_current_row, my_current_col);
#else
	  grid_frame->m_grid->SetCellFont(my_current_row, my_current_col, font);
#endif
	}
	else {
#ifdef __WXWIN2_GRID__
	  grid_frame->m_grid->SetCellTextFont(*frame->hyperlinkFont, my_current_row, my_current_col);
#else
	  grid_frame->m_grid->SetCellFont(my_current_row, my_current_col, *frame->hyperlinkFont);
#endif
	}
      }
      return 1;
    }
  }

  return 0;
}

void CryptDBDocument::OnCellValueChanged(wxGridEvent& event)
// Event handler used to change a cell value except for the
// object's key name.
{
  wxString current_name = grid_frame->m_grid->GetCellValue(DBParms()->current_row,0);
  int current_row = event.GetRow();
  int current_col = event.GetCol();

  if(!change_cell_value(event)) {
    DBParms()->current_name = current_name;
    DBParms()->current_row = current_row;
    DBParms()->current_col = current_col;
  }
  else {
    if(current_col == 0) {
      DBParms()->adding_cell_keyname = 0;
      DBParms()->changing_cell_keyname = 0;
    }
  }

  event.Skip();
}

void CryptDBDocument::OnEditorShown( wxGridEvent& event )
{
  DBParms()->current_row = event.GetRow();
  DBParms()->current_col = event.GetCol();
  
  wxString sbuf = grid_frame->m_grid->GetCellValue(event.GetRow(), 0);
  if(sbuf.IsNull()) {
    DBParms()->adding_cell_keyname = 1;
    DBParms()->changing_cell_keyname = 0;
  }
  else {
    DBParms()->changing_cell_keyname = 1;
    DBParms()->adding_cell_keyname = 0;
    DBParms()->prev_name = grid_frame->m_grid->GetCellValue(event.GetRow(), 0);
    DBParms()->prev_row = event.GetRow();
    DBParms()->prev_col = event.GetCol();
  }

  event.Skip();
}

void CryptDBDocument::OnEditorHidden( wxGridEvent& event )
{
  event.Skip();
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
