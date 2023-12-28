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

Database text template functions
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

void CreateTextTemplate(wxWindow *parent)
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  const char dchar = ',';

  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Creating template CSV file..." << "\n";

  wxFileDialog dialog(parent, "Template file to create:",
		      progcfg->docDir.c_str(), "",
		      "*.csv",
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
    *(frame->statusWin) << "Create template canceled." << "\n";
    return;
  }

  DiskFileB stream;
  stream.df_Create(dialog.GetPath().c_str());
  *(frame->statusWin) << "Creating template file: " << dialog.GetPath()
		      << "\n";
  
  if(!stream) { // Could not open the stream
    *(frame->statusWin) << "Could not write to: " << dialog.GetPath() << "\n";
    ProgramError->Message("Error opening the template file\n");
    return; 
  }

  // Write the item bar to the file first
  char dest[DBStringLength];
  for(int i = 0; i < NumDataMembers; i++) {
    gxString label = child_frame->DBParms()->db_config.GetColName(i, dest);
    ASPrint(label.c_str(), stream, label.length());
    if(i <  (NumDataMembers-1)) stream << dchar;
  }
  stream << as2LineFeed;
  
  *(frame->statusWin) << "Finished." << "\n";
  stream.df_Close();
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

