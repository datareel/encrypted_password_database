// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
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

Database merge functions
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

void Merge(wxWindow *parent)
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  char dest[DBStringLength];
  INFOHOG_t *dbstring;

  // wxWindows single choice index box setup
  int ch = -1;
  const int CHArray = 4; 

  const wxString ch_array[CHArray] = {
    "Update this entry",
    "Skip this entry",
    "Update all entries without prompting",
    "Quit this merge"
  };

  enum ICHOICES {
    UPDATE_ENTRY,
    UPDATE_SKIP,
    UPDATE_ALL,
    QUIT_THIS
  };
  
  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Merging the contents of another database..." << "\n";

  wxString sbuf("*");
  sbuf += child_frame->DBParms()->data_file_extension.c_str();
  wxFileDialog dialog(parent, "Merge from file:",
		      progcfg->docDir.c_str(), "",
		      sbuf,
		      wxFD_OPEN);
  
  if(dialog.ShowModal() != wxID_OK) {
    *(frame->statusWin) << "Merge canceled." << "\n";
    return;
  }

  gxString datafile((const char *)dialog.GetPath().c_str());
  datafile.DeleteAfterLastIncluding(".");
  gxString indexfile(datafile);
  datafile += child_frame->DBParms()->data_file_extension;
  indexfile += child_frame->DBParms()->index_file_extension;
    

  gxDatabase f;
  if(f.Open(datafile.c_str(), gxDBASE_READONLY) != gxDBASE_NO_ERROR) {
    ProgramError->Message("Error opening the merge database copy!\n \
Exiting the merge operation");
    return;
  }
  gxDatabaseConfig dbconfig;
  if(!dbconfig.ReadConfig(&f)) {
    ProgramError->Message("Error reading database merge database config!\n \
Exiting the merge operation");
    f.Close();
    return;
  }
  f.Close();

  char *dup = new char[DBStringLength];
  memmove(dup, &dbconfig.database_name, DBStringLength);
  int crypt_error;
  CryptoHeader eh;

  // Get the length of the encrytpted buffer
  memmove(&eh, dup, sizeof(eh));
  unsigned cryptLen = eh.crypt_len;
    
  // Realign the encrypted buffer
  memmove(dup, (dup+sizeof(eh)), cryptLen);

  // Decrypt the compressed string
  crypt_error = AES_Decrypt(dup, &cryptLen, 
			    (const unsigned char *)DBStringConfig::crypt_key.m_buf(), 
			    DBStringConfig::crypt_key.length());
  if(crypt_error != AES_NO_ERROR) {
#ifdef __APP_DEBUG_VERSION__
    debug_log << "Error decrypting SPPC BL" << "\n" << flush; 
    debug_log << AES_err_string(crypt_error) << "\n" << flush; 
#endif
    delete[] dup;
    dup = 0;
    if(crypt_error == AES_ERROR_BAD_SECRET) {
      ProgramError->Message("The database password does not match the merge database!\n \
Exiting the merge operation");
    }
    else { 
      ProgramError->Message("Error decrypting password hash in the merge database!\n \
Exiting the merge operation");   
    }
    return;
  }

  delete[] dup;
  dup = 0;

  POD newdb;
  INFOHOGKEY key, compare_key;
  // Open only the data file for read only access
  gxDatabaseError err = newdb.Open(datafile.c_str(), indexfile.c_str(), key,
				   InfoHogNodeOrder, gxDBASE_READONLY, 0);
  if(err != gxDBASE_NO_ERROR) {
    ProgramError->Message("Error opening the merge database copy!\n \
Exiting the merge operation");
    EchoDBError(&newdb, frame->statusWin);
    return;
  }

  FAU addr;
  int imports = 0;
  int updates = 0;
  int updateall = 0;
  int grid_position;
  
  FAU oa;            // Object Address
  gxBlockHeader gx;  // Block Header
  gxObjectHeader oh; // Object Header

  FAU gxdfileEOF = newdb.OpenDataFile()->GetEOF();
  
  addr = 0;
  addr = newdb.OpenDataFile()->FindFirstBlock(addr); // Search the entire file

  if(addr == (FAU)0) {
    ProgramError->Message("Merge file does not contain any entries!\n \
Exiting the merge operation");

    *(frame->statusWin) << "No database blocks found in file: "
			<< datafile.c_str() << "\n" << "Exiting the merge operation"
			<< "\n";
#ifdef __APP_DEBUG_VERSION__
    EchoDBError(child_frame->GetPOD(), frame->statusWin);
#endif
    return;
  }


  FAU dl, rm;
  int total = (int)newdb.OpenDataFile()->TotalBlocks();
  int deleted = (int)newdb.OpenDataFile()->DeleteBlocks(&dl, &rm);
  int num_object_lines = total - deleted;

  if(num_object_lines <= 0) {
    ProgramError->Message("Merge file does not contain any entries!\n \
Exiting the merge operation");
    return;
  }

  *(frame->statusWin) << "Merging..." << "\n";

  frame->spanel->ResetPanel();
  frame->spanel->Show(TRUE);
  const int display_count = 10;
  int count = 0;
  int total_count = 0;

  frame->spanel->status_gauge->SetRange(num_object_lines);

  gxString gbuf;
  if(num_object_lines == 1) {
    gbuf << clear << "Merging " << num_object_lines << " object...";
  }
  else {
    gbuf << clear << "Merging " << num_object_lines << " objects...";
  }
  frame->spanel->WriteMessage(gbuf.c_str());

  child_frame->GetGrid()->BeginBatch();
  while(1) { 
    if(count >= display_count) {
      if(imports > 0) {
	sbuf.Printf("Merged %d objects", imports);
	frame->spanel->WriteMessage(sbuf.c_str());
      }
      if(updates > 0) {
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
	*(frame->statusWin) << "Merge operation canceled" << "\n"; 
	child_frame->GetGrid()->EndBatch();
	child_frame->GridFrame()->RefreshGrid();
	return;
      }
      else {
	frame->spanel->cancel_event = 0;
      }
    }
    INFOHOG infohog(child_frame->GetPOD()); // The current database 
    INFOHOG infohog_new(&newdb);
    INFOHOG_t ob;
    if((addr + newdb.OpenDataFile()->BlockHeaderSize()) >= gxdfileEOF) break;
    if(newdb.OpenDataFile()->Read(&gx, sizeof(gxBlockHeader), addr) !=
       gxDBASE_NO_ERROR) {
      ProgramError->Message("Error reading block header\n");
      EchoDBError(child_frame->GetPOD(), frame->statusWin);
      *(frame->statusWin) << "Exiting the merge operation" << "\n";
      child_frame->GetGrid()->EndBatch();
      child_frame->GridFrame()->RefreshGrid();
      return;
    }
    if(gx.block_check_word == gxCheckWord) {
      if((__SBYTE__)gx.block_status == gxNormalBlock) {
	oa = addr + newdb.OpenDataFile()->BlockHeaderSize();;
	if(newdb.OpenDataFile()->Read(&oh, sizeof(gxObjectHeader), oa) !=
	   gxDBASE_NO_ERROR) {
	  ProgramError->Message("Error reading object header\n");
	  EchoDBError(&newdb, frame->statusWin);
	  *(frame->statusWin) << "Exiting the merge operation" << "\n";
	  child_frame->GetGrid()->EndBatch();
	  child_frame->GridFrame()->RefreshGrid();
	  return;
	}
	if(oh.ClassID == infohog.GetClassID()) { 
	  if(!infohog_new.ReadObject(oa)) {
	    ProgramError->Message("Error reading database object\n");
	    EchoDBError(&newdb, frame->statusWin);
	    *(frame->statusWin) << "Exiting the merge operation" << "\n";
	    child_frame->GetGrid()->EndBatch();
	    child_frame->GridFrame()->RefreshGrid();
	    return;
	  }
	  infohog.Copy(infohog_new, 0);
	  if(!infohog.FindObject()) { // This object does not exist
	    // NOTE: This is a safty feature. Using member 0 should always be safe
	    // provided the call has initialized the infohog object correctly.
	    if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) {
	      ProgramError->Message("Error reading cast copy of database object\n");
	      EchoDBError(&newdb, frame->statusWin);
	      *(frame->statusWin) << "Exiting the merge operation" << "\n";
	      child_frame->GetGrid()->EndBatch();
	      child_frame->GridFrame()->RefreshGrid();
	      return;
	    }
	    dbstring = (INFOHOG_t *)infohog.GetMember(0);
	    ob = dbstring->c_str(dest);
	    *(frame->statusWin) << "Merging object: " << ob.c_str(dest)
				<< "\n";
	    if(!infohog.WriteObject(0)) {
	      *(frame->statusWin) << "Could not add:" << ob.c_str(dest)
				  << " to the database" << "\n";
	      EchoDBError(child_frame->GetPOD(), frame->statusWin);
	      *(frame->statusWin) << "Exiting the merge operation" << "\n";
	      child_frame->GetGrid()->EndBatch();
	      child_frame->GridFrame()->RefreshGrid();
	      return;	      
	    }
	    // Update the database grid
	    child_frame->GridFrame()->InsertCell(infohog);
	    imports++;
	    count++;
	    total_count++;
	    frame->spanel->status_gauge->SetValue(total_count);
	  }
	  else { // An entry for this object already exists
	    if(updateall == 0) {
	      // NOTE: This is a safty feature. Using member 0 should always be safe
	      // provided the call has initialized the infohog object correctly.
	      if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) {
		ProgramError->Message("Error reading cast copy of database object\n");
		EchoDBError(&newdb, frame->statusWin);
		*(frame->statusWin) << "Exiting the merge operation" << "\n";
		child_frame->GetGrid()->EndBatch();
		child_frame->GridFrame()->RefreshGrid();
		return;
	      }
	      dbstring = (INFOHOG_t *)infohog.GetMember(0);
	      ob = dbstring->c_str(dest);
	      sbuf = ob.c_str(dest);
	      sbuf += " entry already exists.\n \
Select your choice and click on OK";
	      ch = wxGetSingleChoiceIndex(sbuf, "Merging objects",
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
		if(!infohog.ReadObject()) {
		  // NOTE: This is a safty feature. Using member 0 should always be safe
		  // provided the call has initialized the infohog object correctly.
		  if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) {
		    ProgramError->Message("Error reading cast copy of database object\n");
		    EchoDBError(&newdb, frame->statusWin);
		    *(frame->statusWin) << "Exiting the merge operation" << "\n";
		    child_frame->GetGrid()->EndBatch();
		    child_frame->GridFrame()->RefreshGrid();
		    return;
		  }
		  dbstring = (INFOHOG_t *)infohog.GetMember(0);
		  ob = dbstring->c_str(dest);
		  *(frame->statusWin) << "Error reading object!" << "\n";
		  *(frame->statusWin) << ob.c_str(dest) << " was not updated"
				      << "\n";
		  break;
		}
		*(frame->statusWin) << "Updating object: " << ob.c_str(dest)
				    << "\n";
		if(infohog == infohog_new) break; // Object has not changed
		infohog.ChangeObject(infohog_new);
		grid_position = child_frame->GridFrame()->FindGridInsertPosition(infohog);
		child_frame->GridFrame()->LoadCell(infohog, grid_position);
		break;
	      case UPDATE_SKIP:
		// NOTE: This is a safty feature. Using member 0 should always be safe
		// provided the call has initialized the infohog object correctly.
		if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) {
		  ProgramError->Message("Error reading cast copy of database object\n");
		  EchoDBError(&newdb, frame->statusWin);
		  *(frame->statusWin) << "Exiting the merge operation" << "\n";
		  child_frame->GetGrid()->EndBatch();
		  child_frame->GridFrame()->RefreshGrid();
		  return;
		}
		dbstring = (INFOHOG_t *)infohog.GetMember(0);
		ob = dbstring->c_str(dest);
		*(frame->statusWin) << "Skipping update for object: "
				    << ob.c_str(dest) << "\n";
		break;
	      case UPDATE_ENTRY:
		updates++;
		count++;
		total_count++;
		frame->spanel->status_gauge->SetValue(total_count);
		if(!infohog.ReadObject()) {
		  // NOTE: This is a safty feature. Using member 0 should always be safe
		  // provided the call has initialized the infohog object correctly.
		  if(infohog.GetMemberLen(0) != sizeof(INFOHOG_t)) {
		    ProgramError->Message("Error reading cast copy of database object\n");
		    EchoDBError(&newdb, frame->statusWin);
		    *(frame->statusWin) << "Exiting the merge operation" << "\n";
		    child_frame->GetGrid()->EndBatch();
		    child_frame->GridFrame()->RefreshGrid();
		    return;
		  }
		  dbstring = (INFOHOG_t *)infohog.GetMember(0);
		  ob = dbstring->c_str(dest);
		  *(frame->statusWin) << "Error reading object!" << "\n";
		  *(frame->statusWin) << ob.c_str(dest) << " was not updated"
				      << "\n";
		break;
		}
		*(frame->statusWin) << "Updating object: " << ob.c_str(dest)
				    << "\n";
		if(infohog == infohog_new) break; // Object has not changed
		infohog.ChangeObject(infohog_new);
		grid_position = child_frame->GridFrame()->FindGridInsertPosition(infohog);
		child_frame->GridFrame()->LoadCell(infohog, grid_position);
		break;
	      case QUIT_THIS:
		*(frame->statusWin) << "Merge cancled." << "\n";
		*(frame->statusWin) << "Merged " << imports << " objects."
				    << "\n";
		if(updates) {
		  *(frame->statusWin) << "Updated " << updates << " objects."
				      << "\n";
		}
		child_frame->GetGrid()->EndBatch();
		child_frame->GridFrame()->RefreshGrid();
		return;
	      default:
		break;
	    }
	  }
	}
      }
      addr = addr + gx.block_length; // Goto the next database block
    }
    else {
      addr++; // Keep searching until a valid database block is found
    }
  }

  child_frame->GetGrid()->EndBatch();
  child_frame->GridFrame()->RefreshGrid();
  
  // Set the progress meter
  frame->spanel->status_gauge->SetValue(total_count);

  // Flush the POD database following a database merge
  child_frame->GetPOD()->Flush(); 

  *(frame->statusWin) << "Merged " << imports << " objects." << "\n";
  if(updates) {
    *(frame->statusWin) << "Updated " << updates << " objects." << "\n";
  }

  wxSleep(1); // Allow the user to view the results
  frame->spanel->WriteMessage("Merge complete");
  frame->spanel->WriteMessage("Closing event monitor...");

#ifndef __APP_DEBUG_VERSION__ // Debug apps will not close the status window
  frame->spanel->Close();
#endif
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

