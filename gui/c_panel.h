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

Change panel
*/
// ----------------------------------------------------------- // 
#ifndef __C_PANEL_HPP__
#define __C_PANEL_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define CHANGE_PANEL_ID  2110
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Change panel IDs 
  ID_CHANGE_ACCEPT = 2111,
  ID_CHANGE_CANCEL,
  ID_CHANGE_CLOSE,
  ID_CHANGE_REVERT
};
// --------------------------------------------------------------

struct ChangePanelConfig
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

class ChangePanel : public wxDialog
{
public:
  ChangePanel(wxWindow *parent, wxWindowID id, char *title,
	      int xpos, int ypos, int width, int height,
	      long style = wxDEFAULT_DIALOG_STYLE,
	      char* name = (char *)"dialogBox");
  ~ChangePanel();
  
public: // Event handlers
  void OnAccept(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnClose(wxCommandEvent &event);
  void OnRevert(wxCommandEvent &event);

public: // Helper functions
  void LoadPanel(INFOHOG &ob);
  void ClearPanel();
  void ShowPanel();

private: // Internal processing functions
  void LoadCurrObject();
  void CommitChanges();
  
public:
  wxTextCtrl *db_input[MaxPanelMembers];          // Text input array
  wxStaticText *db_input_labels[MaxPanelMembers]; // Label array
  wxStaticBox *key_name_label; // Key data member label
  wxButton *accept_btn;        // Accept button
  wxButton *close_btn;         // Close button
  wxButton *cancel_btn;        // Cancel button
  wxButton *revert_btn;        // Revert button

public:
  INFOHOG curr_object; // Current object being changed
  INFOHOG prev_object; // Copy of the object before it was changed

  int num_entries;
  int count;
  int change_labels;
  INFOHOG *infohog_a[NumPanelEntries]; // Array of previously inserted objects

  CryptDBDocument *child_frame;

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
ChangePanel *InitChangePanel(wxWindow *parent);

#endif // __C_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
