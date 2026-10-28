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
#ifdef __WXGTK__
extern "C" void g_set_prgname(const char *prgname); // GLib, linked with GTK
extern "C" void gdk_set_program_class(const char *program_class); // GDK
#endif

bool WinApp::OnInit(void)
{
  SecureProcessSetup(); // 10/25/2026: Private files, no core dumps
  EPDBSetUTF8Locale();  // 10/27/2026: wxString char* conversions are UTF-8

#ifdef __WXGTK__
  // 10/27/2026: A fixed Wayland app_id and X11 WM_CLASS, whatever the
  // binary is called, so the desktop finds encrypted_password_database.desktop
  // and its icon (StartupWMClass in the .desktop file)
  g_set_prgname("encrypted_password_database");
  gdk_set_program_class("encrypted_password_database");
  SetClassName("encrypted_password_database");
  SetAppName("encrypted_password_database");
#endif

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
			wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

  // 10/27/2026: The saved size in DIPs, or a share of the screen the
  // window is on, never larger than the screen work area
  int disp = wxDisplay::GetFromWindow(frame);
  wxRect area = wxDisplay(disp == wxNOT_FOUND ? 0 : (unsigned)disp).GetClientArea();
  wxSize want;
  if(progcfg->main_width_dip > 0 && progcfg->main_height_dip > 0) {
    want = frame->FromDIP(wxSize(progcfg->main_width_dip, progcfg->main_height_dip));
  }
  else {
    want = wxSize(area.width * 4 / 5, area.height * 4 / 5);
  }
  want.x = wxMin(want.x, area.width);
  want.y = wxMin(want.y, area.height);
  frame->SetMinSize(frame->FromDIP(wxSize(640, 420)));
  frame->SetSize(want);
  frame->Centre(wxBOTH);
  frame->Show(TRUE);
  SetTopWindow(frame);

#ifdef __APP_DEBUG_VERSION__
    debug_log << "Returning from WinApp::OnInit() call" << "\n" << flush;
#endif

  return TRUE;
}
int WinApp::FilterEvent(wxEvent& event)
{
  wxEventType t = event.GetEventType();
  if(frame && ((t == wxEVT_KEY_DOWN) || (t == wxEVT_LEFT_DOWN) || (t == wxEVT_RIGHT_DOWN) ||
	       (t == wxEVT_MIDDLE_DOWN) || (t == wxEVT_MOUSEWHEEL) || (t == wxEVT_MOTION))) {
    frame->NoteUserActivity();
  }
  return -1; // Process the event normally
}

// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
