// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
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

Database index file rebuild functions
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

int RebuildIndexFile(gxString &index_file, POD *pod, int echo_results,
		     int reload_grid)
{
  INFOHOG infohog(pod);
  INFOHOGKEY key_type;
  gxDatabaseError err;
  const char *tempfile = "tempfile.idx";
  wxString sbuf;

  if(echo_results) {
    if(frame) {
      *(frame->statusWin) << "\n";
      *(frame->statusWin) << "Rebuilding the index file..." << "\n";
    }
  }
  int rv = infohog.RebuildIndexFile(tempfile, 0, 
				    InfoHogNumTrees, InfoHogNodeOrder);
  if(!rv) {
    if(echo_results) {
      if(frame) {
	sbuf = "The index file was not rebuilt!\n";
	*(frame->statusWin) << sbuf;
	ProgramError->Message(sbuf.c_str());
      }
    }
    return 0;
  }

#ifdef __APP_DEBUG_VERSION__
  if(echo_results) {
    if(frame) {
      *(frame->statusWin) << "Closing current index file..." << "\n";
    }
  }
#endif
  pod->CloseIndex(0);

#ifdef __APP_DEBUG_VERSION__
  if(echo_results) {
    if(frame) {
      *(frame->statusWin) << "Deleting the old index file..." << "\n";
    }
  }
#endif
  if(!::wxRemoveFile(index_file.c_str())) {
    if(echo_results) {
      if(frame) {
	sbuf = "Error overwriting exiting file!\n";
	*(frame->statusWin) << sbuf;
	ProgramError->Message(sbuf.c_str());
      }
    }
    return 0;
  }

#ifdef __APP_DEBUG_VERSION__
  if(echo_results) {
    if(frame) {
      *(frame->statusWin) << "Renaming the new index file..." << "\n";
    }
  }
#endif
  ::wxRenameFile(tempfile, index_file.c_str());
  err = pod->OpenIndex(index_file.c_str(), 0,
		       key_type, InfoHogNodeOrder,
		       gxDBASE_READWRITE);

  if(err != gxDBASE_NO_ERROR) {
    if(echo_results) {
      if(frame) {
	sbuf = "Error re-opeing index file.\n";
	*(frame->statusWin) << sbuf;
	ProgramError->Message(sbuf.c_str());
      }
    }   
    return 0;
  }
  
  if(reload_grid) {
    if(frame) {
      if(echo_results) {
	*(frame->statusWin) << "Reloading the database grid..." << "\n";
      }
      frame->ActiveChild()->GridFrame()->ReloadGrid();
    }
  }

  if(echo_results) {
    if(frame) {
      sbuf = "The index file was rebuilt and loaded.\n";
      *(frame->statusWin) << sbuf;
      wxMessageBox(sbuf, "Program Message", wxOK);
    }
  }

  return 1;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

