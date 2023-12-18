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

wxWindow 2.X/3.X application frame work.
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

// --------------------------------------------------------------
// Include files
// --------------------------------------------------------------
#include "encrypted_password_database.h"
// --------------------------------------------------------------

// --------------------------------------------------------------
// Resources
// --------------------------------------------------------------
#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "icons/wxappfw.xpm"
#include "icons/doc.xpm"
#include "bitmaps/addrec.xpm"
#include "bitmaps/delrec.xpm"
#include "bitmaps/findrec.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/print.xpm"
#endif
// --------------------------------------------------------------

// --------------------------------------------------------------
// Internal global variable initialzation
// --------------------------------------------------------------
MainFrame *frame = 0;
wxappfwProgramConfig *progcfg = 0;

#ifdef __APP_DEBUG_VERSION__
DiskFileB debug_log;
#endif
// --------------------------------------------------------------

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type not wxApp
IMPLEMENT_APP(WinApp)

// --------------------------------------------------------------
// Standalone functions
// --------------------------------------------------------------
void InitProgramConfig(int argc, char *argv[])
// Initialize the program using config file or default values
{
  gxString sbuf;

  // Construct a new program configuration on the heap. 
  // Due to the potential size of program config structs 
  // use heap space instead of using stack space.
  progcfg = new wxappfwProgramConfig;

  // Get the ARG 0 directory. This path will be used to set the program's
  // working directory. Under Linux if the application is lanuched from
  // Nautilus or Konqueror the working directory will always be set the 
  // users home directory. To avoid this the working directory will be 
  // set here.
  progcfg->arg0_dir = argv[0];

#ifdef __APP_DEBUG_VERSION__
  debug_log << "Entering InitProgramConfig() function" << "\n" << flush;
  debug_log << "InitProgramConfig() constructing wxappfwProgramConfig object"
	    << "\n" << flush;
#endif

#if defined (__WIN32__)
  progcfg->arg0_dir.DeleteAfterLastIncluding("\\");
  ::wxSetWorkingDirectory(progcfg->arg0_dir.c_str());
  // Set the default directory paths to the program's working directory
  progcfg->homeDir = progcfg->docDir = progcfg->dataDir = progcfg->workDir = (const char *)::wxGetCwd().c_str();

  // Windows XP environmental variable list used by this application
  gxString app_data;
  CheckEnvVar("APPDATA", app_data);
  gxString home_share;
  CheckEnvVar("HOMESHARE", home_share);
  gxString home_drive;
  CheckEnvVar("HOMEDRIVE", home_drive);
  gxString home_path;
  CheckEnvVar("HOMEPATH", home_path);
  gxString system_drive;
  CheckEnvVar("SystemDrive", system_drive);
  gxString user_profile;
  CheckEnvVar("USERPROFILE", user_profile);

#ifdef __APP_DEBUG_VERSION__
  debug_log << "Env setting for APPDATA = " << app_data.c_str() << "\n";
  debug_log << "Env setting for HOMESHARE = " << home_share.c_str() << "\n";
  debug_log << "Env setting for HOMEDRIVE = " << home_drive.c_str() << "\n";
  debug_log << "Env setting for HOMEPATH = " << home_path.c_str() << "\n";
  debug_log << "Env setting for SystemDrive = " << system_drive.c_str() << "\n";
  debug_log << "Env setting for USERPROFILE = " << user_profile.c_str() << "\n";
#endif

  int has_app_dir = 0;
  int has_doc_dir = 0;
  gxString data_dirbuf;
  gxString doc_dirbuf;
  gxString dirbuf;

  // Check for the applications data directory. The programs
  // config file is stored in the user's app_dir in subdirectory
  // named after the program.
  if(!app_data.is_null()) {
    data_dirbuf << clear << app_data << "\\" << progcfg->ProgramName;
    if(::wxDirExists(data_dirbuf.c_str())) {
      has_app_dir = 1;
    }
    else {
      if(futils_mkdir(data_dirbuf.c_str()) != 0) {
	has_app_dir = 0;
      }
      else {
	has_app_dir = 1;
      }
    }
  }
  else {
    has_app_dir = 0;
  }

  // Check for the My Documents directory 
   if(!user_profile.is_null()) {
    doc_dirbuf << clear << user_profile << "\\My Documents";
    if(::wxDirExists(doc_dirbuf.c_str())) { // Has a My Documents dir
      doc_dirbuf << "\\" << progcfg->ProgramName;
      if(futils_mkdir(doc_dirbuf.c_str()) != 0) {
	has_doc_dir = 0;
      }
      else {
	has_doc_dir = 1;
      }
    }
    else { // There is no local My Documents directory 
      // MS C++.NET alternative
      // System::Environment::GetFolderPath(System:Environment::SpecialFolder::Personal);
      if(!home_share.is_null()) { // My Documents has been redirected
	if((!home_drive.is_null()) && (!home_path.is_null())) {
	  doc_dirbuf << clear << home_drive << home_path;
	  if(doc_dirbuf[doc_dirbuf.length()-1] != '\\') doc_dirbuf << "\\";
	  doc_dirbuf << "My Documents";
	  if(::wxDirExists(doc_dirbuf.c_str())) { // Has a My Documents dir
	    doc_dirbuf << "\\" <<  progcfg->ProgramName;
	    if(futils_mkdir(doc_dirbuf.c_str()) != 0) {
	      has_doc_dir = 0;
	    }
	    else {
	      has_doc_dir = 1;
	    }
	  }
	  else {
	    has_doc_dir = 0;
	  }
	}
      }
      else {
	has_doc_dir = 0;
      }
    }
  }

  if(has_app_dir) {
    progcfg->dataDir = data_dirbuf;
  }
  else {
    if(has_doc_dir) {
      progcfg->dataDir = doc_dirbuf;
    }
  }

  if(has_doc_dir) {
    progcfg->homeDir = progcfg->docDir = doc_dirbuf;
  }
  else {
    if(has_app_dir) {
      progcfg->homeDir = progcfg->docDir = progcfg->dataDir = data_dirbuf;
    }
  }
  
  if((!has_app_dir) && (!has_doc_dir)) {
    if(!system_drive.is_null()) {
      dirbuf << clear << system_drive;
      if(dirbuf[dirbuf.length()-1] != '\\') dirbuf << "\\";
      dirbuf << progcfg->ProgramName;
    }
    else {
      dirbuf << clear << "C:\\" << progcfg->ProgramName;
    }
    if(futils_mkdir(dirbuf.c_str()) != 0) {
      progcfg->homeDir = progcfg->docDir = progcfg->dataDir = progcfg->workDir;
    }
    else {
      progcfg->homeDir = progcfg->docDir = progcfg->dataDir = dirbuf;
    }
  } 

  // Set all the program config and log file names  
  progcfg->cfgFile << clear << progcfg->dataDir << "\\" << progcfg->default_cfgFile;
  progcfg->logFile << clear << progcfg->dataDir << "\\" << progcfg->default_logFile;
  progcfg->historyFile << clear << progcfg->dataDir << "\\" << progcfg->default_historyFile;

#elif defined (__UNIX__)
  ::wxSetWorkingDirectory(progcfg->homeDir.c_str());

  int has_home = 1;
  int has_datadir = 1;
  int has_docdir = 1;

  if(!::wxDirExists(progcfg->homeDir.c_str())) {
       if(futils_mkdir(progcfg->homeDir.c_str()) != 0) {
	 has_home = 0;
      }
  }
  
  if(!::wxDirExists(progcfg->dataDir.c_str())) {
       if(futils_mkdir(progcfg->dataDir.c_str()) != 0) {
	 has_datadir = 0;
      }
  }
  
  if(!::wxDirExists(progcfg->docDir.c_str())) {
       if(futils_mkdir(progcfg->docDir.c_str()) != 0) {
	 has_docdir = 0;
       }
  }
  
#endif

#ifdef __APP_DEBUG_VERSION__
  debug_log << "Working directory before config read = " << progcfg->workDir.c_str() << "\n" << flush;
  debug_log << "Home directory before config read = " << progcfg->homeDir.c_str() << "\n" << flush;
  debug_log << "Data directory before config read = " << progcfg->dataDir.c_str() << "\n" << flush;
  debug_log << "Doc directory before config read = " << progcfg->docDir.c_str() << "\n" << flush;
  debug_log << "Config file before config read = " << progcfg->cfgFile.c_str() << "\n" << flush;
  debug_log << "Log file before config read = " << progcfg->logFile.c_str() << "\n" << flush;
  debug_log << "History file before config read = " << progcfg->historyFile.c_str() << "\n" << flush;
#endif

  // Look for CfgFile name override in the shell's environment
  char *CurrentCfgFile = 0;
  if((CurrentCfgFile = getenv(progcfg->envSetting.c_str())) != 0) {
    progcfg->cfgFile = CurrentCfgFile;
  }

  // Expected config values
  char *dataDir = 0;
  char *docDir = 0;
  char *reToolBar = 0;
  int custom_color_red;
  int custom_color_green;
  int custom_color_blue;
  int i;
  char *mode = 0;

  // Load the configuration file
  gxConfig *CfgData = new gxConfig;
  int FileStatus = CfgData->Load(progcfg->cfgFile.c_str());

  if(FileStatus == 0) {
    // No config file found.
#ifdef __APP_DEBUG_VERSION__
    debug_log << "Config file not found" << "\n"
	      << "Building default configuration file" << "\n" << flush;
#endif
    DiskFileB dfile(progcfg->cfgFile.c_str(), DiskFileB::df_READWRITE,
		    DiskFileB::df_CREATE); 
    if(!dfile) {
#ifdef __APP_DEBUG_VERSION__
	debug_log << "Cannot create config file " << progcfg->cfgFile.c_str() << "\n" << flush;
#endif
    }
    else { // Write the default config file
      dfile << "# " << progcfg->ProgramName.c_str() << " configuration file" << "\n";
      dfile << "#" << "\n";
      dfile << "\n";
      dfile << "# Directory settings" << "\n";
      dfile << "DataDirectory = " << progcfg->dataDir.c_str() << "\n";
      dfile << "DocumentDirectory = " << progcfg->docDir.c_str() << "\n";
      dfile << "\n";
      dfile << "# Encryption settings" << "\n";
      dfile << "# Mode 3 = AES 256 CBC" << "\n";
      sbuf << clear << (int)DBStringConfig::mode;
      dfile << "EncryptionMode = " << sbuf.c_str() << "\n";
      dfile << "\n";
      dfile << "# Display settings" << "\n";
      sbuf << clear <<  progcfg->ViewToolBar;
      dfile << "ViewToolBar = " << sbuf.c_str() << "\n";
      dfile << "\n";
      dfile << "# Smart card settings" << "\n";
      dfile << "SC_enginePath = " << progcfg->SC_enginePath.c_str() << "\n";
      dfile << "SC_modulePath = " << progcfg->SC_modulePath.c_str() << "\n";
      dfile << "SC_engine_ID = " << progcfg->SC_engine_ID.c_str() << "\n";
      dfile << "SC_cert_id = " << progcfg->SC_cert_id.c_str() << "\n";
      dfile << "\n";
      dfile.df_Close();
    }

    // Initialize program configuration with default values.

    // ****************************************************************** //
    // Add default value not set by wxappfwProgramConfig constructor here    
    // ****************************************************************** //

  }
  else { 
    // *********************************************************** //
    // Add all startup config file entries here
    // *********************************************************** //
    char *s1 = CfgData->GetStrValue("DataDirectory");
    if(s1) dataDir = s1; 

    char *s2 = CfgData->GetStrValue("DocumentDirectory");
    if(s2) docDir = s2; 

    char *s3 = CfgData->GetStrValue("ViewToolBar");
    if(s3) reToolBar = s3; 

    char *s4 = CfgData->GetStrValue("EncryptionMode"); 
    if(s4) mode = s4; 

    char *s5 = CfgData->GetStrValue("SC_enginePath");
    if(s5) progcfg->SC_enginePath = s5;

    char *s6 = CfgData->GetStrValue("SC_modulePath");
    if(s6) progcfg->SC_modulePath = s6;

    char *s7 = CfgData->GetStrValue("SC_engine_ID");
    if(s7) progcfg->SC_engine_ID = s7; 

    char *s8 = CfgData->GetStrValue("SC_cert_id");
    if(s8) progcfg->SC_cert_id = s8;
    
    // Set the custom color table
    for(i = 0; i < 16; i++) {
      sbuf << clear << "custom_colors" << i << "_red";
      char *s_test1 = CfgData->GetStrValue(sbuf.c_str());
      custom_color_red = CfgData->GetIntValue(sbuf.c_str());

      sbuf << clear << "custom_colors" << i << "_green";
      char *s_test2 = CfgData->GetStrValue(sbuf.c_str());
      custom_color_green = CfgData->GetIntValue(sbuf.c_str());

      sbuf << clear << "custom_colors" << i << "_blue";
      char *s_test3 = CfgData->GetStrValue(sbuf.c_str());
      custom_color_blue = CfgData->GetIntValue(sbuf.c_str());

      if(s_test1 && s_test2 && s_test3) {
	progcfg->custom_colors[i].Set((const unsigned char)custom_color_red,
				      (const unsigned char)custom_color_green,
				      (const unsigned char)custom_color_blue);
      }
    }  
  }

  // Test and initialize all the configuration parameters
  if(dataDir) {
    if(::wxDirExists(dataDir)) {
      progcfg->dataDir = dataDir;
    }
    else {
      if(futils_mkdir(progcfg->dataDir.c_str()) == 0) {
	progcfg->dataDir = dataDir;
      }
    }
  }

  if(docDir) {
    if(::wxDirExists(docDir)) {
      progcfg->docDir = docDir;
    }
    else {
      if(futils_mkdir(progcfg->docDir.c_str()) == 0) {
	progcfg->docDir = docDir;
      }
    }
  }

  if(mode) {
    gxString mode_str = mode;
    if(mode_str.Atoi() > 0) {
      if(mode_str.Atoi() <= 3) {
	DBStringConfig::mode = mode_str.Atoi(); 
      }
    }
  }

  if(reToolBar != 0) {
    if(CaseICmp((const char*)reToolBar, "FALSE") == 0) {
      progcfg->ViewToolBar = 0;	
    }
    // Defaults to true if not set in config file
  }

  CfgData->UnLoad(); // Unload the Config file from memory
  delete CfgData;

  // Command line argument overrides
  if(argc >= 2) {
    char *userdocdir = argv[1];
    if(userdocdir) {
      if(::wxDirExists(userdocdir)) {
	progcfg->docDir = userdocdir;
      }
      else {
	if(futils_mkdir(userdocdir) == 0) {
	  progcfg->docDir = userdocdir;
	}
      }
    }
  }

#ifdef __APP_DEBUG_VERSION__
  debug_log << "Working directory after config read = " << progcfg->workDir.c_str() << "\n" << flush;
  debug_log << "Home directory after config read = " << progcfg->homeDir.c_str() << "\n" << flush;
  debug_log << "Data directory after config read = " << progcfg->dataDir.c_str() << "\n" << flush;
  debug_log << "Doc directory after config read = " << progcfg->docDir.c_str() << "\n" << flush;
  debug_log << "Config file after config read = " << progcfg->cfgFile.c_str() << "\n" << flush;
  debug_log << "Log file after config read = " << progcfg->logFile.c_str() << "\n" << flush;
  debug_log << "History file after config read = " << progcfg->historyFile.c_str() << "\n" << flush;
#endif

#ifdef __APP_DEBUG_VERSION__
  if(debug_log.df_IsOpen()) {
    debug_log << "Exiting InitProgramConfig() function" << "\n" << flush;
  }
#endif
}

POD *OpenDatabase(CryptDBDocument *child_frame, 
		  gxString &fname, POD *pod, gxDatabaseConfig &db_config, 
		  gxString &data_ext, gxString &index_ext, int display_errors,
		  int set_admin_rights)
// Helper function used to open a POD database. Returns a null value if
// an the database cannot be opened.
{
  if(!child_frame) return 0;

  // Generate the new index and data file names
  // NOTE: The wxString::BeforeLast() function will return a null string
  // if the file name does not have a dot extension.
  gxString data_file = fname;
  gxString dbname = fname;
  gxString sbuf;
  
  child_frame->DBParms()->data_file = dbname;
  child_frame->DBParms()->index_file = dbname;

  child_frame->DBParms()->data_file.DeleteAfterLastIncluding(".");
  child_frame->DBParms()->data_file += data_ext;
  child_frame->DBParms()->index_file.DeleteAfterLastIncluding(".");
  child_frame->DBParms()->index_file += index_ext;

  dbname.DeleteAfterLastIncluding(".");
#if defined (__UNIX__)
  dbname.DeleteBeforeLastIncluding("/");
#else 
  dbname.DeleteBeforeLastIncluding("\\");
#endif

  data_file.DeleteAfterLastIncluding(".");
  data_file += data_ext;
  gxString index_file = fname;
  index_file.DeleteAfterLastIncluding(".");
  index_file += index_ext;
  int i;

  // Close the current database
  if(pod) {
    delete pod;
    pod = 0;
  }
  
  pod = new POD;
  if(!pod) {
    if(display_errors) {
      ProgramError->Message(gxDatabaseExceptionMessage(gxDBASE_MEM_ALLOC_ERROR));
    }
    return 0;
  }

  InfoHogKey<DBString> key_type;
  DBStringConfig::DBStringCaseCompare = 0; // Do not compare the case of DBString objects

  ::wxYield();

  FAU_t static_data_size = (FAU_t)(DB_CONFIG_STATIC_AREA_SIZE + DB_AUTH_STATIC_AREA_SIZE);

  // Create or open an existing database using a single index file
  gxDatabaseError err = pod->Open(data_file.c_str(), 
				  index_file.c_str(), key_type,
				  InfoHogNodeOrder, gxDBASE_READWRITE, 
				  InfoHogUseIndexFile, 
				  static_data_size,
				  InfoHogNumTrees,
			  child_frame->DBParms()->database_revision,
			  child_frame->DBParms()->database_revision);

  if(err != gxDBASE_NO_ERROR) {

    ::wxYield();

    // Try to open the database with read-only access
    err = pod->Open(data_file.c_str(), index_file.c_str(), key_type,
		    InfoHogNodeOrder, gxDBASE_READONLY,
		    InfoHogUseIndexFile, 
		    static_data_size,
		    InfoHogNumTrees,
		    child_frame->DBParms()->database_revision,
		    child_frame->DBParms()->database_revision);

    if(err == gxDBASE_NO_ERROR) {
      child_frame->DBParms()->admin_rights = 0;
    }
  }

  if(err != gxDBASE_NO_ERROR) {
    delete pod;
    pod = 0; // Signal that there are no database files open
    if(display_errors) {
      ProgramError->Message(gxDatabaseExceptionMessage(err));
    }
    return 0;
  }

  // Rebuild the index file is neccessary
  if(pod->RebuildIndex()) {

    ::wxYield();

#ifdef __APP_DEBUG_VERSION__
    if(display_errors) {
      ProgramError->Message("Rebuilding the database index file\n");
    }
#endif
    if(!RebuildIndexFile(index_file, pod, display_errors)) {
      delete pod;
      pod = 0; // Signal that there are no database files open
      return 0;
    }
  }
  
  if(!pod->Exists()) { // A new file was created

    unsigned char *static_data = new unsigned char[static_data_size];
    AES_fillrand(static_data, static_data_size);
    err = pod->OpenDataFile()->Write(static_data, static_data_size, pod->OpenDataFile()->FileHeaderSize());
    if(err != gxDBASE_NO_ERROR) {
      delete pod;
      delete static_data;
      pod = 0; // Signal that there are no database files open
      if(display_errors) {
	sbuf << clear << "Error initializing static data area" << "\n" << gxDatabaseExceptionMessage(err);
	ProgramError->Message(sbuf.c_str());
      }
      futils_remove(data_file.c_str());
      futils_remove(index_file.c_str());
      return 0;
    }
    delete static_data;
    
    gxDatabaseParms *dbparms = child_frame->DBParms();

    // Load the database name
    db_config.SetDatabaseName(dbname.c_str());
    db_config.SetPrintDocName(dbname.c_str());

    // Load the default grid labels
    for(i = 0; i < NumDataMembers; i++) {
      db_config.SetColName(i, dbparms->dbgrid_labels[i].c_str());
    }

    // Write the new config
    db_config.WriteConfig(pod->OpenDataFile());
  }
  else { // Read and validate the database config values
    if(!db_config.ReadConfig(pod->OpenDataFile())) {
      // The read failed, so reset the config
      db_config.Clear();
      db_config.SetDatabaseName(dbname.c_str());
      db_config.SetPrintDocName(dbname.c_str());
    }
    else { // Validate the config values for this database
      if(!db_config.Validate()) {
	// The read passed but the config is not valid
	db_config.Clear();
	db_config.SetDatabaseName(dbname.c_str());
	db_config.SetPrintDocName(dbname.c_str());
      }
      else { // The config is good so load the values
	// NOTE: Override any default values here
	if(db_config.print_doc_name.is_null()) {
	  db_config.SetPrintDocName(dbname.c_str());
	}
      }
    }
  }

  return pod;
}

void wxStrStripChar(wxString &s, char c)
// Strip every occurrence of the specified character from a
// wxString object.
{
 int rv = s.Find(c);
 while(rv != -1) {
   s.Remove(rv, 1);
   rv = s.Find(c);
 }
}

int BuildVirtualDB(CryptDBDocument *child_frame)
{
  if(!dllist) return 0;
  
  INFOHOGKEY key, compare_key;
  gxBtree *btx = child_frame->GetPOD()->Index();
  dllist->ClearList();
  
  // Ensure that the in memory buffers and the file data
  // stay in sync during multiple file access.
  btx->TestTree();

  // Walk through the tree starting at the first key
  int num_entries = 0;
  FAU object_id;
  if(btx->FindFirst(key, 0)) {
    ::wxYield();
    object_id = key.ObjectID();
    dllist->Add(object_id);
    num_entries++;
  }
  while(btx->FindNext(key, compare_key, 0)) {
    ::wxYield();
    object_id = key.ObjectID();
    dllist->Add(object_id);
    num_entries++;
  }

  return num_entries;
}

int NumListEntries()
{
  if(!dllist) return 0;
  int num_entries = 0;
  dllistptr = dllist->GetHead();
  while(dllistptr) {
    num_entries++;
    dllistptr = dllistptr->next;
  }
  return num_entries;
}

void FixHomeDirID(gxString &dir)
{
#if defined (__WIN32__)
  if((dir[dir.length()-1] == '\\') || (dir[dir.length()-1] == '/')) {
    dir.DeleteAt((dir.length()-1), 1);
  }
  return;
#endif
  gxString sbuf;
  if(dir[dir.length()-1] == '/') {
    dir.DeleteAt((dir.length()-1), 1);
  }
  if(dir.Find("~/") != 1) {
    sbuf = (const char *)::wxGetHomeDir().c_str();
    sbuf += "/";
    dir.ReplaceString("~/", sbuf.c_str());
  }
}

int CheckEnvVar(const char *var_name, gxString &var_value)
{
  char *envbuf = 0;
  var_value.Clear();
  if((envbuf = getenv(var_name)) != 0) {
    var_value = getenv(var_name);
    return 1;
  }
  return 0;
}

int IsHyperlink(const gxString &s)
{
  if(s.IFind("://") != -1) return 1;
  if(s.IFind("mailto:") != -1) return 1;  
  if(s[0] == '\\') { // UNC path
    if(s[0] == '\\') {
      return 1;
    }
  }
  if(s[1] == ':') { // Windows path
    if(s[2] == '\\') {
      return 1;
    }
  }
  return 0;
}

int LoadCustomColors(wxColourData *data)
{
  int i;
  for(i = 0; i < 16; i++) {
    data->SetCustomColour(i, progcfg->custom_colors[i]);
  }
  return 1;
}

int SaveCustomColors(wxColourData *retData)
{
  gxString sbuf;
  int i;

  gxConfig CfgData(progcfg->cfgFile.c_str());
  for(i = 0; i < 16; i++) {
    progcfg->custom_colors[i] = retData->GetCustomColour(i);
    sbuf << clear << "custom_colors" << i << "_red";
    CfgData.ChangeConfigValue(sbuf.c_str(),
			      (int)progcfg->custom_colors[i].Red());
    sbuf << clear << "custom_colors" << i << "_green";
    CfgData.ChangeConfigValue(sbuf.c_str(),
			      (int)progcfg->custom_colors[i].Green());
    sbuf << clear << "custom_colors" << i << "_blue";
    CfgData.ChangeConfigValue(sbuf.c_str(),
			      (int)progcfg->custom_colors[i].Blue());
  }
  return 1;
}
// --------------------------------------------------------------


unsigned BtreeSearch(gxBtree *btx, int item, POD *pod,
		     INFOHOG_t &ob, int find_all) 
{
  INFOHOGKEY key, compare_key;
  unsigned objects_found = 0;
  dllist->ClearList();
  
  // Walk through the tree starting at the first key
  if(btx->FindFirst(key)) {
    INFOHOG infohog(pod);
    if(!infohog.ReadObject(key.ObjectID())) {
      return objects_found;
    }
    BtreeKeySearch(key, item, pod, ob, objects_found, find_all);
    
    while(btx->FindNext(key, compare_key)) {
#ifdef __wxWINALL__
      ::wxYield();
#endif
      INFOHOG infohog(pod);
      if(!infohog.ReadObject(key.ObjectID())) {
	return objects_found;
      }
      BtreeKeySearch(key, item, pod, ob, objects_found, find_all);
    }
  }
  return objects_found;
}

void BtreeKeySearch(INFOHOGKEY &key, int item, POD *pod,
		    INFOHOG_t &ob, unsigned &objects_found, int find_all)
{
#ifdef __wxWINALL__
  ::wxYield();
#endif

  int offset;
  INFOHOG_t buf;
  FAU object_id;

  char dest1[DBStringLength];  
  char dest2[DBStringLength];  
  if(item == 0) {
    if(find_all == 0) { // Search for single match
      if(key.ObjectName() == ob) { 
	object_id = key.ObjectID();
	dllist->Add(object_id);
	objects_found++;
      }
    }
    else { // Search for all matches
      buf = key.ObjectName();
      char *s1 = buf.c_str(dest1);
      char *s2 = ob.c_str(dest2);
      offset = IFindMatch(s1, s2, 0);
      if(offset != -1) {
	object_id = key.ObjectID();
	dllist->Add(object_id);
	objects_found++;
      }
    }
  }
  else {
    INFOHOG infohog(pod);
    infohog.ReadObject((FAU_t)key.ObjectID());

    if(find_all == 0) { // Search for single match
      if(infohog.GetMemberLen(item) == sizeof(INFOHOG_t)) {
	buf = *((INFOHOG_t *)infohog.GetMember(item));
	gxString sbuf1 = buf.c_str(dest1);
	gxString sbuf2 = ob.c_str(dest2);
	if(sbuf1 == sbuf2) { 
	  object_id = key.ObjectID();
	  dllist->Add(object_id);
	  objects_found++;
	}
      }
    }
    else { // Search for all matche
#ifdef __wxWINALL__
      *(frame->statusWin) << "Searching for all strings..." << "\n";
#endif
      if(infohog.GetMemberLen(item) == sizeof(INFOHOG_t)) {
	buf = *((INFOHOG_t *)infohog.GetMember(item));
	char *s1 = buf.c_str(dest1);
	char *s2 = ob.c_str(dest2);

#ifdef __wxWINALL__
	*(frame->statusWin) << s1 << " " << s2 << "\n";
#endif
	offset = IFindMatch(s1, s2, 0);
	if(offset != -1) {
	  object_id = key.ObjectID();
	  dllist->Add(object_id);
	  objects_found++;
	}
      }
      else {
#ifdef __wxWINALL__
	*(frame->statusWin) << "No object found..." << "\n";
#endif
      }
    }
  }
}

// --------------------------------------------------------------
// Application and framework include source code files 
// --------------------------------------------------------------
#include "a_panel.cpp"
#include "backup.cpp"
#include "compare.cpp"
#include "c_panel.cpp"
#include "export.cpp"
#include "d_panel.cpp"
#include "e_panel.cpp"
#include "f_panel.cpp"
#include "g_thread.cpp"
#include "htmprint.cpp"
#include "import.cpp"
#include "lprinter.cpp"
#include "lp_panel.cpp"
#include "mciplay.cpp"
#include "merge.cpp"
#include "mswpage.cpp"
#include "mswprint.cpp"
#include "m_doc.cpp"
#include "m_grid.cpp"
#include "m_tbar.cpp"
#include "pspage.cpp"
#include "psprint.cpp"
#include "p_config.cpp"
#include "rebuild.cpp"
#include "startdoc.cpp"
#include "stats.cpp"
#include "st_frame.cpp"
#include "st_panel.cpp"
#include "template.cpp"
#include "txtprint.cpp"
#include "winapp.cpp"
#include "new_panel.cpp"
#include "open_panel.cpp"
#include "add_user_panel.cpp"
#include "m_frame.cpp"
// --------------------------------------------------------------

// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
