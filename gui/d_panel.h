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

Display panel
*/
// ----------------------------------------------------------- // 
#ifndef __D_PANEL_HPP__
#define __D_PANEL_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define DISPLAY_PANEL_ID 2831
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Display panel IDs 
  ID_DISPLAY_NEXT = 2832,
  ID_DISPLAY_PREV,
  ID_DISPLAY_CHANGE,
  ID_DISPLAY_REMOVE,
  ID_DISPLAY_CLOSE,
  ID_DISPLAY_PRINT,
  ID_DISPLAY_EXPORT
};
// --------------------------------------------------------------

struct DisplayPanelConfig
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
  static int x_start_pos;
  static int y_start_pos;
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

class DisplayPanel : public wxDialog
{
public:
  DisplayPanel(wxWindow *parent, wxWindowID id, char *title,
	   int xpos, int ypos, int width, int height,
	   long style = wxDEFAULT_DIALOG_STYLE,
	       char* name = (char *)"dialogBox");
  ~DisplayPanel();
  
public: // Event handlers
  void OnNext(wxCommandEvent &event);
  void OnPrev(wxCommandEvent &event);
  void OnClose(wxCommandEvent &event);
  void OnChange(wxCommandEvent &event);
  void OnRemove(wxCommandEvent &event);
  void OnExport(wxCommandEvent &event);

#ifdef __USE_MSW_PRINTING__  
  void OnPrint(wxCommandEvent &event);
#endif
  
public: // Helper functions
  void LoadPanel(INFOHOG &ob);
  void ClearPanel();
  void ShowPanel();
  void ClosePanel();
  int PanelIsOpen() const { return panel_is_open == 1; }
  int CheckForDeletedBlock(int &is_deleted);  
  int ReReadObject(INFOHOG &infohog);

public:
  wxTextCtrl *db_input[MaxPanelMembers];          // Text input array
  wxStaticText *db_input_labels[MaxPanelMembers]; // Label array
  wxStaticBox *key_name_label; // Key data member label
  wxButton *next_btn;   // Next button
  wxButton *prev_btn;   // Previous button
  wxButton *close_btn;  // Close button
  wxButton *change_btn; // Change button
  wxButton *remove_btn; // Revert button
  wxButton *export_btn; // Export button

  int panel_is_open;    // True if this panel is open
  int change_labels;

  CryptDBDocument *child_frame;

#ifdef __USE_MSW_PRINTING__
  wxButton *print_btn;  // Print button
#endif

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
DisplayPanel *InitDisplayPanel(wxWindow *parent);

#endif // __D_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
