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

Database export functions
*/
// ----------------------------------------------------------- //
#include "app_defs.h"

void ExportToASCII(wxWindow *parent)
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  gxString mbuf;
  mbuf << clear << "Warning!!! You are about to write the encrypted database to a plain text file."
       << "\n" << "Text files are not encrypted and can be easily read from many applications."
       << "\n"
       << "\n"
       << "Do you wish to continue the export to plain text operation?";
    int yesno = wxMessageBox(mbuf.c_str(),
			     "Program Warning",
			     wxYES_NO | wxCENTRE | wxICON_QUESTION, parent);
  if(yesno == wxNO) {
    *(frame->statusWin) << "Export operation canceled" << "\n"; 
    return;
  }

  char dest[DBStringLength];  
  DBString *dbstring;
  gxString dbuf;
  const char dchar = ',';
  gxString dchar_str;
  dchar_str << clear << dchar;
  const char filter = '\t';  // Filter out tabs
  const char *Fill = " "; // Fill character string
  unsigned i;
  int num_entries = 0;
  wxString sbuf;
  const int display_count = num_entries/10;
  int count = 0;
  int curr_count = 0;  

  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Exporting database to CSV file..." << "\n";

  frame->SetStatusText("Building the export list...");
  
  if(dllist->IsEmpty()) {
    num_entries = BuildVirtualDB(child_frame);
  }
  else {
    num_entries = NumListEntries();
  }
  
  frame->SetStatusText("");

  wxFileDialog dialog(parent, "Export to file:",
		      progcfg->docDir.c_str(), "",
		      "*.csv",
		      wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

  if(dialog.ShowModal() == wxID_OK) {
#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Opening " << dialog.GetPath() << "\n";
#endif
  }
  else {
    *(frame->statusWin) << "Export canceled." << "\n";
    return;
  }

  // 10/25/2026: Build the export in memory and write it when it is
  // complete, to a new file only the user can read. A canceled or
  // failed export leaves no partial clear text file behind.
  gxString stream;
  gxString export_fname = (const char *)dialog.GetPath().c_str();
  *(frame->statusWin) << "Exporting database to: " << dialog.GetPath() << "\n";

  // Write the item bar to the file first
  for(i = 0; i < (int)NumDataMembers; i++) {
    gxString label = child_frame->DBParms()->db_config.GetColName(i, dest);
    AppendCSVField(stream, label, dchar, filter);
    stream << dchar;
  }
  stream << as2LineFeed;

  stream << as2LineFeed;
  
  int exports = 0;
  dllistptr = dllist->GetHead();
  

  frame->spanel->ResetPanel();
  frame->spanel->status_gauge->SetRange(num_entries);
  frame->spanel->Show(TRUE);
  
  // Load the database entries into the database grid
  frame->spanel->WriteMessage("Exporting database...");

  while(dllistptr) {
    INFOHOG infohog(child_frame->GetPOD());
    // 10/25/2026: Skip records that cannot be read or have a bad key member
    if(!infohog.ReadObject(dllistptr->data) ||
       (infohog.GetMemberLen(0) != sizeof(INFOHOG_t))) {
      dllistptr = dllistptr->next;
      continue;
    }
    
    ::wxYield();

    // Write the key member, which should always be set
    dbstring = (DBString *)infohog.GetMember(0);
    dbuf = dbstring->c_str(dest);
    AppendCSVField(stream, dbuf, dchar, filter);
    stream << dchar;

    // Write the remaining members account for null members
    for(i = 1; i < (unsigned)NumDataMembers; i ++) {
      ::wxYield();
      if(infohog.GetMemberLen(i) == sizeof(INFOHOG_t)) {
	dbstring = (DBString *)infohog.GetMember(i);
	if(!dbstring->is_null()) {
	  dbuf = dbstring->c_str(dest);
	  AppendCSVField(stream, dbuf, dchar, filter);
	}
	else {
	  stream << Fill;
	}
      }
      else {
	stream << Fill;
      }

      // Reached the last member so do not print the delimiter
      if(i == (NumDataMembers - 1)) break;
      stream << dchar;
    }

    stream << as2LineFeed;
    exports++;
    count++;
    
    frame->spanel->status_gauge->SetValue(exports);
    if(count >= display_count) {
      curr_count += count;
      sbuf.Printf("Exported %d out of a %d objects", curr_count, num_entries);
      frame->spanel->WriteMessage(sbuf.c_str());
      count = 0;
    }
    
    ::wxYield();
    if(frame->spanel->cancel_event) {
      int yn = wxMessageBox("Are you sure you want to cancel this operation\n",
			    "Program Message",
			    wxYES_NO | wxCENTRE | wxICON_QUESTION, parent);
      if(yn == wxYES) {
	frame->spanel->Close();
	SecureWipe(stream);
	SecureWipe(dbuf);
	OPENSSL_cleanse(dest, sizeof(dest));
	*(frame->statusWin) << "Export operation canceled" << "\n"; 
	return;
      }
      else {
	frame->spanel->cancel_event = 0;
      }
    }
    dllistptr = dllistptr->next;
  }

  gxString err;
  if(WritePrivateFile(export_fname.c_str(), stream.c_str(), stream.length(), err) != 0) {
    SecureWipe(stream);
    SecureWipe(dbuf);
    OPENSSL_cleanse(dest, sizeof(dest));
    *(frame->statusWin) << "Could not write to: " << export_fname.c_str() << "\n";
    ProgramError->Message(err.c_str());
    frame->spanel->Close();
    return;
  }
  SecureWipe(stream);
  SecureWipe(dbuf);
  OPENSSL_cleanse(dest, sizeof(dest));
  *(frame->statusWin) << "Exported " << exports << " objects." << "\n";

  wxSleep(1); // Allow the user to view the results
  frame->spanel->WriteMessage("Export complete");
  frame->spanel->WriteMessage("Closing event monitor...");

#ifndef __APP_DEBUG_VERSION__ // Debug apps will not close the status window
  frame->spanel->Close();
#endif
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

