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

#ifdef __USE_MSW_PRINTING__

BEGIN_EVENT_TABLE(MSWPagePanel, wxDialog)
  EVT_CLOSE(MSWPagePanel::OnCloseWindow)
  EVT_BUTTON (ID_MSWPAGEPANEL_ACCEPT, MSWPagePanel::OnAccept)
  EVT_BUTTON (ID_MSWPAGEPANEL_CLOSE, MSWPagePanel::OnClose)
  EVT_BUTTON (ID_MSWPAGEPANEL_CANCEL, MSWPagePanel::OnCancel)
  EVT_BUTTON (ID_MSWPAGEPANEL_DEFAULT, MSWPagePanel::OnDefault)
  EVT_BUTTON (ID_MSWPAGEPANEL_HEADER_FONT, MSWPagePanel::OnHeaderFont)
  EVT_BUTTON (ID_MSWPAGEPANEL_HEADER_COLOR, MSWPagePanel::OnHeaderColor)
  EVT_BUTTON (ID_MSWPAGEPANEL_FOOTER_FONT, MSWPagePanel::OnFooterFont)
  EVT_BUTTON (ID_MSWPAGEPANEL_FOOTER_COLOR, MSWPagePanel::OnFooterColor)
END_EVENT_TABLE()

MSWPagePanel::MSWPagePanel(wxWindow *parent, wxWindowID id, char *title,
			   int xpos, int ypos, int width, int height,
			   long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
{

  accept_btn = close_btn = cancel_btn = default_btn = 0;
  notebook = 0;
  panel1 = panel2 = panel3, panel4 = 0;

  fs_label = lm_label = orientation_label = \
    font_label = paper_label = doc_name_label = 0;
  lm_slider = 0;
  orientation_choice = paper_choice = 0;
  doc_name_input = 0;
  
  grid_lines_ckbox = time_date_ckbox = print_header_ckbox = \
    print_footer_ckbox = grid_labels_ckbox = row_numbers_ckbox = 0;

  start_x = 17;
  start_y = 28;
  y_offset_checkbox = 25;
  label_height = 15;
  label_width = -1;

  change_labels = 0;

  for(int i = 0; i < NumDataMembers; i++) include_fields[i] = 0;
  header_name_label = footer_name_label = 0;
  header_name_input = footer_name_input = 0;
  header_font_btn = header_color_btn = footer_font_btn = \
    footer_color_btn = 0;

  has_new_header_font = has_new_header_color = has_new_footer_font = \
    has_new_footer_color = 0;

  Init();
}

MSWPagePanel::~MSWPagePanel()
{
  // NOTE: All objects must be deleted in the order below.

  // Delete the panel objects
  if(fs_label) delete fs_label;
  if(lm_label) delete lm_label;
  if(orientation_label) delete orientation_label;
  if(font_label) delete font_label;
  if(paper_label) delete paper_label;
  if(doc_name_label) delete doc_name_label;
  if(lm_slider) delete lm_slider;
  if(orientation_choice) delete orientation_choice;
  if(paper_choice) delete paper_choice;
  if(doc_name_input) delete doc_name_input;
  if(grid_lines_ckbox) delete grid_lines_ckbox;
  if(time_date_ckbox) delete time_date_ckbox; 
  if(print_header_ckbox) delete print_header_ckbox;
  if(print_footer_ckbox) delete print_footer_ckbox;
  if(grid_labels_ckbox) delete grid_labels_ckbox;
  if(row_numbers_ckbox) delete row_numbers_ckbox;
  for(int i = 0; i < NumDataMembers; i++) delete include_fields[i];
  if(header_name_label) delete header_name_label;
  if(footer_name_label) delete footer_name_label;
  if(header_name_input) delete header_name_input;
  if(footer_name_input) delete footer_name_input;
  if(header_font_btn) delete header_font_btn;
  if(header_color_btn) delete header_color_btn;
  if(footer_font_btn) delete footer_font_btn;
  if(footer_color_btn) delete footer_color_btn;

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

void MSWPagePanel::OnAccept(wxCommandEvent &event)
{
  if(!HasChanged()) {
    wxMessageBox("There are no changes to commit\n" ,
		 "Program Message", wxOK|wxCENTRE, this);
    
    return;
  } 
  if(CommitChanges()) Show(FALSE);
}

int MSWPagePanel::HasChanged()
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return 0;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;
  MSWPrintingParameters *print_config = &child_frame->print_config;

  char dest[DBStringLength];
  int i;

  for(i = 0; i < NumDataMembers; i++) {
    if(include_fields[i]->GetValue()) {
      if(db_config->print_field[i] != 1) return 1;
    }
    if(!include_fields[i]->GetValue()) {
      if(db_config->print_field[i] != 0) return 1;
    }
  }
  if(db_config->print_grid_lines == 1) {
    if(!grid_lines_ckbox->GetValue()) return 1;
  }
  else {
    if(grid_lines_ckbox->GetValue()) return 1;
  }
  if(db_config->print_grid_labels == 1) {
    if(!grid_labels_ckbox->GetValue()) return 1;
  }
  else {
    if(grid_labels_ckbox->GetValue()) return 1;
  }
  if(db_config->print_row_numbers == 1) {
    if(!row_numbers_ckbox->GetValue()) return 1;
  }
  else {
    if(row_numbers_ckbox->GetValue()) return 1;
  }
  if(db_config->print_doc_time_and_date == 1) {
    if(!time_date_ckbox->GetValue()) return 1;
  }
  else {
    if(time_date_ckbox->GetValue()) return 1;
  }
  if(db_config->print_page_header == 1) {
    if(!print_header_ckbox->GetValue()) return 1;
  }
  else {
    if(print_header_ckbox->GetValue()) return 1;
  }
  if(db_config->print_page_footer == 1) {
    if(!print_footer_ckbox->GetValue()) return 1;
  }
  else {
    if(print_footer_ckbox->GetValue()) return 1;
  }

  if(has_new_header_font) return 1;
  if(has_new_header_color) return 1;
  if(has_new_footer_font) return 1;
  if(has_new_footer_color) return 1;

  if(doc_name_input->GetValue() != db_config->print_doc_name.c_str(dest)) return 1;
  if(header_name_input->GetValue() != db_config->print_doc_custom_header.c_str(dest)) return 1;
  if(footer_name_input->GetValue() != db_config->print_doc_custom_footer.c_str(dest)) return 1;

  if(print_config->orientation != orientation_choice->GetSelection()) return 1;
  if(print_config->paper_code != paper_choice->GetSelection()) return 1;
  
  if(lm_slider->GetValue() != db_config->print_left_margin_size) return 1;

  return 0; 
}

void MSWPagePanel::ClosePanel() 
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

void MSWPagePanel::OnClose(wxCommandEvent &event)
{
  ClosePanel();
}

void MSWPagePanel::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
// Define the behaviour for the frame closing. This event handler
// is called when the user has tried to close a a frame or dialog
// box using the window manager (X) or system menu (Windows). Must
// delete all frames except for the main one. 
{
  ClosePanel();
}

void MSWPagePanel::OnCancel(wxCommandEvent &event)
{
  LoadPanel();
  Show(FALSE);
}

void MSWPagePanel::OnDefault(wxCommandEvent &event)
{
  int yn = wxMessageBox("This will reset all current page changes!\
\nContinue?",
			"Program Message",
			wxYES_NO|wxCENTRE|wxICON_EXCLAMATION, this);
  if(yn == wxNO) return;

  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  lm_slider->SetValue(mswpLEFT_MARGIN);
  orientation_choice->SetSelection(mswpORIENTATION);
  paper_choice->SetSelection(mswpPAPERCODE);

  char dest[DBStringLength];
  int i;
  doc_name_input->SetValue(db_config->database_name.c_str(dest));
  
  grid_lines_ckbox->SetValue(TRUE);
  time_date_ckbox->SetValue(TRUE);
  print_header_ckbox->SetValue(TRUE);
  print_footer_ckbox->SetValue(TRUE);
  grid_labels_ckbox->SetValue(TRUE);
  row_numbers_ckbox->SetValue(FALSE);

  for(i = 0; i < NumDataMembers; i++) {
    include_fields[i]->SetValue(TRUE);
  }

  has_new_header_font = 1;
  wxFont default_font(mswpHEADER_FONT_SIZE, wxSWISS, wxNORMAL, wxBOLD);
  wxColour default_color(0, 0, 0);
  header_font = default_font;
  has_new_header_color = 1;
  header_color = default_color;
  has_new_footer_font = 1;
  footer_font = default_font;
  has_new_footer_color = 1;
  footer_color = default_color;

  header_name_input->Clear();
  footer_name_input->Clear();
}

void MSWPagePanel::LoadPanel()
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;
  MSWPrintingParameters *print_config = &child_frame->print_config;
  gxDatabase *f = child_frame->DBParms()->pod->OpenDataFile();
 
  if(!TestDatabase(0, 1, 1)) {
    return;
  }
  if(!db_config->ReadConfig(f)) { 
    *(frame->statusWin) << "Error reading DB config" << "\n"; 
  }
  if(!db_config->Validate()) {
    *(frame->statusWin) << "Error validating DB config" << "\n"; 
  }

  char dest[DBStringLength];
  int i;

  // Set the current list of includes
  for(i = 0; i < NumDataMembers; i++) {
    if(db_config->print_field[i] == 0) {
      include_fields[i]->SetValue(FALSE);
    }
    else {
      include_fields[i]->SetValue(TRUE);
    }
  }
  if(db_config->print_grid_lines == 1) {
    grid_lines_ckbox->SetValue(TRUE);
  }
  else {
    grid_lines_ckbox->SetValue(FALSE);
  }
  if(db_config->print_grid_labels == 1) {
    grid_labels_ckbox->SetValue(TRUE);
  }
  else {
    grid_labels_ckbox->SetValue(FALSE);
  }
  if(db_config->print_row_numbers == 1) {
    row_numbers_ckbox->SetValue(TRUE);
  }
  else {
    row_numbers_ckbox->SetValue(FALSE);
  }
  if(db_config->print_doc_time_and_date == 1) {
    time_date_ckbox->SetValue(TRUE);
  }
  else {
    time_date_ckbox->SetValue(FALSE);
  }
  if(db_config->print_page_header == 1) {
    print_header_ckbox->SetValue(TRUE);
  }
  else {
    print_header_ckbox->SetValue(FALSE);
  }
  if(db_config->print_page_footer == 1) {
    print_footer_ckbox->SetValue(TRUE);
  }
  else {
    print_footer_ckbox->SetValue(FALSE);
  }

  if(db_config->print_doc_name.is_null()) {
    doc_name_input->Clear();
  }
  else {
    doc_name_input->SetValue(db_config->print_doc_name.c_str(dest)); 
  }

  if(db_config->print_doc_custom_header.is_null()) {
    header_name_input->Clear();
  }
  else {
    header_name_input->SetValue(db_config->print_doc_custom_header.c_str(dest)); 
  }

  if(db_config->print_doc_custom_footer.is_null()) {
    footer_name_input->Clear();
  }
  else {
    footer_name_input->SetValue(db_config->print_doc_custom_footer.c_str(dest)); 
  }

  print_config->orientation = db_config->print_orientation;
  print_config->SetOrientation();
  orientation_choice->SetSelection(print_config->orientation);

  print_config->paper_code = db_config->print_paper_size;
  print_config->SetPaperSize();
  paper_choice->SetSelection(print_config->paper_code);

  lm_slider->SetValue(db_config->print_left_margin_size);
}

void MSWPagePanel::ShowPanel()
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
 
  int i;
  gxString sbuf;
  char dest[DBStringLength];

  if(change_labels) {
    start_x = 17;
    start_y = 28;
    for(i = 0; i < NumDataMembers; i++) {
      delete include_fields[i];
      sbuf << clear << "Field " << (i+1) << ": " 
	   << progcfg->global_dbparms.dbgrid_labels[i];

      include_fields[i] = new wxCheckBox(panel4, -1,
					 sbuf.c_str(),
					 wxPoint(start_x, start_y), 
					 wxSize(label_width, label_height));
      start_y += y_offset_checkbox;
    }
    change_labels = 0;
  }

  start_x = 17;
  start_y = 28;
  for(i = 0; i < NumDataMembers; i++) {
    gxString label = child_frame->DBParms()->db_config.GetColName(i, dest);  
    if(label != progcfg->global_dbparms.dbgrid_labels[i]) {
      delete include_fields[i];
      sbuf << clear << "Field " << (i+1) << ": " 
	   << label;
      include_fields[i] = new wxCheckBox(panel4, -1,
					 sbuf.c_str(),
					 wxPoint(start_x, start_y), 
					 wxSize(label_width, label_height));
          change_labels++;
    }
    start_y += y_offset_checkbox;
  }

  has_new_header_font = 0;
  has_new_header_color = 0;
  has_new_footer_font = 0;
  has_new_footer_color = 0;

  LoadPanel();
  Show(TRUE);
}

int MSWPagePanel::CommitChanges()
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return 0;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;
  MSWPrintingParameters *print_config = &child_frame->print_config;
  gxDatabase *f = child_frame->DBParms()->pod->OpenDataFile();
  char dest[DBStringLength];
  int i;

  if(!TestDatabase(0, 1, 1)) {
    return 0;
  }

  int write_db_config = 0;
  int has_includes = 0;
  for(i = 0; i < NumDataMembers; i++) {
    if(include_fields[i]->GetValue()) has_includes++;
  }
  if(!has_includes) {
    ProgramError->Message("You must include at least 1 print include field\n");
    return 0;
  }
  for(i = 0; i < NumDataMembers; i++) {
    if(include_fields[i]->GetValue()) {
      if(db_config->print_field[i] == 0) {
	write_db_config++;
	db_config->print_field[i] = 1;
      }
    }
    else {
      if(db_config->print_field[i] == 1) {
	write_db_config++;
	db_config->print_field[i] = 0;
      }
    }
  }

  if(write_db_config > 0) {
    if(!db_config->WritePrintField(f)) {
      *(frame->statusWin) << "Error saving print field include value" << "\n"; 
    }
    write_db_config = 0;
  }

  write_db_config = 0;
  if(grid_lines_ckbox->GetValue()) {
    if(db_config->print_grid_lines == 0) {
      write_db_config++;
      db_config->print_grid_lines = 1;
    }
  }
  else {
    if(db_config->print_grid_lines == 1) {
      write_db_config++;
      db_config->print_grid_lines = 0;
    }
  }
  if(write_db_config > 0) {
    if(!db_config->WritePrintGridLines(f)) {
      *(frame->statusWin) << "Error saving print grid lines value" << "\n"; 
    }
    write_db_config = 0;
  }
  write_db_config = 0;
  if(grid_labels_ckbox->GetValue()) {
    if(db_config->print_grid_labels == 0) {
      write_db_config++;
      db_config->print_grid_labels = 1;
    }
  }
  else {
    if(db_config->print_grid_labels == 1) {
      write_db_config++;
      db_config->print_grid_labels = 0;
    }
  }
  if(write_db_config > 0) {
    if(!db_config->WritePrintGridLabels(f)) {
      *(frame->statusWin) << "Error saving print grid labels value" << "\n"; 
    }
    write_db_config = 0;
  }
  write_db_config = 0;
  if(row_numbers_ckbox->GetValue()) {
    if(db_config->print_row_numbers == 0) {
      write_db_config++;
      db_config->print_row_numbers = 1;
    }
  }
  else {
    if(db_config->print_row_numbers == 1) {
      write_db_config++;
      db_config->print_row_numbers = 0;
    }
  }
  if(write_db_config > 0) {
    if(!db_config->WritePrintRowNumbers(f)) {
      *(frame->statusWin) << "Error saving print row numbers value" << "\n"; 
    }
    write_db_config = 0;
  }
  write_db_config = 0;
  if(time_date_ckbox->GetValue()) {
    if(db_config->print_doc_time_and_date == 0) {
      write_db_config++;
      db_config->print_doc_time_and_date = 1;
    }
  }
  else {
    if(db_config->print_doc_time_and_date == 1) {
      write_db_config++;
      db_config->print_doc_time_and_date = 0;
    }
  }
  if(write_db_config > 0) {
    if(!db_config->WritePrintDocTimeAndDate(f)) {
      *(frame->statusWin) << "Error saving print time and date stamp value" << "\n"; 
    }
    write_db_config = 0;
  }
  write_db_config = 0;
  if(print_header_ckbox->GetValue()) {
    if(db_config->print_page_header == 0) {
      write_db_config++;
      db_config->print_page_header = 1;
    }
  }
  else {
    if(db_config->print_page_header == 1) {
      write_db_config++;
      db_config->print_page_header = 0;
    }
  }
  if(write_db_config > 0) {
    if(!db_config->WritePrintPageHeader(f)) {
      *(frame->statusWin) << "Error saving print page header value" << "\n"; 
    }
    write_db_config = 0;
  }
  write_db_config = 0;
  if(print_footer_ckbox->GetValue()) {
    if(db_config->print_page_footer == 0) {
      write_db_config++;
      db_config->print_page_footer = 1;
    }
  }
  else {
    if(db_config->print_page_footer == 1) {
      write_db_config++;
      db_config->print_page_footer = 0;
    }
  }
  if(write_db_config > 0) {
    if(!db_config->WritePrintPageFooter(f)) {
      *(frame->statusWin) << "Error saving print page footer value" << "\n"; 
    }
    write_db_config = 0;
  }

  if(has_new_header_font) {
    if(!db_config->WritePrintHeaderFont(f, &header_font)) {
      *(frame->statusWin) << "Error saving page header font value" << "\n";
    }
    has_new_header_font = 0;
  }
  if(has_new_header_color) {
    if(!db_config->WritePrintHeaderColor(f, &header_color)) {
      *(frame->statusWin) << "Error saving page header font color value" << "\n";
    }
    has_new_header_color = 0;
  }

  if(has_new_footer_font) {
    if(!db_config->WritePrintFooterFont(f, &footer_font)) {
      *(frame->statusWin) << "Error saving page footer font value" << "\n";
    }
    has_new_footer_font = 0;
  }
  if(has_new_footer_color) {
    if(!db_config->WritePrintFooterColor(f, &footer_color)) {
      *(frame->statusWin) << "Error saving page footer font color value" << "\n";
    }
    has_new_footer_color = 0;
  }

  if(doc_name_input->GetValue() != db_config->print_doc_name.c_str(dest)) {
    if(doc_name_input->GetValue().IsNull()) {
      if(!db_config->WritePrintDocName(f, db_config->database_name.c_str(dest))) {
	*(frame->statusWin) << "Error saving print document name value" << "\n";
      }
    }
    else {
      if(!db_config->WritePrintDocName(f, doc_name_input->GetValue().c_str())) {
	*(frame->statusWin) << "Error saving print document name value" << "\n";
      }
    }
  }

  if(header_name_input->GetValue() != db_config->print_doc_custom_header.c_str(dest)) {
    if(header_name_input->GetValue().IsNull()) {
      if(!db_config->WritePrintDocCustomHeader(f, db_config->database_name.c_str(dest))) {
	*(frame->statusWin) << "Error saving print document custom header value" << "\n";
      }
    }
    else {
      if(!db_config->WritePrintDocCustomHeader(f, header_name_input->GetValue().c_str())) {
	*(frame->statusWin) << "Error saving print document custom header value" << "\n";
      }
    }
  }

  if(footer_name_input->GetValue() != db_config->print_doc_custom_footer.c_str(dest)) {
    if(!db_config->WritePrintDocCustomFooter(f, footer_name_input->GetValue().c_str())) {
      *(frame->statusWin) << "Error saving print document custom footer value" << "\n";
    }
  }

  if(print_config->orientation != orientation_choice->GetSelection()) {
    print_config->orientation = orientation_choice->GetSelection();
    print_config->SetOrientation();
    db_config->print_orientation = print_config->orientation;
    if(!db_config->WritePrintOrientation(f)) {
      *(frame->statusWin) << "Error saving print orientation value" << "\n";
    }
  }

  if(print_config->paper_code != paper_choice->GetSelection()) {
    print_config->paper_code = paper_choice->GetSelection();
    print_config->SetPaperSize();
    db_config->print_paper_size = print_config->paper_code;
    if(!db_config->WritePrintPaperSize(f)) {
      *(frame->statusWin) << "Error saving print paper size value" << "\n";
    }
  }

  if(lm_slider->GetValue() != db_config->print_left_margin_size) {
    print_config->left_margin = lm_slider->GetValue();
    print_config->right_margin = lm_slider->GetValue();
    db_config->print_left_margin_size = lm_slider->GetValue();
    if(!db_config->WritePrintLeftMarginSize(f)) {
      *(frame->statusWin) << "Error saving print left margin point value" << "\n";
    }
  }

  if(!db_config->Flush(f)) {
    *(frame->statusWin) << "Error flushing DB config" << "\n";
  }

  return 1;
}

void MSWPagePanel::OnHeaderFont(wxCommandEvent &event)
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxFontData data;
  if(db_config->GetPrintHeaderFont(&header_font)) {
    data.SetInitialFont(header_font);
  }

#if defined (__WIN32__)
  data.EnableEffects(0);
  data.SetAllowSymbols(0);
#endif

  wxFontDialog dialog(this, data);
  if(dialog.ShowModal() == wxID_OK) {
    wxFontData retData = dialog.GetFontData();
    header_font = retData.GetChosenFont();
    has_new_header_font = 1;
  }
  else {
    has_new_header_font = 0;
  }
}

void MSWPagePanel::OnHeaderColor(wxCommandEvent &event)
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxColourData data;
  data.SetChooseFull(TRUE);
  LoadCustomColors(&data);

  // Set if user has already selected a color
  if(db_config->GetPrintHeaderColor(&header_color)) {
    data.SetColour(header_color);
  }

  wxColourDialog dlg(this, &data);
  if(dlg.ShowModal() == wxID_OK) {
    wxColourData retData = dlg.GetColourData();
    header_color = retData.GetColour();
    has_new_header_color = 1;
    SaveCustomColors(&retData);
  }
  else {
    has_new_header_color = 0;
  }
}

void MSWPagePanel::OnFooterFont(wxCommandEvent &event)
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxFontData data;
  if(db_config->GetPrintFooterFont(&footer_font)) {
    data.SetInitialFont(footer_font);
  }

#if defined (__WIN32__)
  data.EnableEffects(0);
  data.SetAllowSymbols(0);
#endif

  wxFontDialog dialog(this, data);
  if(dialog.ShowModal() == wxID_OK) {
    wxFontData retData = dialog.GetFontData();
    footer_font = retData.GetChosenFont();
    has_new_footer_font = 1;
  }
  else {
    has_new_footer_font = 0;
  }
}

void MSWPagePanel::OnFooterColor(wxCommandEvent &event)
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxColourData data;
  data.SetChooseFull(TRUE);
  LoadCustomColors(&data);

  // Set if user has already selected a color
  if(db_config->GetPrintFooterColor(&footer_color)) {
    data.SetColour(footer_color);
  }

  wxColourDialog dlg(this, &data);
  if(dlg.ShowModal() == wxID_OK) {
    wxColourData retData = dlg.GetColourData();
    footer_color = retData.GetColour();
    has_new_footer_color = 1;
    SaveCustomColors(&retData);
  }
  else {
    has_new_footer_color = 0;
  }
}

void MSWPagePanel::Init()
{
  notebook = new wxNotebook(this, ID_MSWPAGEPANEL_NOTEBOOK);
  wxLayoutConstraints* layout_const = new wxLayoutConstraints;
  layout_const->left.SameAs(this, wxLeft, 4);
  layout_const->right.SameAs(this, wxRight, 4);
  layout_const->top.SameAs(this, wxTop, 4);
  layout_const->bottom.SameAs(this, wxBottom, 40);
  notebook->SetConstraints(layout_const);

  accept_btn = new wxButton(this, ID_MSWPAGEPANEL_ACCEPT, 
			    "Accept", wxPoint(-1, -1), wxSize(80, 25));
  close_btn = new wxButton(this, ID_MSWPAGEPANEL_CLOSE, 
			   "Close", wxPoint(-1, -1), wxSize(80, 25));
  cancel_btn = new wxButton(this, ID_MSWPAGEPANEL_CANCEL, 
			    "Cancel", wxPoint(-1, -1), wxSize(80, 25));
  default_btn = new wxButton(this, ID_MSWPAGEPANEL_DEFAULT, 
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
  // NOTE Add panel object for this tab here

  const wxString OrChoiceStrings[2] = {
    "Landscape",
    "Portrait"
  };

  const wxString PaperChoiceStrings[5] = {
    "Letter",
    "Legal",
    "Tabloid",
    "A3",
    "A4"  
  };
  
  orientation_label = new wxStaticText(panel1, -1,
				       "Orientation",
				       wxPoint(17, 25));
  
  orientation_choice = new wxChoice(panel1, -1,
				    wxPoint(95, 23), 
				    wxSize(110, 30),
				    2, OrChoiceStrings);
  orientation_choice->SetSelection(mswpORIENTATION);
  
  paper_label = new wxStaticText(panel1, -1,
				 "Paper Size",
				 wxPoint(17, 85),
				 wxSize(70, 15));
  
  paper_choice = new wxChoice(panel1, -1,
			      wxPoint(95, 82), 
			      wxSize(110, 30),
			      5, PaperChoiceStrings);
  paper_choice->SetSelection(mswpPAPERCODE);
  
  

  notebook->AddPage(panel1, "Page Settings", TRUE);

  panel2 = new wxPanel(notebook, -1);
  // NOTE Add panel object for this tab here

  lm_label = new wxStaticText(panel2, -1,
			      "Left Margin",
			      wxPoint(17, 29));
  
  lm_slider = new wxSlider(panel2, -1,
			   mswpLEFT_MARGIN,
			   1, 20,
			   wxPoint(75, 27), wxSize(155, -1),
			   wxSL_LABELS);

  grid_lines_ckbox = new wxCheckBox(panel2, -1,
				    "Print grid lines",
				    wxPoint(17, 105), 
				    wxSize(250, 25));
  grid_labels_ckbox = new wxCheckBox(panel2, -1,
				     "Print grid labels",
				     wxPoint(17, 140), 
				     wxSize(250, 25));
  row_numbers_ckbox = new wxCheckBox(panel2, -1,
				     "Print row numbers",
				     wxPoint(17, 175), 
				     wxSize(250, 25));

  doc_name_label = new wxStaticText(panel2, -1,
				    "Document Name",
				    wxPoint(17, 219),
				    wxSize(120, 15));
  
  doc_name_input = new wxTextCtrl(panel2, -1,
				  "",
				  wxPoint(135, 216),
				  wxSize(200, 25));

  notebook->AddPage(panel2, "Document Settings");

  panel3 = new wxPanel(notebook, -1);
  // NOTE Add panel object for this tab here

  time_date_ckbox = new wxCheckBox(panel3, -1,
				   "Print time and date stamp",
				   wxPoint(17, 25), wxSize(250, 25));

  print_header_ckbox = new wxCheckBox(panel3, -1,
				      "Print page header",
				      wxPoint(17, 50), wxSize(250, 25));

  header_font_btn = new wxButton(panel3, ID_MSWPAGEPANEL_HEADER_FONT, "Header Font",
				 wxPoint(17, 80),
				 wxSize(85, 25)); 
				 
  header_color_btn = new wxButton(panel3, ID_MSWPAGEPANEL_HEADER_COLOR, "Header Color",
				 wxPoint(125, 80),
				 wxSize(85, 25)); 

  header_name_label = new wxStaticText(panel3, -1,
				    "Custom Header",
				    wxPoint(17, 120),
				    wxSize(87, 15));
  
  header_name_input = new wxTextCtrl(panel3, -1,
				  "",
				  wxPoint(107, 116),
				  wxSize(300, 25));

  print_footer_ckbox = new wxCheckBox(panel3, -1,
				      "Print page numbers",
				      wxPoint(17, 155), wxSize(250, 25));

  footer_font_btn = new wxButton(panel3, ID_MSWPAGEPANEL_FOOTER_FONT, "Footer Font",
				 wxPoint(17, 185),
				 wxSize(85, 25)); 
				 
  footer_color_btn = new wxButton(panel3, ID_MSWPAGEPANEL_FOOTER_COLOR, "Footer Color",
				 wxPoint(125, 185),
				 wxSize(85, 25)); 

  footer_name_label = new wxStaticText(panel3, -1,
				    "Custom Footer",
				    wxPoint(17, 225),
				    wxSize(87, 15));
  
  footer_name_input = new wxTextCtrl(panel3, -1,
				  "",
				  wxPoint(107, 221),
				  wxSize(300, 25));

  notebook->AddPage(panel3, "Page headers");

  panel4 = new wxPanel(notebook, -1);
  // NOTE Add panel object for this tab here

  gxString sbuf;

  for(int i = 0; i < NumDataMembers; i++) {
    sbuf << clear << "Field " << (i+1) << ": " 
	 << progcfg->global_dbparms.dbgrid_labels[i];

    include_fields[i] = new wxCheckBox(panel4, -1,
				       sbuf.c_str(),
				       wxPoint(start_x, start_y), 
				       wxSize(label_width, label_height));
    include_fields[i]->SetValue(TRUE);
    start_y += y_offset_checkbox;
  }

  notebook->AddPage(panel4, "Include Fields");

  
  notebook->SetSelection(0);
  SetAutoLayout(TRUE);
  Layout();
  Centre(wxBOTH);
}

MSWPagePanel *InitMSWPagePanel(wxWindow *parent)
{
  int xpos=0; int ypos=0; int width=465; int height=390;

  MSWPagePanel *panel = new MSWPagePanel(parent, MSWPAGE_TAB_PANEL_ID,
					 (char *)"Page Setup",
					 xpos, ypos, width, height,
		  wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
					 (char *)"Page Setup");

  if(!panel) return 0;

  panel->Show(FALSE);
  return panel;
}

#endif // __USE_MSW_PRINTING__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
