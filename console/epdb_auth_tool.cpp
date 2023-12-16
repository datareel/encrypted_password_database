// ------------------------------- //
// -------- start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, BCC32, GCC, HPUX aCC, SOLARIS CC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
// Date Last Modified: 12/10/2023
// Copyright (c) 2001-2023 DataReel Software Development
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

Encrypted password database auth tool 
*/
// ----------------------------------------------------------- // 
#include "gxdlcode.h"

#if defined (__USE_ANSI_CPP__) // Use the ANSI Standard C++ library
#include <iostream>
using namespace std; // Use unqualified names for Standard C++ library
#else // Use the old iostream library by default
#include <iostream.h>
#endif // __USE_ANSI_CPP__

#include "db_auth.h"

#ifdef __MSVC_DEBUG__
#include "leaktest.h"
#endif

#include "gxdbase.h"
#include "gxdstats.h"
#include "memblock.h"
#include "gxlist.h"

MemoryBuffer key;
gxString password;
gxString key_file;
gxString rsa_public_key_file;
gxString smardcard_cert_file;
char public_key[RSA_max_keybuf_len];
unsigned public_key_len = 0;
gxString rsa_key_passphrase;
int has_passphrase = 0;
int ERROR_LEVEL = 0;
gxString executable_name;
gxString HOMEdir;
gxString USERNAME;
SmartCardOB sc;
gxString smartcard_cert_username;
int use_cert_file = 0;
gxString smartcard_cert_file;

int PrintDBConfig(gxDatabaseConfig &db_config);


int main(int argc, char **argv)
{
  HOMEdir = getenv("HOME");
  USERNAME = getenv("USERNAME");
  
  executable_name = argv[0];

  if(argc < 2) {
    cout << "ERROR: You must supply a file name" << "\n";
    return 1;
  }

  gxString sbuf;
  gxString fname;
  gxDatabaseError err;
  int rv;
  
  fname = argv[1];

  if(!futils_exists(fname.c_str()) || !futils_isfile(fname.c_str())) {
    cout << "ERROR: Encrypted DB file " << fname << " does not exist or cannot be read";
    return 1;
  }
  
  gxDatabase *f = new gxDatabase;
  if(!f) {
    cout << "ERROR: Cannot create database object" << "\n";
    return 1;
  }

  cout << "Opening database file " << fname.c_str() << "\n";
  err = f->Open(fname.c_str(),  gxDBASE_READWRITE);
  if(err != gxDBASE_NO_ERROR) {
    cout << "ERROR: " << gxDatabaseExceptionMessage(err) << "\n";
    delete f;
    return 1;
  }

  gxDatabaseConfig db_config;
  DatabaseUserAuth db_auth;

  DatabaseStats(f);

  db_auth.f = f; // Set the file pointer

  cout << "Checking database file for authorized users" << "\n";
  if(db_auth.LoadStaticDataBlocks() != 0) {
    cout << "ERROR: " << db_auth.err.c_str() << "\n";
  }

  cout << "\n";
  cout << "Authorized users stats" << "\n";
  cout << "----------------------" << "\n";
  
  cout << "Static auth data size: " << db_auth.static_data_size << "\n";
  cout << "Static auth bytes used: " << db_auth.static_data_bytes_used << "\n";
  cout << "Number of static blocks: " <<  db_auth.num_static_data_blocks << "\n";
  
  gxListNode<StaticDataBlock> *list_ptr = db_auth.static_block_list.GetHead();
  if(!list_ptr) {
    cout << "INFO: No authorized RSA or smartcard users found in encrypted DB file" << "\n" << flush; ;
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

  key_file = "${HOME}/.encrypted_password_database/keys/master.key";
  key_file.ReplaceString("${HOME}", HOMEdir.c_str());
  if(!futils_exists(key_file.c_str()) || !futils_isfile(key_file.c_str())) {
    cout << "ERROR: Key file " << key_file.c_str() << " does not exist or cannot be read" <<  "\n" << flush;
    return 1;
  }
  cout << "Reading symmetric key file" << "\n";
  if(read_key_file(key_file.c_str(), key, sbuf) != 0) {
    cout << "ERROR: " << sbuf.c_str() << "\n" << flush;
    return 1;
  }

  rsa_public_key_file = "${HOME}/.encrypted_password_database/keys/public.pem";
  rsa_public_key_file.ReplaceString("${HOME}", HOMEdir.c_str());
  
  if(!futils_exists(rsa_public_key_file.c_str()) || !futils_isfile(rsa_public_key_file.c_str())) {
    cout << "ERROR: Public RSA key file " << rsa_public_key_file.c_str() << " does not exist or cannot be read" <<  "\n" << flush;
    return 1;
  }
  rv = RSA_read_key_file(rsa_public_key_file.c_str(), public_key, sizeof(public_key), &public_key_len, &has_passphrase);
  if(rv != RSA_NO_ERROR) {
    std::cerr << "ERROR: " << RSA_err_string(rv) << "\n" << std::flush;
    return 1;
  }


  smartcard_cert_file = "${HOME}/.encrypted_password_database/certs/smartcard_cert.pem";
  smartcard_cert_file.ReplaceString("${HOME}", HOMEdir.c_str());
  if(!futils_exists(smartcard_cert_file.c_str()) || !futils_isfile(smartcard_cert_file.c_str())) {
    cerr << "ERROR: Smart card cert file " << smartcard_cert_file.c_str() << " does not exist or cannot be read" <<  "\n" << flush;
    return 1;
  }
  if(SC_read_cert_file(&sc, smartcard_cert_file.c_str()) != 0) {
    cerr << "ERROR: " << sc.err_string << "\n" << flush;
    return 1;
  }

  
  DBStringConfig::crypt_key = key;
  db_config.ReadConfig(f);
  char *s = db_config.database_name.GetString();
  if(DBStringConfig::AES_error_level != AES_NO_ERROR) {
    cout << "ERROR: Error decrypting database hash " << AES_err_string(DBStringConfig::AES_error_level) << "\n";
    return 1;
  }
  if(!s) {
    cout << "ERROR: Database hash is a null value" << "\n";
    return 1;
  }

  rv = db_auth.AddRSAKeyToStaticArea(key, public_key, public_key_len, USERNAME);
  if(rv != 0) {
    cout << "ERROR: Cannot add public RSA key " << db_auth.err.c_str() << "\n";
    //  return 1;
  }

  use_cert_file = 1;
  rv = db_auth.AddSmartCardCertToStaticArea(&sc, use_cert_file, key, USERNAME);
  if(rv != 0) {
    cout << "ERROR: Cannot add smart card cert " << db_auth.err.c_str() << "\n";
    return 1;
  }
  

  // PrintDBConfig(db_config);
    
  return ERROR_LEVEL;
}

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
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
