// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
// Copyright (c) 2001-2025 DataReel Software Development
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

Document controls
*/
// ----------------------------------------------------------- // 
#ifndef __M_DOCUMENT_HPP__
#define __M_DOCUMENT_HPP__

#include "app_defs.h"
#include "epdb_core.h"

class CryptDBGrid;
class DetailsPane;

class CryptDBDocument : public wxMDIChildFrame
{
public:
  CryptDBDocument(wxMDIParentFrame *parent, const wxString& title, 
		  const wxPoint& pos, const wxSize& size, const long style);
  ~CryptDBDocument();

public: // Event handlers
  void OnActivate(wxActivateEvent& event);
  void OnRefresh(wxCommandEvent& event);
  void OnUpdateRefresh(wxUpdateUIEvent& event);
  void OnChangePosition(wxCommandEvent& event);
  void OnChangeSize(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMove(wxMoveEvent& event);
  void OnClose(wxCloseEvent& event);
  
  // Grid event handlers
  void OnCellValueChanged(wxGridEvent& event);
  void OnCellLeftClick(wxGridEvent& event);
  void OnCellRightClick(wxGridEvent& event);
  void OnCellLeftDClick(wxGridEvent& event);
  void OnCellRightDClick(wxGridEvent& event);
  void OnLabelLeftClick(wxGridEvent& event);
  void OnLabelRightClick(wxGridEvent& event);
  void OnSelectCell(wxGridEvent& event);
  void OnRangeSelected(wxGridRangeSelectEvent& event);
  void OnRowSize(wxGridSizeEvent& event);
  void OnColSize(wxGridSizeEvent& event);
  void OnEditorShown(wxGridEvent&);
  void OnEditorHidden(wxGridEvent&);

public: // Public Member functions
  POD *GetPOD();
  CryptDBGrid *GridFrame();
  gxDatabaseParms *DBParms();
#ifdef __USE_MSW_PRINTING__
  wxPrintData print_data; // 10/27/2026: The printer, paper and orientation used last
#endif
  wxGrid *GetGrid();
  int LoadGridObject(INFOHOG &infohog);

private: // Internal processing functions
  int change_cell_value(wxGridEvent& event);

public: // 10/27/2026: Records grid left, record details right
  void AttachGrid(wxGrid *grid);
  void ShowDetails(int show);
  int CurrentRecordRow(); // The grid row of the selected record, -1 for none
  void SaveSplitterPosition();
  void ApplySearch();
  void OnSearch(wxCommandEvent& event);
  void OnSearchCancel(wxCommandEvent& event);
  wxSearchCtrl *search_ctrl;  // Filters the records
  wxChoice *search_field;     // All fields or one field
  wxStaticText *search_count;
  wxSplitterWindow *splitter;
  DetailsPane *details;
  wxString unlock_description; // How the database was unlocked, for the status bar

public:
  CryptDBGrid *grid_frame;
  EPDBUndoStack undo_stack; // 10/27/2026: In memory only, wiped when the document closes

private:
  DECLARE_EVENT_TABLE()
};

#endif // __M_DOC_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
