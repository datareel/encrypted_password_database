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

Main frame toolbar functions.
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

void MainFrame::RecreateToolbar()
{
  // delete and recreate the toolbar
  wxToolBarBase *toolBar = GetToolBar();

  long style = toolBar ? toolBar->GetWindowStyle() : MAIN_TOOLBAR_STYLE;

  delete toolBar;
  
  SetToolBar(NULL);
  
  style &= ~(wxTB_HORIZONTAL | wxTB_VERTICAL);
  style |= m_horzToolbar ? wxTB_HORIZONTAL : wxTB_VERTICAL;
  
  toolBar = CreateToolBar(style, MAIN_TOOLBAR_ID);
  
  // Set up toolbar
  wxBitmap toolBarBitmaps[7];
  
#if USE_XPM_BITMAPS
  toolBarBitmaps[0] = wxBitmap(new_xpm);
  toolBarBitmaps[1] = wxBitmap(open_xpm);
  toolBarBitmaps[2] = wxBitmap(addrec_xpm);
  toolBarBitmaps[3] = wxBitmap(delrec_xpm);
  toolBarBitmaps[4] = wxBitmap(findrec_xpm);
  toolBarBitmaps[5] = wxBitmap(print_xpm);
  toolBarBitmaps[6] = wxBitmap(help_xpm);
#else // !USE_XPM_BITMAPS
  toolBarBitmaps[0] = wxBITMAP(new);
  toolBarBitmaps[1] = wxBITMAP(open);
  toolBarBitmaps[2] = wxBITMAP(addrec);
  toolBarBitmaps[3] = wxBITMAP(delrec);
  toolBarBitmaps[4] = wxBITMAP(findrec);
  toolBarBitmaps[5] = wxBITMAP(print);
  toolBarBitmaps[6] = wxBITMAP(help);
#endif // USE_XPM_BITMAPS/!USE_XPM_BITMAPS

  if(!m_smallToolbar ) {
    // int w = 2*toolBarBitmaps[0].GetWidth();
    // int h = 2*toolBarBitmaps[0].GetHeight();
    // NOTE: Use standard toolbar bitmap size for OS
    int w = 2*16;
    int h = 2*16;
    for ( size_t n = 0; n < WXSIZEOF(toolBarBitmaps); n++ ) {
      toolBarBitmaps[n] =
	wxBitmap(toolBarBitmaps[n].ConvertToImage().Scale(w, h));
    }
    toolBar->SetToolBitmapSize(wxSize(w, h));
  }

  toolBar->AddTool(WXAPPFW_FILE_NEW, "New", 
		   toolBarBitmaps[0], "Create new database");
  toolBar->AddTool(WXAPPFW_FILE_OPEN, "Open", 
		   toolBarBitmaps[1], "Open existing database");

  toolBar->AddSeparator();
  toolBar->AddTool(WXAPPFW_DATABASE_ADD, "Add", 
		   toolBarBitmaps[2], "Add an entry");
  toolBar->AddTool(WXAPPFW_DATABASE_DELETE, "Delete", 
		   toolBarBitmaps[3], "Delete selected object");
  toolBar->AddTool(WXAPPFW_DATABASE_FIND, "Find", 
		   toolBarBitmaps[4], "Find an object in the database");

  toolBar->AddSeparator();
  toolBar->AddTool(WXAPPFW_PRINT_MSWPRINT, "Print", 
		   toolBarBitmaps[5], "Print current database");

  toolBar->AddSeparator();
  toolBar->AddTool(WXAPPFW_HELP_ABOUT, "Help", 
		   toolBarBitmaps[6], "Help");
  
  // After adding the buttons to the toolbar, must call Realize() to reflect
  // the changes.
  toolBar->Realize();
  
  toolBar->SetRows(m_horzToolbar ? m_rows : 10 / m_rows);
}

// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
