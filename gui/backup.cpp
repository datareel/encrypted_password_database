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

Database backup functions
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

void BackUp(wxWindow *parent)
{
  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Backing up the database to another file..." << "\n";

  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  wxString sbuf("*");
  gxString ebuf;
  sbuf += child_frame->DBParms()->data_file_extension.c_str();
  wxFileDialog dialog(parent, "Backup database to:",
		      progcfg->docDir.c_str(), "",
		      sbuf,
		      wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

  if(dialog.ShowModal() == wxID_OK) {
    if(wxFileExists(dialog.GetPath())) {
      if(!wxRemoveFile(dialog.GetPath())) {
	ProgramError->Message("Error overwriting exiting file\n");
	return;
      }
    }

#ifdef __APP_DEBUG_VERSION__
    *(frame->statusWin) << "Opening " << dialog.GetPath() << "\n";
#endif
  }
  else {
    *(frame->statusWin) << "Backup canceled." << "\n";
    return;
  }

  gxString datafile((const char *)dialog.GetPath().c_str());
  datafile.DeleteAfterLastIncluding(".");
  gxString indexfile(datafile);
  datafile += child_frame->DBParms()->data_file_extension;
  indexfile += child_frame->DBParms()->index_file_extension;
    
  POD newdb;
  INFOHOGKEY key, compare_key;
  FAU_t static_data_size = (FAU_t)(DB_CONFIG_STATIC_AREA_SIZE + DB_AUTH_STATIC_AREA_SIZE);
  gxDatabaseError err = newdb.Open(datafile.c_str(), indexfile.c_str(), key,
				   InfoHogNodeOrder, gxDBASE_READWRITE,
				   InfoHogUseIndexFile, 
				   static_data_size,
				   InfoHogNumTrees,
				   child_frame->DBParms()->database_revision,
				   child_frame->DBParms()->database_revision);
  
  if(err != gxDBASE_NO_ERROR) {
    ProgramError->Message("Error opening the backup copy!\n Exiting the backup operation");
    EchoDBError(&newdb, frame->statusWin);
    return;
  }

  FAU_t start_of_static_area = newdb.OpenDataFile()->StaticArea();

  unsigned char *static_data = new unsigned char[static_data_size];
  AES_fillrand(static_data, static_data_size);
  err = newdb.OpenDataFile()->Write(static_data, static_data_size, start_of_static_area);
  if(err != gxDBASE_NO_ERROR) {
    delete static_data;
    ebuf << clear << "Error initializing static data area" << "\n" << gxDatabaseExceptionMessage(err);
    ProgramError->Message(ebuf.c_str());
    futils_remove(datafile.c_str());
    futils_remove(indexfile.c_str());
    EchoDBError(&newdb, frame->statusWin);
    return;
  }
  delete static_data;
  
  gxDatabaseConfig db_config;
  gxDatabaseParms *dbparms = child_frame->DBParms();
  char dest[DBStringLength];

  // Load the database name
  gxString name_buf = dbparms->db_config.database_name.c_str(dest);
  name_buf << "-Backup Copy";
  db_config.SetDatabaseName(name_buf.c_str());
  
  // Load the default grid labels
  for(int i = 0; i < NumDataMembers; i++) {
    char *p = dbparms->db_config.GetColName(i, dest);
    db_config.SetColName(i, p);
  }
  
  // Write the new config
  db_config.WriteConfig(newdb.OpenDataFile());

  int items = 0;
  INFOHOG infohog(child_frame->GetPOD()); // The current database 
  INFOHOG infohog_new(&newdb);
  gxBtree *btx = child_frame->GetPOD()->Index();

  // Ensure that the in memory buffers and the file data
  // stay in sync during multiple file access.
  btx->TestTree();
  
  int num_entries = btx->NumKeys();
  const int display_count = num_entries/10;
  int count = 0;
  int curr_count = 0;  
  int yn;
    
  frame->spanel->ResetPanel();
  frame->spanel->status_gauge->SetRange(num_entries);
  frame->spanel->Show(TRUE);
  
  // Load the database entries into the database grid
  frame->spanel->WriteMessage("Backing up the database...");

  frame->spanel->WriteMessage("Loading the database entries...");
  if(btx->FindFirst(key, 0)) {
    if(!infohog.ReadObject(key.ObjectID())) {
      ProgramError->Message("Error reading database object\n");
      EchoDBError(child_frame->GetPOD(), frame->statusWin);
      frame->spanel->Close();
      return;
    }
    infohog_new.Copy(infohog, 0); // Copy the data members only
    if(!infohog_new.WriteObject(0)) {
      ProgramError->Message("Error writing to the backup copy\n");
      EchoDBError(&newdb, frame->statusWin);
      frame->spanel->Close();
      return;
    }
    ::wxYield();
    count++;
    items++;
    frame->spanel->status_gauge->SetValue(items);
    if(frame->spanel->cancel_event) {
      yn = wxMessageBox("Are you sure you want to cancel this operation\n",
			"Program Message",
			wxYES_NO | wxCENTRE | wxICON_QUESTION, parent);
      if(yn == wxYES) {
	frame->spanel->Close();
	return;
      }
      else {
	frame->spanel->cancel_event = 0;
      }
    }
    while(btx->FindNext(key, compare_key, 0)) {
      if(!infohog.ReadObject(key.ObjectID())) {
      	ProgramError->Message("Error reading database object\n");
	EchoDBError(child_frame->GetPOD(), frame->statusWin);
	frame->spanel->Close();
      	return;
      }
      ::wxYield();
      infohog_new.Copy(infohog, 0); // Copy the data members only
      if(!infohog_new.WriteObject(0)) {
	ProgramError->Message("Error writing to the backup copy\n");
	EchoDBError(&newdb, frame->statusWin);
	frame->spanel->Close();
	return;
      }
      count++;
      items++;
      frame->spanel->status_gauge->SetValue(items);
      if(count >= display_count) {
	curr_count += count;
	sbuf.Printf("Backed up %d out of a %d objects",
		    curr_count, num_entries);
	frame->spanel->WriteMessage(sbuf.c_str());
	count = 0;
      }
      if(frame->spanel->cancel_event) {
	yn = wxMessageBox("Are you sure you want to cancel this operation\n",
			  "Program Message",
			  wxYES_NO | wxCENTRE | wxICON_QUESTION, parent);
	if(yn == wxYES) {
	  frame->spanel->Close();
	  return;
	}
	else {
	  frame->spanel->cancel_event = 0;
	}
      }
    }
  }

  *(frame->statusWin) << "Backed up " << items << " objects to "
		      << datafile.c_str() << "\n";

  wxSleep(1); // Allow the user to view the results
  frame->spanel->WriteMessage("Backup complete");
  frame->spanel->WriteMessage("Closing event monitor...");

#ifndef __APP_DEBUG_VERSION__ // Debug apps will not close the status window
  frame->spanel->Close();
#endif
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

