// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File Name: epdb_art.h
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 10/27/2026
// Date Last Modified: 10/27/2026
// Copyright (c) 2001-2026 DataReel Software Development
// ----------------------------------------------------------- // 
// ---------- Include File Description and Details  ---------- // 
// ----------------------------------------------------------- // 
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
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

10/27/2026: Icons, colors and DPI helpers. The icons are SVGs built
into the program (epdb_icons.h, made by gui/tools/make_icons.py) and
drawn at any DPI with wxBitmapBundle::FromSVG.
*/
// ----------------------------------------------------------- //   
#ifndef __EPDB_ART_HPP__
#define __EPDB_ART_HPP__

#include "app_defs.h"

const int EPDB_TOOLBAR_ICON_DIP = 24;
const int EPDB_MENU_ICON_DIP = 16;

// Toolbar size by default, 16 DIP for menus and buttons
wxBitmapBundle EPDBIcon(const char *name, int size_dip = EPDB_TOOLBAR_ICON_DIP);
// The application icon at 16 to 256 pixels for frames and dialogs
wxIconBundle EPDBAppIcons();
wxBitmapBundle EPDBAppIconBundle(int size_dip = 64);

int EPDBIsDark();
wxColour EPDBAccentColour();    // #1F5FBF
wxColour EPDBMutedTextColour(); // Secondary text, readable in light and dark
wxColour EPDBWarningColour();
wxColour EPDBOkColour();

// A label in the system GUI font, bold or a step larger for headings
wxStaticText *EPDBHeading(wxWindow *parent, const wxString &text, int larger = 1);
// 10/27/2026: Keeps a top level window inside the work area of its
// display: moved fully on screen, or maximized (maximize = 1) or shrunk
// when it is larger than the display
void EPDBFitOnScreen(wxTopLevelWindow *w, int maximize);
wxStaticText *EPDBNote(wxWindow *parent, const wxString &text, int wrap_dip = 0);
// Monospace font the size of the system GUI font
wxFont EPDBMonoFont();

#endif // __EPDB_ART_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
