// ------------------------------- //
// -------- start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, BCC32, GCC, HPUX aCC, SOLARIS CC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
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

Encrypted password database console functions
*/
// ----------------------------------------------------------- // 
#include "gxdlcode.h"

#if defined (__USE_ANSI_CPP__) // Use the ANSI Standard C++ library
#include <iostream>
using namespace std; // Use unqualified names for Standard C++ library
#else // Use the old iostream library by default
#include <iostream.h>
#endif // __USE_ANSI_CPP__

#include "epdb_functions.h"

int PrintDBConfig(gxDatabaseConfig &db_config)
{
  cout << "Printing database configurtaion" << "\n";
  cout << "-------------------------------" << "\n";
  
  char *s = db_config.database_name.GetString();
  if(DBStringConfig::AES_error_level != AES_NO_ERROR) {
    cout << "ERROR: Error decrypting database name " << AES_err_string(DBStringConfig::AES_error_level) << "\n";
    return 1;
  }
  if(!s) {
    cout << "ERROR: Database name is a null value" << "\n";
    return 1;
  }

  unsigned i = 0;
  unsigned j = 0;
  
  cout << "version_number: " << db_config.version_number << "\n";
  cout << "database_name: " << s << "\n";

  cout << "column_names:";
  for(i = 0; i <  NumDataMembers; i++) {
    s = db_config.column_names[i].GetString();
    if(s) cout << " " << s;
  }
  cout << "\n";
  
  cout << "view_labels: " << db_config.view_labels << "\n";
  cout << "view_row_numbers: " << db_config.view_row_numbers << "\n";

  cout << "col_sizes: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.col_sizes[i];
  }
  cout << "\n";

  cout << "print_field: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.print_field[i];
  }
  cout << "\n";

  cout << "cell_attrib: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_attrib[i];
  }
  cout << "\n";

  cout << "cell_align: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_align[i];
  }
  cout << "\n";

  cout << "cell_border: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_border[i];
  }
  cout << "\n";

  cout << "cell_color: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_color[i];
  }
  cout << "\n";

  cout << "cell_fill: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_fill[i];
  }
  cout << "\n";

  cout << "cell_text_color: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_text_color[i];
  }
  cout << "\n";

  cout << "cell_text_font: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_text_font[i];
  }
  cout << "\n";

  cout << "cell_type: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_type[i];
  }
  cout << "\n";

  cout << "cell_pattern: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_pattern[i];
  }
  cout << "\n";

  cout << "cell_protection: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_protection[i];
  }
  cout << "\n";

  cout << "label_align: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.label_align[i];
  }
  cout << "\n";

  cout << "label_border: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.label_border[i];
  }
  cout << "\n";

  cout << "label_color: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.label_color[i];
  }
  cout << "\n";

  cout << "label_fill: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.label_fill[i];
  }
  cout << "\n";

  cout << "label_text_font: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.label_text_font[i];
  }
  cout << "\n";

  cout << "label_text_color: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.label_text_color[i];
  }
  cout << "\n";

  cout << "label_pattern: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.label_pattern[i];
  }
  cout << "\n";

  cout << "label_protection: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.label_protection[i];
  }
  cout << "\n";

  cout << "display_field: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.display_field[i];
  }
  cout << "\n";

  cout << "cell_misc: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.cell_misc[i];
  }
  cout << "\n";

  cout << "label_misc: ";
  for(i = 0; i <  NumDataMembers; i++) {
    cout << " " <<  db_config.label_misc[i];
  }
  cout << "\n";

  cout << "auto_size: " << db_config.auto_size << "\n";
  cout << "cell_overflow: " << db_config.cell_overflow << "\n";
  cout << "view_grid_lines: " << db_config.view_grid_lines << "\n";
  cout << "grid_line_color: " << db_config.grid_line_color << "\n";
  cout << "view_skip_lines: " << db_config.view_skip_lines << "\n";
  cout << "skip_line_color: " << db_config.skip_line_color << "\n";
  cout << "grid_background_color: " << db_config.grid_background_color << "\n";
  cout << "grid_text_color: " << db_config.grid_text_color << "\n";
  cout << "grid_label_background_color: " << db_config.grid_label_background_color << "\n";
  cout << "grid_label_text_color: " << db_config.grid_label_text_color << "\n";
  cout << "hyperlink_color: " << db_config.hyperlink_color << "\n";

  cout << "grid_label_font: ";
  for(i = 0; i <  FontElements; i++) {
    cout << " " <<  db_config.grid_label_font[i];
  }
  cout << "\n";

  s = db_config.grid_label_font_name.GetString();
  if(s) cout << "grid_label_font_name: " << s << "\n";
  
  cout << "grid_text_font: ";
  for(i = 0; i <  FontElements; i++) {
    cout << " " <<  db_config.grid_text_font[i];
  }
  cout << "\n";

  s = db_config.grid_text_font_name.GetString();
  if(s) cout << "grid_text_font_name: " << s << "\n";
  
  cout << "hyperlink_font: ";
  for(i = 0; i <  FontElements; i++) {
    cout << " " <<  db_config.hyperlink_font[i];
  }
  cout << "\n";

  s = db_config.hyperlink_font_name.GetString();
  if(s) cout << "hyperlink_font_name: " << s << "\n";
  
  s = db_config.print_doc_name.GetString();
  if(s) cout << "print_doc_name: " << s << "\n";
  
  cout << "print_doc_time_and_date: " << db_config.print_doc_time_and_date << "\n";
  cout << "print_grid_lines: " << db_config.print_grid_lines << "\n";
  cout << "print_orientation: " << db_config.print_orientation << "\n";
  cout << "print_left_margin_size: " << db_config.print_left_margin_size << "\n";
  cout << "print_paper_size: " << db_config.print_paper_size << "\n";
  cout << "print_page_header: " << db_config.print_page_header << "\n";

  s = db_config.print_doc_custom_header.GetString();
  if(s) cout << "print_doc_custom_header: " << s << "\n";
  
  cout << "print_header_color: " << db_config.print_header_color << "\n";

  cout << "print_header_font: ";
  for(i = 0; i <  FontElements; i++) {
    cout << " " <<  db_config.print_header_font[i];
  }
  cout << "\n";

  s = db_config.print_header_font_name.GetString();
  if(s) cout << "print_header_font_name: " << s << "\n";
  
  cout << "print_page_footer: " << db_config.print_page_footer << "\n";

  s = db_config.print_doc_custom_footer.GetString();
  if(s) cout << "print_doc_custom_footer: " << s << "\n";
  
  cout << "print_footer_color: " << db_config.print_footer_color << "\n";

  cout << "print_footer_font: ";
  for(i = 0; i <  FontElements; i++) {
    cout << " " <<  db_config.print_footer_font[i];
  }
  cout << "\n";

  s = db_config.print_footer_font_name.GetString();
  if(s) cout << "print_footer_font_name: " << s << "\n";
  
  cout << "print_skip_lines: " << db_config.print_skip_lines << "\n";
  cout << "print_grid_labels: " << db_config.print_grid_labels << "\n";
  cout << "print_row_numbers: " << db_config.print_row_numbers << "\n";
  cout << "print_page_scale: " << db_config.print_page_scale << "\n";

  cout << "cell_text_fonts:";
  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    for(j = 0; j < (unsigned)FontElements; j++) cout << " " << db_config.cell_text_fonts[i][j];
  }
  cout << "\n";

  cout << "cell_text_font_names:";
  for(i = 0; i <  NumDataMembers; i++) {
    s = db_config.cell_text_font_names[1].GetString();
    if(s) cout << " " << s;
  }
  cout << "\n";

  cout << "label_text_fonts:";
  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    for(j = 0; j < (unsigned)FontElements; j++) cout << " " << db_config.label_text_fonts[i][j];
  }
  cout << "\n";

  cout << "label_text_font_names:";
  for(i = 0; i <  NumDataMembers; i++) {
    s = db_config.label_text_font_names[i].GetString();
    if(s) cout << " " << s;
  }
  cout << "\n";

  if(db_config.text_delimiter[0] != 0) {
    if(db_config.text_delimiter[0] == '\t') {
      cout << "text_delimiter 1: tab" << "\n";
    }
    else {
      cout << "text_delimiter 1: " << db_config.text_delimiter[0] << "\n";
    }
  }
  if(db_config.text_delimiter[1] != 0) cout << "text_delimiter 2: " << db_config.text_delimiter[1] << "\n";
  if(db_config.text_delimiter[2] != 0) cout << "text_delimiter 3: " << db_config.text_delimiter[2] << "\n";
  if(db_config.text_delimiter[3] != 0) cout << "text_delimiter 4: " << db_config.text_delimiter[3] << "\n";
  cout << "\n";

  return 0;
}

int PrintDBStats(const char *fname)
{
  gxString err_string;
  gxDatabase *f = OpenEPDB(fname, err_string);
  if(!f) {
    cerr << "ERROR: " << err_string.c_str() << "\n" << flush;
    return 1;
  }

  cout << "Displaying database stats for " << fname << "\n" << flush;
  DatabaseStats(f);

  f->Close();
  delete f;
  return 0;
}

int PrintDBConfig(const char *fname, MemoryBuffer &key)
{
  gxString err_string;
  gxDatabase *f = OpenEPDB(fname, err_string);
  if(!f) {
    cerr << "ERROR: " << err_string.c_str() << "\n" << flush;
    return 1;
  }

  gxDatabaseConfig db_config;
  if(UnlockEPDB(f, key, db_config, err_string) != 0) {
    cerr << "ERROR: " << err_string.c_str() << "\n" << flush;
    f->Close();
    delete f;
    return 1;
  }

  PrintDBConfig(db_config);

  f->Close();
  delete f;
  
  return 0;
}

int ListUsers(const char *fname)
{
  gxString err_string;
  gxDatabase *f = OpenEPDB(fname, err_string);
  if(!f) {
    cerr << "ERROR: " << err_string.c_str() << "\n" << flush;
    return 1;
  }

  DatabaseUserAuth db_auth;
  db_auth.f = f; // Set the file pointer
  
  cout << "Checking database file for authorized users" << "\n" << flush;
  if(db_auth.LoadStaticDataBlocks() != 0) {
    cerr << "ERROR: " << db_auth.err.c_str() << "\n" << flush;
    f->Close();
    delete f;
    return 1;
  }

  cout << "\n" << flush;
  cout << "Authorized users stats" << "\n" << flush;
  cout << "----------------------" << "\n" << flush;
  
  cout << "Static auth data size: " << db_auth.static_data_size << "\n" << flush;
  cout << "Static auth bytes used: " << db_auth.static_data_bytes_used << "\n" << flush;
  cout << "Number of static blocks: " <<  db_auth.num_static_data_blocks << "\n" << flush;
  
  gxListNode<StaticDataBlock> *list_ptr = db_auth.static_block_list.GetHead();
  if(!list_ptr) {
    cout << "INFO: No authorized RSA or smartcard users found in encrypted DB file" << "\n" << flush;
  }
  else {
    cout << "Encrypted file username inventory " << "\n" << flush;
    gxString access_type = "Unknown";
    while(list_ptr) {
      if(list_ptr->data.block_header.block_type == 1) access_type = "RSA key";
      if(list_ptr->data.block_header.block_type == 2) access_type = "Smart Card";
      cout << "Username: " << list_ptr->data.username.c_str() << " Access: " << access_type.c_str() << "\n" << flush;
      list_ptr = list_ptr->next;
    }
  }
  cout << "\n" << flush;

  f->Close();
  delete f;
  return 0;
}

int BuildVirtualEPDB(POD *pod)
{
  if(!dllist || !pod) return 0;

  INFOHOGKEY key, compare_key;
  gxBtree *btx = pod->Index();
  dllist->ClearList();

  btx->TestTree();

  int num_entries = 0;
  FAU object_id;
  if(btx->FindFirst(key, 0)) {
    object_id = key.ObjectID();
    dllist->Add(object_id);
    num_entries++;
  }
  while(btx->FindNext(key, compare_key, 0)) {
    object_id = key.ObjectID();
    dllist->Add(object_id);
    num_entries++;
  }

  return num_entries;
}

int CSVExportEPDB(POD *pod)
{
  if(!dllist || !pod) return 0;

  if(!BuildVirtualEPDB(pod)) return 0;
  
  char dest[DBStringLength];
  DBString *dbstring;
  gxString dbuf;
  const char dchar = ',';
  gxString dchar_str;
  dchar_str << clear << dchar;
  const char *Fill = " "; // Fill character string
  unsigned i;
  int num_entries = 0;
  int exports = 0;
  
  dllistptr = dllist->GetHead();
  while(dllistptr) {
    INFOHOG infohog(pod);
    infohog.ReadObject(dllistptr->data);
    dbstring = (DBString *)infohog.GetMember(0);
    INFOHOG_t ob(dbstring->c_str(dest));
    dbuf << clear << ob.c_str(dest);
    if(dbuf.Find(dchar_str) != -1) dbuf << clear << "\"" <<  ob.c_str(dest) << "\"";
    dbuf.FilterChar('\n');
    cout << dbuf.c_str();
    cout << dchar;
    // Write the remaining members account for null members
    for(i = 1; i < (unsigned)NumDataMembers; i ++) {
      if(infohog.GetMemberLen(i) == sizeof(INFOHOG_t)) {
        dbstring = (DBString *)infohog.GetMember(i);
        if(!dbstring->is_null()) {
          ob = dbstring->c_str(dest);
          dbuf << clear << ob.c_str(dest);
          if(dbuf.Find(dchar_str) != -1) dbuf << clear << "\"" <<  ob.c_str(dest) << "\"";
	  dbuf.FilterChar('\n');
          cout << dbuf.c_str();
        }
        else {
          cout << Fill;
        }
      }
      else {
        cout << Fill;
      }
      // Reached the last member so do not print the delimiter
      if(i == (NumDataMembers - 1)) break;
      cout << dchar;
    }
    cout << "\n" << flush;
    exports++;
    dllistptr = dllistptr->next;
  }

  return exports;
}

POD *OpenEPDB(gxString &fname, int &admin_rights, gxString &err_string)
{
  gxString data_ext = ".ehd";
  gxString index_ext = ".ehx";
  char database_revision = 'E'; // Database revision letter
  return OpenEPDB(fname, data_ext, index_ext, database_revision, admin_rights, err_string);
}

POD *OpenEPDB(gxString &fname, gxString &data_ext, gxString &index_ext, char database_revision, int &admin_rights, gxString &err_string)
{
  err_string.Clear();
  gxString database_file = fname;
  gxString index_file = fname;
  gxString sbuf;
  admin_rights = 1;
  
  database_file.DeleteAfterLastIncluding(".");
  database_file += data_ext;
  index_file.DeleteAfterLastIncluding(".");
  index_file += index_ext;

  if(!futils_exists(database_file.c_str()) || !futils_isfile(database_file.c_str())) {
    err_string << clear << "Database file " << database_file.c_str() << " does not exist or cannot be read";
    return 0;
  }
  if(!futils_exists(index_file.c_str()) || !futils_isfile(index_file.c_str())) {
    err_string << clear << "Index file " << index_file.c_str() << " does not exist or cannot be read";
    return 0;
  }
  
  POD *pod = new POD;
  if(!pod) {
    err_string << clear << "Could not create new POD object";
    return 0;
  }

  InfoHogKey<DBString> key_type;
  FAU_t static_data_size = (FAU_t)(DB_CONFIG_STATIC_AREA_SIZE + DB_AUTH_STATIC_AREA_SIZE);

  // Open existing database using a single index file
  gxDatabaseError err = pod->Open(database_file.c_str(), 
				  index_file.c_str(), key_type,
				  InfoHogNodeOrder, gxDBASE_READWRITE, 
				  InfoHogUseIndexFile, 
				  static_data_size,
				  InfoHogNumTrees,
				  database_revision,
				  database_revision);
  if(err != gxDBASE_NO_ERROR) {
    // Try to open the database with read-only access
    err = pod->Open(database_file.c_str(), index_file.c_str(), key_type,
		    InfoHogNodeOrder, gxDBASE_READONLY,
		    InfoHogUseIndexFile, 
		    static_data_size,
		    InfoHogNumTrees,
		    database_revision,
		    database_revision);

    if(err == gxDBASE_NO_ERROR) {
      admin_rights = 0;
    }
  }
  if(err != gxDBASE_NO_ERROR) {
    delete pod;
    err_string << clear << gxDatabaseExceptionMessage(err);
    return 0;
  }

  // Rebuild the index file is neccessary
  if(pod->RebuildIndex()) {
    delete pod;
    err_string << clear << "Index file need to be rebuilt " << index_file.c_str();
    return 0;
  }
  
  return pod;
}

gxDatabase *OpenEPDB(const char *fname, gxString &err_string)
{
  err_string.Clear();
  
  if(!futils_exists(fname) || !futils_isfile(fname)) {
    err_string << clear << "Encrypted password database file " << fname << " does not exist or cannot be read";
    return 0;
  }

  gxDatabaseError err;
  gxDatabase *f = new gxDatabase;
  if(!f) {
    err_string << clear << "ERROR: Cannot create database object";
    return 0;
  }

  err = f->Open(fname,  gxDBASE_READWRITE);
  if(err != gxDBASE_NO_ERROR) {
    err_string << clear << gxDatabaseExceptionMessage(err);
    delete f;
    return 0;
  }

  return f;
}
int UnlockEPDB(gxDatabase *f, MemoryBuffer &key, gxString &err_string)
{
  gxDatabaseConfig db_config;
  return UnlockEPDB(f, key, db_config, err_string);
}

int UnlockEPDB(gxDatabase *f, MemoryBuffer &key, gxDatabaseConfig &db_config, gxString &err_string)
{
  err_string.Clear();
  
  if(!f) { 
    err_string << clear << "Database object is a null pointer";
    return 1;
  }
  
  if(!f->IsOpen() || !f->ReadyForReading()) {
    err_string << clear << "Database is not open and ready for reading";
    return 1;
  }
  
  DBStringConfig::crypt_key = key;
  db_config.Clear();
  db_config.ReadConfig(f);
  char *s = db_config.database_name.GetString();
  if(DBStringConfig::AES_error_level != AES_NO_ERROR) {
    err_string << clear << "Error decrypting database hash " << AES_err_string(DBStringConfig::AES_error_level);
    return 1;
  }
  if(!s) {
    err_string << clear << "Database hash is a null value";
    return 1;
  }

  return 0;
}
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
