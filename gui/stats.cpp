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

Database statistics functions
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

int TestDatabaseForRunningThreads() 
// Function used to test for running threads before performing any
// database operation. Returns true if threads are running of false
// if no threads are currently running.
{
  // Single threaded application
  return 0; // No threads are currently running
}

int TestDatabase(int test_num_entries, int test_admin_rights, 
		 int test_threads)
// Test the database to ensure that the database has been opened and
// contains entries if the "test_num_entries" variable id true. Returns
// ture if the database tests good or false if any errors are found.
// The "test_admin_rights" variable is used by functions that require
// write access to test the database before attempting a write operation.
{
  if(test_threads) { // Test for running threads
    if(TestDatabaseForRunningThreads()) return 0;
  }

  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database frame is currently open\n");
    return 0;
  }
  if(!child_frame->GetPOD()) {
    ProgramError->Message("No database is currently open\n");
    return 0;
  }

  if(child_frame->GetPOD()->TestDatabase() != 0) { 
    int yn = wxMessageBox("The database has been modified by another \
process.\nDo you wish to reload the database grid?\n", "Program Message",
			  wxYES_NO | wxCENTRE | wxICON_QUESTION, frame);
    if(yn == wxYES) {
      child_frame->GridFrame()->ReloadGrid(0);
    }
  }

  if(test_admin_rights) {
    if(!child_frame->DBParms()->admin_rights) {
      ProgramError->Message("You do not have write access to this database\n");
      return 0;
    }
  }
  
  long num_entries = (long)child_frame->GetPOD()->Index()->NumKeys();
  if(test_num_entries) {
    if(num_entries == 0) {
      ProgramError->Message("The database index is empty\n");
      return 0;
    }
  }

  return 1;
}

void EchoDBError(POD *pod, wxTextCtrl *statusWin)
// Echo the last reported data and index file errors to the
// specified text window.
{
#ifdef __APP_DEBUG_VERSION__
  *(statusWin) << "DataFile->" << pod->DataFileExceptionMessage() << "\n";
  *(statusWin) << "IndexFile->" << pod->IndexFileExceptionMessage() << "\n";
#else
  *(statusWin) << "(1) " << pod->DataFileExceptionMessage() << "\n";
  *(statusWin) << "(2) " << pod->IndexFileExceptionMessage() << "\n";
#endif
}

int CheckError(gxDatabase *f, wxTextCtrl *statusWin)
// Check the last reported error. Prints the error
// and returns a non-zero value if an error condition
// was reached.
{
  if(f->GetDatabaseError() != gxDBASE_NO_ERROR) {
    *(statusWin) << f->DatabaseExceptionMessage() << "\n";
    return 1;
  }
  return 0;
}

int CheckError(gxDatabaseError err, wxTextCtrl *statusWin)
// Check the last reported error. Prints the error
// and returns a non-zero value if an error condition 
// was reached.
{
  if(err != gxDBASE_NO_ERROR) {
    *(statusWin) << gxDatabaseExceptionMessage(err) << "\n";
    return 1;
  }
  return 0;
}

void PrintFontInfo(gxString &mbuf, wxFont *font)
{
  if(font->GetFaceName().IsNull()) {
    mbuf << "No-Name";
  }
  else {
    mbuf << (const char *)font->GetFaceName().c_str();    
  }
  mbuf << ", ";
  mbuf << font->GetPointSize();
  mbuf << ", ";
  switch(font->GetFamily()) { 
    case wxDEFAULT:
      mbuf << "Default-Family";
      break;
    case wxDECORATIVE:
      mbuf << "Decorative";
      break;
    case wxROMAN:	
      mbuf << "Serif";
      break;
    case wxSCRIPT:
      mbuf << "Handwriting";
      break;
    case wxSWISS:
      mbuf << "Sans-serif";
      break;
      
    case wxMODERN:
      mbuf <<"Fixed";
      break;
    default:
      mbuf << "No-Family";
      break;
  }
  mbuf << ", ";

  switch(font->GetStyle()) {
    case wxNORMAL:
      mbuf << "Normal";
      break;
    case wxSLANT:
      mbuf << "Slant";
      break;
    case wxITALIC:
      mbuf << "Italic";
      break;
    default:
      mbuf << "No-Style";
      break;
  }
  mbuf << ", ";

  switch(font->GetWeight()) {
    case wxNORMAL:
      mbuf << "Normal";
      break;
    case wxLIGHT:
      mbuf << "Light";
      break;
    case wxBOLD:
      mbuf << "Bold";
      break;
    default:
      mbuf << "No-Weight";
      break;
  }
  mbuf << ", ";
  if(font->GetUnderlined()) {
    mbuf << "Underlined";
    mbuf << ", ";
  }
  mbuf << font->GetDefaultEncoding() << " Encoding";
}

void DisplayDatabaseConfig(CryptDBDocument *child_frame, wxTextCtrl *statusWin)
{
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;
  unsigned i;
  char dest[DBStringLength];

  *(statusWin) << "\n";
  *(statusWin) << "----- Database configuration -----" << "\n";
  gxString mbuf;
  wxColour color;
#ifdef __APP_DEBUG_VERSION__
  mbuf << "Size of db_config = " << db_config->SizeOf() << "\n";
#endif
  mbuf << "Auto size cells = " << db_config->auto_size << "\n";
  mbuf << "Cell overflow = " << db_config->cell_overflow << "\n";
  mbuf << "View grid labels = " << db_config->view_labels << "\n";
  mbuf << "View grid lines = " << db_config->view_grid_lines << "\n";

  if(db_config->GetGridLineColor(&color)) {
    mbuf << "Grid line color = " 
	 << color.Red() << ", " << color.Green() << ", " << color.Blue() 
	 << "\n";
  }
  else {
    mbuf << "Grid line color = DEFAULT" << "\n";
  }

  if(db_config->GetGridBackgroundColor(&color)) {
    mbuf << "Grid background color = " 
	 << color.Red() << ", " << color.Green() << ", " << color.Blue() 
	 << "\n";
  }
  else {
    mbuf << "Grid background color = DEFAULT" << "\n";
  }
  if(db_config->GetGridTextColor(&color)) {
    mbuf << "Grid text color = " 
	 << color.Red() << ", " << color.Green() << ", " << color.Blue() 
	 << "\n";
  }
  else {
    mbuf << "Grid text color = DEFAULT" << "\n";
  }

  if(db_config->GetGridLabelBackgroundColor(&color)) {
    mbuf << "Grid label background = " 
	 << color.Red() << ", " << color.Green() << ", " << color.Blue() 
	 << "\n";
  }
  else {
    mbuf << "Grid label background color = DEFAULT" << "\n";
  }

  if(db_config->GetGridLabelTextColor(&color)) {
    mbuf << "Grid label text color = " 
	 << color.Red() << ", " << color.Green() << ", " << color.Blue() 
	 << "\n";
  }
  else {
    mbuf << "Grid label text color = DEFAULT" << "\n";
  }

  if(db_config->GetHyperlinkColor(&color)) {
    mbuf << "Hyperlink color = " 
	 << color.Red() << ", " << color.Green() << ", " << color.Blue() 
	 << "\n";
  }
  else {
    mbuf << "Hyperlink color = DEFAULT" << "\n";
  }

  wxFont font;
  if(db_config->GetGridLabelFont(&font)) {
    mbuf << "Grid label font = ";
    PrintFontInfo(mbuf, &font);
    mbuf << "\n";
  }
  else {
    mbuf << "Grid label font = DEFAULT" << "\n";
  }
  if(db_config->GetGridTextFont(&font)) {
    mbuf << "Grid text font = ";
    PrintFontInfo(mbuf, &font);
    mbuf << "\n";
  }
  else {
    mbuf << "Grid text font = DEFAULT" << "\n";
  }
  if(db_config->GetHyperlinkFont(&font)) {
    mbuf << "Hyperlink font = ";
    PrintFontInfo(mbuf, &font);
    mbuf << "\n";
  }
  else {
    mbuf << "Hyperlink font = DEFAULT" << "\n";
  }

  mbuf << "View row numbers = " << db_config->view_row_numbers << "\n";
  mbuf << "View skip lines = " << db_config->view_skip_lines << "\n";

  if(db_config->GetSkipLineColor(&color)) {
    mbuf << "Skip line color = " 
	 << color.Red() << ", " << color.Green() << ", " << color.Blue() 
	 << "\n";
  }
  else {
    mbuf << "Skip line color = DEFAULT" << "\n";
  }

  char dchar = db_config->GetTextDelimiter();
  mbuf << "Text delimiter = ";
  switch(dchar) {
    case '\t':
      mbuf << "Tab";
      break;
    case ' ':
      mbuf << "Space";
      break;
    default:
      mbuf << dchar;
      break;
  }
  mbuf << "\n";

  mbuf << "\n";
  mbuf << "----- Print configuration -----" << "\n";

  if(!db_config->print_doc_name.is_null()) {
    mbuf << "Document name = " << db_config->print_doc_name.c_str(dest) << "\n";
  }
  else {
    mbuf << "Document name = " << "NOT SET" << "\n";
  }
  if(db_config->print_grid_lines == 1) {
    mbuf << "Print grid lines = TRUE" << "\n";
  }
  else {
    mbuf << "Print grid lines = FALSE" << "\n";
  }
  if(db_config->print_grid_labels == 1) {
    mbuf << "Print grid labels = TRUE" << "\n";
  }
  else {
    mbuf << "Print grid labels = FALSE" << "\n";
  }
  if(db_config->print_row_numbers == 1) {
    mbuf << "Print row numbers = TRUE" << "\n";
  }
  else {
    mbuf << "Print row numbers = FALSE" << "\n";
  }
  if(db_config->print_doc_time_and_date == 1) {
    mbuf << "Print time and date stamp = TRUE" << "\n";
  }
  else {
    mbuf << "Print time and date stamp = FALSE" << "\n";
  }
  if(db_config->print_page_header == 1) {
    mbuf << "Print page header = TRUE" << "\n";
    if(!db_config->print_doc_custom_header.is_null()) {
      mbuf << "Custom header = " 
	   << db_config->print_doc_custom_header.c_str(dest)
	   << "\n";
    }
    if(db_config->GetPrintHeaderColor(&color)) {
      mbuf << "Header text color = " 
	   << color.Red() << ", " << color.Green() << ", " << color.Blue() 
	   << "\n";
    }
    else {
      mbuf << "Header text color = DEFAULT" << "\n";
    }
    if(db_config->GetPrintHeaderFont(&font)) {
      mbuf << "Header font = ";
      PrintFontInfo(mbuf, &font);
      mbuf << "\n";
    }
    else {
      mbuf << "Header font = DEFAULT" << "\n";
    }
  }
  else {
    mbuf << "Print page header = FALSE" << "\n";
  }
  if(db_config->print_page_footer == 1) {
    mbuf << "Print page footer = TRUE" << "\n";
    if(!db_config->print_doc_custom_footer.is_null()) {
      mbuf << "Custom footer = " 
	   << db_config->print_doc_custom_footer.c_str(dest)
	   << "\n";
    }
    if(db_config->GetPrintFooterColor(&color)) {
      mbuf << "Footer text color = " 
	   << color.Red() << ", " << color.Green() << ", " << color.Blue() 
	   << "\n";
    }
    else {
      mbuf << "Footer text color = DEFAULT" << "\n";
    }
    if(db_config->GetPrintFooterFont(&font)) {
      mbuf << "Footer font = ";
      PrintFontInfo(mbuf, &font);
      mbuf << "\n";
    }
    else {
      mbuf << "Footer font = DEFAULT" << "\n";
    }
  }
  else {
    mbuf << "Print page footer = FALSE" << "\n";
  }
  if(db_config->print_orientation == 0) {
    mbuf << "Page orientation = landscape" << "\n";
  }
  if(db_config->print_orientation == 1) {
    mbuf << "Page orientation = portrait" << "\n";
  }

  mbuf << "Paper size = ";
  switch((int)db_config->print_paper_size) {
    case 0:
      mbuf << "letter";
      break;
    case 1:
      mbuf << "legal";
      break;
    case 2:
      mbuf << "tabloid";
      break;
    case 3: 
      mbuf << "A3";
      break;
    case 4: 
      mbuf << "A4";
      break;
   default:
     mbuf << "letter";
     break;
  }
  mbuf << "\n";

  mbuf << "Left margin = " << db_config->print_left_margin_size << "\n";
  
  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    if(db_config->print_field[i] == 1) {
      mbuf << "Include field " << (i+1) << "\n"; 
    }
    else {
      mbuf << "Exclude field " << (i+1) << "\n"; 
    }
  }

  // Display the config
  *(statusWin) << mbuf.c_str();
}

void DisplayDataFileStats(POD *pod, wxTextCtrl *statusWin)
{
  gxDatabase *f = pod->OpenDataFile();
  
  *(statusWin) << "\n";
  const char *fname = f->DatabaseName();
  char rev_letter = f->GetRevLetter();
  char *signature = f->GetSignature();

  *(statusWin) << "----- Data file statistics -----" << "\n";
  *(statusWin) << "File Name: " << fname << "\n";
  *(statusWin) << "File Signature: " << signature << "\n"; 
  if(rev_letter == ' ' || rev_letter == 0)
    *(statusWin) << "Revision Letter: " << "Zero" << "\n";
  else
    *(statusWin) << "Revision Letter: " << rev_letter << "\n";
  *(statusWin) << "Library Version: " << (long)f->GetVersion() << "\n";
  *(statusWin) << "File Header Size: " << (long)f->FileHeaderSize() << "\n";
  *(statusWin) << "Block Header Size: " << (long)f->BlockHeaderSize() << "\n";
  *(statusWin) << "Static area size: " << (long)f->StaticArea() << "\n";
  *(statusWin) << "Static file size: " << (long)f->FileSize(fname) << "\n";
  *(statusWin) << "File Status: ";
  if(f->IsOpen())
    *(statusWin) << "Open ";
  else
    *(statusWin) << "Closed ";
  if(f->ReadyForWriting())
    *(statusWin) << "Read/Write ";
  else
    *(statusWin) << "Read/Only ";
  *(statusWin) << "\n";

  *(statusWin) << "Free Space: " << (long)f->GetDatabaseFreeSpace() << "\n";
  *(statusWin) << "End of File: " << (long)f->GetEOF() << "\n";
  *(statusWin) << "Heap Start: " << (long)f->GetHeapStart() << "\n";
  *(statusWin) << "Highest Block: " << (long)f->GetHighestBlock() << "\n";
  *(statusWin) << "Total Blocks: " << (long)f->TotalBlocks() << "\n";

  FAU tl, dl, rm;
  tl = f->DeleteBlocks(&dl, &rm);
  *(statusWin) << "Deleted/Removed: " << (long)dl << "/"
	       << (long)rm << " (" << (long)tl << ")" << "\n";

  gxFileLockHeader flh;
  switch(rev_letter) {
    case 'b': case 'B': case 'c': case 'C':
      f->ReadFileLockHdr(flh);
      *(statusWin) << "File Lock Protect: "
		   << (int)flh.file_lock_protect << "\n";
      *(statusWin) << "File Read Lock Value: "
		   << (int)flh.file_read_lock << "\n";
      *(statusWin) << "File Write Lock Value: "
		   << (int)flh.file_write_lock << "\n";
      break;
    default:
      break;
  }
  delete signature;
}

void DisplayIndexFileStats(POD *pod, wxTextCtrl *statusWin)
{
  gxBtree *btx = pod->Index();
  gxDatabase *f = btx->gxDatabasePtr();  
  const char *fname = f->DatabaseName();

  char rev_letter = f->GetRevLetter();

  *(statusWin) << "\n";
  *(statusWin) << "----- Index file statistics -----" << "\n";
  *(statusWin) << "File Name: " << fname << "\n";
  *(statusWin) << "File Status: ";
  if(f->IsOpen())
    *(statusWin) << "Open ";
  else
    *(statusWin) << "Closed ";
  if(f->ReadyForWriting())
    *(statusWin) << "Read/Write ";
  else
    *(statusWin) << "Read/Only ";
  *(statusWin) << "\n";

  *(statusWin) << "Static area size: " << (long)f->StaticArea() << "\n";
  *(statusWin) << "Root address: " << (long)btx->Root() << "\n";
  *(statusWin) << "Number of nodes: " << (long)btx->NumNodes() << "\n";
  
  *(statusWin) << "Number of entries: " << (long)btx->NumKeys() << "\n";

  *(statusWin) << "Btree order: " << (long)btx->NodeOrder() << "\n";
  *(statusWin) << "Btree height: " << (long)btx->BtreeHeight() << "\n";
  gxFileLockHeader flh;
  switch(rev_letter) {
    case 'b': case 'B': case 'c': case 'C':
      f->ReadFileLockHdr(flh);
      *(statusWin) << "File Lock Protect: "
		   << (int)flh.file_lock_protect << "\n";
      *(statusWin) << "File Read Lock Value: "
		   << (int)flh.file_read_lock << "\n";
      *(statusWin) << "File Write Lock Value: "
		   << (int)flh.file_write_lock << "\n";
      break;
    default:
      break;
  }
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

