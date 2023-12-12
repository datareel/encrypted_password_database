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

Page setup panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

#ifdef __USE_APP_POSTSCRIPT_PRINTING__

BEGIN_EVENT_TABLE(PSPagePanel, wxDialog)
  EVT_BUTTON    (ID_PSPAGE_ACCEPT, PSPagePanel::OnAccept)
  EVT_BUTTON    (ID_PSPAGE_CANCEL, PSPagePanel::OnCancel)
  EVT_BUTTON    (ID_PSPAGE_CLOSE, PSPagePanel::OnClose)
  EVT_BUTTON    (ID_PSPAGE_DEFAULT, PSPagePanel::OnDefault)
END_EVENT_TABLE()

PSPagePanel::PSPagePanel(wxWindow *parent, wxWindowID id, char *title,
		     int xpos, int ypos, int width, int height,
		     long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
  
{
  lpp_label = cell_label = fs_label = lm_label = orientation_label = \
    font_label = paper_label = doc_name_label = 0;
  lpp_slider = cell_slider = fs_slider = lm_slider = 0;
  orientation_choice = font_choice = paper_choice = 0;
  doc_name_input = 0;
  accept_btn = close_btn = cancel_btn = default_btn = 0;
}
  
PSPagePanel::~PSPagePanel()
{
  if(lpp_label) lpp_label;
  if(cell_label) delete cell_label;
  if(fs_label) delete fs_label;
  if(lm_label) delete lm_label;
  if(orientation_label) delete orientation_label;
  if(font_label) delete font_label;
  if(paper_label) delete paper_label;
  if(doc_name_label) delete doc_name_label;
  if(lpp_slider) delete lpp_slider;
  if(cell_slider) delete cell_slider;
  if(fs_slider) delete fs_slider;
  if(lm_slider) delete lm_slider;
  if(orientation_choice) delete orientation_choice;
  if(font_choice) delete font_choice;
  if(paper_choice) delete paper_choice;
  if(doc_name_input) delete doc_name_input;
  if(accept_btn) delete accept_btn;
  if(close_btn) delete close_btn;
  if(cancel_btn) delete cancel_btn;
  if(default_btn) delete default_btn;
}

void PSPagePanel::OnClose(wxCommandEvent &WXUNUSED(event))
{
  Show(FALSE);
}

void PSPagePanel::OnAccept(wxCommandEvent &WXUNUSED(event))
{

}

void PSPagePanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{

}

void PSPagePanel::OnDefault(wxCommandEvent &WXUNUSED(event))
{

}

PSPagePanel *InitPSPagePanel(wxWindow *parent)
{
  int xpos=50; int ypos=50; int width=350; int height=365;
  PSPagePanel *pspanel = new PSPagePanel(parent, PSPAGE_PANEL_ID,
					    "Postscript Page Setup",
					    xpos, ypos, width, height,
		    wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
					    "Postscript Page Setup");

  if(!pspanel) return 0;

  const wxString OrChoiceStrings[2] = {
    "Landscape",
    "Portrait"
  };

  const wxString FontChoiceStrings[6] = {
    "Swiss",      // wxSWISS
    "Roman",      // wxROMAN
    "Decorative", // wxDECORATIVE
    "Modern",     // wxMODERN 
    "Script",     // wxSCRIPT
    "System"      // wxDEFAULT
  };

  const wxString PaperChoiceStrings[5] = {
    "Letter",
    "Legal",
    "Tabloid",
    "A3",
    "A4"  
  };
  
  pspanel->lpp_label = new wxStaticText(pspanel, -1,
					"Lines Per Page",
					wxPoint(9, 14),
					wxSize(75, 14));
  pspanel->lpp_slider = new wxSlider(pspanel, -1,
				     PSPrintSetup->lines_per_page,
				     1, 200,
				     wxPoint(90, 12), wxSize(155, -1),
				     wxSL_LABELS);

  pspanel->cell_label = new wxStaticText(pspanel, -1,
					 "Cell Length",
					 wxPoint(9, 52),
					 wxSize(70, 14));
  pspanel->cell_slider = new wxSlider(pspanel, -1,
				      PSPrintSetup->cell_len,
				      1, 50,
				      wxPoint(75, 50), wxSize(155, -1),
				      wxSL_LABELS);

  pspanel->fs_label = new wxStaticText(pspanel, -1,
				       "Font Size",
				       wxPoint(9, 80),
				       wxSize(70, 14));
  pspanel->fs_slider = new wxSlider(pspanel, -1,
				    (int)PSPrintSetup->font_size,
				    1, 50,
				    wxPoint(75, 78), wxSize(155, -1),
				    wxSL_LABELS);
  
  pspanel->lm_label = new wxStaticText(pspanel, -1,
				       "Left Margin",
				       wxPoint(9, 108),
				       wxSize(70, 14));
  pspanel->lm_slider = new wxSlider(pspanel, -1,
				    PSPrintSetup->lr_margin,
				    1, 20,
				    wxPoint(75, 106), wxSize(155, -1),
				     wxSL_LABELS);
  
  pspanel->orientation_label = new wxStaticText(pspanel, -1,
						"Orientation",
						wxPoint(9, 150),
						wxSize(70, 14));
  
  pspanel->orientation_choice = new wxChoice(pspanel, -1,
					     wxPoint(75, 148), 
					     wxSize(80, 25),
					      2, OrChoiceStrings);
  pspanel->orientation_choice->SetSelection(PSPrintSetup->orientation);

  pspanel->font_label = new wxStaticText(pspanel, -1,
					 "Font",
					 wxPoint(9, 185),
					 wxSize(70, 14));

  pspanel->font_choice = new wxChoice(pspanel, -1,
				      wxPoint(75, 183), 
				      wxSize(80, 25),
				      6, FontChoiceStrings);
  pspanel->font_choice->SetSelection((int)PSPrintSetup->item_font);
  
  
  pspanel->paper_label = new wxStaticText(pspanel, -1,
					  "Paper Size",
					  wxPoint(9, 220),
					  wxSize(70, 14));

  pspanel->paper_choice = new wxChoice(pspanel, -1,
				       wxPoint(75, 218), 
				       wxSize(80, 25),
				       5, PaperChoiceStrings);
  pspanel->paper_choice->SetSelection(PSPrintSetup->paper_code);


  pspanel->doc_name_label = new wxStaticText(pspanel, -1,
					     "Document name",
					     wxPoint(9, 264),
					     wxSize(85, 14));
  
  pspanel->doc_name_input = new wxTextCtrl(pspanel, -1,
					   PSPrintSetup->document_name,
					   wxPoint(95, 260),
					   wxSize(200, 25));
  
  pspanel->accept_btn = new wxButton(pspanel, ID_PSPAGE_ACCEPT, "Accept",
				     wxPoint(17, 300),
				     wxSize(50, 25));
  
  pspanel->cancel_btn = new wxButton(pspanel, ID_PSPAGE_CANCEL, "Cancel",
				     wxPoint(83, 300),
				     wxSize(50, 25));
  
  pspanel->close_btn = new wxButton(pspanel, ID_PSPAGE_CLOSE, "Close",
				    wxPoint(149, 300),
				    wxSize(50, 25));


  pspanel->default_btn = new wxButton(pspanel, ID_PSPAGE_DEFAULT, "Default",
				      wxPoint(215, 300),
				      wxSize(50, 25));

  pspanel->Show(FALSE);

  return pspanel;
}

#endif // __USE_APP_POSTSCRIPT_PRINTING__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
