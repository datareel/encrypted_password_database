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

wxWindow 2.X/3.X application class.
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

bool WinApp::OnInit(void)
{
#ifdef __APP_DEBUG_VERSION__
  gxString work_dir = (const char *)wxGetCwd();
  gxString debug_file_name;

#if defined (__WIN32__)
  gxString user_profile;
  CheckEnvVar("USERPROFILE", user_profile);
  gxString debugdir = user_profile;
  debugdir << "\\.encrypted_password_database\\logs";
  if(!::wxDirExists(debugdir.c_str())) {
   futils_mkdir(debugdir.c_str());
  }
  if(!::wxDirExists(debugdir.c_str())) {
    // Error making the log dir
    debug_file_name << clear << work_dir << "\\debug.log";
  }
  else {
    debug_file_name << clear << debugdir << "\\debug.log";
  }
#else // If not WIN32 assume a UNIX path
  gxString debugdir = (const char *)wxGetHomeDir().c_str();
  debugdir << "/.encrypted_password_database/logs";
  debug_file_name << clear << debugdir << "/debug.log";
  if(!::wxDirExists(debugdir.c_str())) {
   futils_mkdir(debugdir.c_str());
  }
#endif
  debug_log.df_Create(debug_file_name.c_str());
  debug_log << "Debug log file = " << debug_file_name.c_str() << flush;
  debug_log << "InitProgramConfig() call" << "\n" << flush;
#endif

  // Set all the program parameters base on the config file values
  InitProgramConfig(argc, argv);
  // NOTE: Cannot access progcfg-> members until the InitProgramConfig()
  // call has returned.

 
#ifdef __APP_DEBUG_VERSION__
  progcfg->debug_file_name = debug_file_name;
  debug_log << "Constructing main frame" << "\n" << flush;
#endif

  frame = new MainFrame((wxFrame *)NULL, MAIN_FRAME_ID, 
			progcfg->ProgramName.c_str(),
			wxPoint(50, 50), 
			wxSize(progcfg->MainFrameWidth, 
			       progcfg->MainFrameHeight),
			wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
  
  frame->Centre(wxBOTH);
  frame->Show(TRUE);
  SetTopWindow(frame);

#ifdef __APP_DEBUG_VERSION__
    debug_log << "Returning from WinApp::OnInit() call" << "\n" << flush;
#endif

  return TRUE;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
