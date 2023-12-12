// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 1999-2023 DataReel Software Development
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

Add panel
*/
// ----------------------------------------------------------- // 
#ifndef __A_PANEL_HPP__
#define __A_PANEL_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define ADD_PANEL_ID 283
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Add panel IDs 
  ID_ADD_ACCEPT = 284,
  ID_ADD_CANCEL,
  ID_ADD_CLOSE,
  ID_ADD_REVERT,
};
// --------------------------------------------------------------

struct AddPanelConfig
{
  // Height and width constants for panel objects
  static int key_label_height;
  static int key_label_width;
  static int button_height;
  static int button_width;
  static int text_input_height;
  static int text_input_width;
  static int label_height;
  static int label_width;
  static int x_offset;
  static int y_offset_label;
  static int y_offset_textbox;
  static int x_offset_button;
  static int y_offset_button;

  // Panel height and postion
  static int xpos;
  static int ypos;
  static int width;
};

class AddPanel : public wxDialog
{
public:
  AddPanel(wxWindow *parent, wxWindowID id, char *title,
	   int xpos, int ypos, int width, int height,
	   long style = wxDEFAULT_DIALOG_STYLE,
	   char* name = (char *)"dialogBox");
  ~AddPanel();
  
public: // Event handlers
  void OnAccept(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnClose(wxCommandEvent &event);
  void OnRevert(wxCommandEvent &event);
  
public: // Helper functions
  void ClearPanel();
  void LoadPanel(INFOHOG &ob);
  void ShowPanel();

public:
  wxTextCtrl *db_input[MaxPanelMembers];          // Text input array
  wxStaticText *db_input_labels[MaxPanelMembers]; // Label array
  wxStaticBox *key_name_label; // Key data member label
  wxButton *accept_btn;        // Accept button
  wxButton *close_btn;         // Close button
  wxButton *cancel_btn;        // Cancel button
  wxButton *revert_btn;        // Revert button

  int num_entries;
  int count;
  int change_labels;
  INFOHOG *infohog_a[NumPanelEntries]; // Array of previously inserted objects

  CryptDBDocument *child_frame;

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
AddPanel *InitAddPanel(wxWindow *parent);

#endif // __A_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
