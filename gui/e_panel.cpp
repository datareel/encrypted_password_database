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

Edit panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(EditTabPanel, wxDialog)
  EVT_CLOSE(EditTabPanel::OnCloseWindow)
  EVT_BUTTON (ID_EDITTABPANEL_ACCEPT, EditTabPanel::OnAccept)
  EVT_BUTTON (ID_EDITTABPANEL_CLOSE, EditTabPanel::OnClose)
  EVT_BUTTON (ID_EDITTABPANEL_CANCEL, EditTabPanel::OnCancel)
  EVT_BUTTON (ID_EDITTABPANEL_DEFAULT, EditTabPanel::OnDefault)
  EVT_BUTTON (ID_EDITTABPANEL_DIRECTORY, EditTabPanel::OnBrowseDirectory)
END_EVENT_TABLE()

EditTabPanel::EditTabPanel(wxWindow *parent, wxWindowID id, char *title,
			   int xpos, int ypos, int width, int height,
			   long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
{

  directory_input = 0;
  directory_label = 0;
  directory_btn = accept_btn = close_btn = cancel_btn = default_btn = 0;
  notebook = 0;
  panel1 = 0;
  panel2 = 0;
  ekey_label = 0;
  mode_radio_box = 0;

  Init();
}

EditTabPanel::~EditTabPanel()
{
  // NOTE: All objects must be deleted in the order below.

  // Delete the panel objects
  if(directory_label) delete directory_label;
  if(directory_input) delete directory_input;
  if(directory_btn) delete directory_btn;
  if(ekey_label) delete ekey_label;
  if(mode_radio_box) delete mode_radio_box;

  // NOTE: Do not delete notebook panels here. The panels are 
  // deleted by the notebook destructor

  // Delete the notebook
  if(notebook) delete notebook;

  // Delete the dialog objects
  if(accept_btn) delete accept_btn;
  if(close_btn) delete close_btn;
  if(cancel_btn) delete cancel_btn;
  if(default_btn) delete default_btn;
}

void EditTabPanel::OnAccept(wxCommandEvent &event)
{
  if(!HasChanged()) {
    wxMessageBox("There are no changes to commit\n" ,
		 "Program Message", wxOK|wxCENTRE, this);
    
    return;
  } 
  if(CommitChanges()) Show(FALSE);
}

int EditTabPanel::HasChanged()
{
  // Get the current panel values
  gxString docDir((const char *)directory_input->GetValue().c_str());


  if(progcfg->docDir != docDir) {
    return 1; // A panel value has been modified
  }

  int mode = (mode_radio_box->GetSelection()+1);
  int curr_mode = (int)DBStringConfig::mode;

  if(mode != curr_mode) {
    return 1; // A panel value has been modified
  }

  return 0; 
}

void EditTabPanel::ClosePanel() 
{
  if(HasChanged()) {
    int yn = wxMessageBox("The configuration has been modifed!\nAccept these \
changes?\n",
			  "Program Message",
			  wxYES_NO | wxCENTRE | wxICON_QUESTION, this);
    if(yn == wxYES) CommitChanges();
  }
  Show(FALSE);
}

void EditTabPanel::OnClose(wxCommandEvent &event)
{
  ClosePanel();
}

void EditTabPanel::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
// Define the behaviour for the frame closing. This event handler
// is called when the user has tried to close a a frame or dialog
// box using the window manager (X) or system menu (Windows). Must
// delete all frames except for the main one. 
{
  ClosePanel();
}

void EditTabPanel::OnCancel(wxCommandEvent &event)
{
  LoadPanel();
  Show(FALSE);
}

void EditTabPanel::OnDefault(wxCommandEvent &event)
{
  int yn = wxMessageBox("This will reset all current configuration changes!\
\nContinue?",
			"Program Message",
			wxYES_NO|wxCENTRE|wxICON_EXCLAMATION, this);
  if(yn == wxNO) return;

  directory_input->SetValue(progcfg->docDir.c_str());

  int mode = (int)DBStringConfig::mode-1;
  mode_radio_box->SetSelection(mode);
}

void EditTabPanel::OnBrowseDirectory(wxCommandEvent &event)
{
  // Save the current directory information
  wxString current_directory;
  current_directory = (const char *)::wxGetCwd().c_str();

  wxDirDialog dialog(this, "Selecting a folder location for database files", 
		     progcfg->docDir.c_str());
  
  if (dialog.ShowModal() == wxID_OK) {
    directory_input->SetValue(dialog.GetPath());
  }

  // Restore the current directory
  ::wxSetWorkingDirectory(current_directory);
}

void EditTabPanel::LoadPanel()
{
  directory_input->Clear();
  directory_input->SetValue(progcfg->docDir.c_str());

  int mode = (int)DBStringConfig::mode-1;
  mode_radio_box->SetSelection(mode);
}

void EditTabPanel::ShowPanel()
{
  LoadPanel();

  ShowModal();

  // TODO/NOTE: Must implement in all wxWin 2.6.X code
  // 02/07/2010: Not working on Linux version, so convert to ShowModal() call
  // 02/07/2010: Need to retest in Windows version
  // if wxCHECK_VERSION(2, 6, 0)
  // ShowModal();
  // #else
  // Show(TRUE);
  // #endif
}

int EditTabPanel::CommitChanges()
{
  gxConfig CfgData(progcfg->cfgFile.c_str());

  gxString docDir((const char *)directory_input->GetValue().c_str());
  int mode = (mode_radio_box->GetSelection()+1);

  // Write any changes to the config file
  if(docDir != progcfg->docDir) {
    FixHomeDirID(docDir);
    if(!::wxDirExists(docDir.c_str())) {
      if(futils_mkdir(docDir.c_str()) != 0) {
	ProgramError->Message("Cannot make new document directory");
	return 0;
      }
    }
  
    // Set the new names for the doc directory and database files
    progcfg->docDir = docDir;
    CfgData.ChangeConfigValue("DocumentDirectory", 
			      (const char *)docDir.c_str());
  }

  if((char)mode != DBStringConfig::mode) {
    DBStringConfig::mode = mode;
    CfgData.ChangeConfigValue("EncryptionMode", mode);
  }

  return 1;
}

void EditTabPanel::Init()
{
  notebook = new wxNotebook(this, ID_EDITTABPANEL_NOTEBOOK);
  wxLayoutConstraints* layout_const = new wxLayoutConstraints;
  layout_const->left.SameAs(this, wxLeft, 4);
  layout_const->right.SameAs(this, wxRight, 4);
  layout_const->top.SameAs(this, wxTop, 4);
  layout_const->bottom.SameAs(this, wxBottom, 40);
  notebook->SetConstraints(layout_const);

  accept_btn = new wxButton(this, ID_EDITTABPANEL_ACCEPT, 
			    "Accept", wxPoint(-1, -1), wxSize(80, 25));
  close_btn = new wxButton(this, ID_EDITTABPANEL_CLOSE, 
			   "Close", wxPoint(-1, -1), wxSize(80, 25));
  cancel_btn = new wxButton(this, ID_EDITTABPANEL_CANCEL, 
			    "Cancel", wxPoint(-1, -1), wxSize(80, 25));
  default_btn = new wxButton(this, ID_EDITTABPANEL_DEFAULT, 
			     "Default", wxPoint(-1, -1), wxSize(80, 25));

  layout_const = new wxLayoutConstraints;
  layout_const->right.SameAs(this, wxRight, 4);
  layout_const->bottom.SameAs(this, wxBottom, 4);
  layout_const->height.AsIs();
  layout_const->width.AsIs();
  default_btn->SetConstraints(layout_const);

  layout_const = new wxLayoutConstraints;
  layout_const->right.SameAs(default_btn, wxLeft, 4);
  layout_const->bottom.SameAs(this, wxBottom, 4);
  layout_const->height.AsIs();
  layout_const->width.AsIs();
  cancel_btn->SetConstraints(layout_const);

  layout_const = new wxLayoutConstraints;
  layout_const->right.SameAs(cancel_btn, wxLeft, 4);
  layout_const->bottom.SameAs(this, wxBottom, 4);
  layout_const->height.AsIs();
  layout_const->width.AsIs();
  close_btn->SetConstraints(layout_const);

  layout_const = new wxLayoutConstraints;
  layout_const->right.SameAs(close_btn, wxLeft, 4);
  layout_const->bottom.SameAs(this, wxBottom, 4);
  layout_const->height.AsIs();
  layout_const->width.AsIs();
  accept_btn->SetConstraints(layout_const);

  // Add panels to the tabbed dialog
  panel1 = new wxPanel(notebook, -1);

  directory_label = new wxStaticText(panel1, -1,
				     "Default Documents Folder",
				     wxPoint(25, 24));

  directory_input = new wxTextCtrl(panel1, -1,
				   progcfg->docDir.c_str(),
				   wxPoint(25, 50),
				   wxSize(255, 25));
  
  directory_btn = new wxButton(panel1, ID_EDITTABPANEL_DIRECTORY, 
			       "Browse",
			       wxPoint(292, 49),
			       wxSize(65, 25));

  notebook->AddPage(panel1, "File Locations", TRUE);
  

  panel2 = new wxPanel(notebook, -1);

  static const wxString choices[3] = {
    "128-bit Encryption",
    "192-bit Encryption",
    "256-bit Encryption"
  };

#if defined (__LINUX__)
  mode_radio_box = new wxRadioBox(panel2, -1,
				  "Encryption Strength",
				  wxPoint(20, 13),
				  wxSize(190, 109), 3, choices, 
				  1, wxRA_SPECIFY_COLS);
#else
  mode_radio_box = new wxRadioBox(panel2, -1,
				  "Encryption Strength",
				  wxPoint(20, 13),
				  wxSize(195, 85), 3, choices, 
				  0, wxRA_SPECIFY_ROWS);
#endif

  notebook->AddPage(panel2, "Security");

  notebook->SetSelection(0);
  SetAutoLayout(TRUE);
  Layout();
  Centre(wxBOTH);
}

EditTabPanel *InitEditTabPanel(wxWindow *parent)
{
  int xpos=0; int ypos=0; int width=465; int height=390;

  EditTabPanel *panel = new EditTabPanel(parent, EDIT_TAB_PANEL_ID,
					 (char *)"Program Config Panel",
					 xpos, ypos, width, height,
		  wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
					 (char *)"Program Config Panel");

  if(!panel) return 0;

  panel->Show(FALSE);
  return panel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
