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

  // 10/27/2026: The column names of this database
  char dest[DBStringLength];
  search_choicebox->Clear();
  for(int i = 0; i < NumDataMembers; i++) {
    search_choicebox->Append(wxString::FromUTF8(child_frame->DBParms()->db_config.GetColName(i, dest)));
  }
  search_choicebox->SetSelection(0);
  db_input->SetFocus();
  db_input->SelectAll();
  CentreOnParent();
  ShowModal();
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
// 10/27/2026: Sizers; the search box above the grid also filters records
{
  FindPanel *panel = new FindPanel(parent, FIND_PANEL_ID, (char *)"Find Records", -1, -1, -1, -1,
				   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, (char *)"Find Records");
  if(!panel) return 0;
  panel->SetIcons(EPDBAppIcons());
  const int pad = panel->FromDIP(12);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer *g = new wxFlexGridSizer(2, panel->FromDIP(6), panel->FromDIP(8));
  g->AddGrowableCol(1, 1);
  g->Add(new wxStaticText(panel, ID_FIND_STATICTEXT1, "Find"), 0, wxALIGN_CENTER_VERTICAL);
  panel->db_input = new wxTextCtrl(panel, ID_FIND_TEXTCONTROL1, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  panel->db_input->SetMinSize(wxSize(panel->FromDIP(280), -1));
  g->Add(panel->db_input, 1, wxEXPAND);
  g->Add(new wxStaticText(panel, ID_FIND_STATICTEXT2, "In field"), 0, wxALIGN_CENTER_VERTICAL);
  wxString search_choices[NumDataMembers];
  for(int i = 0; i < NumDataMembers; i++) search_choices[i] = progcfg->global_dbparms.dbgrid_labels[i].c_str();
  panel->search_choicebox = new wxChoice(panel, ID_FIND_CHOICEBOX1, wxDefaultPosition, wxDefaultSize,
					 NumDataMembers, search_choices);
  panel->search_choicebox->SetSelection(0);
  g->Add(panel->search_choicebox, 0);
  top->Add(g, 0, wxEXPAND | wxALL, pad);
  top->Add(EPDBNote(panel, "Finds the records and shows them one at a time. To filter the list, "
		    "type in the search box above the records (Ctrl+F).", 360), 0, wxLEFT | wxRIGHT, pad);
  wxBoxSizer *b = new wxBoxSizer(wxHORIZONTAL);
  b->AddStretchSpacer();
  panel->close_btn = new wxButton(panel, ID_FIND_CLOSE, "Close");
  b->Add(panel->close_btn, 0);
  panel->search_btn = new wxButton(panel, ID_FIND_SEARCH, "Find");
  panel->search_btn->SetDefault();
  b->Add(panel->search_btn, 0, wxLEFT, panel->FromDIP(6));
  top->Add(b, 0, wxEXPAND | wxALL, pad);
  panel->SetEscapeId(ID_FIND_CLOSE);
  panel->SetSizerAndFit(top);
  panel->SetMinSize(panel->GetSize());
  panel->Show(FALSE);
  return panel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
