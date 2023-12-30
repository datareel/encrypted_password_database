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

Application grid code 
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(CryptDBGrid, wxFrame)

END_EVENT_TABLE()

CryptDBGrid::CryptDBGrid(wxWindow *parent, const wxPoint& pos, 
			 const wxSize& size)
  : wxFrame(parent, -1, "Database Grid", pos, size,
		     wxSUNKEN_BORDER |
		     wxNO_FULL_REPAINT_ON_RESIZE |
		     wxVSCROLL | wxHSCROLL)
{
  SetBackgroundColour(wxColour("WHITE"));
  // TODO: Use dirty to check for changes before user closes grid.
  m_dirty = FALSE;

  parent_frame = parent;
  m_grid = 0;
  grid_pos = pos;
  grid_size = size;

  m_grid = new wxGrid(parent_frame, CHILD_FRAME_ID, grid_pos, grid_size);
}

long CryptDBGrid::InitGrid()
{
  long num_rows = dbparms.default_num_rows;
  long num_entries = 0;

  if(dbparms.pod) { // Ensure that the database has been opened
    num_entries = (long)dbparms.pod->Index()->NumKeys();
    if(num_entries != 0) { // This database is not empty
      // NOTE: Adding some additonal rows for user to enter data on the grid
      num_rows = num_entries+dbparms.default_num_rows;
    }
  }

  // Set the default text font and color
  wxFont font;
  if(dbparms.db_config.GetGridTextFont(&font)) {
    m_grid->SetDefaultCellFont(font);
  }
  else {
    m_grid->SetDefaultCellFont(*frame->itemFont);   
  }
  wxColor color;
  if(dbparms.db_config.GetGridTextColor(&color)) {
    m_grid->SetDefaultCellTextColour(color);
  }

  m_grid->CreateGrid(num_rows, NumDataMembers);
  MakeGridColLabels();

  dbparms.row_label_size = m_grid->GetDefaultRowLabelSize();

  if(dbparms.db_config.view_labels == 0)  m_grid->SetColLabelSize(0);

  if(dbparms.db_config.view_row_numbers == 1) {
    m_grid->SetRowLabelAlignment(wxCENTRE, -1);
    m_grid->SetRowLabelSize(m_grid->GetDefaultRowLabelSize());
  }
  else {
    m_grid->SetRowLabelSize(0);
  }

  // If enabled override the default grid setting

  // If disabled override the default grid setting
  if(dbparms.db_config.view_grid_lines == 0) m_grid->EnableGridLines(0);
  if(dbparms.db_config.cell_overflow == 0)  m_grid->SetDefaultCellOverflow(0);

  // If enabled set automatically with no refresh
  SetGridLineColor(0);
  SetGridLabelBackgroundColor(0);
  SetGridLabelTextColor(0);
  SetGridBackgroundColor(0); // Set the default background color first
  SetSkipLines(0);           // Set the skip line color
  SetGridLabelFont(0);

  dbparms.current_row = 0;
  dbparms.current_col = 0;
  dbparms.current_name = "";

  // Return the number of index file entries when the grid was
  // first constructed. This will prevent crashed in multi-threaded
  // applications if the number of index file entires increase before
  // the grid could be loaded on screen.
  return num_entries;
}

void CryptDBGrid::MakeGridColLabels()
{
  char dest[DBStringLength];

  // Set the grid line color
  m_grid->SetGridLineColour(*frame->gridlineColor);
  
  int i;
  m_grid->SetColLabelSize(dbparms.column_label_size);
  m_grid->SetColLabelAlignment(wxCENTRE, -1);
  for(i = 0; i < NumDataMembers; i++) {
    char *p = dbparms.db_config.GetColName(i, dest);
    m_grid->SetLabelValue(wxHORIZONTAL, p, i);
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Col name = " << p << "\n";
#endif
  }

  // Set the cell widths
  for(i = 0; i < NumDataMembers; i++) {
    m_grid->SetColumnWidth(i, (long)dbparms.db_config.col_sizes[i]);
    
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Setting the cell widths" << "\n";
    *(frame->statusWin) << "Col " << i << " size = "
			<< (long)dbparms.db_config.col_sizes[i] << "\n";
#endif
  }

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Database name = " 
		      << dbparms.db_config.database_name.c_str(dest) << "\n";
#endif
}

void CryptDBGrid::UpdateGridPosition()
// Function used to update the current grid position variables.
{
  dbparms.current_row = m_grid->GetCursorRow();
  dbparms.current_col = m_grid->GetCursorColumn();
  dbparms.current_name = m_grid->GetCellValue(dbparms.current_row, 0);
}

void CryptDBGrid::ClearCell(int row_number)
{
  if(row_number == -1) row_number = dbparms.current_row;
  for(int i = 0; i < NumDataMembers; i++) {
    m_grid->SetCellValue("", row_number, i);
  }
}

void CryptDBGrid::InsertCell(INFOHOG &infohog)
{
  int row_number = FindGridInsertPosition(infohog);
  m_grid->InsertRows(row_number);
  LoadCell(infohog, row_number);
  SetResetSkipLines(0);
  RefreshGrid();
}

int CryptDBGrid::FindGridInsertPosition(INFOHOG &infohog)
// Returns the sort-ordered row number where this
// object should be inserted into the database
// grid.
{
  long num_entries = dbparms.pod->Index()->NumKeys();
  if(num_entries == 0) return 0;
  
  char dest[DBStringLength];

  // NOTE: This is a safty feature. Using member 0 should always be safe
  // provided the call has initialized the infohog object correctly.
  if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) return 0; 

  INFOHOG_t *dbstring = (INFOHOG_t *)infohog.GetMember(0);
  INFOHOG_t object_name(dbstring->c_str(dest));
  INFOHOGKEY key(object_name); 
  INFOHOGKEY compare_key;
  gxBtree *btx = dbparms.pod->Index();

  // Couldn't find the previous position
  if(!btx->FindPrev(key, compare_key)) return 0;
  wxString prev_key(key.ObjectName().c_str(dest));
  prev_key.MakeUpper();
  wxString sbuf;

  int rows = m_grid->GetRows();
  int pos = 0; // Start at row zero
  
  // If all the row have been deleted then row zero must be used
  // for this insertion
  if(rows == 0) return 0;

  // Find the row excluding row zero
  while(rows != 0) {
    sbuf = m_grid->GetCellValue(pos, 0);
    sbuf.MakeUpper();
    pos++; // Go to the next row
    rows--;
    if(prev_key == sbuf) return pos;
  }
  return 0;
}

int CryptDBGrid::SetSkipLines(wxColour *color, int refresh_grid)
{
  if(dbparms.db_config.view_skip_lines == 0) return 0;

  int rows = m_grid->GetRows();
  int pos = 0; // Start at row zero
  
  // All row have been deleted
  if(rows == 0) return 0;

  int i;

  // Set row 0 first
  for(i = 0; i < NumDataMembers; i++) {
    m_grid->SetCellBackgroundColour(0, i, *color);     
  }
  
  // Set every other row after 0
  while(rows > 0) {
    pos += 2;
    rows -= 2;
    if(rows > 0) {
      for(i = 0; i < NumDataMembers; i++) {
	m_grid->SetCellBackgroundColour(pos, i, *color);
      }
    }
  }

  if(refresh_grid) RefreshGrid();

  return 1;
}

int CryptDBGrid::SetGridLineColor(int refresh_grid)
{
  wxColour color;

  if(!dbparms.db_config.GetGridLineColor(&color)) {
    color.Set(0, 0, 0); // Use the default color
  }

  SetGridLineColor(&color, refresh_grid);
  return 1;
}

int CryptDBGrid::SetGridLineColor(wxColour *color, int refresh_grid)
{
  m_grid->SetGridLineColour(*color);
  if(refresh_grid) RefreshGrid();
  return 1;
}

int CryptDBGrid::SetGridLabelBackgroundColor(int refresh_grid)
{
  wxColour color;

  if(!dbparms.db_config.GetGridLabelBackgroundColor(&color)) {
    color.Set(192, 192, 192); // Use the default color
  }

  SetGridLabelBackgroundColor(&color, refresh_grid);
  return 1;
}

int CryptDBGrid::SetGridLabelBackgroundColor(wxColour *color, int refresh_grid)
{
  m_grid->SetLabelBackgroundColour(*color);
  if(refresh_grid) RefreshGrid();
  return 1;
}

int CryptDBGrid::SetGridLabelTextColor(int refresh_grid)
{
  wxColour color;

  if(!dbparms.db_config.GetGridLabelTextColor(&color)) {
    color.Set(0, 0, 0); // Use the default color
  }

  SetGridLabelTextColor(&color, refresh_grid);
  return 1;
}

int CryptDBGrid::SetGridLabelTextColor(wxColour *color, int refresh_grid)
{
  m_grid->SetLabelTextColour(*color);
  if(refresh_grid) RefreshGrid();
  return 1;
}

int CryptDBGrid::SetGridLabelFont(int refresh_grid)
{
  // Set the default font
  wxFont font(10, wxSWISS, wxNORMAL, wxBOLD);

  // Check to see if the user has set a font
  dbparms.db_config.GetGridLabelFont(&font);

  SetGridLabelFont(&font, refresh_grid);
  return 1;
}

int CryptDBGrid::SetGridLabelFont(wxFont *font, int refresh_grid)
{
  m_grid->SetLabelFont(*font);
  if(refresh_grid) RefreshGrid();
  return 1;
}

int CryptDBGrid::SetGridTextColor(int refresh_grid)
{
  wxColour color;

  if(!dbparms.db_config.GetGridTextColor(&color)) {
    color.Set(0, 0, 0); // Use the default color
  }

  SetGridTextColor(&color, refresh_grid);
  return 1;
}

int CryptDBGrid::SetGridTextColor(wxColour *color, int refresh_grid)
{
  int rows = m_grid->GetRows();
  int pos = 0; // Start at row zero
  
  // All row have been deleted
  if(rows == 0) return 0;

  int i;

  // Set row 0 first
  for(i = 0; i < NumDataMembers; i++) {
    if(!m_grid->GetCellValue(0, i).IsNull()) {
      if(!IsHyperlink((const char *)m_grid->GetCellValue(0, i).c_str())) {
	m_grid->SetCellTextColour(0, i, *color);  
      }
    }
  }
  
  // Set every other row after 0
  while(rows > 0) {
    pos++;
    rows--;
    if(rows > 0) {
      for(i = 0; i < NumDataMembers; i++) {
	if(!m_grid->GetCellValue(pos, i).IsNull()) {
	  if(!IsHyperlink((const char *)m_grid->GetCellValue(pos, i).c_str())) {
	    m_grid->SetCellTextColour(pos, i, *color);
	  }
	}
      }
    }
  }

  // Reset the default cell text color
  m_grid->SetDefaultCellTextColour(*color);

  if(refresh_grid) RefreshGrid();
  return 1;
}

int CryptDBGrid::SetGridTextFont(int refresh_grid)
{
  // Set the default font
  wxFont font(10, wxSWISS, wxNORMAL, wxNORMAL);

  // Check to see if the user has set a font
  dbparms.db_config.GetGridTextFont(&font);

  SetGridTextFont(&font, refresh_grid);
  return 1;
}

int CryptDBGrid::SetGridTextFont(wxFont *font, int refresh_grid)
{
  int rows = m_grid->GetRows();
  int pos = 0; // Start at row zero
  
  // All row have been deleted
  if(rows == 0) return 0;

  int i;

  // Set row 0 first
  for(i = 0; i < NumDataMembers; i++) {
    if(!m_grid->GetCellValue(0, i).IsNull()) {
      if(!IsHyperlink((const char *)m_grid->GetCellValue(0, i).c_str())) {
	m_grid->SetCellFont(0, i, *font);
      }     
    }
  }
  
  // Set every other row after 0
  while(rows > 0) {
    pos++;
    rows--;
    if(rows > 0) {
      for(i = 0; i < NumDataMembers; i++) {
	if(!m_grid->GetCellValue(pos, i).IsNull()) {
	  if(!IsHyperlink((const char *)m_grid->GetCellValue(pos, i).c_str())) {
	    m_grid->SetCellFont(pos, i, *font);
	  }
	}
      }
    }
  }

  // Reset the default cell font
  m_grid->SetDefaultCellFont(*font);

  if(refresh_grid) RefreshGrid();
  return 1;
}

int CryptDBGrid::SetHyperlinkColor(int refresh_grid)
{
  wxColour color;

  if(!dbparms.db_config.GetHyperlinkColor(&color)) {
    color.Set(0, 0, 255); // Use the default color
  }

  SetHyperlinkColor(&color, refresh_grid);
  return 1;
}

int CryptDBGrid::SetHyperlinkColor(wxColour *color, int refresh_grid)
{
  int rows = m_grid->GetRows();
  int pos = 0; // Start at row zero
  
  // All row have been deleted
  if(rows == 0) return 0;

  int i;

  // Set row 0 first
  for(i = 0; i < NumDataMembers; i++) {
    if(!m_grid->GetCellValue(0, i).IsNull()) {
      if(IsHyperlink((const char *)m_grid->GetCellValue(0, i).c_str())) {
	m_grid->SetCellTextColour(0, i, *color);  
      }
    }
  }
  
  // Set every other row after 0
  while(rows > 0) {
    pos++;
    rows--;
    if(rows > 0) {
      for(i = 0; i < NumDataMembers; i++) {
	if(!m_grid->GetCellValue(pos, i).IsNull()) {
	  if(IsHyperlink((const char *)m_grid->GetCellValue(pos, i).c_str())) {
	    m_grid->SetCellTextColour(pos, i, *color);
	  }
	}
      }
    }
  }

  if(refresh_grid) RefreshGrid();
  return 1;
}

int CryptDBGrid::SetHyperlinkFont(int refresh_grid)
{
  // Set the default font
  wxFont font(10, wxSWISS, wxNORMAL, wxNORMAL);

  // Check to see if the user has set a font
  dbparms.db_config.GetHyperlinkFont(&font);

  SetHyperlinkFont(&font, refresh_grid);
  return 1;
}

int CryptDBGrid::SetHyperlinkFont(wxFont *font, int refresh_grid)
{
  int rows = m_grid->GetRows();
  int pos = 0; // Start at row zero
  
  // All row have been deleted
  if(rows == 0) return 0;

  int i;

  // Set row 0 first
  for(i = 0; i < NumDataMembers; i++) {
    if(!m_grid->GetCellValue(0, i).IsNull()) {
      if(IsHyperlink((const char *)m_grid->GetCellValue(0, i).c_str())) {
	m_grid->SetCellFont(0, i, *font);
      }     
    }
  }
  
  // Set every other row after 0
  while(rows > 0) {
    pos++;
    rows--;
    if(rows > 0) {
      for(i = 0; i < NumDataMembers; i++) {
	if(!m_grid->GetCellValue(pos, i).IsNull()) {
	  if(IsHyperlink((const char *)m_grid->GetCellValue(pos, i).c_str())) {
	    m_grid->SetCellFont(pos, i, *font);
	  }
	}
      }
    }
  }

  if(refresh_grid) RefreshGrid();
  return 1;
}

int CryptDBGrid::SetGridBackgroundColor(int refresh_grid)
{
  wxColour color;

  if(!dbparms.db_config.GetGridBackgroundColor(&color)) {
    color.Set(255, 255, 255); // Use the default color
  }

  SetGridBackgroundColor(&color, refresh_grid);
  return 1;
}

int CryptDBGrid::SetGridBackgroundColor(wxColour *color, int refresh_grid)
{
  int rows = m_grid->GetRows();
  int pos = 0; // Start at row zero
  
  // All row have been deleted
  if(rows == 0) return 0;

  int i;

  // Set row 0 first
  for(i = 0; i < NumDataMembers; i++) {
    m_grid->SetCellBackgroundColour(0, i, *color);     
  }
  
  // Set every other row after 0
  while(rows > 0) {
    pos++;
    rows--;
    if(rows > 0) {
      for(i = 0; i < NumDataMembers; i++) {
	m_grid->SetCellBackgroundColour(pos, i, *color);
      }
    }
  }

  if(refresh_grid) RefreshGrid();
  return 1;
}

int CryptDBGrid::SetResetSkipLines(int refresh_grid)
{
  if(dbparms.db_config.view_skip_lines == 0) return 0;
  SetGridBackgroundColor(refresh_grid);
  return SetSkipLines(refresh_grid);
}

int CryptDBGrid::SetSkipLines(int refresh_grid)
{
  if(dbparms.db_config.view_skip_lines == 0) return 0;

  wxColour color;

  if(!dbparms.db_config.GetSkipLineColor(&color)) {
    color.Set(254, 255, 206); // Use the default color
  }

  SetSkipLines(&color, refresh_grid);
  return 1;
}

int CryptDBGrid::FindGridPosition(wxString &name)
// Returns the row number of the specified object or -1
// to indicate an error condition.
{
  wxString sbuf;

  wxString key(name); 
  key.MakeUpper();
  int rows = m_grid->GetRows();
  int pos = 0; // Start at row zero
  
  // Return an error condition if all the row have been deleted
  if(rows == 0) return -1;

  // Find the row excluding row zero
  while(rows != 0) {
    sbuf = m_grid->GetCellValue(pos, 0);
    sbuf.MakeUpper();
    if(sbuf == key) return pos; // Found the object
    pos++; // Go to the next row
    rows--;
  }
  return -1; // The object was not found in the grid
}

int CryptDBGrid::FindGridPosition(INFOHOG &infohog)
// Returns the row number of the specified object or -1
// to indicate an error condition.
{
  wxString sbuf;
  char dest[DBStringLength];

  // NOTE: This is a safty feature. Using member 0 should always be safe
  // provided the call has initialized the infohog object correctly.
  if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) return -1; 

  INFOHOG_t *dbstring = (INFOHOG_t *)infohog.GetMember(0);
  wxString key(dbstring->c_str(dest)); 
  key.MakeUpper();
  int rows = m_grid->GetRows();
  int pos = 0; // Start at row zero
  
  // Return an error condition if all the row have been deleted
  if(rows == 0) return -1;

  // Find the row excluding row zero
  while(rows != 0) {
    sbuf = m_grid->GetCellValue(pos, 0);
    sbuf.MakeUpper();
    if(sbuf == key) return pos; // Found the object
    pos++; // Go to the next row
    rows--;
  }
  return -1; // The object was not found in the grid
}

void CryptDBGrid::MoveCell(INFOHOG &infohog, int row_number)
{
  if(row_number == -1) row_number = dbparms.current_row;
  m_grid->DeleteRows(row_number);
  InsertCell(infohog);

  // Update the current position variables
  dbparms.current_row = row_number;
  dbparms.current_name = m_grid->GetCellValue(dbparms.current_row, 0);
}

void CryptDBGrid::LoadCell(INFOHOG &infohog, int row_number)
{
  if(row_number == -1) row_number = dbparms.current_row;

  gxString cell_str;
  char dest[DBStringLength];
  wxFont font;
  wxColor color;

  for(int i = 0; i < NumDataMembers; i++) {
    // NOTE: Set the cell font and color here
    if(dbparms.db_config.GetGridTextFont(&font)) {
      m_grid->SetCellTextFont(font, row_number, i);
    }
    else {
      m_grid->SetCellTextFont(*frame->itemFont, row_number, i);
    }
    if(dbparms.db_config.GetGridTextColor(&color)) {
      m_grid->SetCellTextColour(row_number, i, color);
    }
    else {    
      m_grid->SetCellTextColour(row_number, i, *frame->cellFontColor);
    }

    if(infohog.GetMemberLen(i) == sizeof(INFOHOG_t)) {
      INFOHOG_t *dbstring = (INFOHOG_t *)infohog.GetMember(i);
      if(!dbstring->is_null()) {
	cell_str.Clear();
	cell_str = dbstring->c_str(dest);
	if(IsHyperlink((const char *)cell_str.c_str())) {
	  if(dbparms.db_config.GetHyperlinkColor(&color)) {
	    m_grid->SetCellTextColour(row_number, i, color);
	  }
	  else {
	    m_grid->SetCellTextColour(row_number, i, *frame->hyperlinkColor);
	  }
	  if(dbparms.db_config.GetHyperlinkFont(&font)) {
	    m_grid->SetCellTextFont(font, row_number, i);
	  }
	  else {
	    m_grid->SetCellTextFont(*frame->hyperlinkFont, row_number, i);
	  }
	}
	m_grid->SetCellValue(cell_str.c_str(), row_number, i);
	::wxYield();
      }
      else {
	// Clear the cell if the member is null
	m_grid->SetCellValue("", row_number, i);
      }
    }
    else {
      // Clear the cell if member is an invalid size
      m_grid->SetCellValue("", row_number, i);
    }
  }
}
			 
void CryptDBGrid::LoadGrid(long num_entries)
{
  if(num_entries == 0) return;
  long i = num_entries;
  
  // Ensure that the proper number of rows have been allocated
  if(i > m_grid->GetRows()) {
    m_grid->InsertRows(0, (i - m_grid->GetRows()));
  }
  
  const long display_count = num_entries/10;
  long count = 0;
  long curr_count = 0;
  wxString sbuf;
  frame->spanel->status_gauge->SetRange(num_entries);
  frame->spanel->Show(TRUE);
  
  // Load the database entries into the database grid
  frame->spanel->WriteMessage("Initializing the database engine...");
  INFOHOGKEY key, compare_key;
  gxBtree *btx = dbparms.pod->Index();
  
  // Ensure that the in memory buffers and the file data
  // stay in sync during multiple file access.
  btx->TestTree();

  // Walk through the tree starting at the first key
  unsigned row_number = 0;
  int yn;

  // Start a BeginBatch/EndBatch pair between which, calls to SetCellValue
  // will not cause a refresh. NOTE: You can nest, but not overlap, these
  // two functions.
  m_grid->BeginBatch();

#ifdef __APP_DEBUG_VERSION__
  sbuf.Printf("Loading %d entires...", (int)num_entries);
  frame->spanel->WriteMessage(sbuf.c_str());
  sbuf.Printf("Total number of index keys = %d",
	      (int)dbparms.pod->Index()->NumKeys());
  frame->spanel->WriteMessage(sbuf.c_str());
  sbuf.Printf("Total number of grid rows = %d", m_grid->GetRows());
  frame->spanel->WriteMessage(sbuf.c_str());
#else
  if(dbparms.display_status) {
    frame->spanel->WriteMessage("Loading the database entires...");
  }
#endif
  
  if(btx->FindFirst(key, 0)) {
    INFOHOG infohog(dbparms.pod, NumDataMembers);
    if(!infohog.ReadObject(key.ObjectID())) {
      ProgramError->Message("Error reading database object\n");
      if(dbparms.display_status) {
	frame->spanel->Close();
      }
      m_grid->EndBatch();
      RefreshGrid();
      return;
    }
    
    ::wxYield();

    LoadCell(infohog, row_number++);
    count++;
    dbparms.current_name = m_grid->GetCellValue(0, 0);
    frame->spanel->status_gauge->SetValue(row_number);
    if(frame->spanel->cancel_event) {
      yn = wxMessageBox("Are you sure you want to cancel this operation\n",
			"Program Message",
			wxYES_NO | wxCENTRE | wxICON_QUESTION, this);
      if(yn == wxYES) {
	frame->spanel->Close();
	m_grid->EndBatch();
	RefreshGrid();
	return;
      }
      else {
	frame->spanel->cancel_event = 0;
      }
    }
    while(btx->FindNext(key, compare_key, 0)) {
      INFOHOG infohog(dbparms.pod, NumDataMembers);
      if(!infohog.ReadObject(key.ObjectID())) {
      	ProgramError->Message("Error reading database object\n");
	frame->spanel->Close();
	m_grid->EndBatch();
	RefreshGrid();
      	return;
      }
      ::wxYield();

      LoadCell(infohog, row_number++);
      frame->spanel->status_gauge->SetValue(row_number);
      count++;
      if(count >= display_count) {
	curr_count += count;
	sbuf.Printf("Loaded %d out of a %d objects", (int)curr_count, (int)num_entries);
	frame->spanel->WriteMessage(sbuf.c_str());
	count = 0;
      }
      if(frame->spanel->cancel_event) {
	yn = wxMessageBox("Are you sure you want to cancel this operation\n",
			  "Program Message",
			  wxYES_NO | wxCENTRE | wxICON_QUESTION, this);
	if(yn == wxYES) {
	  frame->spanel->Close();
	  m_grid->EndBatch();
	  RefreshGrid();
	  return;
	}
	else {
	  frame->spanel->cancel_event = 0;
	}
      }
    }
  }
  m_grid->EndBatch();

  if(dbparms.db_config.auto_size == 1) {
    m_grid->AutoSizeColumns();
    m_grid->AutoSizeRows();
    // Reset the DB config col values without saving
    for(i = 0; i < NumDataMembers; i++) {
      int col_width = m_grid->GetColumnWidth(i);
      dbparms.db_config.col_sizes[i] = col_width;
    }
  }

  RefreshGrid();

  dbparms.current_row = 0;
  dbparms.current_col = 0;
  dbparms.current_name = m_grid->GetCellValue(dbparms.current_row, 0);

  wxSleep(1); // Allow the user to view the results
  frame->spanel->WriteMessage("Database initialization complete");
  frame->spanel->WriteMessage("Closing event monitor...");

#ifndef __APP_DEBUG_VERSION__ // Debug apps will not close the status window
    frame->spanel->Close();
#endif
}

void CryptDBGrid::ClearGrid()
{
  m_grid->ClearGrid(); // Reset the column labels
  RefreshGrid();
}

void CryptDBGrid::RefreshGrid()
{
  if(m_grid) {
    // Force the grid to redraw itself correctly
    m_grid->BeginBatch();
    m_grid->EndBatch();
    m_grid->Show(TRUE);
  }
}

void CryptDBGrid::ReloadGrid(int test_database)
{
  m_grid->ClearGrid(); // Reset the column labels
  RefreshGrid();
  if(test_database) {
    if(!TestDatabase(0, 1, 0)) return;
  }
  long num_entries = dbparms.pod->Index()->NumKeys();
  if(num_entries == 0) return;
  LoadGrid(num_entries);
}

// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
