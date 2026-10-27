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

Main frame toolbar functions.
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

void MainFrame::RecreateToolbar()
// 10/27/2026: SVG icons at 24 DIP with text. The search box is at the
// top of each database tab.
{
  wxToolBarBase *toolBar = GetToolBar();
  long style = toolBar ? toolBar->GetWindowStyle() : MAIN_TOOLBAR_STYLE;
  delete toolBar;
  SetToolBar(NULL);

  style &= ~(wxTB_HORIZONTAL | wxTB_VERTICAL);
  style |= wxTB_HORIZONTAL;
  toolBar = CreateToolBar(style, MAIN_TOOLBAR_ID);

  toolBar->AddTool(WXAPPFW_FILE_NEW, "New", EPDBIcon("new_db"), "Create a new database");
  toolBar->AddTool(WXAPPFW_FILE_OPEN, "Open", EPDBIcon("open"), "Unlock and open an existing database");
  toolBar->AddTool(WXAPPFW_FILE_LOCK, "Lock", EPDBIcon("lock"), "Close every open database (Ctrl+Shift+L)");
  toolBar->AddSeparator();
  toolBar->AddTool(WXAPPFW_DATABASE_ADD, "Add", EPDBIcon("add_record"), "Add a record");
  toolBar->AddTool(WXAPPFW_DATABASE_CHANGE, "Edit", EPDBIcon("edit"), "Edit the selected record");
  toolBar->AddTool(WXAPPFW_DATABASE_DELETE, "Delete", EPDBIcon("delete"), "Delete the selected record");
  toolBar->AddSeparator();
  toolBar->AddTool(WXAPPFW_RECORD_COPY_USER, "Copy user", EPDBIcon("copy_user"), "Copy the username");
  toolBar->AddTool(WXAPPFW_RECORD_COPY_PASSWORD, "Copy password", EPDBIcon("password"),
		   "Copy the password, it is cleared from the clipboard after a while");
  toolBar->AddTool(WXAPPFW_RECORD_OPEN_LINK, "Open link", EPDBIcon("open_link"), "Open the website link");
  toolBar->AddSeparator();
  toolBar->AddTool(WXAPPFW_PRINT_MSWPRINT, "Print", EPDBIcon("print"), "Print the current database");
  toolBar->Realize();
}

// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
