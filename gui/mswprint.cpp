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

Application framework printing classes
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

#ifdef __USE_MSW_PRINTING__

MSWPrintingParameters::MSWPrintingParameters()
{
  // Database printing parameters
  print_list = 0;
  print_all_entries = 1;
  print_all_cols = 1;
  
  // Database grid line parameters
  draw_grid_lines = 1;
  lineto_titlebar = 0;
  lineto_right_margin = 1;
  lineto_endof_grid = 0;

  // Set current print settings using the mswp defaults
  lines_per_page = mswpLINES_PER_PAGE; 
  font_size = mswpFONT_SIZE;   
  orientation = mswpORIENTATION;
  font_code = mswpFONTCODE;        
  paper_code = mswpPAPERCODE;
  left_margin = mswpLEFT_MARGIN;  
  right_margin = mswpRIGHT_MARGIN;  
  top_margin = mswpTOP_MARGIN; 
  bottom_margin = mswpBOTTOM_MARGIN; 
  cell_scale = mswpCELL_SCALE;
  
  // Global print data, to remember settings during the session
#if defined(__WXGTK__) || defined(__WXMOTIF__)
  //  print_data = wxThePrintSetupData;
#endif

  // Set the printer paper size and orientation
  SetPaperSize();
  SetOrientation();
}    

void MSWPrintingParameters::SetOrientation()
{
  if(orientation == 0) { 
    print_data.SetOrientation(wxLANDSCAPE);
  }
  else {
    print_data.SetOrientation(wxPORTRAIT);
  }
}

void MSWPrintingParameters::SetPaperSize()
// Set the printer paper size.
{
  // NOTE: All sheet sizes must accommodate 1/8" or 3 mm head, foot, and
  // fore edge trim margins as non-printable areas.
  switch(paper_code) {
    case 0: 
      print_data.SetPaperId(wxPAPER_LETTER); // 8.5 X 11 non-metric paper size
      break;

    case 1:
      print_data.SetPaperId(wxPAPER_LEGAL); // 8.5 X 14 non-metric paper size
      break;

    case 2:
      print_data.SetPaperId(wxPAPER_TABLOID); // 11 X 17 non-metric paper size
      break;

    case 3:
      print_data.SetPaperId(wxPAPER_A3); // 297 X 420 metric paper size
      break;
    
    case 4:
      print_data.SetPaperId(wxPAPER_A4); // 210 X 297 metric paper size

      break;

    default:
      print_data.SetPaperId(wxPAPER_LETTER);
      break;
  };
}

mswPrintout::mswPrintout(const wxString &title) : wxPrintout(title.c_str())
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout constructor call" << "\n" << flush;
#endif

  num_lines = 0;

  child_frame = frame->ActiveChild();
  db_config = &child_frame->DBParms()->db_config;
  print_config = &child_frame->print_config;

  frame->SetStatusText("Building the list to print...");

  // Build a virtual database that will be printed
  if(print_config->print_list == 1) {
    num_lines = NumListEntries();
  }
  else if(print_config->print_all_entries == 1) {
    num_lines = BuildVirtualDB(child_frame);
  }
  else { // Print all entries by default
    num_lines = BuildVirtualDB(child_frame);
  }

  frame->SetStatusText("");

  // Initialize the virtual list
  dllistptr = dllist->GetHead();

  // Reset the top and bottem margins if users is not use a page
  // header for footer.
  if(!db_config->print_page_header) {
    print_config->top_margin = mswpTOP_MARGIN_NO_HEADER;
  }
  else {
    print_config->top_margin = mswpTOP_MARGIN;
  }
  if(!db_config->print_page_footer) {
    print_config->bottom_margin = mswpBOTTOM_MARGIN_NO_FOOTER;
  }
  else {
    print_config->bottom_margin = mswpBOTTOM_MARGIN;
  }

  SetAllFonts();
}
 
mswPrintout::mswPrintout()
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout default constructor call" << "\n" << flush;
#endif

  child_frame = frame->ActiveChild();
  db_config = &child_frame->DBParms()->db_config;
  print_config = &child_frame->print_config;

  if(!db_config->print_page_header) {
    print_config->top_margin = mswpTOP_MARGIN_NO_HEADER;
  }
  else {
    print_config->top_margin = mswpTOP_MARGIN;
  }
  if(!db_config->print_page_footer) {
    print_config->bottom_margin = mswpBOTTOM_MARGIN_NO_FOOTER;
  }
  else {
    print_config->bottom_margin = mswpBOTTOM_MARGIN;
  }


  SetAllFonts();
}

void mswPrintout::SetAllFonts()
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::SetAllFonts() call" << "\n" << flush;
#endif

  SetFont();
  SetItemBarFont();
  SetHeaderFont();
  SetFooterFont();
}

void mswPrintout::SetFont()
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::SetFont() call" << "\n" << flush;
#endif

  if(!db_config->GetGridTextFont(&printer_font)) {
    printer_font = *frame->itemFont;
  }

  char_width = .6 * printer_font.GetPointSize();
}

void mswPrintout::SetItemBarFont()
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::SetItemBarFont() call" << "\n" << flush;
#endif

  if(!db_config->GetGridLabelFont(&label_font)) {
    wxFont new_font(10, wxSWISS, wxNORMAL, wxBOLD);
    label_font = new_font;
  }
}

void mswPrintout::OnPreparePrinting()
// Called once by the framework before any other demands are made
// of the wxPrintout object. 
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::OnPreparePrinting() call" << "\n" << flush;
#endif

}

bool mswPrintout::OnPrintPage(int page)
// Called by the framework when a page should be printed.
// Returning FALSE cancels the print job. The application
// can use wxPrintout::GetDC to obtain a device context to
// draw on.
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::OnPrintPage() call" << "\n" << flush;
#endif

  wxDC *dc = GetDC();
  if(dc) {
    DrawTextPage(dc, page);
#ifdef __APP_DEBUG_VERSION__
    debug_log << "End of mswPrintout::OnPrintPage() call" << "\n" << flush;
#endif
    return TRUE;
  }
  else {
#ifdef __APP_DEBUG_VERSION__
    debug_log << "End of mswPrintout::OnPrintPage() call" << "\n" << flush;
#endif
    return FALSE;
  }
}

bool mswPrintout::OnBeginDocument(int startPage, int endPage)
// Called by the framework at the start of document printing.
// OnBeginDocument is called once for every copy printed. 
// The base wxPrintout::OnBeginDocument must be called
// (and the return value checked) from within the overriden
// function, since it calls wxDC::StartDoc.
{
  // NOTE: Order of function calls each time a page is previewed:
  // mswPrintout::OnBeginDocument() call
  // mswPrintout::OnPrintPage() call
  // mswPrintout::DrawTextPage() call
  // mswPrintout::ScaleDC() call
  // mswPrintout::WritePageHeader() call
  // mswPrintout::WritePageNumber() call

#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::OnBeginDocument() call" << "\n" << flush;
#endif

  if (!wxPrintout::OnBeginDocument(startPage, endPage)) {
#ifdef __APP_DEBUG_VERSION__
    debug_log << "End of mswPrintout::OnBeginDocument() call" << "\n" << flush;
#endif
    return FALSE; // Returns FALSE if the print job is canceled
  }

#ifdef __APP_DEBUG_VERSION__
  debug_log << "End of mswPrintout::OnBeginDocument() call" << "\n" << flush;
#endif
  return TRUE;
}

bool mswPrintout::HasPage(int pageNum)
// Should be overriden to return TRUE if the document has this page,
// or FALSE if not. Returning FALSE signifies the end of the document.
// By default, HasPage behaves as if the document has only one page.
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::HasPage() call" << "\n" << flush;
#endif

  int minPage, maxPage, selPageFrom, selPageTo;
  GetPageInfo(&minPage, &maxPage, &selPageFrom, &selPageTo);
  return (pageNum == minPage) || (pageNum <= maxPage);
}

void mswPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom,
			      int *selPageTo)
// Called by the framework to obtain information from the application
// about minimum and maximum page values that the user can select, and
// the required page range to be printed.
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::GetPageInfo() call" << "\n" << flush;
#endif

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Calculating page info" << "\n"; 
#endif

  first_page = 1; // Always starting on page one
  last_page = 1;  // Default is the first page

  *minPage = first_page;     // Always starting on page one
  *selPageFrom = first_page; // Always starting on page one

  int lines = num_lines;

  for(;;) {
    if(lines <= print_config->lines_per_page) break;
    lines = lines - print_config->lines_per_page;
    last_page++; // Increment the page count
  }

  *selPageTo = last_page;
  *maxPage = last_page;
}

void mswPrintout::ScaleDC(wxDC *dc)
// Scale the DC so that the printout roughly represents the
// the screen scaling.
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::ScaleDC() call" << "\n" << flush;
#endif

  // Get the page height and width
  GetPageSizeMM(&page_width, &page_height);
  
  // Get the logical pixels per inch of screen and printer
  GetPPIScreen(&ppiScreenX, &ppiScreenY);
  GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

  // Scale the DC so that the printout roughly represents the
  // the screen scaling.
  scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

  // Calculate conversion factor for converting millimetres into
  // logical units. There are approx. 25.1 mm to the inch. There
  // are ppi device units to the inch. Therefore 1 mm corresponds
  // to ppi/25.1 device units.
  logUnitsFactor = (float)(ppiPrinterX / (scale * 25.1));

  // Check real page size in case it is reduced by print preview 
  int pageWidth, pageHeight;
  int w, h;
  dc->GetSize(&w, &h);
#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "DC width = " << w << "\n";
  *(frame->statusWin) << "DC height = " << h << "\n";
#endif
  
  GetPageSizePixels(&pageWidth, &pageHeight);
  
  // Do not change if printer pageWidth == current DC width
  overallScale = scale * (float)(w/(float)pageWidth);
  dc->SetUserScale(overallScale, overallScale);
  
#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "User scaled DC width = " << w << "\n";
  *(frame->statusWin) << "User scaled DC height = " << h << "\n";
  *(frame->statusWin) << "Page width (pixels) = " << page_width << "\n"; 
  *(frame->statusWin) << "Page height (pixels) = " << page_height << "\n";

  GetPageSizeMM(&page_width, &page_height);
  *(frame->statusWin) << "Page width (mm) = " << page_width << "\n"; 
  *(frame->statusWin) << "Page height (mm) = " << page_height << "\n";

  *(frame->statusWin) << "ppiScreenX = " << ppiScreenX << "\n";
  *(frame->statusWin) << "ppiScreenY = " << ppiScreenY << "\n";
  *(frame->statusWin) << "ppiPrinterX = " << ppiPrinterX << "\n";
  *(frame->statusWin) << "ppiPrinterY = " << ppiPrinterY << "\n";
  *(frame->statusWin) << "logUnitsFactor = " << logUnitsFactor << "\n";
  *(frame->statusWin) << "Scale = " << scale << "\n";
  *(frame->statusWin) << "Overall scale = " << overallScale << "\n";
  *(frame->statusWin) << "Character width = " << char_width << "\n";
#endif

#ifdef __APP_DEBUG_VERSION__
  debug_log << "End of mswPrintout::ScaleDC() call" << "\n" << flush;
#endif
}

void mswPrintout::DrawTextPage(wxDC *dc, int page)
// Write page by page with margins, header, and page numbers
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::DrawTextPage() call" << "\n" << flush;
#endif

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Printing page " << page << " of " << last_page
		      << "\n";
#endif
  
  char dest[DBStringLength];
  INFOHOG_t *dbstring = 0;
  gxString gxbuf1;


  wxFont font, current_text_font;
  wxColor color, current_text_color;
  wxColor default_text_color(0, 0, 0);
  wxPen grid_line_pen = dc->GetPen();

  if(db_config->GetGridLineColor(&color)) grid_line_pen.SetColour(color);

  dc->SetFont(label_font);

  if(db_config->GetGridLabelTextColor(&color)) {
    dc->SetTextForeground(color); 
  }
  ScaleDC(dc);
  
  start_x = (float)(logUnitsFactor * print_config->left_margin);
  start_y = (float)(logUnitsFactor * print_config->top_margin);

  int end = page * print_config->lines_per_page;  // Last line to write
  int start = end - print_config->lines_per_page; // First line to write

  long textW, textH; // Text width and height 
  char *p;
  p = db_config->GetColName(0, dest);
  dc->GetTextExtent(p, &textW, &textH);

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Text Start = " << start_y << "\n";
  *(frame->statusWin) << "Text height = " << textH << "\n";
  *(frame->statusWin) << "Text width  = " << textW << "\n"; 
#endif
  
  float xpos = start_x; 
  float ypos = start_y;
  float x_offset = xpos;
  float lineto = xpos;
  int rightMargin = (int)print_config->right_margin;  
  float rightMarginLogical = (float)(logUnitsFactor * \
				     (page_width - rightMargin));
  int i, length;
  int cell_length[NumDataMembers];
  int char_offset[NumDataMembers];

  for(i = 0; i < NumDataMembers; i++) {
    if(db_config->print_field[i] == 1) {
      // Variable length print offset calculation 
      // NOTE: The number of pixels per cell must be slightly adjusted
      // to match the on-screen grid to the print preview window
      char_offset[i] = long(db_config->col_sizes[i]+15);
      cell_length[i] = long(db_config->col_sizes[i]/(char_width + print_config->cell_scale));

      // Set for extra characters if auto sizing is enabled
      if(db_config->auto_size == 1) cell_length[i] += 5;

      if((i == 0) && (db_config->print_row_numbers == 1)) {
	cell_length[0] += 15;
      }
      if(db_config->print_grid_labels == 1) {
	gxbuf1 = db_config->GetColName(i, dest);
	if(gxbuf1.is_null()) gxbuf1 = " ";
	PrintText(dc, gxbuf1.c_str(), cell_length[i],
		  x_offset, ypos); 
      }
      x_offset += char_offset[i];
    }
  }

  // Get the grid line settings if printing grid lines
  if(db_config->print_grid_lines == 1) {
    lineto = x_offset; // Get the current x offset position
    if(print_config->lineto_titlebar) {
      // Draw grid line to end of title bar
      gxbuf1 = db_config->GetColName((NumDataMembers-1), dest);
      if(gxbuf1.is_null()) gxbuf1 = " ";
      length = gxbuf1.length();
      lineto += int((length + 1) * char_width);
    }
    else if(print_config->lineto_endof_grid) {
      // Draw grid line to the end of the screen grid
      lineto += char_offset[(NumDataMembers - 1)];
      // Ensure that the line does not exceed the right margin
      if(lineto > rightMarginLogical) lineto = rightMarginLogical;
    }
    else if(print_config->lineto_right_margin) {
      // Draw grid line to the end of the right margin
      lineto = rightMarginLogical;
    }
    else {
      // Draw grid line to the end of the right margin by default
      lineto = rightMarginLogical;
    }
  }

  // Adjust if ypos is printing the grid labels
  if(db_config->print_grid_labels == 1) ypos += textH;

  if(db_config->print_grid_lines == 1) {
    // Draw the grid line
    dc->SetPen(grid_line_pen);
    dc->DrawLine((int)xpos , (int)ypos, 
		 (int)lineto, (int)ypos);
  }

  ypos += mswpLINE_WIDTH;

  int lines = 0;

  dc->SetFont(printer_font);

  if(db_config->GetGridTextColor(&color)) {
    dc->SetTextForeground(color); 
  }
  else {
    dc->SetTextForeground(default_text_color); 
  }

  dllistptr = dllist->GetHead();
  
  // Find the current object in the list
  int row_number = 1;
  for(i = 0; i < start; i++) {
    if(dllistptr) {
      dllistptr = dllistptr->next;
      row_number++;
    }
  }

  // Code used Ensure that no text is printed past the page footers
  dc->SetFont(footer_font);

  // Dummy page number used to calculate the text extent
  wxString sbuf("Page 1 of 1"); 
  long xFooterTextExtent, yFooterTextExtent;
  dc->GetTextExtent(sbuf, &xFooterTextExtent, &yFooterTextExtent);
  int bottomMargin = (int)print_config->bottom_margin;
  float bottomMarginLogical = (float)(logUnitsFactor * (page_height - \
							bottomMargin));
#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Footer text height = " << yFooterTextExtent << "\n";
  *(frame->statusWin) << "Bottom Margin = "
		      << (bottomMarginLogical-yFooterTextExtent) << "\n";
  *(frame->statusWin) << "Text End = "
		      << (bottomMarginLogical-yFooterTextExtent) << "\n";
  *(frame->statusWin) << "Text area = "
		      << ((bottomMarginLogical-yFooterTextExtent)-start_y)
		      << "\n";
#endif
  
  dc->SetFont(printer_font);

  while(dllistptr) {
    INFOHOG infohog(child_frame->GetPOD());
    infohog.ReadObject(dllistptr->data);
    x_offset = xpos;
    ypos += mswpLINE_WIDTH;

    for(i = 0; i < NumDataMembers; i++) {
      if(db_config->print_field[i] == 1) {
	if(infohog.GetMemberLen(i) == sizeof(INFOHOG_t)) {
	  dbstring = (INFOHOG_t *)infohog.GetMember(i);
	  if((i == 0) && (db_config->print_row_numbers == 1)) {
	    gxbuf1 << clear << "(" << row_number++ << ") " 
		   << dbstring->c_str(dest);
	  }
	  else {
	    gxbuf1 << clear << dbstring->c_str(dest);
	  }
	}
	else {
	  if((i == 0) && (db_config->print_row_numbers == 1)) {
	    gxbuf1 << clear << "(" << row_number++ << ") " 
		   << dbstring->c_str(dest);
	  }
	  else {
	    gxbuf1 = " ";
	  }
	}
	if(gxbuf1.is_null()) gxbuf1 = " ";
	if(IsHyperlink(gxbuf1)) {
	  current_text_color = dc->GetTextForeground();
	  current_text_font = dc->GetFont();
	  if(db_config->GetHyperlinkColor(&color)) {
	    dc->SetTextForeground(color);	      
	  }
	  else {
	    dc->SetTextForeground(*frame->hyperlinkColor);    
	  }
	  if(db_config->GetHyperlinkFont(&font)) {
	    dc->SetFont(font);
	  }
	  else {
	    dc->SetFont(*frame->hyperlinkFont);
	  }
	}
	PrintText(dc, gxbuf1.c_str(), cell_length[i], x_offset, ypos); 
	if(IsHyperlink(gxbuf1)) {
	  dc->SetTextForeground(current_text_color);	      
	  dc->SetFont(current_text_font);
	}
	x_offset += char_offset[i];
      }
    }
    
    // NOTE: This is a safty feature. Using member 0 should always be safe
    // provided the call has initialized the infohog object correctly.
    if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) break;

    dbstring = (INFOHOG_t *)infohog.GetMember(0);
    gxbuf1 << clear << dbstring->c_str(dest);
    dc->GetTextExtent(gxbuf1.c_str(), &textW, &textH);
    ypos += textH;
    ypos += mswpLINE_WIDTH;
    if(db_config->print_grid_lines == 1) {
      dc->DrawLine((int)xpos , (int)ypos, (int)lineto, (int)ypos);
    }
    
    // TODO: Add draw skip line code here
    /*
    dc->SetBrush(* wxCYAN_BRUSH);
    dc->DrawRectangle((int)xpos , (int)ypos, 
                      (int)lineto, (textH+mswpLINE_WIDTH));
    */

    ypos += mswpLINE_WIDTH;
    lines++;
    if(ypos >= (bottomMarginLogical-yFooterTextExtent)) {
      if(print_config->lines_per_page != lines) {
	print_config->lines_per_page = lines;
#ifdef __APP_DEBUG_VERSION__
	*(frame->statusWin) << "Lines per page = "
			    << print_config->lines_per_page << "\n";
#endif
	int minPage, maxPage, selPageFrom, selPageTo;
	GetPageInfo(&minPage, &maxPage, &selPageFrom, &selPageTo);
      }
      break;
    }
    dllistptr = dllistptr->next;
  }

  if(db_config->print_page_header) {  
    wxString document_name, document_date;
    if(!db_config->print_doc_name.is_null()) {
      document_name = db_config->print_doc_name.c_str(dest);
    }
    else {
      document_name = db_config->database_name.c_str(dest);
    }

    gxString gbuf;
    if(!db_config->print_doc_custom_header.is_null()) {
      gbuf = db_config->print_doc_custom_header.c_str(dest);
      gbuf.ReplaceString("$NAME", db_config->print_doc_name.c_str(dest));
      gbuf.ReplaceString("$name", db_config->print_doc_name.c_str(dest));
      document_name = gbuf.c_str();
    }
 
    if(db_config->print_doc_time_and_date == 1) {
      SysTime sys_time;
      document_date = sys_time.GetSystemDateTime();
    }
    WritePageHeader(dc, document_name, document_date);
  }
  if(db_config->print_page_footer) {
    WritePageNumber(dc, page);
  }

#ifdef __APP_DEBUG_VERSION__
  debug_log << "End of mswPrintout::DrawTextPage() call" << "\n" << flush;
#endif
}

void mswPrintout::SetHeaderFont()
// Set the header font
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::SetHeaderFont() call" << "\n" << flush;
#endif

  if(!db_config->GetPrintHeaderFont(&header_font)) {
    wxFont new_font(mswpHEADER_FONT_SIZE, wxSWISS, wxNORMAL, wxNORMAL);
    header_font = new_font;
  }
}
 
void mswPrintout::SetFooterFont()
// Set the footer font
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::SetFooterFont() call" << "\n" << flush;
#endif

  if(!db_config->GetPrintFooterFont(&footer_font)) {
    wxFont new_font(mswpFOOTER_FONT_SIZE, wxSWISS, wxNORMAL, wxNORMAL);
    footer_font = new_font;
  }
}

void mswPrintout::WritePageHeader(wxDC *dc, wxString &doc_name,
				  wxString &doc_date)
// Writes header on top of page. Margin units are in millimetres.
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::WritePageHeader() call" << "\n" << flush;
#endif

  wxColour color;
  dc->SetFont(header_font);
  
  if(db_config->GetPrintHeaderColor(&color)) {
    dc->SetTextForeground(color); 
  }

  // Offset the margins to move text above the first line
  int leftMargin  = (int)print_config->left_margin / 2;
  int topMargin   = (int)print_config->top_margin / 2;
  int rightMargin = (int)print_config->right_margin / 2;

  float topMarginLogical = (float)(logUnitsFactor * topMargin);

  long xExtentName, yExtentName, xExtentDate, yExtentDate;
  float xPos, offset;
  dc->GetTextExtent(doc_name, &xExtentName, &yExtentName);
  if(doc_date != "") { // Printing document name and date strings
    dc->GetTextExtent(doc_date, &xExtentDate, &yExtentDate);

    // Draw the document's name left justified
    PrintText(dc, doc_name, start_x, topMarginLogical);

    // Draw the document's date right justified
    if(print_config->orientation == 0) // Landscape printing
      offset = (logUnitsFactor * page_width) - \
       (logUnitsFactor * (print_config->right_margin + mswpPRINTABLE_OFFSET_X));
    else
      offset = (logUnitsFactor * page_width) - \
	(logUnitsFactor * (print_config->right_margin + mswpPRINTABLE_OFFSET_X));
      
    xPos = (float)(offset - xExtentDate);
    PrintText(dc, doc_date, xPos, topMarginLogical);
  }
  else { // Center the document's name
    xPos = (float)(((((page_width - leftMargin - rightMargin)/2.0)
		     +leftMargin)*logUnitsFactor) - (xExtentName/2.0));
    PrintText(dc, doc_name, xPos, topMarginLogical);
  }
}

void mswPrintout::WritePageNumber(wxDC *dc, int pagenum)
// Writes page number on bottom of page. Margin units are in millimetres.
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::WritePageNumber() call" << "\n" << flush;
#endif

  char dest[DBStringLength];
  wxColour color;
  dc->SetFont(footer_font);

  if(db_config->GetPrintFooterColor(&color)) {
    dc->SetTextForeground(color); 
  }

  // Offset the margins to move text below the last line
  int leftMargin = (int)print_config->left_margin;
  int bottomMargin = (int)print_config->bottom_margin / 2;
  int rightMargin = (int)print_config->right_margin;

  float bottomMarginLogical = (float)(logUnitsFactor * (page_height - \
							bottomMargin));
  long xExtent, yExtent;
  wxString sbuf;
  sbuf.Printf("Page %d of %d", pagenum, last_page);

  gxString gbuf, pagenum_str, last_page_str;
  if(!db_config->print_doc_custom_footer.is_null()) {
    gbuf = db_config->print_doc_custom_footer.c_str(dest);      
    pagenum_str << clear << pagenum;
    last_page_str << clear << last_page;
    gbuf.ReplaceString("$n", pagenum_str.c_str());
    gbuf.ReplaceString("$N", pagenum_str.c_str());
    gbuf.ReplaceString("$ln", last_page_str.c_str());
    gbuf.ReplaceString("$LN", last_page_str.c_str());
    sbuf = gbuf.c_str();
  }

  dc->GetTextExtent(sbuf, &xExtent, &yExtent);
  float xPos = (float)(((((page_width - leftMargin - rightMargin)/2.0)
			 +leftMargin)*logUnitsFactor) - (xExtent/2.0));

  bottomMarginLogical -= yExtent;
  PrintText(dc, sbuf, xPos, bottomMarginLogical);
}

void mswPrintout::PrintText(wxDC *dc, char *s, int max_length,
			    float xpos, float ypos)
{
  wxString sbuf(s);
  PrintText(dc, sbuf, max_length, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, char *s, float xpos, float ypos)
{
  wxString sbuf(s);
  PrintText(dc, sbuf, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, wxString &s, int max_length,
			   float xpos, float ypos)
// Draw text in the device context.
{
  if((s == "") || (max_length <= 0)) return;

  if((int)s.length() > max_length) {
    s.Truncate(max_length);
    dc->DrawText(s,(int)xpos, (int)ypos);
  }
  else {
    dc->DrawText(s, (int)xpos, (int)ypos);
  }
}

void mswPrintout::PrintText(wxDC *dc, wxString &s, float xpos, float ypos)
// Draw text in the device context.
{
  if(s == "") return;
  dc->DrawText(s, (int)xpos, (int)ypos);
}

void mswPrintout::PrintText(wxDC *dc, char c, float xpos, float ypos)
{
  char s[2];
  s[0] = c; // Copy char into a string buffer
  s[1] = 0; // Null terminate the string
  wxString sbuf(s);
  PrintText(dc, sbuf, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, int i, int max_length,
			   float xpos, float ypos)
{
  wxString sbuf;
  sbuf.Printf("%d", i);
  PrintText(dc, sbuf, max_length, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, long i, int max_length,
			   float xpos, float ypos)
{
  wxString sbuf;
  sbuf.Printf("%d", (int)i);
  PrintText(dc, sbuf, max_length, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, double i, int max_length,
			   float xpos, float ypos)
{
  wxString sbuf;
  sbuf.Printf("%.2f", i);
  PrintText(dc, sbuf, max_length, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, float i, int max_length,
			   float xpos, float ypos)
{
  wxString sbuf;
  sbuf.Printf("%.2f", i);
  PrintText(dc, sbuf, max_length, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, int i, float xpos, float ypos)
{
  wxString sbuf;
  sbuf.Printf("%d", i);
  PrintText(dc, sbuf, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, long i, float xpos, float ypos)
{
  wxString sbuf;
  sbuf.Printf("%d", (int)i);
  PrintText(dc, sbuf, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, double i, float xpos, float ypos)
{
  wxString sbuf;
  sbuf.Printf("%.2f", i);
  PrintText(dc, sbuf, xpos, ypos);
}

void mswPrintout::PrintText(wxDC *dc, float i, float xpos, float ypos)
{
  wxString sbuf;
  sbuf.Printf("%.2f", i);
  PrintText(dc, sbuf, xpos, ypos);
}

int mswPrintout::InitPrintout()
// Helper function used to initialize printer parameters that are
// dependent on the wxPrinter DC for the selected printer. Returns
// false if an error occurs or true if successful. NOTE: The wxPrinter
// DC for this object is not initialized by the printing frame until
// the actual printing process starts. Therefore DC dependent parameters
// such as the number of lines per page (required prior to printing text
// and database files) cannot be accurately calculated. To overcome this
// without modifing the wxPrinting framework this OS dependent function
// was added to initialize printer parameter using the printer DC for the
// currently selected printer. This function must be called prior to
// constucting the mswPrintout object that will be used to do the
// actual printing.
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::InitPrintout() call" << "\n" << flush;
#endif

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Initializing the printout parameters" << "\n";
#endif

  // Get a device context for the currently selected printer
#if defined (__WIN32__)
  wxPrinterDC *dc = new wxPrinterDC(print_config->print_data);
#else
  wxPostScriptDC *dc = new wxPostScriptDC(print_config->print_data);
#endif
  
  if(!child_frame) return 0;
  if(!db_config) return 0;

  if((!dc) || (!dc->IsOk())) {
    if (dc) delete dc;
    return 0;
  }

  int logPPIScreenX = 0;
  int logPPIScreenY = 0;
  int logPPIPrinterX = 0;
  int logPPIPrinterY = 0;

#if defined (__WIN32__) // WIN32 device context specifics
  HDC hdc = ::GetDC(NULL);
  logPPIScreenX = ::GetDeviceCaps(hdc, LOGPIXELSX);
  logPPIScreenY = ::GetDeviceCaps(hdc, LOGPIXELSY);
  logPPIPrinterX = ::GetDeviceCaps((HDC)dc->GetHDC(), LOGPIXELSX);
  logPPIPrinterY = ::GetDeviceCaps((HDC)dc->GetHDC(), LOGPIXELSY);
  ::ReleaseDC(NULL, hdc);
#else
  logPPIScreenX = 100;
  logPPIScreenY = 100;
  // TODO: Fix below
  //  logPPIPrinterX = wxPostScriptDC::GetResolution();
  //  logPPIPrinterY = wxPostScriptDC::GetResolution();
#endif
  
  if((logPPIPrinterX == 0) || (logPPIPrinterY == 0)) {
    delete dc;
    return 0;
  }

  SetPPIScreen(logPPIScreenX, logPPIScreenY);
  SetPPIPrinter(logPPIPrinterX, logPPIPrinterY);

  int w, h;
  dc->GetSize(&w, &h);
  SetPageSizePixels((int)w, (int)h);
  dc->GetSizeMM(&w, &h);
  SetPageSizeMM((int)w, (int)h);

  // Set the text font for the item bar
  dc->SetFont(label_font); 

  ScaleDC(dc);
  float ypos = logUnitsFactor * print_config->top_margin;
  wxString sbuf("Item Bar Font");
  long textW, textH; // Text width and height 
  dc->GetTextExtent(sbuf, &textW, &textH);

  // Account for the item bar plus the one line separator
  ypos += textH;
  ypos += mswpLINE_WIDTH;
  
  int bottomMargin = (int)print_config->bottom_margin;
  float bottomMarginLogical = (float)(logUnitsFactor * (page_height - \
							bottomMargin));
  long xFooterTextExtent = 0;
  long yFooterTextExtent = 0;

  if(db_config->print_page_footer) {
    dc->SetFont(footer_font);
    sbuf = "Page Number Font";
    dc->GetTextExtent(sbuf, &xFooterTextExtent, &yFooterTextExtent);
  }

  // Set the text font for the objects
  dc->SetFont(printer_font);
  sbuf = "Printer Font";
  dc->GetTextExtent(sbuf, &textW, &textH);

  // Count the actual number of lines per page
  int lines = 0;
  while(1) {
    ypos += (textH + (mswpLINE_WIDTH*3));
    lines++;
    if(ypos >= (bottomMarginLogical-yFooterTextExtent)) {
      print_config->lines_per_page = lines;
      break;
    }
  }

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Text Start = "
		      << (logUnitsFactor * print_config->top_margin) << "\n";
  *(frame->statusWin) << "Text height = " << textH << "\n";
  *(frame->statusWin) << "Footer text height = " << yFooterTextExtent << "\n";
  *(frame->statusWin) << "Bottom Margin = "
		      << (bottomMarginLogical-yFooterTextExtent) << "\n";
    
  *(frame->statusWin) << "Lines per page = " << print_config->lines_per_page
    		      << "\n";
  *(frame->statusWin) << "Text End = "
		      << (bottomMarginLogical - yFooterTextExtent) << "\n";
  float page_top = (float)(logUnitsFactor * print_config->bottom_margin);
  float page_bottom = bottomMarginLogical-yFooterTextExtent;
  *(frame->statusWin) << "Text area = " << (page_bottom - page_top) << "\n";
#endif

  delete dc;
  return 1;
}

int mswPrintout::InitPrintPreview()
// Helper function used to initialize print preview parameters that are
// dependent on the wxPrinter DC for the selected printer. Returns
// false if an error occurs or true if successful. NOTE: This function
// is similar to the mswPrintout::InitPrintout() function but uses
// calculated values in order to roughly match the Printer DC to the
// page display on the screen. This function must be called prior to
// constucting the mswPrintout objects that will be used to do the
// actual printing previewing.
{
#ifdef __APP_DEBUG_VERSION__
  debug_log << "mswPrintout::InitPrintPreview() call" << "\n" << flush;
#endif

#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Initializing the print preview parameters" << "\n";
#endif

  // Get a device context for the currently selected printer
#if defined (__WIN32__)
  wxPrinterDC *dc = new wxPrinterDC(print_config->print_data);
#else
  wxPostScriptDC *dc = new wxPostScriptDC(print_config->print_data);
#endif

  if((!dc) || (!dc->IsOk())) {
    if (dc) delete dc;
    return 0;
  }

  if(!child_frame) return 0;
  if(!db_config) return 0;

  int logPPIScreenX = 0;
  int logPPIScreenY = 0;
  int logPPIPrinterX = 0;
  int logPPIPrinterY = 0;

#if defined (__WIN32__) // WIN32 device context specifics
  HDC hdc = ::GetDC(NULL);
  logPPIScreenX = ::GetDeviceCaps(hdc, LOGPIXELSX);
  logPPIScreenY = ::GetDeviceCaps(hdc, LOGPIXELSY);
  logPPIPrinterX = ::GetDeviceCaps((HDC)dc->GetHDC(), LOGPIXELSX);
  logPPIPrinterY = ::GetDeviceCaps((HDC)dc->GetHDC(), LOGPIXELSY);
  ::ReleaseDC(NULL, hdc);
#else
  logPPIScreenX = 100;
  logPPIScreenY = 100;
  // TODO: Fix below
  //  logPPIPrinterX = wxPostScriptDC::GetResolution();
  // logPPIPrinterY = wxPostScriptDC::GetResolution();
#endif
  
  if((logPPIPrinterX == 0) || (logPPIPrinterY == 0)) {
    delete dc;
    return 0;
  }
  
  SetPPIScreen(logPPIScreenX, logPPIScreenY);
  SetPPIPrinter(logPPIPrinterX, logPPIPrinterY);
  
  int w, h;
  dc->GetSize(&w, &h);
  SetPageSizePixels((int)w, (int)h);
  dc->GetSizeMM(&w, &h);
  SetPageSizeMM((int)w, (int)h);
  
  ScaleDC(dc);
  float font_scale = (float)1.7;
  int bottomMargin = (int)print_config->bottom_margin;
  float bottomMarginLogical = (float)(logUnitsFactor * (page_height - \
							bottomMargin));
  int yFooterTextExtent = 0;
  if(db_config->print_page_footer) {
    yFooterTextExtent = (int)(footer_font.GetPointSize() + (footer_font.GetPointSize()/font_scale));
  }
    
  float page_top = (float)(logUnitsFactor * print_config->top_margin);
  float page_bottom = bottomMarginLogical-yFooterTextExtent;

  int textH;
  textH = (int)(printer_font.GetPointSize() + (printer_font.GetPointSize()/font_scale)); 

  // Account for the item bar plus the one line separator
  page_top += textH;
  page_top += mswpLINE_WIDTH;
  float page_area = page_bottom - page_top;
  print_config->lines_per_page = int(page_area / (textH + mswpLINE_WIDTH*3));
#ifdef __APP_DEBUG_VERSION__
  *(frame->statusWin) << "Text Start = " << page_top << "\n";
  *(frame->statusWin) << "Text height = " << textH << "\n";
  *(frame->statusWin) << "Footer text height = " << yFooterTextExtent << "\n";
  *(frame->statusWin) << "Bottom Margin = "
		      << (bottomMarginLogical-yFooterTextExtent) << "\n";
    
  *(frame->statusWin) << "Lines per page = " << print_config->lines_per_page
    		      << "\n";
  *(frame->statusWin) << "Text End = "
		      << (bottomMarginLogical-yFooterTextExtent) << "\n";
  *(frame->statusWin) << "Text area = " << page_area << "\n";
#endif

  delete dc;
  return 1;
}

#endif // __USE_MSW_PRINTING__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
