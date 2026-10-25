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

#endif // __DB_AUTH_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
