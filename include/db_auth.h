// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
// Date Last Modified: 07/23/2025
// Copyright (c) 2001-2025 DataReel Software Development
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

Encrypted database auth functions
*/
// ----------------------------------------------------------- //   
#ifndef __DB_AUTH_HPP__
#define __DB_AUTH_HPP__

#include "m_globals.h"
#include "fckeymgmt.h"

class DatabaseUserAuth
{
public:
  DatabaseUserAuth();
    ~DatabaseUserAuth();

public:
  int LoadStaticDataBlocks();  
  int CountUserBlocks(const gxString &username, gxUINT32 block_type);
  int UnlockFromBlock(gxUINT32 block_status, const MemoryBuffer &block_secret,
		      MemoryBuffer &keyset);
  int UpdateStaticData();
  int WriteStaticDataArea();
  int AddRSAKeyToStaticArea(const MemoryBuffer &secret,
			    char public_key[], unsigned public_key_len,
			    const gxString &rsa_key_username, char *passphrase = 0,
			    int use_oaep = 0);
  int DecryptWithRSAKey(char private_key[], unsigned private_key_len,
			const gxString &rsa_key_username, char *passphrase = 0);

#ifdef __ENABLE_SMART_CARD__
  int AddSmartCardCertToStaticArea(SmartCardOB *sc, int use_cert_file,
				   const MemoryBuffer &secret, const gxString &smartcard_cert_username,
				   int use_oaep = 0);
  int DecryptWithSmartcard(SmartCardOB *sc, const gxString &smartcard_cert_username);
#endif // __ENABLE_SMART_CARD__

public:
  gxList<StaticDataBlock> static_block_list;
  int ERROR_LEVEL;
  gxString err;
  gxDatabase *f;
  unsigned char *static_data;
  unsigned static_data_size;
  unsigned static_data_bytes_used;
  unsigned num_static_data_blocks;
};

// 10/25/2026: Static area block_status values. PKCS#1 v1.5 blocks can
// be read by every version, OAEP (SHA-256) blocks only by 10/25/2026
// and later versions.
const gxUINT32 DB_AUTH_BLOCK_STATUS_PKCS1 = 1;
const gxUINT32 DB_AUTH_BLOCK_STATUS_OAEP = 2;
const unsigned DB_AUTH_MAX_USERNAME_LEN = 255;

#ifdef __ENABLE_SMART_CARD__
// 10/26/2026: Reads a cert exported from a smart card, PEM or DER (such
// as a .cer file exported on Windows), into sc for
// SC_cert_file_public_key_encrypt(). Returns 0 if the file holds an
// X.509 cert with an RSA key.
int EPDBReadSmartCardCertFile(SmartCardOB *sc, const char *fname, gxString &err);
#endif // __ENABLE_SMART_CARD__

// 10/25/2026: Returns 0 if the key decrypts the database name
int VerifyDatabaseKey(gxDatabase *f, const MemoryBuffer &key, gxString &err, int &aes_error);

// --------------------------------------------------------------
// 10/25/2026: Database format v2
// --------------------------------------------------------------
// v1 encrypts each field with a key derived from the secret (password,
// key file or the secret in an RSA or smart card block) by
// EVP_BytesToKey with 1000 iterations, and has no MAC over the whole
// database. v2 derives a master key once per open with
// PBKDF2-HMAC-SHA256 (600000 iterations, 32-byte salt), derives
// separate field encryption, field MAC, key check and database MAC keys
// from it with KBKDF-HMAC-SHA256, and seals the database with a MAC
// over the v2 header, the config, the auth area and every record, bound
// to the record address and field number.
//
// A v2 header is stored in the config static area at
// EPDB_V2_HEADER_OFFSET, not encrypted:
//   0  magic "EPDBV2\x1A\x01"   8      80 key check value   32
//   8  format version (2026103)  4     112 database MAC       32
//  12  header length (256)       4     144 reserved          112
//  16  KDF ID (1 = PBKDF2-HMAC-SHA256) 4
//  20  KDF iterations            4
//  24  flags (0)                 4
//  28  reserved                  4
//  32  PBKDF2 salt               32
//  64  database ID               16
//
// v1 RSA and smart card blocks (block_status 1 or 2) wrap the secret.
// They are kept when a database is upgraded, with their header version
// changed to STATIC_DATA_BLOCK_VERSION_V2 so older versions do not use
// them, and open a v2 database through PBKDF2 like a password. Users
// added to a v2 database get OAEP blocks (block_status 3) that wrap the
// master key.
//
// In memory a v2 database key is a "keyset": a MemoryBuffer holding the
// magic "EPDBKEY2", the master key, the four derived keys and the
// database ID. It is used everywhere the v1 secret was used, such as
// DBStringConfig::crypt_key. DBString encrypts and decrypts v2 fields
// when its key is a keyset.
const unsigned EPDB_V2_HEADER_OFFSET = 49152;
const unsigned EPDB_V2_HEADER_LEN = 256;
const unsigned EPDB_V2_FORMAT_VERSION = 2026103;
const unsigned EPDB_V2_KDF_PBKDF2_SHA256 = 1;
const unsigned EPDB_V2_ITERATIONS = 600000;
const unsigned EPDB_V2_MIN_ITERATIONS = 100000;
const unsigned EPDB_V2_MAX_ITERATIONS = 10000000;
const unsigned EPDB_V2_SALT_LEN = 32;
const unsigned EPDB_V2_ID_LEN = 16;
const unsigned EPDB_V2_KEY_LEN = 32;
const unsigned EPDB_V2_MAC_LEN = 32;
const unsigned EPDB_V2_MAC_OFFSET = 112;
const unsigned EPDB_V2_KEYSET_LEN = 8 + (5 * EPDB_V2_KEY_LEN) + EPDB_V2_ID_LEN;
const gxUINT32 STATIC_DATA_BLOCK_VERSION_V2 = 2026103;
const gxUINT32 DB_AUTH_BLOCK_STATUS_OAEP_MASTER = 3;

enum { // Keys in a keyset
  EPDB_KEY_MASTER = 0,
  EPDB_KEY_FIELD_ENC,
  EPDB_KEY_FIELD_MAC,
  EPDB_KEY_CHECK,
  EPDB_KEY_DB_MAC
};

int EPDBIsKeyset(const MemoryBuffer &key);
const unsigned char *EPDBKeysetKey(const MemoryBuffer &keyset, int which);
const unsigned char *EPDBKeysetID(const MemoryBuffer &keyset);
int EPDBIsV2(gxDatabase *f);
int EPDBReadHeader(gxDatabase *f, unsigned char hdr[EPDB_V2_HEADER_LEN]);
int EPDBKeysetFromMaster(const unsigned char master[EPDB_V2_KEY_LEN],
			 const unsigned char db_id[EPDB_V2_ID_LEN], MemoryBuffer &keyset);
int EPDBUnlock(gxDatabase *f, const MemoryBuffer &secret, MemoryBuffer &keyset,
	       gxString &err, int &aes_error);
int EPDBUnlockMaster(gxDatabase *f, const unsigned char master[EPDB_V2_KEY_LEN],
		     MemoryBuffer &keyset, gxString &err);
int EPDBInitV2(gxDatabase *f, const MemoryBuffer &secret, MemoryBuffer &keyset, gxString &err);
int EPDBEncryptField(const MemoryBuffer &keyset, const unsigned char *in, unsigned in_len,
		     unsigned char *out, unsigned out_size, unsigned *out_len);
int EPDBDecryptField(const MemoryBuffer &keyset, const unsigned char *in, unsigned in_len,
		     unsigned char *out, unsigned out_size, unsigned *out_len);
int EPDBComputeMAC(gxDatabase *f, const MemoryBuffer &keyset,
		   unsigned char mac[EPDB_V2_MAC_LEN], gxString &err);
int EPDBSeal(gxDatabase *f, const MemoryBuffer &keyset, gxString &err);
int EPDBVerify(POD *pod, const MemoryBuffer &keyset, gxString &err);
int EPDBUpgrade(const char *data_file, const MemoryBuffer &secret,
		gxString &backup_data_file, gxString &err);

// --------------------------------------------------------------
// 10/27/2026: Key management, access log and self-destruct
// --------------------------------------------------------------
// Bytes these features use. None of them are read by 2026.102 and
// older versions, so their files stay readable by those versions.
//
// v2 header reserved bytes (EPDBInitV2() writes zeros, the database
// MAC covers them, older versions keep them when they re-seal):
//   144 "EPX1"                      4
//   148 flags (EPDB_EXT_*)          4
//   152 key generation              4  (1 after the first rekey)
//   156 last rekey time, UTC        8
//   164 self-destruct attempt limit 4
// Config static area, after the v2 header, random filled when the
// database is made and not covered by the database MAC, so they can be
// written without the key:
//   EPDB_CTL_OFFSET  (49408) self-destruct control block, 64 bytes (fckeymgmt.h)
//   EPDB_RING_OFFSET (49472) in-file access log ring, 160 entries, 15424 bytes
const unsigned EPDB_EXT_OFFSET = 144;
const unsigned EPDB_EXT_LOG_IN_FILE = 0x01;   // Opens and changes are logged in the file
const unsigned EPDB_EXT_SELF_DESTRUCT = 0x02; // Mirror of the control block flag
const unsigned EPDB_CTL_OFFSET = EPDB_V2_HEADER_OFFSET + EPDB_V2_HEADER_LEN;
const unsigned EPDB_RING_OFFSET = EPDB_CTL_OFFSET + 64;
const unsigned EPDB_RING_ENTRIES = 160;

struct EPDBHeaderExt {
  EPDBHeaderExt() { flags = key_generation = sd_limit = 0; rekey_time = 0; }
  unsigned flags;
  unsigned key_generation;
  unsigned long long rekey_time;
  unsigned sd_limit;
};

// Read and write the header extension. Write re-seals with keyset.
int EPDBReadExt(gxDatabase *f, EPDBHeaderExt &x);
int EPDBWriteExt(gxDatabase *f, const EPDBHeaderExt &x, const MemoryBuffer &keyset, gxString &err);
// Self-destruct control block. Returns 1 if the file has one.
int EPDBReadControl(gxDatabase *f, FCControlBlock &c);
int EPDBReadControl(const char *data_file, FCControlBlock &c);
// Writes the control block and flushes the file to disk
int EPDBWriteControl(gxDatabase *f, const FCControlBlock &c, gxString &err);
int EPDBWriteControl(const char *data_file, const FCControlBlock &c, gxString &err);
// In-file access log ring
int EPDBRingInit(gxDatabase *f, const MemoryBuffer &keyset, gxString &err);
int EPDBRingAppend(gxDatabase *f, const MemoryBuffer &keyset, const FCRingEntry &e,
		   const char *slot_username, gxString &err);
int EPDBRingRead(gxDatabase *f, const MemoryBuffer &keyset, gxList<FCRingEntry> &entries,
		 unsigned long long &written, gxString &err);
// Hex of the database ID, read from the v2 header without the key
gxString EPDBIDHex(gxDatabase *f);
gxString EPDBIDHex(const char *data_file);

#ifndef __ENABLE_SMART_CARD__
struct SmartCardOB;
#endif

// A user to keep in a rekey
struct EPDBRekeyUser {
  gxUINT32 block_type;   // 1 = RSA key, 2 = smart card
  gxString username;
  gxString source;       // Public key file, cert file or "card"
};

// Removes the RSA (block_type 1) or smart card (2) block of a user from
// a v2 database. The database is copied to temporary files, changed,
// re-sealed and checked, then the originals are kept as a backup
// (unless keep_backup is 0) and replaced. With dry_run everything is
// done and checked on the copy, which is then removed. Returns 0 if
// successful.
int EPDBRemoveUser(const char *data_file, const MemoryBuffer &keyset, gxUINT32 block_type,
		   const gxString &username, int keep_backup, gxString &backup_data_file, gxString &err,
		   int dry_run = 0);
// Rekey: a new salt, database ID and master key, every field encrypted
// again, and a new block for each user in keep, wrapped with their
// public key (sc is used for "card" users and never logs in). new_secret
// is the password or key file contents the master key is derived from;
// null makes a random secret nobody knows (only RSA and smart card users
// open the database then). The new database must open and hold the same
// records before it replaces the original. Returns 0 if successful and
// the new keyset in new_keyset.
int EPDBRekey(const char *data_file, const MemoryBuffer &old_keyset, const MemoryBuffer *new_secret,
	      gxList<EPDBRekeyUser> &keep, SmartCardOB *sc, int keep_backup,
	      MemoryBuffer &new_keyset, gxString &backup_data_file, gxString &err, int dry_run = 0);
// Opens the data and index files as a POD
POD *EPDBOpenPOD(const gxString &data_file, const gxString &index_file, gxDatabaseAccessMode mode, gxString &err);
// Copies a database to temporary files and renames checked temporary
// files over the originals, keeping a backup when asked. Return 0 if
// successful.
int EPDBCopyDatabase(const gxString &dfile, const gxString &ifile, const gxString &tdfile,
		     const gxString &tifile, gxString &err);
int EPDBReplaceDatabase(const gxString &dfile, const gxString &ifile, const gxString &tdfile,
			const gxString &tifile, int keep_backup, const char *tag,
			gxString &backup_data_file, gxString &err);
// Object addresses of every record listed in the index
int EPDBIndexAddresses(POD *pod, gxList<FAU_t> &addrs, gxString &err);

// Crypto-shreds a database after too many failed opens: every key
// block, the v2 header keys and salt, the config and every record in
// the data and index files are overwritten with random data and the
// control block is marked destroyed. Returns 0 if successful.
int EPDBDestroy(const char *data_file, gxString &err);

#endif // __DB_AUTH_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
