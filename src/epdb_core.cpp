// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 10/27/2026
// Date Last Modified: 10/27/2026
// Copyright (c) 2001-2026 DataReel Software Development
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

Encrypted Password Database core functions, see epdb_core.h
*/
// ----------------------------------------------------------- //
#include "gxdlcode.h"

#include "m_globals.h"
#include "epdb_core.h"
#include "fcfile.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "../fcrypt/3plibs/cjson/cJSON.h"

const char *epdb_field_names[NumDataMembers] = {
  "title", "username", "password", "previous-password", "security-questions",
  "account-information", "website-link", "notes"
};

// Uses key for DBString and index compares while in scope, as the GUI
// builds the index
struct EPDBKeyScope {
  EPDBKeyScope(const MemoryBuffer &key) {
    saved_key = DBStringConfig::crypt_key;
    saved_case = DBStringConfig::DBStringCaseCompare;
    DBStringConfig::crypt_key = key;
    DBStringConfig::DBStringCaseCompare = 0;
  }
  ~EPDBKeyScope() {
    DBStringConfig::crypt_key = saved_key;
    DBStringConfig::DBStringCaseCompare = saved_case;
    saved_key.Clear(1);
  }
  MemoryBuffer saved_key;
  int saved_case;
};

void EPDBRecord::Copy(const EPDBRecord &ob)
{
  for(int i = 0; i < NumDataMembers; i++) {
    value[i] = ob.value[i];
    given[i] = ob.given[i];
  }
}

void EPDBRecord::Wipe()
{
  for(int i = 0; i < NumDataMembers; i++) {
    SecureWipe(value[i]);
    given[i] = 0;
  }
}

void EPDBWipeRecords(gxList<EPDBRecord> &recs)
{
  for(gxListNode<EPDBRecord> *p = recs.GetHead(); p; p = p->next) p->data.Wipe();
  recs.ClearList();
}

static gxString EPDBFieldKey(const char *s)
// Lower case, spaces and underscores as dashes
{
  gxString k;
  for(const char *p = s; p && *p; p++) {
    char c = *p;
    if(c == ' ' || c == '_') c = '-';
    k << (char)tolower((unsigned char)c);
  }
  k.TrimLeading('-');
  k.TrimTrailing('-');
  return k;
}

int EPDBFieldIndex(const gxString &name, gxDatabaseConfig *cfg)
{
  gxString k = EPDBFieldKey(name.c_str());
  for(int i = 0; i < NumDataMembers; i++) {
    if(k == epdb_field_names[i]) return i;
    gxString f;
    f << "field" << (i+1);
    if(k == f) return i;
  }
  if(cfg) {
    for(int i = 0; i < NumDataMembers; i++) {
      char sbuf[DBStringLength];
      memset(sbuf, 0, sizeof(sbuf));
      char *col = cfg->GetColName(i, sbuf);
      if(col && *col && EPDBFieldKey(col) == k) return i;
    }
  }
  return -1;
}

static int EPDBMemberString(INFOHOG &ih, unsigned i, const MemoryBuffer &key, gxString &out)
{
  out.Clear();
  if(i >= ih.GetNumMembers() || ih.GetMemberLen(i) != sizeof(DBString)) return 1;
  DBString *ds = (DBString *)ih.GetMember(i);
  int e = 0;
  MemoryBuffer k(key);
  char *p = ds->GetString(k, e);
  k.Clear(1);
  if(!p || e) return 0;
  if(p != DBStringNULLPtr::DBStringNUllStr) {
    out = p;
    OPENSSL_cleanse(p, strlen(p));
    delete[] p;
  }
  return 1;
}

static int EPDBLoadRecord(INFOHOG &ih, const MemoryBuffer &key, EPDBRecord &rec, gxString &err)
{
  rec.Wipe();
  for(int i = 0; i < NumDataMembers; i++) {
    if(!EPDBMemberString(ih, i, key, rec.value[i])) {
      err << clear << "Cannot decrypt a record field";
      rec.Wipe();
      return EPDB_REC_ERROR;
    }
    rec.given[i] = 1;
  }
  return EPDB_REC_OK;
}

int EPDBReadRecord(POD *pod, const MemoryBuffer &key, const gxString &name, EPDBRecord &rec, gxString &err)
{
  EPDBKeyScope scope(key);
  INFOHOG ih(pod, NumDataMembers);
  INFOHOG_t k(name.c_str());
  ih.SetMember(&k, sizeof(k), 0);
  if(!ih.FindObject() || !ih.ReadObject()) {
    err << clear << "Record " << name << " is not in the database";
    return EPDB_REC_NOT_FOUND;
  }
  return EPDBLoadRecord(ih, key, rec, err);
}

int EPDBReadAllRecords(POD *pod, const MemoryBuffer &key, gxList<EPDBRecord> &recs, gxString &err)
{
  EPDBKeyScope scope(key);
  gxList<FAU_t> addrs;
  if(EPDBIndexAddresses(pod, addrs, err) != 0) return EPDB_REC_ERROR;
  // Sorted by title, without regard to case, like the GUI grid
  gxList<EPDBRecord> tmp;
  for(gxListNode<FAU_t> *a = addrs.GetHead(); a; a = a->next) {
    INFOHOG ih(pod);
    if(!ih.ReadObject(a->data)) {
      err << clear << "Error reading a database record";
      EPDBWipeRecords(tmp);
      return EPDB_REC_ERROR;
    }
    EPDBRecord r;
    if(EPDBLoadRecord(ih, key, r, err) != EPDB_REC_OK) {
      EPDBWipeRecords(tmp);
      return EPDB_REC_ERROR;
    }
    gxListNode<EPDBRecord> *pos = tmp.GetHead();
    while(pos && CaseICmp(pos->data.value[0], r.value[0]) <= 0) pos = pos->next;
    if(pos) tmp.AddBefore(pos, r);
    else tmp.Add(r);
  }
  for(gxListNode<EPDBRecord> *p = tmp.GetHead(); p; p = p->next) recs.Add(p->data);
  EPDBWipeRecords(tmp);
  return EPDB_REC_OK;
}

static int EPDBSetField(INFOHOG &ih, unsigned i, const MemoryBuffer &key, const gxString &value, gxString &err)
{
  DBString s;
  int e = 0;
  MemoryBuffer k(key);
  gxString &v = (gxString &)value;
  int ok = s.SetString(k, e, v.c_str(), v.length());
  k.Clear(1);
  if(!ok || e) {
    err << clear << "The value of " << epdb_field_names[i] << " is too long or cannot be encrypted (fields hold up to "
	<< (DBStringLength - cytpro_overhead) << " characters after compression)";
    return EPDB_REC_ERROR;
  }
  ih.SetMember(&s, sizeof(s), i);
  return EPDB_REC_OK;
}

int EPDBApplyRecordOp(POD *pod, const MemoryBuffer &key, const EPDBRecordOp &op_in, EPDBRecord *before, gxString &err)
{
  EPDBKeyScope scope(key);
  EPDBRecordOp &op = (EPDBRecordOp &)op_in;
  op.result = 0;
  gxString name = op.name;
  name.TrimLeading(' ');
  name.TrimTrailing(' ');
  if(name.is_null()) {
    err << clear << "The record name is empty";
    return EPDB_REC_ERROR;
  }
  gxString new_name = name;
  if(op.rec.given[0]) {
    new_name = op.rec.value[0];
    new_name.TrimLeading(' ');
    new_name.TrimTrailing(' ');
    if(new_name.is_null()) {
      err << clear << "The title (record name) cannot be empty";
      return EPDB_REC_ERROR;
    }
  }
  if((op.op == EPDB_OP_ADD || op.op == EPDB_OP_ADD_SKIP) && op.rec.given[0] && new_name != name) {
    err << clear << "The title " << new_name << " does not match the record name " << name;
    return EPDB_REC_ERROR;
  }

  INFOHOG old(pod, NumDataMembers);
  INFOHOG_t k(name.c_str());
  old.SetMember(&k, sizeof(k), 0);
  int exists = old.FindObject() && old.ReadObject();
  if(exists && before && EPDBLoadRecord(old, key, *before, err) != EPDB_REC_OK) return EPDB_REC_ERROR;

  if(op.op == EPDB_OP_ADD_SKIP && exists) {
    op.result = 4;
    return EPDB_REC_OK;
  }
  if((op.op == EPDB_OP_ADD || op.op == EPDB_OP_ADD_SKIP) && exists) {
    err << clear << "Record " << name << " already exists in the database";
    return EPDB_REC_EXISTS;
  }
  if((op.op == EPDB_OP_UPDATE || op.op == EPDB_OP_DELETE) && !exists) {
    err << clear << "Record " << name << " is not in the database";
    return EPDB_REC_NOT_FOUND;
  }
  if(op.op == EPDB_OP_DELETE) {
    if(!old.DeleteObject()) {
      err << clear << "Could not delete record " << name;
      return EPDB_REC_ERROR;
    }
    op.result = 3;
    return EPDB_REC_OK;
  }
  if(exists && CaseICmp(new_name, name) != 0) {
    INFOHOG chk(pod, NumDataMembers);
    INFOHOG_t nk(new_name.c_str());
    chk.SetMember(&nk, sizeof(nk), 0);
    if(chk.FindObject()) {
      err << clear << "Record " << new_name << " already exists in the database";
      return EPDB_REC_EXISTS;
    }
  }

  // The new record, as text: given values, and for an update the old
  // values of the fields not given. It is written like a new record
  // (empty fields unset, as the GUI Add panel does), after the old one is
  // deleted. InfoHog::ChangeObject() is not used: it skips members whose
  // bytes did not change without moving past them.
  EPDBRecord merged;
  if(exists && op.op != EPDB_OP_PUT && EPDBLoadRecord(old, key, merged, err) != EPDB_REC_OK) return EPDB_REC_ERROR;
  for(int i = 1; i < NumDataMembers; i++) {
    if(op.rec.given[i] || op.op == EPDB_OP_PUT || !exists) merged.value[i] = op.rec.value[i];
  }
  INFOHOG nw(pod, NumDataMembers);
  INFOHOG_t nk(new_name.c_str());
  nw.SetMember(&nk, sizeof(nk), 0);
  for(int i = 1; i < NumDataMembers; i++) {
    if(merged.value[i].is_null()) continue;
    if(EPDBSetField(nw, i, key, merged.value[i], err) != EPDB_REC_OK) return EPDB_REC_ERROR;
  }
  merged.Wipe();
  if(exists && !old.DeleteObject()) {
    err << clear << "Could not update record " << name;
    return EPDB_REC_ERROR;
  }
  if(!nw.WriteObject()) {
    err << clear << "Could not write record " << new_name;
    return EPDB_REC_ERROR;
  }
  op.result = exists ? 2 : 1;
  return EPDB_REC_OK;
}

int EPDBRecordBatch(const char *data_file, const MemoryBuffer &key, gxList<EPDBRecordOp> &ops, int dry_run,
		    unsigned &num_done, gxString &err)
{
  num_done = 0;
  gxString dfile(data_file);
  if(dfile.length() < 5 || CaseICmp(gxString(dfile.c_str() + dfile.length() - 4), ".ehd") != 0) {
    err << clear << "The database file name must end in .ehd";
    return EPDB_REC_ERROR;
  }
  gxString ifile = ReplaceFileExtension(dfile, ".ehx");
  gxString tdfile = ReplaceFileExtension(dfile, ".change.ehd");
  gxString tifile = ReplaceFileExtension(dfile, ".change.ehx");
  if(futils_exists(tdfile.c_str()) || futils_exists(tifile.c_str())) {
    err << clear << tdfile.c_str() << " exists from an earlier change that did not finish, check it and remove it";
    return EPDB_REC_ERROR;
  }
  if(EPDBCopyDatabase(dfile, ifile, tdfile, tifile, err) != 0) return EPDB_REC_ERROR;
  int rv = EPDB_REC_OK;
  POD *pod = EPDBOpenPOD(tdfile, tifile, gxDBASE_READWRITE, err);
  if(!pod) rv = EPDB_REC_ERROR;
  for(gxListNode<EPDBRecordOp> *p = ops.GetHead(); rv == EPDB_REC_OK && p; p = p->next) {
    gxString e;
    rv = EPDBApplyRecordOp(pod, key, p->data, 0, e);
    if(rv != EPDB_REC_OK) err << clear << "Change " << (num_done + 1) << " (" << p->data.name << "): " << e;
    else num_done++;
  }
  if(rv == EPDB_REC_OK) {
    pod->Flush();
    if(EPDBIsKeyset(key)) {
      if(EPDBSeal(pod->OpenDataFile(), key, err) != 0 || EPDBVerify(pod, key, err) != 0) rv = EPDB_REC_ERROR;
    }
  }
  if(pod) delete pod;
  if(rv == EPDB_REC_OK && !dry_run) {
    gxString backup;
    if(EPDBReplaceDatabase(dfile, ifile, tdfile, tifile, 0, "change", backup, err) != 0) rv = EPDB_REC_ERROR;
  }
  if(rv != EPDB_REC_OK || dry_run) {
    gxString keep = err;
    futils_remove(tdfile.c_str());
    futils_remove(tifile.c_str());
    err = keep;
  }
  if(rv != EPDB_REC_OK) num_done = 0;
  return rv;
}

// ----------------------------------------------------------- //
// JSON and name=value input
// ----------------------------------------------------------- //
gxString EPDBRecordJSON(const EPDBRecord &rec)
{
  gxString s;
  s << "{";
  for(int i = 0; i < NumDataMembers; i++) {
    if(i) s << ", ";
    s << "\"" << epdb_field_names[i] << "\": " << FC_json_string(rec.value[i]);
  }
  s << "}";
  return s;
}

static int EPDBJSONObjectToRecord(cJSON *obj, unsigned n, EPDBRecord &rec, gxString &err)
{
  if(!cJSON_IsObject(obj)) {
    err << clear << "Record " << n << " of the JSON input is not an object";
    return -1;
  }
  for(cJSON *c = obj->child; c; c = c->next) {
    int i = -1;
    for(int j = 0; j < NumDataMembers; j++) if(c->string && strcmp(c->string, epdb_field_names[j]) == 0) i = j;
    if(i < 0) {
      err << clear << "Record " << n << " of the JSON input has an unknown field \"" << (c->string ? c->string : "") << "\"";
      return -1;
    }
    if(!cJSON_IsString(c)) {
      err << clear << "Record " << n << " field \"" << c->string << "\" is not a string";
      return -1;
    }
    rec.value[i] = c->valuestring;
    rec.given[i] = 1;
  }
  return 0;
}

int EPDBParseJSONRecords(const MemoryBuffer &data, gxList<EPDBRecord> &recs, gxString &err)
{
  cJSON *root = FC_json_parse(data, err);
  if(!root) return -1;
  int rv = 0;
  if(cJSON_IsArray(root)) {
    unsigned n = 0;
    for(cJSON *c = root->child; rv == 0 && c; c = c->next) {
      EPDBRecord r;
      rv = EPDBJSONObjectToRecord(c, ++n, r, err);
      if(rv == 0) recs.Add(r);
    }
  }
  else {
    EPDBRecord r;
    rv = EPDBJSONObjectToRecord(root, 1, r, err);
    if(rv == 0) recs.Add(r);
  }
  FC_json_free(root);
  if(rv != 0) EPDBWipeRecords(recs);
  return rv;
}

int EPDBNameValuesToRecord(gxList<FCNameValue> &list, gxDatabaseConfig *cfg, EPDBRecord &rec, gxString &err)
{
  for(gxListNode<FCNameValue> *p = list.GetHead(); p; p = p->next) {
    int i = EPDBFieldIndex(p->data.name, cfg);
    if(i < 0) {
      err << clear << "Line " << p->data.line << ": unknown field " << p->data.name;
      return -1;
    }
    if(rec.given[i]) {
      err << clear << "Line " << p->data.line << ": field " << epdb_field_names[i] << " is given twice";
      return -1;
    }
    rec.value[i] = p->data.value;
    rec.given[i] = 1;
  }
  return 0;
}

// ----------------------------------------------------------- //
// CSV
// ----------------------------------------------------------- //
gxString EPDBCSVHeader(gxDatabaseConfig &cfg, const MemoryBuffer &key)
{
  EPDBKeyScope scope(key);
  gxString s;
  for(int i = 0; i < NumDataMembers; i++) {
    char dest[DBStringLength];
    memset(dest, 0, sizeof(dest));
    gxString label = cfg.GetColName(i, dest);
    AppendCSVField(s, label, ',', '\t');
    s << ",";
  }
  s << "\n\n";
  return s;
}

gxString EPDBCSVRow(const EPDBRecord &rec)
// Same as the GUI export: an empty field is a space
{
  gxString s;
  for(int i = 0; i < NumDataMembers; i++) {
    if(rec.value[i].is_null()) s << " ";
    else AppendCSVField(s, rec.value[i], ',', '\t');
    if(i < NumDataMembers - 1) s << ",";
  }
  s << "\n";
  return s;
}

int EPDBParseCSV(const MemoryBuffer &data_in, gxList<EPDBRecord> &recs, gxString &err)
{
  MemoryBuffer &data = (MemoryBuffer &)data_in;
  const char *p = (const char *)data.m_buf();
  unsigned len = data.length(), pos = 0, line = 1, row = 0;
  while(pos < len) {
    // One row
    gxString fields[NumDataMembers + 1];
    unsigned nf = 0;
    unsigned start_line = line;
    int empty_row = 1;
    for(;;) {
      gxString f;
      int quoted = 0;
      if(pos < len && p[pos] == '"') {
	quoted = 1;
	pos++;
	for(;;) {
	  if(pos >= len) {
	    err << clear << "CSV line " << start_line << ": a quoted field is not closed";
	    EPDBWipeRecords(recs);
	    return -1;
	  }
	  if(p[pos] == '"') {
	    if(pos + 1 < len && p[pos+1] == '"') { f << '"'; pos += 2; continue; }
	    pos++;
	    break;
	  }
	  if(p[pos] == '\n') line++;
	  f << p[pos++];
	}
      }
      while(pos < len && p[pos] != ',' && p[pos] != '\n' && p[pos] != '\r') f << p[pos++];
      if(!f.is_null() || quoted) empty_row = 0;
      if(nf <= (unsigned)NumDataMembers) {
	if(f == " " && !quoted) f.Clear(); // The GUI writes a space for an empty field
	fields[nf] = f;
      }
      nf++;
      SecureWipe(f);
      if(pos < len && p[pos] == ',') { pos++; continue; }
      if(pos < len && p[pos] == '\r') pos++;
      if(pos < len && p[pos] == '\n') { pos++; line++; }
      break;
    }
    if(empty_row) continue;
    row++;
    // The GUI header line ends with a comma: 9 fields, the last empty
    if(row == 1) {
      int header = 1;
      for(int i = 0; i < NumDataMembers && header; i++) {
	if(EPDBFieldIndex(fields[i], 0) != i && fields[i] != "Title" && CaseICmp(fields[0], "Title") != 0) header = 0;
      }
      if(header || CaseICmp(fields[0], "title") == 0) {
	for(unsigned i = 0; i <= (unsigned)NumDataMembers; i++) SecureWipe(fields[i]);
	continue;
      }
    }
    if(nf > (unsigned)NumDataMembers && !(nf == (unsigned)NumDataMembers + 1 && fields[NumDataMembers].is_null())) {
      err << clear << "CSV line " << start_line << " has " << nf << " fields, records have " << NumDataMembers;
      EPDBWipeRecords(recs);
      return -1;
    }
    EPDBRecord r;
    for(int i = 0; i < NumDataMembers; i++) {
      r.value[i] = fields[i];
      r.given[i] = 1;
    }
    for(unsigned i = 0; i <= (unsigned)NumDataMembers; i++) SecureWipe(fields[i]);
    if(r.value[0].is_null()) {
      err << clear << "CSV line " << start_line << " has no title";
      EPDBWipeRecords(recs);
      return -1;
    }
    recs.Add(r);
  }
  return 0;
}

// ----------------------------------------------------------- //
// Info
// ----------------------------------------------------------- //
int EPDBGetInfo(const char *data_file, POD *pod, EPDBInfo &info, gxString &err)
{
  info.users.ClearList();
  gxDatabase f;
  if(f.Open(data_file, gxDBASE_READONLY) != gxDBASE_NO_ERROR) {
    err << clear << "Cannot open " << data_file;
    return -1;
  }
  info.format = EPDBIsV2(&f) ? 2 : 1;
  if(info.format == 2) {
    info.id = EPDBIDHex(&f);
    EPDBHeaderExt x;
    if(EPDBReadExt(&f, x)) {
      info.flags = x.flags;
      info.key_generation = x.key_generation;
      info.rekey_time = x.rekey_time;
    }
    FCControlBlock c;
    if(EPDBReadControl(&f, c)) {
      info.sd_on = c.SelfDestruct();
      info.sd_limit = c.limit;
      info.sd_failed = c.failed;
      info.destroyed = c.destroyed;
    }
  }
  if(!info.destroyed) {
    DatabaseUserAuth db_auth;
    db_auth.f = &f;
    if(db_auth.LoadStaticDataBlocks() == 0) {
      for(gxListNode<StaticDataBlock> *p = db_auth.static_block_list.GetHead(); p; p = p->next) {
	gxString u = p->data.username;
	if(p->data.block_header.block_type == 1) { info.rsa_users++; u << " (RSA key)"; }
	else if(p->data.block_header.block_type == 2) { info.smartcard_users++; u << " (Smart Card)"; }
	else continue;
	info.users.Add(u);
      }
    }
  }
  f.Close();
  if(pod) {
    gxList<FAU_t> addrs;
    if(EPDBIndexAddresses(pod, addrs, err) == 0) {
      info.records = 0;
      for(gxListNode<FAU_t> *a = addrs.GetHead(); a; a = a->next) info.records++;
    }
  }
  return 0;
}

gxString EPDBInfoJSON(const char *data_file, const EPDBInfo &info_in)
{
  EPDBInfo &info = (EPDBInfo &)info_in;
  gxString s;
  s << "{\"file\": " << FC_json_string(gxString(data_file)) << ", \"format\": " << info.format;
  if(info.records >= 0) s << ", \"records\": " << info.records;
  s << ", \"id\": " << FC_json_string(info.id);
  s << ", \"key_generation\": " << info.key_generation << ", \"last_rekey_time\": " << (unsigned long)info.rekey_time;
  s << ", \"in_file_log\": " << ((info.flags & EPDB_EXT_LOG_IN_FILE) ? "true" : "false");
  s << ", \"self_destruct\": " << (info.sd_on ? "true" : "false") << ", \"self_destruct_attempts\": " << info.sd_limit
    << ", \"failed_opens\": " << info.sd_failed << ", \"destroyed\": " << (info.destroyed ? "true" : "false");
  s << ", \"users\": [";
  int n = 0;
  for(gxListNode<gxString> *p = info.users.GetHead(); p; p = p->next) {
    if(n++) s << ", ";
    s << FC_json_string(p->data);
  }
  s << "]}";
  return s;
}

// ----------------------------------------------------------- //
// Merge
// ----------------------------------------------------------- //
int EPDBMergeDatabases(const char *dst_file, const MemoryBuffer &dst_key, gxList<EPDBRecord> &src_records,
		       int policy, int dry_run, unsigned &added, unsigned &replaced, unsigned &skipped, gxString &err)
{
  added = replaced = skipped = 0;
  gxList<EPDBRecordOp> ops;
  for(gxListNode<EPDBRecord> *p = src_records.GetHead(); p; p = p->next) {
    EPDBRecordOp op;
    op.op = (policy == EPDB_MERGE_REPLACE) ? EPDB_OP_PUT : ((policy == EPDB_MERGE_SKIP) ? EPDB_OP_ADD_SKIP : EPDB_OP_ADD);
    op.name = p->data.value[0];
    op.rec = p->data;
    ops.Add(op);
  }
  unsigned done = 0;
  int rv = EPDBRecordBatch(dst_file, dst_key, ops, dry_run, done, err);
  if(rv == EPDB_REC_OK) {
    for(gxListNode<EPDBRecordOp> *p = ops.GetHead(); p; p = p->next) {
      if(p->data.result == 1) added++;
      else if(p->data.result == 2) replaced++;
      else if(p->data.result == 4) skipped++;
    }
  }
  for(gxListNode<EPDBRecordOp> *p = ops.GetHead(); p; p = p->next) p->data.rec.Wipe();
  return rv;
}

// ----------------------------------------------------------- //
// Password generator
// ----------------------------------------------------------- //
int EPDBGeneratePassword(unsigned length, int symbols, gxString &password)
{
  static const char *classes[4] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz", "0123456789", "!#$%&()*+,-./:;<=>?@[]^_{|}~"
  };
  int nclasses = symbols ? 4 : 3;
  if(length < 8 || length > 256) return -1;
  gxString all;
  for(int c = 0; c < nclasses; c++) all << classes[c];
  for(int tries = 0; tries < 100; tries++) {
    password.Clear(1);
    int seen[4] = { 0, 0, 0, 0 };
    for(unsigned i = 0; i < length; i++) {
      unsigned char b;
      unsigned limit = 256 - (256 % all.length()); // No modulo bias
      do {
	if(!FC_rand_bytes(&b, 1, 1)) return -1;
      } while(b >= limit);
      char ch = all[b % all.length()];
      password << ch;
      for(int c = 0; c < nclasses; c++) if(strchr(classes[c], ch)) seen[c] = 1;
    }
    int ok = 1;
    for(int c = 0; c < nclasses; c++) if(!seen[c]) ok = 0;
    if(ok) return 0;
  }
  return -1;
}

// ----------------------------------------------------------- //
// Undo and redo
// ----------------------------------------------------------- //
EPDBUndoStep *EPDBUndoStack::NewStep(const char *label)
{
  EPDBUndoStep *s = new EPDBUndoStep;
  s->label = label;
  return s;
}

void EPDBUndoStack::FreeSteps(gxList<EPDBUndoStep *> &l)
{
  for(gxListNode<EPDBUndoStep *> *p = l.GetHead(); p; p = p->next) {
    EPDBUndoStep *s = p->data;
    for(gxListNode<EPDBRecordOp> *o = s->undo_ops.GetHead(); o; o = o->next) o->data.rec.Wipe();
    for(gxListNode<EPDBRecordOp> *o = s->redo_ops.GetHead(); o; o = o->next) o->data.rec.Wipe();
    SecureWipe(s->label);
    delete s;
  }
  l.ClearList();
}

void EPDBUndoStack::Clear()
{
  FreeSteps(undo);
  FreeSteps(redo);
  if(group) {
    gxList<EPDBUndoStep *> g;
    g.Add(group);
    FreeSteps(g);
    group = 0;
  }
}

unsigned EPDBUndoStack::Depth() const
{
  unsigned n = 0;
  for(gxListNode<EPDBUndoStep *> *p = ((gxList<EPDBUndoStep *> &)undo).GetHead(); p; p = p->next) n++;
  return n;
}

void EPDBUndoStack::Push(EPDBUndoStep *step)
{
  FreeSteps(redo); // A new change ends the redo history
  undo.AddToFront(step);
  // Cap the depth, dropping the oldest
  unsigned n = 0;
  gxListNode<EPDBUndoStep *> *p = undo.GetHead();
  while(p) {
    gxListNode<EPDBUndoStep *> *next = p->next;
    if(++n > depth) {
      gxList<EPDBUndoStep *> old;
      old.Add(p->data);
      undo.Remove(p);
      FreeSteps(old);
    }
    p = next;
  }
}

void EPDBUndoStack::Add(EPDBUndoStep *s)
{
  if(group) {
    // The group's undo ops run newest change first, each change's own
    // ops in their order. Redo ops run oldest first.
    for(gxListNode<EPDBRecordOp> *o = s->undo_ops.GetTail(); o; o = o->prev) group->undo_ops.AddToFront(o->data);
    for(gxListNode<EPDBRecordOp> *o = s->redo_ops.GetHead(); o; o = o->next) group->redo_ops.Add(o->data);
    gxList<EPDBUndoStep *> l;
    l.Add(s);
    FreeSteps(l);
    return;
  }
  Push(s);
}

static EPDBRecordOp EPDBMakeOp(int op, const gxString &name, const EPDBRecord *rec)
{
  EPDBRecordOp o;
  o.op = op;
  o.name = name;
  if(rec) {
    o.rec = *rec;
    for(int i = 0; i < NumDataMembers; i++) o.rec.given[i] = 1;
  }
  return o;
}

void EPDBUndoStack::RecordAdd(const EPDBRecord &after)
{
  gxString label;
  label << "Add " << after.value[0];
  EPDBUndoStep *s = NewStep(label.c_str());
  EPDBRecordOp u = EPDBMakeOp(EPDB_OP_DELETE, after.value[0], 0);
  EPDBRecordOp r = EPDBMakeOp(EPDB_OP_PUT, after.value[0], &after);
  s->undo_ops.Add(u);
  s->redo_ops.Add(r);
  Add(s);
}

void EPDBUndoStack::RecordUpdate(const EPDBRecord &before, const EPDBRecord &after)
{
  gxString label;
  label << "Change " << before.value[0];
  EPDBUndoStep *s = NewStep(label.c_str());
  int renamed = CaseICmp(before.value[0], after.value[0]) != 0;
  if(renamed) {
    EPDBRecordOp u1 = EPDBMakeOp(EPDB_OP_DELETE, after.value[0], 0);
    EPDBRecordOp r1 = EPDBMakeOp(EPDB_OP_DELETE, before.value[0], 0);
    s->undo_ops.Add(u1);
    s->redo_ops.Add(r1);
  }
  EPDBRecordOp u2 = EPDBMakeOp(EPDB_OP_PUT, before.value[0], &before);
  EPDBRecordOp r2 = EPDBMakeOp(EPDB_OP_PUT, after.value[0], &after);
  s->undo_ops.Add(u2);
  s->redo_ops.Add(r2);
  Add(s);
}

void EPDBUndoStack::RecordDelete(const EPDBRecord &before)
{
  gxString label;
  label << "Delete " << before.value[0];
  EPDBUndoStep *s = NewStep(label.c_str());
  EPDBRecordOp u = EPDBMakeOp(EPDB_OP_PUT, before.value[0], &before);
  EPDBRecordOp r = EPDBMakeOp(EPDB_OP_DELETE, before.value[0], 0);
  s->undo_ops.Add(u);
  s->redo_ops.Add(r);
  Add(s);
}

void EPDBUndoStack::BeginGroup(const char *label)
{
  if(group) EndGroup();
  group = NewStep(label);
}

void EPDBUndoStack::EndGroup()
{
  if(!group) return;
  EPDBUndoStep *g = group;
  group = 0;
  if(!g->undo_ops.GetHead()) {
    gxList<EPDBUndoStep *> l;
    l.Add(g);
    FreeSteps(l);
    return;
  }
  Push(g);
}

gxString EPDBUndoStack::UndoLabel()
{
  return undo.GetHead() ? undo.GetHead()->data->label : gxString();
}

gxString EPDBUndoStack::RedoLabel()
{
  return redo.GetHead() ? redo.GetHead()->data->label : gxString();
}

int EPDBUndoStack::Apply(POD *pod, const MemoryBuffer &key, gxList<EPDBRecordOp> &ops, gxString &err)
{
  for(gxListNode<EPDBRecordOp> *o = ops.GetHead(); o; o = o->next) {
    int rv = EPDBApplyRecordOp(pod, key, o->data, 0, err);
    if(rv != EPDB_REC_OK) return rv;
  }
  return EPDB_REC_OK;
}

int EPDBUndoStack::Undo(POD *pod, const MemoryBuffer &key, gxString &err)
{
  if(group) EndGroup();
  gxListNode<EPDBUndoStep *> *p = undo.GetHead();
  if(!p) {
    err << clear << "Nothing to undo";
    return EPDB_REC_ERROR;
  }
  EPDBUndoStep *s = p->data;
  int rv = Apply(pod, key, s->undo_ops, err);
  if(rv != EPDB_REC_OK) return rv;
  undo.Remove(p);
  redo.AddToFront(s);
  return EPDB_REC_OK;
}

int EPDBUndoStack::Redo(POD *pod, const MemoryBuffer &key, gxString &err)
{
  gxListNode<EPDBUndoStep *> *p = redo.GetHead();
  if(!p) {
    err << clear << "Nothing to redo";
    return EPDB_REC_ERROR;
  }
  EPDBUndoStep *s = p->data;
  int rv = Apply(pod, key, s->redo_ops, err);
  if(rv != EPDB_REC_OK) return rv;
  redo.Remove(p);
  undo.AddToFront(s);
  return EPDB_REC_OK;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
