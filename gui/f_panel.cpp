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

Find panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(FindPanel, wxDialog)
  EVT_BUTTON    (ID_FIND_SEARCH, FindPanel::OnSearch)
  EVT_BUTTON    (ID_FIND_CLOSE, FindPanel::OnClose)
  EVT_TEXT_ENTER(ID_FIND_TEXTCONTROL1, FindPanel::OnTextControl1Enter)
END_EVENT_TABLE()

FindPanel::FindPanel(wxWindow *parent, wxWindowID id, char *title,
		     int xpos, int ypos, int width, int height,
		     long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
  
{
  search_choice_lable = 0;
  db_input = 0;
  key_name_label = 0;
  search_choicebox = 0;
  search_btn = 0;
  close_btn = 0;  

  child_frame = 0;
}
  
FindPanel::~FindPanel()
{
  if(db_input) delete db_input;
  if(key_name_label) delete key_name_label;
  if(search_choice_lable) delete search_choice_lable;
  if(search_choicebox) delete search_choicebox;
  if(search_btn) delete search_btn;
  if(close_btn) delete close_btn;
}

void FindPanel::OnClose(wxCommandEvent &WXUNUSED(event))
{
  Show(FALSE);
}

void FindPanel::OnTextControl1Enter(wxCommandEvent &event)
{
  StartSearch();
}

void FindPanel::OnSearch(wxCommandEvent &WXUNUSED(event))
{
  StartSearch();
}

void FindPanel::ShowPanel()
{
  child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  if(frame->dpanel->PanelIsOpen()) {
    frame->dpanel->ClosePanel();
  }

  if(search_choicebox) delete search_choicebox;
    wxString search_choices[NumDataMembers];
  char dest[DBStringLength];

  for(int i = 0; i < NumDataMembers; i++) {
    search_choices[i] = \
      child_frame->DBParms()->db_config.GetColName(i, dest);
  }

  search_choicebox = new wxChoice(this, ID_FIND_CHOICEBOX1,
				  wxPoint(17, 84), 
				  wxSize(105, 25),
				  NumDataMembers, 
				  (const wxString *)search_choices);
  search_choicebox->SetSelection(0);


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

void FindPanel::StartSearch()
{
  Show(FALSE);
  gxString gxbuf1;

  gxString sbuf((const char *)db_input->GetValue().c_str());

  // Trim the leading and trailing spaces from the key name
  sbuf.TrimLeadingSpaces(); sbuf.TrimTrailingSpaces(); 

  if(sbuf.is_null()) {
    ProgramError->Message("You must enter a string to search for\n");
    return;
  }

  unsigned objects_found;
  if(sbuf.Find(child_frame->DBParms()->WildCard1) != -1) {
    // Search the database for all sub-string matches
    sbuf.FilterString(child_frame->DBParms()->WildCard1);

    gxbuf1 << clear << "Searching for string " << sbuf.c_str() << " ...";
    frame->SetStatusText(gxbuf1.c_str());

#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Searching for string " << sbuf.c_str() << "\n";
    *(frame->statusWin) << "Search criteria: "
			<< search_choicebox->GetSelection() << "\n";
#endif
    
    INFOHOG_t key(sbuf.c_str());
    objects_found = BtreeSearch(child_frame->GetPOD()->Index(),
				search_choicebox->GetSelection(),
				child_frame->GetPOD(), key, 1);
  }
  else {
    gxbuf1 << clear << "Searching for string " << sbuf.c_str() << "...";
    frame->SetStatusText(gxbuf1.c_str());

#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Searching for string " << sbuf.c_str() << "\n";
#endif

    // Search the database for all matches of the specified string
    INFOHOG_t key(sbuf.c_str());
    objects_found = BtreeSearch(child_frame->GetPOD()->Index(),
				search_choicebox->GetSelection(),
				child_frame->GetPOD(), key, 0);
  }

  frame->SetStatusText("");

  if(objects_found == 0) {
    ProgramError->Message("No matching objects were found\n");
    return;
  }

  wxString mesg;
  if(objects_found > 1) {
    mesg.Printf("Found %d matching objects\nDisplay the objects?\n",
		objects_found);
  }
  else {
    mesg.Printf("Found %d matching object\nDisplay the object?",
		objects_found);
  }

  int yn = wxMessageBox(mesg, "Program Message",
			wxYES_NO | wxCENTRE | wxICON_QUESTION, this);
  if(yn == wxNO) return;

  Show(FALSE);
  dllistptr = dllist->GetHead();
  INFOHOG infohog(child_frame->GetPOD());
  infohog.ReadObject(dllistptr->data);
  frame->dpanel->LoadPanel(infohog);
  frame->dpanel->ShowPanel();
}

FindPanel *InitFindPanel(wxWindow *parent)
{
  int xpos=50; int ypos=50; int width=330; int height=220;
  FindPanel *panel = new FindPanel(parent, FIND_PANEL_ID,
				   (char *)"Find Database Object",
				   xpos, ypos, width, height,
		  wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
				   (char *)"Find Database Object");

  if(!panel) return 0;
  
  wxString search_choices[NumDataMembers];
  char dest[DBStringLength];

  for(int i = 0; i < NumDataMembers; i++) {
    search_choices[i] = \
      progcfg->global_dbparms.db_config.GetColName(i, dest);
  }
  
  panel->key_name_label = new wxStaticBox(panel, ID_FIND_STATICTEXT1,
					  "Database Search",
					  wxPoint(9, 4),
					  wxSize(300, 60));
  
  panel->db_input = new wxTextCtrl(panel, ID_FIND_TEXTCONTROL1,
				   "",
				   wxPoint(17, 28),
				   wxSize(280, 25),
				   wxTE_PROCESS_ENTER);
  
  panel->search_choice_lable = new wxStaticText(panel, ID_FIND_STATICTEXT2,
						"Search Criteria",
						wxPoint(17, 63)); 
  
  panel->search_choicebox = new wxChoice(panel, ID_FIND_CHOICEBOX1,
					 wxPoint(17, 84), 
					 wxSize(150, 95),
					 NumDataMembers, 
					 (const wxString *)search_choices);
  panel->search_choicebox->SetSelection(0);


  panel->search_btn = new wxButton(panel, ID_FIND_SEARCH, "Search",
				   wxPoint(17, 123),
				   wxSize(60, 25));

  panel->close_btn = new wxButton(panel, ID_FIND_CLOSE, "Close",
				  wxPoint(81, 123),
				  wxSize(60, 25));

  panel->Show(FALSE);

  return panel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
