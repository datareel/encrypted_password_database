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

Page setup panel
*/
// ----------------------------------------------------------- // 
#ifndef __MSW_PAGE_PANEL_HPP__
#define __MSW_PAGE_PANEL_HPP__

#include "app_defs.h"

#ifdef __USE_MSW_PRINTING__

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define MSWPAGE_TAB_PANEL_ID 9800
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Main panel IDs 
  ID_MSWPAGEPANEL_ACCEPT = 9801,
  ID_MSWPAGEPANEL_CLOSE,
  ID_MSWPAGEPANEL_CANCEL,
  ID_MSWPAGEPANEL_DEFAULT,
  ID_MSWPAGEPANEL_NOTEBOOK,
  ID_MSWPAGEPANEL_HEADER_FONT,
  ID_MSWPAGEPANEL_HEADER_COLOR,
  ID_MSWPAGEPANEL_FOOTER_FONT,
  ID_MSWPAGEPANEL_FOOTER_COLOR

};
// --------------------------------------------------------------

class MSWPagePanel: public wxDialog
{
public:
  MSWPagePanel(wxWindow *parent, wxWindowID id, char *title,
	       int xpos, int ypos, int width, int height,
	       long style = wxDEFAULT_DIALOG_STYLE,
	       char* name = (char *)"dialogBox");
  ~MSWPagePanel();

public: // Event handlers
  void OnCloseWindow(wxCloseEvent& WXUNUSED(event));
  void OnAccept(wxCommandEvent &event);
  void OnClose(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnDefault(wxCommandEvent &event);
  void OnHeaderFont(wxCommandEvent &event);
  void OnHeaderColor(wxCommandEvent &event);
  void OnFooterFont(wxCommandEvent &event);
  void OnFooterColor(wxCommandEvent &event);

public: // Helper functions
  void LoadPanel();
  void ShowPanel();
  void ClosePanel();

private: // Internal processing functions
  int CommitChanges();
  int HasChanged();
  void Init();


private:
  int change_labels;
  int start_x;
  int start_y;
  int y_offset_checkbox;
  int label_height;
  int label_width;
  int has_new_header_font;
  wxFont header_font;
  int has_new_header_color;
  wxColour header_color;
  int has_new_footer_font;
  wxFont footer_font;
  int has_new_footer_color;
  wxColour footer_color;

public:
  // Tab member functions
  wxNotebook* notebook;

  // Panel conrol buttons
  wxButton *accept_btn;
  wxButton *close_btn;
  wxButton *cancel_btn;
  wxButton *default_btn;

  // Tab panels
  wxPanel *panel1;
  wxPanel *panel2;
  wxPanel *panel3;
  wxPanel *panel4;

  // Panel objects
  wxStaticText *fs_label;
  wxStaticText *lm_label;
  wxStaticText *orientation_label;
  wxStaticText *font_label;
  wxStaticText *paper_label;
  wxStaticText *doc_name_label;
  wxSlider *lm_slider;
  wxChoice *orientation_choice;
  wxChoice *paper_choice;
  wxTextCtrl *doc_name_input;

  // Extended print options
  wxCheckBox *grid_lines_ckbox;
  wxCheckBox *time_date_ckbox;
  wxCheckBox *print_header_ckbox;
  wxCheckBox *print_footer_ckbox;
  wxCheckBox *grid_labels_ckbox;
  wxCheckBox *row_numbers_ckbox;
  wxCheckBox *include_fields[NumDataMembers];
  wxStaticText *header_name_label;
  wxStaticText *footer_name_label;
  wxTextCtrl *header_name_input;
  wxTextCtrl *footer_name_input;
  wxButton *header_font_btn;
  wxButton *header_color_btn;
  wxButton *footer_font_btn;
  wxButton *footer_color_btn;

  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
MSWPagePanel *InitMSWPagePanel(wxWindow *parent);

#endif // __USE_MSW_PRINTING__

#endif //__MSW_PAGE_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
