// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 2001-2024 DataReel Software Development
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

Application grid code 
*/
// ----------------------------------------------------------- // 
#ifndef __M_GRID_HPP__
#define __M_GRID_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
const int GRID_FRAME_ID = 2571;
// --------------------------------------------------------------

class CryptDBGrid : public wxFrame
{
public:
  CryptDBGrid(wxWindow *parent, const wxPoint& pos, const wxSize& size);

public: // Event handlers

public: // Base clase interface

public: // Grid functions
  long InitGrid();
  void MakeGridColLabels();
  void UpdateGridPosition();
  void InsertCell(INFOHOG &infohog);
  void ClearCell(int row_number = -1);
  void LoadCell(INFOHOG &infohog, int row_number = -1);
  void MoveCell(INFOHOG &infohog, int row_number = -1);
  int FindGridInsertPosition(INFOHOG &infohog);
  int FindGridPosition(INFOHOG &infohog);
  int FindGridPosition(wxString &name);
  void LoadGrid(long num_entries);
  void ReloadGrid(int test_database = 1);
  void ClearGrid();
  void RefreshGrid();
  int SetGridBackgroundColor(int refresh_grid = 1);
  int SetGridBackgroundColor(wxColour *color, int refresh_grid = 1);
  int SetGridTextColor(int refresh_grid = 1);
  int SetGridTextColor(wxColour *color, int refresh_grid = 1);
  int SetSkipLines(int refresh_grid = 1);
  int SetSkipLines(wxColour *color, int refresh_grid = 1);
  int SetResetSkipLines(int refresh_grid = 1);
  int SetGridLabelBackgroundColor(int refresh_grid = 1);
  int SetGridLabelBackgroundColor(wxColour *color, int refresh_grid = 1);
  int SetGridLabelTextColor(int refresh_grid = 1);
  int SetGridLabelTextColor(wxColour *color, int refresh_grid = 1);
  int SetHyperlinkColor(int refresh_grid = 1);
  int SetHyperlinkColor(wxColour *color, int refresh_grid = 1);
  int SetGridLabelFont(int refresh_grid = 1);
  int SetGridLabelFont(wxFont *font, int refresh_grid = 1);
  int SetGridTextFont(int refresh_grid = 1);
  int SetGridTextFont(wxFont *font, int refresh_grid = 1);
  int SetHyperlinkFont(int refresh_grid = 1);
  int SetHyperlinkFont(wxFont *font, int refresh_grid = 1);
  int SetGridLineColor(int refresh_grid = 1);
  int SetGridLineColor(wxColour *color, int refresh_grid = 1);

public:
  bool IsDirty() const { return m_dirty; }

public:
  wxGrid *m_grid;
  wxWindow *parent_frame;
  wxPoint grid_pos;
  wxSize grid_size; 
  gxDatabaseParms dbparms;

private:
    bool m_dirty;

private:
  DECLARE_EVENT_TABLE()
};

#endif // __M_GRID_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
