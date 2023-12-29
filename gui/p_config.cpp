// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/27/2023
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

Program configuration data structure. All configurable program
parameters are contain in the structure. Any modifications to
the default values will effect all parts of the program.
*/
// ----------------------------------------------------------- //   
#include "app_defs.h"

wxappfwProgramConfig::wxappfwProgramConfig()
{
  int i;

  RememberFrameSize = 1;
  EnableLogging = 1;
  EnableTimer = 0;
  MainFrameHeight = default_MainFrameHeight = 1024;
  MainFrameWidth = default_MainFrameWidth = 1280;
  ViewToolBar = 1;

  logFile = default_logFile = "encrypted_password_database.log";

#if defined (__WIN32__)
  path_sepc = '\\';
  path_sep = "\\";
  cfgFile = default_cfgFile = "encrypted_password_database.ini";
  dataDir = "Encrypted Password Database";
  docDir = "Encrypted Password Database";
  Platform = "Windows 10";
  default_web_browser = "Windows default application";
  default_mail_client = "Windows default application";
  default_help_launch = "Windows default application";
  default_file_manager = "Windows default application";
#elif defined (__WXMAC__)
#error MAC platform currently not supported in this version of app framework
#elif defined (__UNIX__)
  path_sepc = '/';
  path_sep = "/";
  default_cfgFile = "encrypted_password_database.cfg";
  homeDir = (const char *)wxGetHomeDir().c_str();
  if(homeDir == "") homeDir = ".";
  homeDir << "/" << ".encrypted_password_database";
  dataDir << clear << homeDir << "/" << "data";
  docDir << clear << homeDir << "/" << "docs";
  logDir << clear << homeDir << "logs";
  workDir = (const char *)wxGetCwd();
  USERNAME = getenv("USER");
  
  // Set the configuration file path                                           
  cfgFile << clear << homeDir << path_sep << default_cfgFile;
  logFile << clear << logDir << path_sep << default_logFile;
  historyFile << clear << dataDir << path_sep << default_historyFile;

  // Get the user ID                                                           
  user_id = getuid();
  effective_user_id = geteuid(); // chmod u+s or chmod 4510                    

  // Check to see if this user is root before setting the install directory    
  if((user_id == 0) || (effective_user_id == 0)) {
    if(futils_exists("/opt")) {
      install_directory = "/opt/encrypted_password_database";
    }
    else {
      install_directory = "/usr/local/encrypted_password_database";
    }
  }
  else {
    install_directory = (const char *)wxGetHomeDir().c_str();
    if(install_directory == "") install_directory = ".";
    install_directory += "/encrypted_password_database";
  }

#if defined(__WXGTK__) && defined (__LINUX__)
  Platform = "Linux GTK";
#else // Default to generic UNIX
  Platform = "UNIX";
#endif

  default_web_browser = "firefox";
  default_mail_client = "thunderbird";
  default_help_launch = "firefox";

  // KDE file viewer excutable = konqueror
  // GNOME file viewer excutable = nautilus
  default_file_manager = "nautilus";
#else
#error You must define a target platform: __WIN32__ or __UNIX__
#endif

  // Set the programs default values
  web_browser = default_web_browser;
  mail_client = default_mail_client;
  help_launch = default_help_launch;
  file_manager = default_file_manager;

  envSetting = "EPDB_CFG";
  ProgramName = "Encrypted Password Database";
  VersionString = "2023.107";
#ifdef __APP_DEBUG_VERSION__
  ProgramName << " Encrypted Password Database Debug Version";
#endif

  historyFile = default_historyFile = "encrypted_password_database.txt";
  produced_by = "Datareel Open Source";
  copyright = "Copyright (c) Datareel Open Source"; 
  copyright_dates = "2001-2024";
  release_date = "12/27/2023";
  default_url = "https://datareel.com";
  support_email = "datareel@datareel.com";
  online_help_page = "https://datareel.com/encrypted_password_database/help";
  suppress_exit_thread_warning = 0;
  verbose_debugging = 0;
  document_name = "Encrypted Password Database";

  // MDI Interface
  num_child_frames = 0;
  active_child_frame = 0;

  complex_passwords = 0; // Disable complex password requirment by default
  password_length = 1;
  min_password_length = 8;
  password_aging = 0;

  // Colors and font init
  for(i = 0; i < 16; i++) {
    custom_colors[i].Set((const unsigned char)(i*16),
			 (const unsigned char)(i*16),
			 (const unsigned char)(i*16));
  }

  // Smart card settings
  SC_enginePath = SC_get_default_enginePath(); 
  SC_modulePath = SC_get_default_modulePath();
  SC_engine_ID = SC_get_default_engine_ID();
  SC_cert_id = SC_get_default_cert_id();
}
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
