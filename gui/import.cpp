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

Database import functions
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

// 10/25/2026: Lines longer than this are skipped with a message. They
// were split into two lines at 1024 bytes and imported as two records.
const unsigned ImportMaxLine = 65536;

struct ImportLineBuffer { // Frees the line buffer on every return path
  ImportLineBuffer(unsigned n) { p = new char[n]; p[0] = 0; }
  ~ImportLineBuffer() { delete[] p; }
  char *p;
};

static int ReadImportLine(DiskFileB &stream, char *buf, unsigned buf_len, int &bad_line)
// 10/25/2026: Read one line into buf. Sets bad_line if the line is too
// long or has a NUL character, which used to cut the line short. The
// rest of a long line is read and dropped. Returns 0 at end of file.
{
  unsigned len = 0;
  char c;
  int got = 0;
  bad_line = 0;
  while(stream.df_Read(&c, 1) == DiskFileB::df_NO_ERROR) {
    got = 1;
    if(c == '\n') break;
    if(c == 0) bad_line = 1;
    if(len < buf_len - 1) buf[len++] = c;
    else bad_line = 1;
  }
  buf[len] = 0;
  return got;
}

static int ReadImportRecord(DiskFileB &stream, char *buf, unsigned buf_len, int &bad_line,
			    gxString &rec, int &rfc)
// 10/27/2026: Read one CSV record. A quoted field with line breaks
// (written by this version's export or a spreadsheet) spans lines, which
// are joined with LF. If the quote does not close within the size of a
// record, the first line is read alone as before, for older files with a
// value that starts with a quote. rfc is set if the record has a quoted
// field with a line break or a doubled quote. Returns 0 at end of file.
{
  rec.Clear();
  rfc = 0;
  if(!ReadImportLine(stream, buf, buf_len, bad_line)) return 0;
  if(bad_line) return 1;
  rec = buf;
  rec.TrimTrailing('\n');
  rec.TrimTrailing('\r');
  gxString fields[NumDataMembers + 2];
  unsigned num = 0;
  int open = 0;
  if(ParseCSVRecord(rec, fields, NumDataMembers + 2, num, open, rfc) == 0) {
    for(unsigned i = 0; i < NumDataMembers + 2; i++) SecureWipe(fields[i]);
    return 1;
  }
  // Join the next lines while the quoted field is open
  const unsigned max_record = NumDataMembers * (2 * DBStringLength + 3);
  FAU_t next_line = stream.df_Tell();
  gxString joined(rec);
  int ok = 0;
  for(int lines = 0; lines < DBStringLength && joined.length() <= max_record; lines++) {
    int bad = 0;
    if(!ReadImportLine(stream, buf, buf_len, bad) || bad) break;
    gxString line(buf);
    line.TrimTrailing('\n');
    line.TrimTrailing('\r');
    joined << "\n" << line;
    SecureWipe(line);
    if(ParseCSVRecord(joined, fields, NumDataMembers + 2, num, open, rfc) == 0) {
      ok = (num <= (unsigned)NumDataMembers + 1);
      break;
    }
  }
  for(unsigned i = 0; i < NumDataMembers + 2; i++) SecureWipe(fields[i]);
  if(ok) {
    SecureWipe(rec);
    rec = joined;
    rfc = 1;
  }
  else {
    stream.df_Seek(next_line); // Read the first line alone
    rfc = 0;
  }
  SecureWipe(joined);
  return 1;
}

static int SkipImportRecord(const gxString &rec, CryptDBDocument *child_frame)
// 10/27/2026: A record with no values (a header line of a database
// without column names) or the column names line, with this database's
// names or the default ones, is not imported
{
  gxString fields[NumDataMembers + 2];
  unsigned num = 0;
  int open = 0, rfc = 0;
  int skip = 0;
  if(ParseCSVRecord(rec, fields, NumDataMembers + 2, num, open, rfc) == 0) {
    int empty = 1, own = 0, defaults = 0;
    char dest[DBStringLength];
    for(int i = 0; i < NumDataMembers; i++) {
      if(i < (int)num && !fields[i].is_null()) empty = 0;
      if(i < (int)num) {
	if(fields[i] == child_frame->DBParms()->db_config.GetColName(i, dest)) own++;
	if(fields[i] == progcfg->global_dbparms.dbgrid_labels[i]) defaults++;
      }
    }
    skip = empty || own == NumDataMembers || defaults == NumDataMembers;
  }
  for(unsigned i = 0; i < NumDataMembers + 2; i++) SecureWipe(fields[i]);
  return skip;
}

int has_grid_labels(const char *line, CryptDBDocument *child_frame) 
{
  char dest[DBStringLength]; 
  const char dchar = ',';
  int num;
  char words[MAXWORDS][MAXWORDLENGTH];
  int has_label = 0;

  // 10/25/2026: A line parse() cannot split is not the label line. It
  // returned 1 and the line was skipped without a message.
  if(parse(line, words, &num, dchar) == 1) return 0;
    
  for(int i = 0; i < NumDataMembers; i++) {
    if(*words[i] != 0) {
      gxString label = child_frame->DBParms()->db_config.GetColName(i, dest);
      if(label == words[i]) {
	has_label++;
      }
    }
  }
  if(has_label == NumDataMembers) return 1;

  return 0;
}

int count_import_lines(DiskFileB &stream, CryptDBDocument *child_frame) 
{
  int num_object_lines = 0;
  const unsigned MaxLine = ImportMaxLine;
  ImportLineBuffer line_buf(MaxLine);
  char *LineBuffer = line_buf.p;
  int bad_line = 0;
  wxString sbuf;
  const char dchar = ',';
  int num;

  while(!stream.df_EOF()) { // Read in file line by line
    ::wxYield(); // Allow the GUI to update the display

    gxString rec;
    int rfc = 0;
    if(!ReadImportRecord(stream, LineBuffer, MaxLine, bad_line, rec, rfc)) break;
    if(bad_line) continue;
    if(SkipImportRecord(rec, child_frame)) { SecureWipe(rec); continue; }
    if(rfc) { // 10/27/2026: A record with quoted line breaks or quotes
      rec.TrimLeadingSpaces();
      if(!rec.is_null() && rec[0] != ',') num_object_lines++;
      SecureWipe(rec);
      continue;
    }
    SecureWipe(rec);
    
    // 10/27/2026: The line as bytes, not converted to text, so values in
    // any encoding are kept as they are
    gxString gxbuf(LineBuffer);
    gxbuf.FilterChar('\r');
    gxbuf.FilterChar('\n');
    gxbuf.TrimLeadingSpaces();
    gxbuf.TrimTrailingSpaces();
    
    // Parse the text line
    if(!gxbuf.is_null()) {
      // Ensure that the template does not get added
      if(has_grid_labels(gxbuf.c_str(), child_frame)) continue; 
      // 10/25/2026: Count the lines the import reads, ParseCVSLine() has
      // no word limits
      unsigned n = 0;
      gxString *fields = ParseCVSLine(gxbuf, n);
      if(fields) {
	if(n > 0 && !fields[0].is_null()) num_object_lines++;
	delete[] fields;
      }
    }
  }
  stream.df_Rewind();
  return num_object_lines;
}


void ImportFromASCII(wxWindow *parent)
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  char dest[DBStringLength];  
  DBString *dbstring;
  gxString *csv_array = 0;
  int i;
  unsigned num;
  const unsigned MaxLine = ImportMaxLine;
  ImportLineBuffer line_buf(MaxLine);
  char *LineBuffer = line_buf.p;
  int bad_line = 0;
  wxString sbuf;
  const char dchar = ',';
  int linecount = 0;
  int imports = 0;
  int updates = 0;
  int updateall = 0;
  int grid_position = -1;
  gxString gbuf;

  // wxWindows single choice index box setup
  int ch = -1;
  const int CHArray = 4; 
  
  const wxString ch_array[CHArray] = {
    "Update this entry",
    "Skip this entry",
    "Update all entries without prompting",
    "Quit this import"
  };

  enum ICHOICES {
    UPDATE_ENTRY = 0,
    UPDATE_SKIP,
    UPDATE_ALL,
    QUIT_THIS
  };
  
  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Importing database from CSV file..." << "\n";

  wxFileDialog dialog(parent, "Import from file:",
		      progcfg->docDir.c_str(), "",
		      "*.csv",
		      wxFD_OPEN);
  
  if(dialog.ShowModal() != wxID_OK) {
    *(frame->statusWin) << "Import canceled." << "\n";
    return;
  }

  DiskFileB stream;
  
  stream.df_Open(dialog.GetPath().c_str());
  *(frame->statusWin) << "Importing database from: " << dialog.GetPath()
		      << "\n";
  
  if(!stream) { // Could not open the stream
    ProgramError->Message("Error opening the import text file!\n \
Exiting the import operation");
    return;
  }

  int num_object_lines = count_import_lines(stream, child_frame);
  if(num_object_lines == 0) { 
    ProgramError->Message("The import file does not contain any entires!\n \
Exiting the import operation");
    return;
  }

  frame->spanel->ResetPanel();
  frame->spanel->Show(TRUE);
  const int display_count = 10;
  int count = 0;
  int total_count = 0;

  // Load the database entries into the database grid
  frame->spanel->status_gauge->SetRange(num_object_lines);

  if(num_object_lines == 1) {
    gbuf << clear << "Importing " << num_object_lines << " object...";
  }
  else {
    gbuf << clear << "Importing " << num_object_lines << " objects...";
  }
  frame->spanel->WriteMessage(gbuf.c_str());

  child_frame->GetGrid()->BeginBatch();
  GUIUndoGroupGuard undo_group(child_frame, "Import"); // 10/27/2026: One undo step
  csv_array = 0;
  while(!stream.df_EOF()) { // Read in file line by line
    if(count >= display_count) {
      if(imports > 0) {
	if(imports == 1)
	  sbuf.Printf("Imported %d object", imports);
	else
	  sbuf.Printf("Imported %d objects", imports);
	frame->spanel->WriteMessage(sbuf.c_str());
      }
      if(updates > 0) {
	if(updates == 1)
	  sbuf.Printf("Updated %d object", updates);
	else
	  sbuf.Printf("Updated %d objects", updates);
	frame->spanel->WriteMessage(sbuf.c_str());
      }
      count = 0;
    }
    ::wxYield(); // Allow the GUI to update the display
    if(frame->spanel->cancel_event) {
      int yn = wxMessageBox("Are you sure you want to cancel this operation\n",
			    "Program Message",
			    wxYES_NO | wxCENTRE | wxICON_QUESTION, parent);
      if(yn == wxYES) {
	frame->spanel->Close();
	*(frame->statusWin) << "Import operation canceled" << "\n"; 
	child_frame->GetGrid()->EndBatch();
	child_frame->GridFrame()->RefreshGrid();
	return;
      }
      else {
	frame->spanel->cancel_event = 0;
      }
    }

    gxString rec;
    int rfc = 0;
    if(!ReadImportRecord(stream, LineBuffer, MaxLine, bad_line, rec, rfc)) break;
    linecount++;
    if(bad_line) {
      *(frame->statusWin) << "Skipped line " << (int)linecount
			  << ": longer than " << (int)(MaxLine-1) << " characters or has a NUL character" << "\n";
      continue;
    }
    if(SkipImportRecord(rec, child_frame)) { SecureWipe(rec); continue; }
    
    // Copy contents of the array to temporary holding buffer
    // 10/27/2026: A record with quoted line breaks or doubled quotes is
    // split exactly, older single line records as before
    // 10/27/2026: Kept as bytes, not converted to text, so values in any
    // encoding are imported as they are
    gxString gxbuf(rfc ? rec.c_str() : LineBuffer);
    if(!rfc) {
      gxbuf.FilterChar('\r');
      gxbuf.FilterChar('\n');
    }
    gxbuf.TrimLeadingSpaces();
    gxbuf.TrimTrailingSpaces();

    if(csv_array != 0) { // Free memory with every line read
      delete[] csv_array;
      csv_array = 0;
    }
    
    // Parse the text line
    if(!gxbuf.is_null()) {

      // Ensure that the template does not get added
      if(has_grid_labels(gxbuf.c_str(), child_frame)) continue; 

      if(rfc) {
	unsigned n = 0;
	int open = 0, has_rfc = 0;
	csv_array = new gxString[NumDataMembers + 2];
	ParseCSVRecord(rec, csv_array, NumDataMembers + 2, n, open, has_rfc);
	num = (n > (unsigned)NumDataMembers + 2) ? NumDataMembers + 2 : n;
      }
      else {
	csv_array = ParseCVSLine(gxbuf, num);
      }
      SecureWipe(rec);

      if(!csv_array[0].is_null()) {
	// Import or update each object
    	INFOHOG infohog(child_frame->GetPOD());
	INFOHOG infohog_a(child_frame->GetPOD());

	// 10/25/2026: Undo the formula escaping added by the export
	for(i = 0; i < num; i++) UnescapeCSVFormula(csv_array[i]);

	// Trim all leading and trailing spaces (10/27/2026: as bytes)
	gxString title(csv_array[0]);
	title.TrimLeadingSpaces();
	title.TrimTrailingSpaces();

	INFOHOG_t ob(title.c_str()); 
	infohog.SetMember(&ob, sizeof(ob), 0); // Set the item name

	// Set the other data members
	if(num > NumDataMembers) num = NumDataMembers;
	for(i = 1; i < num; i ++) {
	  if(!csv_array[i].is_null()) {
	    ob = csv_array[i].c_str();
	    infohog.SetMember(&ob, sizeof(ob), i);
	  }
	}

	infohog_a.SetMember(infohog.GetMember(0), 
			    infohog.GetMemberLen(0),
			    0);
	GUIUndoMark undo_mark; // 10/27/2026
	gxString undo_name = GUIObjectName(infohog);
	GUIUndoBefore(child_frame, undo_name, undo_mark);
	if(!infohog_a.FindObject()) { // This object does not exist
	  dbstring = (DBString *)infohog.GetMember(0);
	  ob = dbstring->c_str(dest);
	  *(frame->statusWin) << "Importing object: " 
			      << ob.c_str(dest)
			      << "\n";
	  if(!infohog.WriteObject(0)) {
	    *(frame->statusWin) << "Could not add:" << ob.c_str(dest)
				<< " to the database" << "\n";
	    continue;
	  }
	  GUIUndoAfter(undo_mark, undo_name, 0);
	  // Update the database grid
	  child_frame->GridFrame()->InsertCell(infohog);
	  imports++;
	  count++;
	  total_count++;
	  frame->spanel->status_gauge->SetValue(total_count);
	  continue;
	}
	else { // An entry for this object already exists
	  if(updateall == 0) {
	    dbstring = (DBString *)infohog.GetMember(0);
	    ob = dbstring->c_str(dest);
	    sbuf = ob.c_str(dest);
	    sbuf += " entry already exists.\nSelect your choice and click on OK";
	    ch = wxGetSingleChoiceIndex(sbuf, "Importing Objects",
					CHArray, ch_array, parent);
	  }
	  if(updateall == 1) ch = UPDATE_ALL;
	  switch(ch) {
	    case UPDATE_ALL:
	      updateall = 1;
	      updates++;
	      count++;
	      total_count++;
	      frame->spanel->status_gauge->SetValue(total_count);
	      if(!infohog_a.ReadObject()) {
		dbstring = (DBString *)infohog_a.GetMember(0);
		ob = dbstring->c_str(dest);
		*(frame->statusWin) << "Error reading object!" << "\n";
		*(frame->statusWin) << ob.c_str(dest) << " was not updated"
				    << "\n";
		continue;
	      }
	      dbstring = (DBString *)infohog_a.GetMember(0);
	      ob = dbstring->c_str(dest);
	      *(frame->statusWin) << "Updating object: " << ob.c_str(dest)
				  << "\n";

	      // The object has not changed
	      if(infohog == infohog_a) continue; 

	      infohog_a.ChangeObject(infohog);
	      GUIUndoAfter(undo_mark, undo_name, 0);
	      grid_position = child_frame->GridFrame()->FindGridInsertPosition(infohog);
	      child_frame->GridFrame()->LoadCell(infohog, grid_position);
	      break;
	    case UPDATE_SKIP:
	      dbstring = (DBString *)infohog_a.GetMember(0);
	      ob = dbstring->c_str(dest);
	      *(frame->statusWin) << "Skipping update for object: "
				  << ob.c_str(dest) << "\n";
	      break;
	    case UPDATE_ENTRY:
	      updates++;
	      total_count++;
	      frame->spanel->status_gauge->SetValue(total_count);
	      if(!infohog_a.ReadObject()) {
		dbstring = (DBString *)infohog_a.GetMember(0); 
		ob = dbstring->c_str(dest);
		*(frame->statusWin) << "Error reading object!" << "\n";
		*(frame->statusWin) << ob.c_str(dest) << " was not updated"
				    << "\n";
		continue;
	      }
	      dbstring = (DBString *)infohog_a.GetMember(0); 
	      ob = dbstring->c_str(dest);
	      *(frame->statusWin) << "Updating object: " << ob.c_str(dest)
				  << "\n";

	      // The object has not changed
	      if(infohog == infohog_a) continue; 
	      infohog_a.ChangeObject(infohog);
	      GUIUndoAfter(undo_mark, undo_name, 0);
	      grid_position = child_frame->GridFrame()->FindGridInsertPosition(infohog);
	      child_frame->GridFrame()->LoadCell(infohog, grid_position);
	      break;
	    case QUIT_THIS:
	      *(frame->statusWin) << "Import cancled." << "\n";
	      *(frame->statusWin) << "Imported " << imports << " objects."
				<< "\n";
	      if(updates) {
		*(frame->statusWin) << "Updated " << updates << " objects."
				    << "\n";
	      }
	      stream.df_Close();
	      child_frame->GetGrid()->EndBatch();
	      child_frame->GridFrame()->RefreshGrid();
	      return;
	    default:
	      break;
	  }
	}
      }
    }
  }

  if(csv_array != 0) { 
    delete[] csv_array;
    csv_array = 0;
  }

  // Set the progress meter
  frame->spanel->status_gauge->SetValue(total_count);

  // Flush the POD database following a database import
  child_frame->GetPOD()->Flush(); 

  stream.df_Close();
  child_frame->GetGrid()->EndBatch();
  child_frame->GridFrame()->RefreshGrid();
  
  *(frame->statusWin) << "Imported " << imports << " objects." << "\n";
  {
    gxString detail;
    detail << imports << " added, " << updates << " updated";
    GUILog(child_frame, "import", gxString(), 1, detail.c_str());
  }
  if(updates) {
    *(frame->statusWin) << "Updated " << updates << " objects." << "\n";
  }

  wxSleep(1); // Allow the user to view the results
  frame->spanel->WriteMessage("Import complete");
  frame->spanel->WriteMessage("Closing event monitor...");

#ifndef __APP_DEBUG_VERSION__ // Debug apps will not close the status window
  frame->spanel->Close();
#endif
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

