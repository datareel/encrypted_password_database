// ------------------------------- //
// -------- start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
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

Encrypted password database command line functions
*/
// ----------------------------------------------------------- // 
#ifndef __EPDB_FUNCTIONS_HPP__
#define __EPDB_FUNCTIONS_HPP__

#include "db_auth.h"
#include "c_thread.h"
#include "gxdbase.h"
#include "gxdstats.h"
#include "memblock.h"
#include "gxlist.h"

// Database functions
POD *OpenEPDB_POD(gxString &fname, gxString &err_string, gxDatabaseAccessMode mode = gxDBASE_READONLY);
POD *OpenEPDB_POD(gxString &fname, gxString &data_ext, gxString &index_ext, char database_revision, gxString &err_string, gxDatabaseAccessMode mode = gxDBASE_READONLY);
int BuildVirtualEPDB(POD *pod);
extern int epdb_btree_errors;
const int EPDB_OUTPUT_DEFAULT = 0; // Delimited fields, empty fields are a space
const int EPDB_OUTPUT_CSV = 1;     // RFC 4180 CSV
const int EPDB_OUTPUT_FIELD = 2;   // One field per record, as stored
extern int epdb_output_mode;
extern unsigned epdb_output_field;
extern int epdb_output_null;
int ReadSecretFile(const char *fname, gxString &secret, gxString &err);
int ReadSecretFD(const char *fd_str, gxString &secret, gxString &err);
int ExportEPDB(POD *pod, char delimiter = 0, char filter_char = 0, char replace_char = 0, char replace_char_with = 0, int *read_errors = 0);
int ExportListObjectsEPDB(POD *pod, char delimiter = 0, char filter_char = 0, char replace_char = 0, char replace_char_with = 0, int *read_errors = 0);
unsigned SearchEPDB(gxBtree *btx, int item, POD *pod, INFOHOG_t &ob, int find_all = 0);
void KeySearchEPDB(INFOHOGKEY &key, int item, POD *pod, INFOHOG_t &ob, unsigned &objects_found, int find_all = 0);
gxDatabase *OpenEPDB(const char *fname, gxString &err_string, gxDatabaseAccessMode mode = gxDBASE_READONLY);
int UnlockEPDB(gxDatabase *f, MemoryBuffer &key, gxString &err_string);
int UnlockEPDB(gxDatabase *f, MemoryBuffer &key, gxDatabaseConfig &db_config, gxString &err_string);
void StoreObjectAddress(BtreeNode *n);
typedef void (*BtreeVisitFunc)(BtreeNode *Node);
void gxBtreeWalk(FAU_t t, BtreeVisitFunc Visit, gxBtree *tree);

// 10/27/2026: Add, update and delete records from the command line.
// Field values come from name=value input (fckeymgmt.h), never argv.
const int EPDB_RECORD_ADD = 1;
const int EPDB_RECORD_UPDATE = 2;
const int EPDB_RECORD_DELETE = 3;
// Field number (0 to NumDataMembers-1) for a field name: title,
// username, password, previous-password, security-questions,
// account-information, website-link, notes, field1 to field8, or the
// database's own column name. Returns -1 if not found.
int EPDBRecordFieldIndex(const gxString &name, gxDatabaseConfig *cfg);
// Applies a record change to an unlocked database (key is the v2
// keyset or v1 secret). A v2 database is re-sealed. new_name is set to
// the record name after the change. Returns 0 if successful.
int EPDBRecordChange(const char *fname, const MemoryBuffer &key, int op, const gxString &record_name,
		     gxList<FCNameValue> &fields, gxString &new_name, gxString &err);
extern const char *epdb_record_field_names[NumDataMembers];

// Console based display functions
int PrintDBConfig(const char *fname, MemoryBuffer &key);
int PrintDBConfig(gxDatabaseConfig &db_config);
int ListUsers(const char *fname);
int PrintDBStats(const char *fname);

#endif // __EPDB_FUNCTIONS_HPP__ 
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
