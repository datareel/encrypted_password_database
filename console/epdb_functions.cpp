// ------------------------------- //
// -------- start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, BCC32, GCC, HPUX aCC, SOLARIS CC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
// Date Last Modified: 01/08/2024
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
#include <ctype.h>

#if defined (__UNIX__)
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

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

void StoreObjectAddress(BtreeNode *n)
// Prints a single B-tree node.
{
  INFOHOGKEY key;
  BtreeKeyCount_t i = (BtreeKeyCount_t)0;
  while (i < n->key_count) {
    n->LoadKey(key, (BtreeKeyLocation_t)i);
    FAU object_id = key.ObjectID();
    dllist->Add(object_id);
    i++;
  }
}

// 10/25/2026: Output format for --db-list and --db-find-key
int epdb_output_mode = EPDB_OUTPUT_DEFAULT;
unsigned epdb_output_field = 0; // Field for EPDB_OUTPUT_FIELD, 0 is the key
int epdb_output_null = 0;       // End single field values with NUL

// 10/25/2026: Set when a B-tree node cannot be read during a walk, so a
// damaged index file is reported instead of listing part of the database.
int epdb_btree_errors = 0;
const unsigned gxBtreeMaxDepthWalk = 64; // Deeper than any valid tree

void gxBtreeWalk(FAU_t t, BtreeVisitFunc Visit, gxBtree *tree)
// Recursive function used to walk through the B-tree node by node.
{
  BtreeKeyCount_t i;

  // Ensure that the in memory buffers and the file data
  // stay in sync during multiple file access.
  if(tree) {
    tree->TestTree();
  }
  else {
    return;
  }

  BtreeNode n(tree->KeySize(), tree->NodeOrder());
  
  // 10/25/2026: A node that points back up the tree recursed forever
  static unsigned depth = 0;
  if(t != (FAU_t)0) {
    if(epdb_btree_errors) return;
    if(depth >= gxBtreeMaxDepthWalk) {
      epdb_btree_errors++;
      return;
    }
    if(tree->ReadNode(n, t) != gxDBASE_NO_ERROR) {
      epdb_btree_errors++;
      return;
    }
    n.node_address = t;
    (*Visit)(&n); // Process the node data
    BtreeKeyCount_t nc = n.key_count;
    FAU_t p;

    depth++;
    for(i = (BtreeKeyCount_t)-1; i < nc; i++) {
      p = n.GetBranch(i);
      gxBtreeWalk(p, Visit, tree);  
    }
    depth--;
  }
}

int BuildVirtualEPDB(POD *pod)
{
  if(!dllist || !pod) return 0;

  int num_entries = 0;
  gxBtree *btx = pod->Index();
  dllist->ClearList();
  epdb_btree_errors = 0;
  gxBtreeWalk(btx->Root(), StoreObjectAddress, btx);
  if(epdb_btree_errors) {
    cerr << "ERROR: Cannot read the database index file" << "\n" << flush;
    dllist->ClearList();
    return 0;
  }

  dllistptr = dllist->GetHead();
  while(dllistptr) {
    num_entries++;
    dllistptr = dllistptr->next;
  }

  return num_entries;
}

int ExportEPDB(POD *pod, char delimiter, char filter_char, char replace_char, char replace_char_with, int *read_errors)
{
  if(!pod) return 0;
  if(!BuildVirtualEPDB(pod)) return 0;
  return ExportListObjectsEPDB(pod, delimiter, filter_char, replace_char, replace_char_with, read_errors);
}

int ExportListObjectsEPDB(POD *pod, char delimiter, char filter_char, char replace_char, char replace_char_with, int *read_errors)
// Writes the objects in the dllist to stdout. Returns the number of
// objects written or -1 if a field could not be decrypted. Records that
// cannot be read are skipped and counted in read_errors.
// 10/25/2026: A field that cannot be decrypted stops the export. It
// used to be written as an empty field.
{
  if(read_errors) *read_errors = 0;
  if(!dllist || !pod) return 0;
  
  char dest[DBStringLength];
  DBString *dbstring;
  gxString dbuf;
  if(delimiter == 0) delimiter = '\t';
  gxString delimiter_str;
  delimiter_str << clear << delimiter;
  const char *Fill = " "; // Fill character string
  unsigned i;
  int exports = 0;
  
  dllistptr = dllist->GetHead();
  while(dllistptr) {
    INFOHOG infohog(pod);
    // 10/25/2026: Skip records that cannot be read or have a bad key member
    if(!infohog.ReadObject(dllistptr->data) ||
       (infohog.GetMemberLen(0) != sizeof(INFOHOG_t))) {
      cerr << "ERROR: Cannot read database record, skipping" << "\n" << flush;
      if(read_errors) (*read_errors)++;
      dllistptr = dllistptr->next;
      continue;
    }
    // Decrypt all the fields of the record before writing any of it
    gxString fields[NumDataMembers];
    int is_set[NumDataMembers];
    for(i = 0; i < (unsigned)NumDataMembers; i++) {
      is_set[i] = 0;
      if(infohog.GetMemberLen(i) != sizeof(INFOHOG_t)) continue;
      dbstring = (DBString *)infohog.GetMember(i);
      int err = 0;
      char *s = dbstring->GetString(DBStringConfig::crypt_key, err);
      if(!s || err != 0) {
	cerr << "ERROR: Cannot decrypt database record field " << i << " " << AES_err_string(err) << "\n" << flush;
	for(unsigned j = 0; j < (unsigned)NumDataMembers; j++) fields[j].Clear(1);
	return -1;
      }
      if(s != DBStringNULLPtr::DBStringNUllStr) {
	if(s[0] != 0) { fields[i] = s; is_set[i] = 1; }
	OPENSSL_cleanse(s, strlen(s));
	delete[] s;
      }
    }
    if(!is_set[0]) { // Every record has a key name
      cerr << "ERROR: Database record has no key name, skipping" << "\n" << flush;
      if(read_errors) (*read_errors)++;
      dllistptr = dllistptr->next;
      continue;
    }
    // 10/25/2026: Single field output, the value as stored with no
    // quoting, ended by a newline or with --null by a NUL character
    if(epdb_output_mode == EPDB_OUTPUT_FIELD) {
      unsigned f = epdb_output_field;
      if(is_set[f] && !epdb_output_null && fields[f].Find("\n") != -1) {
	cerr << "ERROR: Field " << (f+1) << " of record " << fields[0].c_str()
	     << " has a newline, use --null to separate values with NUL characters" << "\n" << flush;
	for(unsigned j = 0; j < (unsigned)NumDataMembers; j++) fields[j].Clear(1);
	return -1;
      }
      if(is_set[f]) cout.write(fields[f].c_str(), fields[f].length());
      if(epdb_output_null) cout.put('\0'); else cout << "\n";
      cout << flush;
      for(unsigned j = 0; j < (unsigned)NumDataMembers; j++) fields[j].Clear(1);
      exports++;
      dllistptr = dllistptr->next;
      continue;
    }

    // 10/25/2026: RFC 4180 CSV output. Fields with the delimiter, a quote,
    // CR or LF are quoted with quotes doubled. Empty fields are empty.
    if(epdb_output_mode == EPDB_OUTPUT_CSV) {
      for(i = 0; i < (unsigned)NumDataMembers; i ++) {
	if(i > 0) cout << delimiter;
	if(!is_set[i]) continue;
	dbuf << clear << fields[i];
	if(filter_char != 0) dbuf.FilterChar(filter_char);
	if(replace_char != 0 && replace_char_with != 0) dbuf.ReplaceChar(replace_char, replace_char_with);
	if((dbuf.Find(delimiter_str) != -1) || (dbuf.Find("\"") != -1) ||
	   (dbuf.Find("\r") != -1) || (dbuf.Find("\n") != -1)) {
	  dbuf.ReplaceString("\"", "\"\"");
	  cout << "\"" << dbuf.c_str() << "\"";
	}
	else {
	  cout << dbuf.c_str();
	}
	dbuf.Clear(1);
	fields[i].Clear(1);
      }
      cout << "\r\n" << flush;
      exports++;
      dllistptr = dllistptr->next;
      continue;
    }

    for(i = 0; i < (unsigned)NumDataMembers; i ++) {
      if(is_set[i]) {
	// 10/25/2026: Filter and replace characters before quoting, remove
	// CR as well as LF, and double quotes inside a quoted field. The
	// filter and replace used to run after quoting, which could add
	// an unquoted delimiter.
	dbuf << clear << fields[i];
	dbuf.FilterChar('\n');
	dbuf.FilterChar('\r');
	if(filter_char != 0) dbuf.FilterChar(filter_char);
	if(replace_char != 0 && replace_char_with != 0) dbuf.ReplaceChar(replace_char, replace_char_with);
	if(dbuf.Find(delimiter_str) != -1) {
	  dbuf.ReplaceString("\"", "\"\"");
	  cout << "\"" << dbuf.c_str() << "\"";
	}
	else {
	  cout << dbuf.c_str();
	}
	dbuf.Clear(1);
	fields[i].Clear(1);
      }
      else {
	cout << Fill;
      }
      // Reached the last member so do not print the delimiter
      if(i == (unsigned)(NumDataMembers - 1)) break;
      cout << delimiter;
    }
    cout << "\n" << flush;
    exports++;
    dllistptr = dllistptr->next;
  }

  return exports;
}

POD *OpenEPDB_POD(gxString &fname, gxString &err_string, gxDatabaseAccessMode mode)
{
  gxString data_ext = ".ehd";
  gxString index_ext = ".ehx";
  char database_revision = 'E'; // Database revision letter
  return OpenEPDB_POD(fname, data_ext, index_ext, database_revision, err_string, mode);
}

POD *OpenEPDB_POD(gxString &fname, gxString &data_ext, gxString &index_ext, char database_revision, gxString &err_string, gxDatabaseAccessMode mode)
{
  err_string.Clear();
  gxString database_file = fname;
  gxString index_file = fname;
  gxString sbuf;
  
  
  // 10/25/2026: The name must end in the data file extension, so the
  // file checked with the key is the file listed. Replace only the
  // extension of the file name for the index file.
  if(fname.length() <= data_ext.length() ||
     CaseICmp(gxString(fname.c_str() + (fname.length() - data_ext.length())), data_ext) != 0) {
    err_string << clear << "Database file name " << fname.c_str() << " does not end in " << data_ext.c_str();
    return 0;
  }
  database_file = ReplaceFileExtension(fname, data_ext.c_str());
  index_file = ReplaceFileExtension(fname, index_ext.c_str());

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
				  InfoHogNodeOrder, mode, 
				  InfoHogUseIndexFile, 
				  static_data_size,
				  InfoHogNumTrees,
				  database_revision,
				  database_revision);
  if(err != gxDBASE_NO_ERROR) {
    delete pod;
    err_string << clear << gxDatabaseExceptionMessage(err);
    return 0;
  }

  // Rebuild the index file is neccessary
  if(pod->RebuildIndex() && mode == gxDBASE_READWRITE) {
    delete pod;
    err_string << clear << "Index file need to be rebuilt " << index_file.c_str();
    return 0;
  }
  
  return pod;
}

gxDatabase *OpenEPDB(const char *fname, gxString &err_string, gxDatabaseAccessMode mode)
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

  err = f->Open(fname,  mode);
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
  
  // 10/25/2026: Check the key. For a v2 database the password or key is
  // replaced by the database keyset.
  MemoryBuffer keyset;
  int aes_error = 0;
  if(EPDBUnlock(f, key, keyset, err_string, aes_error) != 0) return 1;
  key = keyset;
  keyset.Clear(1);
  DBStringConfig::crypt_key = key;
  db_config.Clear();
  db_config.ReadConfig(f);

  return 0;
}

unsigned SearchEPDB(gxBtree *btx, int item, POD *pod, INFOHOG_t &ob, int find_all)
{
  if(!pod || !btx) return 0;
  if(!BuildVirtualEPDB(pod)) return 0;

  unsigned objects_found = 0;
  INFOHOGKEY key;
  gxList<FAU> address_list;
  
  dllistptr = dllist->GetHead();
  while(dllistptr) {
    FAU object_address = dllistptr->data;
    address_list.Add(object_address);
    dllistptr = dllistptr->next;
  }

  dllist->ClearList();
  gxListNode<FAU> *ptr =  address_list.GetHead();
  while(ptr) {
    INFOHOG infohog(pod);
    // 10/25/2026: Report records that cannot be read instead of
    // silently ending the search
    if(!infohog.ReadObject(ptr->data) ||
       (infohog.GetMemberLen(0) != sizeof(INFOHOG_t))) {
      cerr << "ERROR: Cannot read database record" << "\n" << flush;
      epdb_btree_errors++;
      ptr = ptr->next;
      continue;
    }
    DBString dbstring = *((DBString *)infohog.GetMember(0));
    FAU object_id = ptr->data;
    key.SetObjectName(dbstring);
    key.SetObjectID(object_id); 
    KeySearchEPDB(key, item, pod, ob, objects_found, find_all);
    ptr = ptr->next;
  }
  return objects_found;
}

void KeySearchEPDB(INFOHOGKEY &key, int item, POD *pod,
		   INFOHOG_t &ob, unsigned &objects_found, int find_all)
{
  int offset;
  INFOHOG_t buf;
  FAU object_id;
  objects_found = 0;
  
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
    else { // Search for all matches
      if(infohog.GetMemberLen(item) == sizeof(INFOHOG_t)) {
	buf = *((INFOHOG_t *)infohog.GetMember(item));
	char *s1 = buf.c_str(dest1);
	char *s2 = ob.c_str(dest2);
	offset = IFindMatch(s1, s2, 0);
	if(offset != -1) {
	  object_id = key.ObjectID();
	  dllist->Add(object_id);
	  objects_found++;
	}
      }
      else {
	objects_found = 0;
      }
    }
  }
}
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

// 10/25/2026: Read a password, passphrase or PIN from a file or an open
// file descriptor, so scripts do not have to put it on the command line
// where other users can see it with ps. The secret is the first line,
// without the line ending. Returns 0 if successful or 1 with err set.
static int ReadSecretLine(int fd, gxString &secret, gxString &err, const char *name)
{
  const unsigned max_len = 4096;
  char buf[max_len+1];
  unsigned len = 0;
  int rv = 0;
  while(len < max_len) {
#if defined (__UNIX__)
    ssize_t n = read(fd, buf+len, 1);
#else
    int n = _read(fd, buf+len, 1);
#endif
    if(n < 0) {
      err << clear << "Cannot read " << name;
      rv = 1;
      break;
    }
    if(n == 0) break; // End of file
    if(buf[len] == '\n') break;
    len++;
  }
  if(rv == 0 && len >= max_len) {
    err << clear << name << " is too long";
    rv = 1;
  }
  if(rv == 0 && len > 0 && buf[len-1] == '\r') len--;
  if(rv == 0 && len == 0) {
    err << clear << name << " is empty";
    rv = 1;
  }
  if(rv == 0) {
    secret.Clear(1);
    secret.SetString(buf, len);
  }
  OPENSSL_cleanse(buf, sizeof(buf));
  return rv;
}

int ReadSecretFile(const char *fname, gxString &secret, gxString &err)
{
  err.Clear();
#if defined (__UNIX__)
  int fd = open(fname, O_RDONLY|O_NOFOLLOW|O_CLOEXEC);
  if(fd < 0) {
    err << clear << "Cannot open secret file " << fname << " (it must exist and not be a symbolic link)";
    return 1;
  }
  struct stat st;
  if(fstat(fd, &st) != 0 || !S_ISREG(st.st_mode)) {
    err << clear << "Secret file " << fname << " is not a regular file";
    close(fd);
    return 1;
  }
  if(st.st_uid != geteuid()) {
    err << clear << "Secret file " << fname << " is not owned by the current user";
    close(fd);
    return 1;
  }
  if(st.st_mode & (S_IRWXG|S_IRWXO)) {
    err << clear << "Secret file " << fname << " can be read or written by other users, use chmod 600";
    close(fd);
    return 1;
  }
  int rv = ReadSecretLine(fd, secret, err, fname);
  close(fd);
  return rv;
#else
  int fd = _open(fname, _O_RDONLY|_O_BINARY);
  if(fd < 0) {
    err << clear << "Cannot open secret file " << fname;
    return 1;
  }
  int rv = ReadSecretLine(fd, secret, err, fname);
  _close(fd);
  return rv;
#endif
}

int ReadSecretFD(const char *fd_str, gxString &secret, gxString &err)
{
  err.Clear();
  if(!fd_str || !*fd_str || strlen(fd_str) > 6) {
    err << clear << "Invalid file descriptor " << (fd_str ? fd_str : "");
    return 1;
  }
  for(const char *p = fd_str; *p; p++) {
    if(*p < '0' || *p > '9') {
      err << clear << "Invalid file descriptor " << fd_str;
      return 1;
    }
  }
  gxString name;
  name << clear << "file descriptor " << fd_str;
  return ReadSecretLine(atoi(fd_str), secret, err, name.c_str());
}
