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

WIN32/UNIX function used to open known file types.

Under WIN32 this function uses the shell execute function to open 
accosiated file types without knowing the executable needed to open 
the file.

Required WIN32 Libraries:
user32.lib
shell32.lib
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

void StartDocument(const gxString &document_name, const gxString &app_name)
{
#if defined (__WIN32__)
  gxString exec(app_name.c_str());
  if(CaseICmp(exec, "Windows default application") != 0) {
    gxString command(app_name);
    command << ' ' << document_name;
    wxExecute(command.c_str());
    return;
  }

  // Use the Windows default application to launch this document
  HWND hwnd = NULL;
  HINSTANCE h = ShellExecute(hwnd, NULL, document_name.c_str(), NULL, NULL,
			     SW_SHOWNORMAL);
  if((long)h <= 32) {
    gxString sbuf;

    switch ((long)h) {
      case SE_ERR_NOASSOC:
	sbuf << clear << "ShellExecute Error: No association for file \
extension " << document_name << "\n";
	break;
	
      case ERROR_FILE_NOT_FOUND:
      case ERROR_PATH_NOT_FOUND:
	sbuf << clear << "ShellExecute Error: Document '" << document_name 
	     << "' not found" << "\n";
	break;

      default:
	sbuf << clear << "ShellExecute Error: Error " << (long)h 
	     << " while opening document " << document_name << "\n";
	break;
    }

    // Report the error message
    wxMessageBox(sbuf.c_str(), "Program Message", 
		 wxOK|wxCENTRE|wxICON_EXCLAMATION);
  }
#else
  if(app_name == "") return;
  gxString command(app_name);
  command << ' ' << document_name;
  wxExecute(command.c_str());
#endif
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

