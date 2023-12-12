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

Database import functions
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

int has_grid_labels(const char *line, CryptDBDocument *child_frame) 
{
  char dest[DBStringLength]; 
  const char dchar = child_frame->DBParms()->db_config.GetTextDelimiter();
  int num;
  char words[MAXWORDS][MAXWORDLENGTH];
  int has_label = 0;

  if(parse(line, words, &num, dchar) == 1) return 1;
    
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
  const int MaxLine = 1024;
  char LineBuffer[MaxLine];
  wxString sbuf;
  const char dchar = child_frame->DBParms()->db_config.GetTextDelimiter();
  int num;

  while(!stream.df_EOF()) { // Read in file line by line
    ::wxYield(); // Allow the GUI to update the display

    stream.df_GetLine(LineBuffer, MaxLine, '\n');
    
    // Copy contents of the array to temporary holding buffer
    sbuf = LineBuffer;
    
    // Strip all line feeds and carriage returns
    wxStrStripChar(sbuf, '\r');
    wxStrStripChar(sbuf, '\n');

    // Trim all leading and trailing spaces
    sbuf.Trim(); sbuf.Trim(FALSE);
    
    // Parse the text line
    if(sbuf != "") {
      // Ensure that the template does not get added
      if(has_grid_labels(sbuf.c_str(), child_frame)) continue; 
      char words[MAXWORDS][MAXWORDLENGTH];
      if(parse((const char *)sbuf.c_str(), words, &num, dchar) == 1) continue;
      if(*words[0] != 0) num_object_lines++; 
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
  int i, num;
  const int MaxLine = 1024;
  char LineBuffer[MaxLine];
  wxString sbuf;
  const char dchar = child_frame->DBParms()->db_config.GetTextDelimiter();
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
  *(frame->statusWin) << "Importing database from ASCII file \
delimited by tabs..." << "\n";

  wxFileDialog dialog(parent, "Import from file:",
		      progcfg->docDir.c_str(), "",
		      "*.txt",
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

    stream.df_GetLine(LineBuffer, MaxLine, '\n');
    linecount++;
    
    // Copy contents of the array to temporary holding buffer
    sbuf = LineBuffer;
    
    // Strip all line feeds and carriage returns
    wxStrStripChar(sbuf, '\r');
    wxStrStripChar(sbuf, '\n');

    // Trim all leading and trailing spaces
    sbuf.Trim(); sbuf.Trim(FALSE);
    
    // Parse the text line
    if(sbuf != "") {

      // Ensure that the template does not get added
      if(has_grid_labels(sbuf.c_str(), child_frame)) continue; 

      char words[MAXWORDS][MAXWORDLENGTH];
      if(parse((const char *)sbuf.c_str(), words, &num, dchar) == 1) {
	*(frame->statusWin) << "Parse error!" << "\n";
	*(frame->statusWin) << "Import operation canceled." << "\n";
	break;
      }

      if(num == 0) continue; 

      if(*words[0] != 0) {

	// Import or update each object
    	INFOHOG infohog(child_frame->GetPOD());
	INFOHOG infohog_a(child_frame->GetPOD());

	sbuf = words[0];
	// Trim all leading and trailing spaces
	sbuf.Trim(); sbuf.Trim(FALSE);	 

	INFOHOG_t ob(sbuf.c_str()); 
	infohog.SetMember(&ob, sizeof(ob), 0); // Set the item name

	// Set the other data members
	if(num > NumDataMembers) num = NumDataMembers;
	for(i = 1; i < num; i ++) {
	  if(*words[i] != 0) ob = words[i];
	  infohog.SetMember(&ob, sizeof(ob), i);
	}

	infohog_a.SetMember(infohog.GetMember(0), 
			    infohog.GetMemberLen(0),
			    0);
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
	    sbuf += " entry already exists.\n \
Select your choice and click on OK";
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

  // Set the progress meter
  frame->spanel->status_gauge->SetValue(total_count);

  // Flush the POD database following a database import
  child_frame->GetPOD()->Flush(); 

  stream.df_Close();
  child_frame->GetGrid()->EndBatch();
  child_frame->GridFrame()->RefreshGrid();
  
  *(frame->statusWin) << "Imported " << imports << " objects." << "\n";
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

