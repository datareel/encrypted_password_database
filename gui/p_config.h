// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 2001-2024 DataReel Software Development
// ----------------------------------------------------------- // 
// ---------- Include File Description and Details  ---------- // 
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
#ifndef __P_CONFIG_HPP__
#define __P_CONFIG_HPP__

#include "app_defs.h"

struct wxappfwProgramConfig {
  wxappfwProgramConfig();
  ~wxappfwProgramConfig() { }
  
  // Frame, logging, and timer variables
  int RememberFrameSize; // True to allow the program to save frame sizes  
  int EnableLogging;     // True to enable log functions
  int EnableTimer;       // True to start timer automatically
  int ViewToolBar; 

  // Program directory path and file name information
  gxString workDir;         // Program's working directory
  gxString cfgFile;         // Program's current configuraion file
  gxString default_cfgFile; // Program's default configuraion file
  gxString logFile;         // Program's current log file
  gxString default_logFile; // Program's default log file
  gxString envSetting;      // Environmental variable
  gxString dataDir;         // Local data directory
  gxString docDir;          // Local document directory
  gxString homeDir;         // UNIX home dir or Windows user profile directory
  gxString historyFile;     // History file
  gxString default_historyFile; // Default history file
  gxString arg0_dir;            // ARG 0 present working directory
  gxString USERNAME;
#if defined (__UNIX__)
  gxString logDir;
#endif

  // Config values used to adjust frame sizes
  int MainFrameHeight;
  int MainFrameWidth;
  
  // Default frame sizes
  int default_MainFrameHeight;
  int default_MainFrameWidth;

  // Message variables
  int suppress_exit_thread_warning;
  int verbose_debugging;

  // Application variables
  int complex_passwords;
  int password_length;
  int min_password_length;
  int password_aging;
  
  // Printer document variables
  gxString document_name;

  // Program information strings
  gxString ProgramName;
  gxString Platform;
  gxString VersionString;
  gxString produced_by;
  gxString copyright;
  gxString copyright_dates;
  gxString release_date;
  gxString default_url;
  gxString support_email;
  gxString online_help_page;

  // Database config
  gxDatabaseParms global_dbparms;

  // Database definitions to read from the DEF file
  gxString table_def, key_def, family_def, fkey_def;
  
  // User and group definitions to read from the DEF file
  gxString user_def, group_def;

  // MDI Interface
  gxList<CryptDBDocument *> child_frame_list;
  int num_child_frames;
  CryptDBDocument *active_child_frame;

  // External applications
  gxString web_browser;
  gxString mail_client;
  gxString help_launch;
  gxString file_manager;

  // Operating system dependent variables                              
  gxString path_sep;
  char path_sepc;
  gxString install_directory;
  int user_id;           // User ID for this user                              
  int effective_user_id; // Effective user ID for this user

  // Default external applications
  gxString default_web_browser;
  gxString default_mail_client;
  gxString default_help_launch;
  gxString default_file_manager;

  // Colors and fonts
  wxColour custom_colors[16];

  // Smart card settings
  gxString SC_enginePath;
  gxString SC_modulePath;
  gxString SC_engine_ID;
  gxString SC_cert_id;
};

#endif  // __P_CONFIG_HPP__ 
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

