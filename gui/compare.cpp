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

Database index file compare functions
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

void CompareIndexFile()
{
  CryptDBDocument *child_frame = frame->ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  
  wxString sbuf;
  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Comparing the index file to the data file..."
		      << "\n";
  
  INFOHOG infohog(child_frame->GetPOD());
  if(!infohog.CompareIndex(0)) {
    sbuf = "The index file does not match the data file!\n\
The index file needs to be rebuilt.\n";
    *(frame->statusWin) << sbuf;
    ProgramError->Message(sbuf.c_str());
    return;
  }
  
  sbuf = "The index file checks good.\n";
  *(frame->statusWin) << sbuf;
  wxMessageBox(sbuf, "Program Message", wxOK);
}

// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

