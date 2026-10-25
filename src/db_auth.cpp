// ------------------------------- //
// -------- start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, BCC32, GCC, HPUX aCC, SOLARIS CC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
// Date Last Modified: 01/08/2024
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

Encrypted database auth functions
*/
// ----------------------------------------------------------- // 
#include "gxdlcode.h"

#include "m_globals.h"
#include "db_auth.h"
#include "fcfile.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#if defined (__UNIX__)
#include <sys/stat.h>
#endif

int VerifyDatabaseKey(gxDatabase *f, const MemoryBuffer &key, gxString &err, int &aes_error)
// 10/25/2026: Check a key against the database by decrypting the stored
// database name, which is authenticated with an HMAC. A database name
// that was never set (first byte 0) is rejected: it used to make every
// key look valid. Returns 0 if the key is good.
{
  aes_error = AES_NO_ERROR;
  if(!f) {
    err << clear << "Error database pointer is null";
    return -1;
  }
  gxDatabaseConfig db_config;
  if(!db_config.ReadConfig(f)) {
    err << clear << "Error reading database config";
    return -1;
  }
  MemoryBuffer k(key);
  int error_level = 0;
  char *s = db_config.database_name.GetString(k, error_level);
  k.Clear(1);
  if(error_level != AES_NO_ERROR) {
    aes_error = error_level;
    err << clear << "Error decrypting database hash " << AES_err_string(error_level);
    return -1;
  }
  if(!s) {
    err << clear << "Database hash is a null value";
    return -1;
  }
  if(s == DBStringNULLPtr::DBStringNUllStr) {
    err << clear << "Database hash is not set, the database file is damaged";
    return -1;
  }
  OPENSSL_cleanse(s, strlen(s));
  delete[] s;
  return 0;
}

DatabaseUserAuth::DatabaseUserAuth()
{
    f = 0;
    ERROR_LEVEL = 0;
    static_data_size = DB_AUTH_STATIC_AREA_SIZE;
    static_data = 0;
    static_data_bytes_used = 0;
    num_static_data_blocks = 0;
}
  
DatabaseUserAuth::~DatabaseUserAuth()
{
  if(static_data) delete[] static_data;
}

int DatabaseUserAuth::LoadStaticDataBlocks()
{
  ERROR_LEVEL = 0;
  if(!f) {
    ERROR_LEVEL = -1;
    err << clear << "Error database pointer is null for DB auth";
    return ERROR_LEVEL;
  }

  if(!static_data) static_data = new unsigned char[static_data_size];

  FAU_t start_of_auth_data = f->FileHeaderSize() + DB_CONFIG_STATIC_AREA_SIZE;
  gxDatabaseError db_error;

  db_error = f->Read(static_data, static_data_size, start_of_auth_data);
  if(db_error != gxDBASE_NO_ERROR) {
    ERROR_LEVEL = db_error;
    err << clear << "Error reading static auth area in DB file " << gxDatabaseExceptionMessage(db_error);
    return ERROR_LEVEL;
  }

  // 10/25/2026: The static auth area is not authenticated, so every length
  // read from it is checked against the space left in the area and the
  // size of the buffer it is copied to. Older versions copied username_len
  // bytes into a 1024 byte stack buffer and read ciphertext_len bytes past
  // the end of the area. Same checks as FCryptCache::LoadStaticDataBlocks().
  StaticDataBlockHdr static_data_block_header;
  StaticDataBlock static_data_block;
  const unsigned hdr_len = sizeof(static_data_block_header);
  const unsigned max_ciphertext_len = 8192;
  unsigned block_offset = 0;
  unsigned offset = 0;
  char username_buf[1024];
  char username[1024];
  
  num_static_data_blocks = 0;
  static_data_bytes_used = 0;
  static_block_list.Clear();

  for(;;) {
    // Find the next block header
    int found_block = 0;
    for(block_offset = offset; (static_data_size >= hdr_len) && (block_offset <= (static_data_size - hdr_len)); block_offset += hdr_len) {
      static_data_block_header.WipeHeader(); // Clear all existing header data before read
      memmove(&static_data_block_header, static_data+block_offset, hdr_len);
      // 10/25/2026: Blocks of v2 databases have STATIC_DATA_BLOCK_VERSION_V2
      if(((static_data_block_header.version == STATIC_DATA_BLOCK_VERSION) ||
	  (static_data_block_header.version == STATIC_DATA_BLOCK_VERSION_V2)) &&
	 (static_data_block_header.checkword == 0xFEFE)) {
	found_block = 1;
	break;
      }
    }
    if(!found_block) break;

    unsigned pos = block_offset + hdr_len;
    unsigned remaining = static_data_size - pos;
    unsigned ciphertext_len = static_data_block_header.ciphertext_len;
    unsigned username_len = static_data_block_header.username_len;
    if((ciphertext_len > max_ciphertext_len) || (ciphertext_len > remaining) ||
       (AES_MAX_HMAC_LEN > (remaining - ciphertext_len)) ||
       (username_len >= sizeof(username_buf)) ||
       (username_len > (remaining - ciphertext_len - AES_MAX_HMAC_LEN))) {
      ERROR_LEVEL = -1;
      err << clear << "Invalid block in static auth area of DB file";
      static_block_list.Clear();
      num_static_data_blocks = 0;
      OPENSSL_cleanse(username_buf, sizeof(username_buf));
      OPENSSL_cleanse(username, sizeof(username));
      return ERROR_LEVEL;
    }

    num_static_data_blocks++;
    static_data_block.Wipe();
    static_data_block.block_header = static_data_block_header;
    static_data_block.rsa_ciphertext.Cat(static_data+pos, ciphertext_len);
    pos += ciphertext_len;
    static_data_block.hmac.Cat(static_data+pos, AES_MAX_HMAC_LEN);
    pos += AES_MAX_HMAC_LEN;
    memset(username_buf, 0, sizeof(username_buf));
    memset(username, 0, sizeof(username));
    if(username_len > 0) {
      memmove(username_buf, static_data+pos, username_len);
      static_data_block.username_encoded = username_buf;
      // The decoded name is never longer than the encoded name
      gxsBase64Decode(username_buf, username);
      username[sizeof(username)-1] = 0;
      static_data_block.username = username;
    }
    pos += username_len;
    static_block_list.Add(static_data_block);
    offset = pos;
  }

  static_data_bytes_used = (num_static_data_blocks == 0) ? 0 : offset;
  OPENSSL_cleanse(username_buf, sizeof(username_buf));
  OPENSSL_cleanse(username, sizeof(username));
    
  return ERROR_LEVEL;
}

int DatabaseUserAuth::UpdateStaticData()
{
  ERROR_LEVEL = 0;
  gxListNode<StaticDataBlock> *ptr = static_block_list.GetHead();
  unsigned offset = 0;
  MemoryBuffer mbuf;
  StaticDataBlockHdr static_data_block_header;
  
  AES_fillrand(static_data, static_data_size);

  // If the list is empty we will be clearing the the static auth area
  if(!ptr) return 0;
  
  offset = 0;
  while(ptr) {
    mbuf.Clear(1);
    mbuf.Cat(&ptr->data.block_header, sizeof(static_data_block_header));
    mbuf.Cat(ptr->data.rsa_ciphertext.m_buf(), ptr->data.rsa_ciphertext.length());
    mbuf.Cat(ptr->data.hmac.m_buf(), ptr->data.hmac.length());
    mbuf.Cat(ptr->data.username_encoded.c_str(), ptr->data.username_encoded.length());
    if((static_data_size < mbuf.length()) || (offset > (static_data_size - mbuf.length()))) {
      ERROR_LEVEL = -1;
      err << clear << "Out of free space in static data area";
      mbuf.Clear(1);
      return -1;
    }
    memmove(static_data+offset, mbuf.m_buf(), mbuf.length());
    offset += mbuf.length();
    ptr = ptr->next;
  }

  mbuf.Clear(1);
  return 0;
}

int DatabaseUserAuth::WriteStaticDataArea()
{
  ERROR_LEVEL = 0;

  if(!f) {
    ERROR_LEVEL = -1;
    err << clear << "Error database pointer is null for DB auth";
    return ERROR_LEVEL;
  }

  FAU_t start_of_auth_data = f->FileHeaderSize() + DB_CONFIG_STATIC_AREA_SIZE;
  gxDatabaseError db_error;

  db_error = f->Write(static_data, static_data_size, start_of_auth_data);
  if(db_error != gxDBASE_NO_ERROR) {
    ERROR_LEVEL = db_error;
    err << clear << "Error writing static auth area to DB file " << gxDatabaseExceptionMessage(db_error);
    return ERROR_LEVEL;
  }

  return 0;
}

int DatabaseUserAuth::AddRSAKeyToStaticArea(const MemoryBuffer &secret,
					    char public_key[], unsigned public_key_len,
					    const gxString &rsa_key_username, char *passphrase,
					    int use_oaep)
{
  gxString sbuf;
  gxUINT32 version;
  StaticDataBlockHdr static_data_block_header;
  char username_buf[1024];
  unsigned char rsa_ciphertext[8192];
  unsigned rsa_ciphertext_len;
  int rv;

  ERROR_LEVEL = 0;
  if(!f) {
    ERROR_LEVEL = -1;
    err << clear << "Error database pointer is null for DB auth";
    return ERROR_LEVEL;
  }
  
  // 10/25/2026: Check the key. For a v2 database the key is a keyset and
  // the new block wraps the master key with OAEP, for v1 it wraps the
  // secret.
  MemoryBuffer ks;
  int aes_error = 0;
  if(EPDBUnlock(f, secret, ks, err, aes_error) != 0) {
    ERROR_LEVEL = aes_error ? aes_error : -1;
    return ERROR_LEVEL;
  }
  int v2 = EPDBIsKeyset(ks);
  DBStringConfig::crypt_key = ks;
  MemoryBuffer wrap_secret;
  if(v2) {
    wrap_secret.Cat(EPDBKeysetKey(ks, EPDB_KEY_MASTER), EPDB_V2_KEY_LEN);
    use_oaep = 1;
  }
  else {
    wrap_secret = secret;
  }

  memset(rsa_ciphertext, 0, sizeof(rsa_ciphertext));
  RSA_openssl_init();  

  StaticDataBlock static_data_block;
  unsigned char SALT[AES_MAX_SALT_LEN];
  unsigned char IV[AES_MAX_IV_LEN];
  unsigned char KEY[AES_MAX_KEY_LEN];
  if(AES_init_salt(SALT, sizeof(SALT)) != AES_NO_ERROR) { // 10/25/2026: Check the result
    ERROR_LEVEL = -1;
    err << clear << "Cannot generate a random salt";
    return ERROR_LEVEL;
  }
  
  rv = AES_derive_key((const unsigned char*)wrap_secret.m_buf(), wrap_secret.length(), SALT, sizeof(SALT), KEY, sizeof(KEY), IV, sizeof(IV), 1000);
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "Failed to generate derived key " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
  MemoryBuffer p_secret;
  p_secret.Cat(SALT, AES_MAX_SALT_LEN);
  p_secret.Cat(wrap_secret.m_buf(), wrap_secret.length());
  
  // 10/25/2026: Use OAEP with SHA-256 when asked. Older versions can
  // only read PKCS#1 v1.5 blocks, see DB_AUTH_BLOCK_STATUS_OAEP.
  rv = RSA_public_key_encrypt((const unsigned char *)public_key, public_key_len,
			      p_secret.m_buf(), p_secret.length(),
			      rsa_ciphertext, sizeof(rsa_ciphertext), &rsa_ciphertext_len,
			      use_oaep ? RSA_PKCS1_OAEP_PADDING : RSA_padding, passphrase);
  if(rv != RSA_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "RSA encrypt public key error " << RSA_err_string(rv);
    return ERROR_LEVEL;
  }

  if(LoadStaticDataBlocks() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  gxListNode<StaticDataBlock> *ptr = static_block_list.GetHead();

  while(ptr) {
    if((ptr->data.username == rsa_key_username) && (ptr->data.block_header.block_type == 1)) {
      ERROR_LEVEL = 1;
      err << clear << "An RSA key for username " << ptr->data.username << " already exists";
      return ERROR_LEVEL;
    }
    ptr = ptr->next;
  }

  static_data_block_header.FormatHeader();
  unsigned char hash[AES_MAX_HMAC_LEN];
  memset(username_buf, 0, sizeof(username_buf));
  unsigned username_buf_len = 0;
  
  static_data_block_header.block_len = 0;
  static_data_block_header.block_type = 1;
  static_data_block_header.block_status = v2 ? DB_AUTH_BLOCK_STATUS_OAEP_MASTER :
    (use_oaep ? DB_AUTH_BLOCK_STATUS_OAEP : DB_AUTH_BLOCK_STATUS_PKCS1);
  if(v2) static_data_block_header.version = STATIC_DATA_BLOCK_VERSION_V2;
  
  static_data_block_header.ciphertext_len = rsa_ciphertext_len;
  static_data_block_header.block_len += rsa_ciphertext_len;
  static_data_block_header.block_len += sizeof(hash);

  // 10/25/2026: The encoded name must fit in username_buf
  if(rsa_key_username.is_null() || rsa_key_username.length() > DB_AUTH_MAX_USERNAME_LEN) {
    ERROR_LEVEL = -1;
    err << clear << "The username must be 1 to " << DB_AUTH_MAX_USERNAME_LEN << " characters";
    return ERROR_LEVEL;
  }
  gxsBase64Encode(rsa_key_username.c_str(), username_buf, rsa_key_username.length());
  username_buf_len = strlen(username_buf);
  static_data_block_header.username_len = username_buf_len;
  
  static_data_block.block_header = static_data_block_header;
  static_data_block.rsa_ciphertext.Cat(rsa_ciphertext, rsa_ciphertext_len);
  rv = AES_HMAC(KEY, sizeof(KEY), rsa_ciphertext, rsa_ciphertext_len, hash, sizeof(hash));
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "Failed to generate HMAC for RSA ciphertext " << AES_err_string(rv);
    return ERROR_LEVEL;
  }

  static_data_block.hmac.Cat(hash, sizeof(hash));
  static_data_block.username = rsa_key_username;
  static_data_block.username_encoded = username_buf;

  static_block_list.Add(static_data_block);

  if(UpdateStaticData() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  if(WriteStaticDataArea() != 0) return -1;

  // 10/25/2026: A v2 database MAC covers the auth area
  if(v2 && EPDBSeal(f, ks, err) != 0) {
    ERROR_LEVEL = -1;
    return ERROR_LEVEL;
  }

  return ERROR_LEVEL;
}

#ifdef __ENABLE_SMART_CARD__
int DatabaseUserAuth::AddSmartCardCertToStaticArea(SmartCardOB *sc, int use_cert_file,
						   const MemoryBuffer &secret, const gxString &smartcard_cert_username,
						   int use_oaep)
{
  gxString sbuf;
  gxUINT32 version;
  StaticDataBlockHdr static_data_block_header;
  char username_buf[1024];
  unsigned char ciphertext[8192];
  unsigned ciphertext_len;
  int rv;

  ERROR_LEVEL = 0;
  if(!f) {
    ERROR_LEVEL = -1;
    err << clear << "Error database pointer is null for DB auth";
    return ERROR_LEVEL;
  }
  
  // 10/25/2026: Check the key. For a v2 database the key is a keyset and
  // the new block wraps the master key with OAEP, for v1 it wraps the
  // secret.
  MemoryBuffer ks;
  int aes_error = 0;
  if(EPDBUnlock(f, secret, ks, err, aes_error) != 0) {
    ERROR_LEVEL = aes_error ? aes_error : -1;
    return ERROR_LEVEL;
  }
  int v2 = EPDBIsKeyset(ks);
  DBStringConfig::crypt_key = ks;
  MemoryBuffer wrap_secret;
  if(v2) {
    wrap_secret.Cat(EPDBKeysetKey(ks, EPDB_KEY_MASTER), EPDB_V2_KEY_LEN);
    use_oaep = 1;
  }
  else {
    wrap_secret = secret;
  }

  memset(ciphertext, 0, sizeof(ciphertext));

  StaticDataBlock static_data_block;
  unsigned char SALT[AES_MAX_SALT_LEN];
  unsigned char IV[AES_MAX_IV_LEN];
  unsigned char KEY[AES_MAX_KEY_LEN];
  if(AES_init_salt(SALT, sizeof(SALT)) != AES_NO_ERROR) { // 10/25/2026: Check the result
    ERROR_LEVEL = -1;
    err << clear << "Cannot generate a random salt";
    return ERROR_LEVEL;
  }
  
  rv = AES_derive_key((const unsigned char*)wrap_secret.m_buf(), wrap_secret.length(), SALT, sizeof(SALT), KEY, sizeof(KEY), IV, sizeof(IV), 1000);
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "Failed to generate derived key " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
  MemoryBuffer p_secret;
  p_secret.Cat(SALT, AES_MAX_SALT_LEN);
  p_secret.Cat(wrap_secret.m_buf(), wrap_secret.length());

  if(use_cert_file) {
    rv =  SC_cert_file_public_key_encrypt(sc, p_secret.m_buf(), p_secret.length(), ciphertext, sizeof(ciphertext), &ciphertext_len,
					  use_oaep ? RSA_PKCS1_OAEP_PADDING : SC_RSA_padding);
  }
  else {
    rv =  SC_public_key_encrypt(sc, p_secret.m_buf(), p_secret.length(), ciphertext, sizeof(ciphertext), &ciphertext_len,
				use_oaep ? RSA_PKCS1_OAEP_PADDING : SC_RSA_padding);
  }
  
  if(rv != 0) {
    ERROR_LEVEL = rv;
    err << clear << "Smart card cert encrypt error " << sc->err_string;
    return ERROR_LEVEL;
  }

  if(LoadStaticDataBlocks() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  gxListNode<StaticDataBlock> *ptr = static_block_list.GetHead();

  while(ptr) {
    if((ptr->data.username == smartcard_cert_username) && (ptr->data.block_header.block_type == 2)) {
      ERROR_LEVEL = 1;
      err << clear << "A smart card cert for username " << ptr->data.username << " already exists";
      return ERROR_LEVEL;
    }
    ptr = ptr->next;
  }

  static_data_block_header.FormatHeader();
  unsigned char hash[AES_MAX_HMAC_LEN];
  memset(username_buf, 0, sizeof(username_buf));
  unsigned username_buf_len = 0;
  
  static_data_block_header.block_len = 0;
  static_data_block_header.block_type = 2;
  static_data_block_header.block_status = v2 ? DB_AUTH_BLOCK_STATUS_OAEP_MASTER :
    (use_oaep ? DB_AUTH_BLOCK_STATUS_OAEP : DB_AUTH_BLOCK_STATUS_PKCS1);
  if(v2) static_data_block_header.version = STATIC_DATA_BLOCK_VERSION_V2;
  
  static_data_block_header.ciphertext_len = ciphertext_len;
  static_data_block_header.block_len += ciphertext_len;
  static_data_block_header.block_len += sizeof(hash);

  // 10/25/2026: The encoded name must fit in username_buf
  if(smartcard_cert_username.is_null() || smartcard_cert_username.length() > DB_AUTH_MAX_USERNAME_LEN) {
    ERROR_LEVEL = -1;
    err << clear << "The username must be 1 to " << DB_AUTH_MAX_USERNAME_LEN << " characters";
    return ERROR_LEVEL;
  }
  gxsBase64Encode(smartcard_cert_username.c_str(), username_buf, smartcard_cert_username.length());
  username_buf_len = strlen(username_buf);
  static_data_block_header.username_len = username_buf_len;
  
  static_data_block.block_header = static_data_block_header;
  static_data_block.rsa_ciphertext.Cat(ciphertext, ciphertext_len);
  rv = AES_HMAC(KEY, sizeof(KEY), ciphertext, ciphertext_len, hash, sizeof(hash));
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "Failed to generate HMAC for smart card ciphertext " << AES_err_string(rv);
    return ERROR_LEVEL;
  }

  static_data_block.hmac.Cat(hash, sizeof(hash));
  static_data_block.username = smartcard_cert_username;
  static_data_block.username_encoded = username_buf;

  static_block_list.Add(static_data_block);

  if(UpdateStaticData() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  if(WriteStaticDataArea() != 0) return -1;

  // 10/25/2026: A v2 database MAC covers the auth area
  if(v2 && EPDBSeal(f, ks, err) != 0) {
    ERROR_LEVEL = -1;
    return ERROR_LEVEL;
  }

  return ERROR_LEVEL;
}
#endif // __ENABLE_SMART_CARD__

int DatabaseUserAuth::DecryptWithRSAKey(char private_key[], unsigned private_key_len,
					const gxString &rsa_key_username, char *passphrase)
{
  ERROR_LEVEL = 0;
  int rv;
  unsigned decrypted_data_len = 0;
  unsigned char rsa_decrypted_message[2048];
  unsigned char hash[AES_MAX_HMAC_LEN];
  unsigned char SALT[AES_MAX_SALT_LEN];
  unsigned char IV[AES_MAX_IV_LEN];
  unsigned char KEY[AES_MAX_KEY_LEN];
  MemoryBuffer aes_file_decrypt_secret;
  
  if(LoadStaticDataBlocks() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  gxListNode<StaticDataBlock> *list_ptr = static_block_list.GetHead();
  if(!list_ptr) {
    ERROR_LEVEL = -1;
    err << clear << "No authorized RSA users found in encrypted DB file";
    return ERROR_LEVEL;
  }
  
  int found_key = 0;
  while(list_ptr) {
    memset(rsa_decrypted_message, 0, sizeof(rsa_decrypted_message));
    memset(hash, 0, sizeof(hash));
    decrypted_data_len = 0;
    if((rsa_key_username == list_ptr->data.username) && (list_ptr->data.block_header.block_type == 1)) {
      found_key = 1;
      break;
    }
    list_ptr = list_ptr->next;
  }

  // 10/25/2026: Blocks are added one per user, so a second block for the
  // same user was inserted into the file. Refuse to pick one.
  if(found_key && CountUserBlocks(rsa_key_username, 1) > 1) {
    ERROR_LEVEL = -1;
    err << clear << "The database has more than one RSA key block for user " << rsa_key_username;
    return ERROR_LEVEL;
  }

  if(!found_key) {
    ERROR_LEVEL = -1;
    err << clear << "The private RSA key provided for user " << rsa_key_username << " does not have access to decrypt the database";
    return ERROR_LEVEL;
  }

  if(list_ptr->data.block_header.block_type != 1) {
    ERROR_LEVEL = -1;
    if(list_ptr->data.block_header.block_type == 2) {
      err << clear << "User " << list_ptr->data.username << " access type is smart card";
    }
    else {
      err << clear << "User " << list_ptr->data.username << " access type is not RSA key";
    }
    return ERROR_LEVEL;
    
  }

  rv = RSA_private_key_decrypt((const unsigned char *)private_key, private_key_len,
			       list_ptr->data.rsa_ciphertext.m_buf(), list_ptr->data.rsa_ciphertext.length(),
			       rsa_decrypted_message, sizeof(rsa_decrypted_message), &decrypted_data_len,
			       ((list_ptr->data.block_header.block_status == DB_AUTH_BLOCK_STATUS_OAEP) ||
			       (list_ptr->data.block_header.block_status == DB_AUTH_BLOCK_STATUS_OAEP_MASTER)) ?
			       RSA_PKCS1_OAEP_PADDING : RSA_padding, passphrase);
  if(rv != RSA_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "RSA private key decrypt failed " << RSA_err_string(rv);
    return ERROR_LEVEL;
  }

  // 10/25/2026: The plaintext must hold the salt and a secret. A shorter
  // plaintext made the length below wrap to about 4 GB.
  if((decrypted_data_len < (AES_MAX_SALT_LEN + AES_MIN_SECRET_LEN)) ||
     (decrypted_data_len > sizeof(rsa_decrypted_message))) {
    OPENSSL_cleanse(rsa_decrypted_message, sizeof(rsa_decrypted_message));
    ERROR_LEVEL = -1;
    err << clear << "Invalid decrypted key length for user " << rsa_key_username;
    return ERROR_LEVEL;
  }
  memmove(SALT, rsa_decrypted_message, AES_MAX_SALT_LEN);
  aes_file_decrypt_secret.Clear(1);
  aes_file_decrypt_secret.Cat(rsa_decrypted_message+AES_MAX_SALT_LEN, (decrypted_data_len-AES_MAX_SALT_LEN));
     
  rv = AES_derive_key((const unsigned char*)aes_file_decrypt_secret.m_buf(), aes_file_decrypt_secret.length(),
		      SALT, sizeof(SALT), KEY, sizeof(KEY), IV, sizeof(IV), 1000);
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Failed to generate derived key " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
      
  rv = AES_HMAC(KEY, sizeof(KEY), list_ptr->data.rsa_ciphertext.m_buf(), list_ptr->data.rsa_ciphertext.length(), hash, sizeof(hash));
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Failed to generate HMAC for RSA ciphertext " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
  if(CRYPTO_memcmp(hash, list_ptr->data.hmac.m_buf(), sizeof(hash)) != 0) { // 10/25/2026: Constant time
    ERROR_LEVEL = -1;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Message authentication failed bad HMAC for RSA ciphertext";
    return ERROR_LEVEL;
  }

  // 10/25/2026: Check the recovered key decrypts the database. The block
  // HMAC is keyed from the key in the block, so it does not show the
  // block was made by someone who knew the database key.
  // 10/25/2026: A v1 block holds the database secret, which gives the
  // v1 key or, through PBKDF2, the v2 keyset. A block added to a v2
  // database holds the v2 master key.
  MemoryBuffer keyset;
  if(UnlockFromBlock(list_ptr->data.block_header.block_status, aes_file_decrypt_secret, keyset) != 0) {
    ERROR_LEVEL = -1;
    aes_file_decrypt_secret.Clear(1);
    err << " (the RSA key block for user " << rsa_key_username << " does not hold this database's key)";
    return ERROR_LEVEL;
  }

  DBStringConfig::crypt_key = keyset;
  keyset.Clear(1);
  aes_file_decrypt_secret.Clear(1);
  
  return ERROR_LEVEL;
}

#ifdef __ENABLE_SMART_CARD__
int DatabaseUserAuth::DecryptWithSmartcard(SmartCardOB *sc, const gxString &smartcard_cert_username)
{
  ERROR_LEVEL = 0;
  int rv;
  unsigned decrypted_data_len = 0;
  unsigned char rsa_decrypted_message[2048];
  unsigned char hash[AES_MAX_HMAC_LEN];
  unsigned char SALT[AES_MAX_SALT_LEN];
  unsigned char IV[AES_MAX_IV_LEN];
  unsigned char KEY[AES_MAX_KEY_LEN];
  MemoryBuffer aes_file_decrypt_secret;
  
  if(LoadStaticDataBlocks() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  gxListNode<StaticDataBlock> *list_ptr = static_block_list.GetHead();
  if(!list_ptr) {
    ERROR_LEVEL = -1;
    err << clear << "No authorized smartcard users found in encrypted DB file";
    return ERROR_LEVEL;
  }

  int found_key = 0;
  while(list_ptr) {
    memset(rsa_decrypted_message, 0, sizeof(rsa_decrypted_message));
    memset(hash, 0, sizeof(hash));
    decrypted_data_len = 0;
    if((smartcard_cert_username == list_ptr->data.username) && (list_ptr->data.block_header.block_type == 2)) {
     found_key = 1;
      break;
    }
    list_ptr = list_ptr->next;
  }

  // 10/25/2026: Refuse a second block for the same user
  if(found_key && CountUserBlocks(smartcard_cert_username, 2) > 1) {
    ERROR_LEVEL = -1;
    err << clear << "The database has more than one smart card block for user " << smartcard_cert_username;
    return ERROR_LEVEL;
  }

  if(!found_key) {
    ERROR_LEVEL = -1;
    err << clear << "The smart card provided for user " << smartcard_cert_username << " does not have access to decrypt the database";
    return ERROR_LEVEL;
  }
  
  if(list_ptr->data.block_header.block_type != 2) {
    ERROR_LEVEL = -1;
    if(list_ptr->data.block_header.block_type == 1) {
      err << clear << "User " << list_ptr->data.username << " access type is RSA key";
    }
    else {
      err << clear << "User " << list_ptr->data.username << " access type is not smart card";
    }
    return ERROR_LEVEL; // 10/25/2026: Was missing
  }
  
  rv = SC_private_key_decrypt(sc, list_ptr->data.rsa_ciphertext.m_buf(), list_ptr->data.rsa_ciphertext.length(),
			      rsa_decrypted_message, sizeof(rsa_decrypted_message), &decrypted_data_len,
			      ((list_ptr->data.block_header.block_status == DB_AUTH_BLOCK_STATUS_OAEP) ||
			       (list_ptr->data.block_header.block_status == DB_AUTH_BLOCK_STATUS_OAEP_MASTER)) ?
			      RSA_PKCS1_OAEP_PADDING : SC_RSA_padding);
  if(rv != 0) {
    ERROR_LEVEL = rv;
    err << clear << "Smart card private key decrypt failed for cert ID " << sc->cert_id << " " << sc->err_string;
    return ERROR_LEVEL;
  }
  
  // 10/25/2026: The plaintext must hold the salt and a secret. A shorter
  // plaintext made the length below wrap to about 4 GB.
  if((decrypted_data_len < (AES_MAX_SALT_LEN + AES_MIN_SECRET_LEN)) ||
     (decrypted_data_len > sizeof(rsa_decrypted_message))) {
    OPENSSL_cleanse(rsa_decrypted_message, sizeof(rsa_decrypted_message));
    ERROR_LEVEL = -1;
    err << clear << "Invalid decrypted key length for user " << smartcard_cert_username;
    return ERROR_LEVEL;
  }
  memmove(SALT, rsa_decrypted_message, AES_MAX_SALT_LEN);
  aes_file_decrypt_secret.Clear(1);
  aes_file_decrypt_secret.Cat(rsa_decrypted_message+AES_MAX_SALT_LEN, (decrypted_data_len-AES_MAX_SALT_LEN));
  
  rv = AES_derive_key((const unsigned char*)aes_file_decrypt_secret.m_buf(), aes_file_decrypt_secret.length(),
		      SALT, sizeof(SALT), KEY, sizeof(KEY), IV, sizeof(IV), 1000);
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Failed to generate derived key " << AES_err_string(rv);
    return rv;
  }
  
  rv = AES_HMAC(KEY, sizeof(KEY), list_ptr->data.rsa_ciphertext.m_buf(), list_ptr->data.rsa_ciphertext.length(), hash, sizeof(hash));
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Failed to generate HMAC for smart card ciphertext " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
  if(CRYPTO_memcmp(hash, list_ptr->data.hmac.m_buf(), sizeof(hash)) != 0) { // 10/25/2026: Constant time
    ERROR_LEVEL = -1;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Message authentication failed bad HMAC for smart card ciphertext";
    return ERROR_LEVEL;
  }
  
  // 10/25/2026: Check the recovered key decrypts the database
  MemoryBuffer keyset;
  if(UnlockFromBlock(list_ptr->data.block_header.block_status, aes_file_decrypt_secret, keyset) != 0) {
    ERROR_LEVEL = -1;
    aes_file_decrypt_secret.Clear(1);
    err << " (the smart card block for user " << smartcard_cert_username << " does not hold this database's key)";
    return ERROR_LEVEL;
  }

  DBStringConfig::crypt_key = keyset;
  keyset.Clear(1);

  aes_file_decrypt_secret.Clear(1);
  return ERROR_LEVEL;
}
#endif // __ENABLE_SMART_CARD__

int DatabaseUserAuth::UnlockFromBlock(gxUINT32 block_status, const MemoryBuffer &block_secret,
				      MemoryBuffer &keyset)
// 10/25/2026: Gets the database key from the secret recovered from a
// user block and checks it. Returns 0 if successful.
{
  if(block_status == DB_AUTH_BLOCK_STATUS_OAEP_MASTER) {
    MemoryBuffer &s = (MemoryBuffer &)block_secret;
    if(s.length() != EPDB_V2_KEY_LEN) {
      err << clear << "The user block has a bad key length";
      return -1;
    }
    return EPDBUnlockMaster(f, (const unsigned char *)s.m_buf(), keyset, err);
  }
  int aes_error = 0;
  return EPDBUnlock(f, block_secret, keyset, err, aes_error);
}

int DatabaseUserAuth::CountUserBlocks(const gxString &username, gxUINT32 block_type)
// 10/25/2026: Number of loaded static area blocks for a user and type
{
  int count = 0;
  gxListNode<StaticDataBlock> *ptr = static_block_list.GetHead();
  while(ptr) {
    if((ptr->data.username == username) && (ptr->data.block_header.block_type == block_type)) count++;
    ptr = ptr->next;
  }
  return count;
}

// --------------------------------------------------------------
// 10/25/2026: Database format v2, see db_auth.h
// --------------------------------------------------------------
static const unsigned char EPDB_V2_MAGIC[8] = { 'E', 'P', 'D', 'B', 'V', '2', 0x1A, 0x01 };
static const unsigned char EPDB_KEYSET_MAGIC[8] = { 'E', 'P', 'D', 'B', 'K', 'E', 'Y', '2' };
static const unsigned char EPDB_FIELD_MAGIC[4] = { 'E', 'P', '2', 0x01 };
static const char *EPDB_LABELS[5] = {
  0, // The master key is not derived
  "EPDB v2 field encryption",
  "EPDB v2 field MAC",
  "EPDB v2 key check",
  "EPDB v2 database MAC"
};
static const char *EPDB_LABEL_KEY_CHECK = "EPDB v2 key check value";
static const char *EPDB_LABEL_FIELD_TAG = "EPDB v2 field";
static const char *EPDB_LABEL_DB_MAC = "EPDB v2 database";

static void epdb_put32(unsigned char *p, unsigned v)
{
  p[0] = (unsigned char)(v & 0xFF); p[1] = (unsigned char)((v >> 8) & 0xFF);
  p[2] = (unsigned char)((v >> 16) & 0xFF); p[3] = (unsigned char)((v >> 24) & 0xFF);
}

static unsigned epdb_get32(const unsigned char *p)
{
  return (unsigned)p[0] | ((unsigned)p[1] << 8) | ((unsigned)p[2] << 16) | ((unsigned)p[3] << 24);
}

static void epdb_put64(unsigned char *p, FAU_t v)
{
  unsigned long long u = (unsigned long long)v;
  for(int i = 0; i < 8; i++) p[i] = (unsigned char)((u >> (8*i)) & 0xFF);
}

static FAU_t epdb_header_address(gxDatabase *f)
{
  return (FAU_t)f->FileHeaderSize() + (FAU_t)EPDB_V2_HEADER_OFFSET;
}

int EPDBIsKeyset(const MemoryBuffer &key)
{
  MemoryBuffer &k = (MemoryBuffer &)key;
  return (k.length() == EPDB_V2_KEYSET_LEN) &&
    (memcmp(k.m_buf(), EPDB_KEYSET_MAGIC, sizeof(EPDB_KEYSET_MAGIC)) == 0);
}

const unsigned char *EPDBKeysetKey(const MemoryBuffer &keyset, int which)
{
  MemoryBuffer &k = (MemoryBuffer &)keyset;
  return (const unsigned char *)k.m_buf() + 8 + (which * EPDB_V2_KEY_LEN);
}

const unsigned char *EPDBKeysetID(const MemoryBuffer &keyset)
{
  MemoryBuffer &k = (MemoryBuffer &)keyset;
  return (const unsigned char *)k.m_buf() + 8 + (5 * EPDB_V2_KEY_LEN);
}

int EPDBReadHeader(gxDatabase *f, unsigned char hdr[EPDB_V2_HEADER_LEN])
// Returns 1 if the file has a v2 header, 0 if it does not and -1 if the
// header area cannot be read.
{
  if(!f) return -1;
  if(f->Read(hdr, EPDB_V2_HEADER_LEN, epdb_header_address(f)) != gxDBASE_NO_ERROR) return -1;
  return memcmp(hdr, EPDB_V2_MAGIC, sizeof(EPDB_V2_MAGIC)) == 0 ? 1 : 0;
}

int EPDBIsV2(gxDatabase *f)
{
  unsigned char hdr[EPDB_V2_HEADER_LEN];
  return EPDBReadHeader(f, hdr) == 1;
}

static int epdb_check_header(const unsigned char hdr[EPDB_V2_HEADER_LEN], gxString &err)
{
  if(epdb_get32(hdr+8) != EPDB_V2_FORMAT_VERSION) {
    err << clear << "Unsupported database format version " << epdb_get32(hdr+8)
	<< ", this program needs a newer version";
    return -1;
  }
  if(epdb_get32(hdr+12) != EPDB_V2_HEADER_LEN || epdb_get32(hdr+16) != EPDB_V2_KDF_PBKDF2_SHA256 ||
     epdb_get32(hdr+20) < EPDB_V2_MIN_ITERATIONS || epdb_get32(hdr+20) > EPDB_V2_MAX_ITERATIONS) {
    err << clear << "The database v2 header is damaged";
    return -1;
  }
  return 0;
}

static int epdb_hmac(const unsigned char *key, const char *label,
		     const unsigned char *a, unsigned a_len,
		     const unsigned char *b, unsigned b_len,
		     unsigned char out[EPDB_V2_MAC_LEN])
{
  AES_hmac_ctx *h = AES_hmac_new(key, EPDB_V2_KEY_LEN);
  if(!h) return 0;
  int ok = AES_hmac_update(h, (const unsigned char *)label, strlen(label) + 1);
  if(ok && a_len) ok = AES_hmac_update(h, a, a_len);
  if(ok && b_len) ok = AES_hmac_update(h, b, b_len);
  if(ok) ok = AES_hmac_final(h, out, EPDB_V2_MAC_LEN);
  AES_hmac_free(h);
  return ok;
}

int EPDBKeysetFromMaster(const unsigned char master[EPDB_V2_KEY_LEN],
			 const unsigned char db_id[EPDB_V2_ID_LEN], MemoryBuffer &keyset)
// Returns 1 if successful
{
  unsigned char k[EPDB_V2_KEYSET_LEN];
  memmove(k, EPDB_KEYSET_MAGIC, 8);
  memmove(k+8, master, EPDB_V2_KEY_LEN);
  int ok = 1;
  for(int i = EPDB_KEY_FIELD_ENC; ok && i <= EPDB_KEY_DB_MAC; i++) {
    ok = FC_kbkdf_sha256(master, EPDB_LABELS[i], db_id, EPDB_V2_ID_LEN, k + 8 + (i * EPDB_V2_KEY_LEN));
  }
  memmove(k + 8 + (5 * EPDB_V2_KEY_LEN), db_id, EPDB_V2_ID_LEN);
  keyset.Clear(1);
  if(ok) keyset.Cat(k, sizeof(k));
  OPENSSL_cleanse(k, sizeof(k));
  return ok;
}

static int epdb_check_keyset(const unsigned char hdr[EPDB_V2_HEADER_LEN], const MemoryBuffer &keyset,
			     gxString &err)
// Checks the key check value and database ID of the header
{
  unsigned char kcv[EPDB_V2_MAC_LEN];
  if(memcmp(EPDBKeysetID(keyset), hdr+64, EPDB_V2_ID_LEN) != 0) {
    err << clear << "The key is for a different database";
    return -1;
  }
  if(!epdb_hmac(EPDBKeysetKey(keyset, EPDB_KEY_CHECK), EPDB_LABEL_KEY_CHECK,
		hdr+64, EPDB_V2_ID_LEN, 0, 0, kcv)) {
    err << clear << "Cannot compute the key check value";
    return -1;
  }
  int rv = CRYPTO_memcmp(kcv, hdr+80, sizeof(kcv)) == 0 ? 0 : -1;
  if(rv != 0) err << clear << "The password or key is not correct for this database";
  OPENSSL_cleanse(kcv, sizeof(kcv));
  return rv;
}

int EPDBUnlockMaster(gxDatabase *f, const unsigned char master[EPDB_V2_KEY_LEN],
		     MemoryBuffer &keyset, gxString &err)
// Builds the keyset of a v2 database from its master key. Returns 0 if
// the key is correct.
{
  unsigned char hdr[EPDB_V2_HEADER_LEN];
  if(EPDBReadHeader(f, hdr) != 1) {
    err << clear << "The database is not a v2 database";
    return -1;
  }
  if(epdb_check_header(hdr, err) != 0) return -1;
  if(!EPDBKeysetFromMaster(master, hdr+64, keyset)) {
    err << clear << "Cannot derive the database keys";
    return -1;
  }
  if(epdb_check_keyset(hdr, keyset, err) != 0) {
    keyset.Clear(1);
    return -1;
  }
  int aes_error = 0;
  if(VerifyDatabaseKey(f, keyset, err, aes_error) != 0) {
    keyset.Clear(1);
    return -1;
  }
  return 0;
}

int EPDBUnlock(gxDatabase *f, const MemoryBuffer &secret, MemoryBuffer &keyset,
	       gxString &err, int &aes_error)
// Checks the secret (password, key file or the secret from a v1 RSA or
// smart card block) or keyset against the database and returns the key
// to use: the secret itself for a v1 database or the keyset for a v2
// database. Returns 0 if successful.
{
  aes_error = AES_NO_ERROR;
  err.Clear();
  unsigned char hdr[EPDB_V2_HEADER_LEN];
  int v2 = EPDBReadHeader(f, hdr);
  if(v2 < 0) {
    err << clear << "Error reading the database config area";
    return -1;
  }
  if(!v2) {
    if(EPDBIsKeyset(secret)) {
      err << clear << "The database is not a v2 database";
      return -1;
    }
    keyset = secret;
    return VerifyDatabaseKey(f, keyset, err, aes_error);
  }
  if(epdb_check_header(hdr, err) != 0) return -1;
  if(EPDBIsKeyset(secret)) {
    keyset = secret;
  }
  else {
    MemoryBuffer &s = (MemoryBuffer &)secret;
    unsigned char master[EPDB_V2_KEY_LEN];
    if(!FC_pbkdf2_sha256((const unsigned char *)s.m_buf(), s.length(), hdr+32, EPDB_V2_SALT_LEN,
			 epdb_get32(hdr+20), master, sizeof(master))) {
      err << clear << "Cannot derive the database key";
      return -1;
    }
    int ok = EPDBKeysetFromMaster(master, hdr+64, keyset);
    OPENSSL_cleanse(master, sizeof(master));
    if(!ok) {
      err << clear << "Cannot derive the database keys";
      return -1;
    }
  }
  if(epdb_check_keyset(hdr, keyset, err) != 0) {
    aes_error = AES_ERROR_AUTH_FAILED;
    keyset.Clear(1);
    return -1;
  }
  if(VerifyDatabaseKey(f, keyset, err, aes_error) != 0) {
    keyset.Clear(1);
    return -1;
  }
  return 0;
}

int EPDBInitV2(gxDatabase *f, const MemoryBuffer &secret, MemoryBuffer &keyset, gxString &err)
// Writes a new v2 header with a random salt and database ID and returns
// the keyset for the secret. The database MAC is set by EPDBSeal().
// Returns 0 if successful.
{
  MemoryBuffer &s = (MemoryBuffer &)secret;
  if(s.length() < AES_MIN_SECRET_LEN) {
    err << clear << "The password or key is too short";
    return -1;
  }
  unsigned char hdr[EPDB_V2_HEADER_LEN];
  memset(hdr, 0, sizeof(hdr));
  memmove(hdr, EPDB_V2_MAGIC, sizeof(EPDB_V2_MAGIC));
  epdb_put32(hdr+8, EPDB_V2_FORMAT_VERSION);
  epdb_put32(hdr+12, EPDB_V2_HEADER_LEN);
  epdb_put32(hdr+16, EPDB_V2_KDF_PBKDF2_SHA256);
  epdb_put32(hdr+20, EPDB_V2_ITERATIONS);
  if(!FC_rand_bytes(hdr+32, EPDB_V2_SALT_LEN, 0) || !FC_rand_bytes(hdr+64, EPDB_V2_ID_LEN, 0)) {
    err << clear << "Cannot generate random data";
    return -1;
  }
  unsigned char master[EPDB_V2_KEY_LEN];
  int ok = FC_pbkdf2_sha256((const unsigned char *)s.m_buf(), s.length(), hdr+32, EPDB_V2_SALT_LEN,
			    EPDB_V2_ITERATIONS, master, sizeof(master));
  if(ok) ok = EPDBKeysetFromMaster(master, hdr+64, keyset);
  OPENSSL_cleanse(master, sizeof(master));
  if(ok) ok = epdb_hmac(EPDBKeysetKey(keyset, EPDB_KEY_CHECK), EPDB_LABEL_KEY_CHECK,
			hdr+64, EPDB_V2_ID_LEN, 0, 0, hdr+80);
  if(!ok) {
    keyset.Clear(1);
    err << clear << "Cannot derive the database keys";
    return -1;
  }
  if(f->Write(hdr, sizeof(hdr), epdb_header_address(f)) != gxDBASE_NO_ERROR) {
    keyset.Clear(1);
    err << clear << "Cannot write the database v2 header";
    return -1;
  }
  return 0;
}

int EPDBEncryptField(const MemoryBuffer &keyset, const unsigned char *in, unsigned in_len,
		     unsigned char *out, unsigned out_size, unsigned *out_len)
// Encrypts a field: magic, IV, AES-256-CBC ciphertext and an HMAC-SHA256
// tag over the IV and ciphertext. Returns an AES error code.
{
  *(out_len) = 0;
  unsigned ct_max = in_len + 16;
  if(out_size < sizeof(EPDB_FIELD_MAGIC) + 16 + ct_max + EPDB_V2_MAC_LEN) return AES_ERROR_BUFFER_OVERFLOW;
  unsigned char *iv = out + sizeof(EPDB_FIELD_MAGIC);
  unsigned char *ct = iv + 16;
  memmove(out, EPDB_FIELD_MAGIC, sizeof(EPDB_FIELD_MAGIC));
  if(!FC_rand_bytes(iv, 16, 0)) return AES_ERROR_SALT_INIT_FAILED;
  unsigned ct_len = 0;
  if(!FC_aes256_cbc(1, EPDBKeysetKey(keyset, EPDB_KEY_FIELD_ENC), iv, in, in_len, ct, &ct_len)) {
    return AES_ERROR_EVP_ENCRYPT_CIPHER_FINAL;
  }
  if(!epdb_hmac(EPDBKeysetKey(keyset, EPDB_KEY_FIELD_MAC), EPDB_LABEL_FIELD_TAG,
		iv, 16 + ct_len, 0, 0, ct + ct_len)) {
    return AES_ERROR_HMAC_FINAL_FAILED;
  }
  *(out_len) = sizeof(EPDB_FIELD_MAGIC) + 16 + ct_len + EPDB_V2_MAC_LEN;
  return AES_NO_ERROR;
}

int EPDBDecryptField(const MemoryBuffer &keyset, const unsigned char *in, unsigned in_len,
		     unsigned char *out, unsigned out_size, unsigned *out_len)
// Checks the tag in constant time and decrypts a field. Returns an AES
// error code.
{
  *(out_len) = 0;
  const unsigned overhead = sizeof(EPDB_FIELD_MAGIC) + 16 + EPDB_V2_MAC_LEN;
  if(in_len < overhead + 16 || ((in_len - overhead) % 16) != 0) return AES_ERROR_BAD_BUFFER_LEN;
  if(memcmp(in, EPDB_FIELD_MAGIC, sizeof(EPDB_FIELD_MAGIC)) != 0) return AES_ERROR_BAD_MODE;
  const unsigned char *iv = in + sizeof(EPDB_FIELD_MAGIC);
  unsigned ct_len = in_len - overhead;
  const unsigned char *ct = iv + 16;
  unsigned char tag[EPDB_V2_MAC_LEN];
  if(!epdb_hmac(EPDBKeysetKey(keyset, EPDB_KEY_FIELD_MAC), EPDB_LABEL_FIELD_TAG,
		iv, 16 + ct_len, 0, 0, tag)) {
    return AES_ERROR_HMAC_FINAL_FAILED;
  }
  if(CRYPTO_memcmp(tag, ct + ct_len, sizeof(tag)) != 0) return AES_ERROR_AUTH_FAILED;
  if(out_size < ct_len + 16) return AES_ERROR_BUFFER_OVERFLOW;
  if(!FC_aes256_cbc(0, EPDBKeysetKey(keyset, EPDB_KEY_FIELD_ENC), iv, ct, ct_len, out, out_len)) {
    OPENSSL_cleanse(out, out_size);
    *(out_len) = 0;
    return AES_ERROR_AUTH_FAILED;
  }
  return AES_NO_ERROR;
}

// Walks the InfoHog objects in a data file in file order. For each
// object calls visit() with the block address and the raw member count
// and members. Returns 0 if successful.
typedef int (*EPDBObjectVisit)(void *data, FAU_t block_address, FAU_t object_address,
			       const unsigned char *members, unsigned members_len);

static int epdb_walk_objects(gxDatabase *f, EPDBObjectVisit visit, void *data, gxString &err)
{
  FAU_t eof = f->GetEOF();
  FAU_t hdr_size = (FAU_t)f->BlockHeaderSize();
  FAU_t addr = f->FindFirstBlock((FAU_t)0);
  gxBlockHeader blk;
  gxObjectHeader oh;
  while(addr != (FAU_t)0 && (addr + hdr_size) < eof) {
    if(f->Read(&blk, sizeof(gxBlockHeader), addr) != gxDBASE_NO_ERROR) {
      err << clear << "Error reading a database block";
      return -1;
    }
    if(blk.block_check_word != gxCheckWord) {
      addr++; // Search for the next block
      continue;
    }
    FAU_t block_len = (FAU_t)(__ULWORD__)blk.block_length;
    if(block_len < hdr_size || (addr + block_len) > eof) {
      err << clear << "A database block length is damaged";
      return -1;
    }
    if((__SBYTE__)blk.block_status == gxNormalBlock) {
      FAU_t oa = addr + hdr_size;
      if(f->Read(&oh, sizeof(gxObjectHeader), oa) != gxDBASE_NO_ERROR) {
	err << clear << "Error reading a database object header";
	return -1;
      }
      if(oh.ClassID == ClassInfoHogID) {
	// The members are inside the block: count, then length and bytes
	FAU_t start = oa + sizeof(gxObjectHeader);
	FAU_t avail = (addr + block_len) - start;
	if(avail < 4 || avail > (FAU_t)(1024*1024)) {
	  err << clear << "A database object is damaged";
	  return -1;
	}
	unsigned char *buf = new unsigned char[(unsigned)avail];
	if(f->Read(buf, (__ULWORD__)avail, start) != gxDBASE_NO_ERROR) {
	  delete[] buf;
	  err << clear << "Error reading a database object";
	  return -1;
	}
	// Find the used length: count, then for each member length and bytes
	unsigned num = epdb_get32(buf), pos = 4;
	int bad = (num == 0 || num > InfoHogMaxMembers);
	for(unsigned i = 0; !bad && i < num; i++) {
	  if(pos + 4 > (unsigned)avail) { bad = 1; break; }
	  unsigned len = epdb_get32(buf+pos);
	  pos += 4;
	  if(len > (unsigned)avail - pos) { bad = 1; break; }
	  pos += len;
	}
	int rv = bad ? -1 : (*visit)(data, addr, oa, buf, pos);
	OPENSSL_cleanse(buf, (unsigned)avail);
	delete[] buf;
	if(bad) {
	  err << clear << "A database object is damaged";
	  return -1;
	}
	if(rv != 0) return rv;
      }
    }
    addr += block_len;
  }
  return 0;
}

static int epdb_mac_visit(void *data, FAU_t block_address, FAU_t, const unsigned char *members,
			  unsigned members_len)
{
  AES_hmac_ctx *h = (AES_hmac_ctx *)data;
  unsigned char a[8];
  epdb_put64(a, block_address);
  if(!AES_hmac_update(h, a, sizeof(a))) return -1;
  // Members as stored: count, then for each member its length and bytes.
  // Adding the member number to each binds the bytes to their column.
  unsigned num = epdb_get32(members), pos = 4;
  unsigned char n[4];
  if(!AES_hmac_update(h, members, 4)) return -1;
  for(unsigned i = 0; i < num; i++) {
    unsigned len = epdb_get32(members+pos);
    epdb_put32(n, i);
    if(!AES_hmac_update(h, n, 4)) return -1;
    if(!AES_hmac_update(h, members+pos, 4 + len)) return -1;
    pos += 4 + len;
  }
  if(pos != members_len) return -1;
  return 0;
}

int EPDBComputeMAC(gxDatabase *f, const MemoryBuffer &keyset,
		   unsigned char mac[EPDB_V2_MAC_LEN], gxString &err)
// HMAC-SHA256 over the v2 header (without the MAC), the config, the
// auth area and every record in file order. Returns 0 if successful.
{
  unsigned char hdr[EPDB_V2_HEADER_LEN];
  if(EPDBReadHeader(f, hdr) != 1) {
    err << clear << "The database is not a v2 database";
    return -1;
  }
  gxDatabaseConfig cfg;
  unsigned cfg_len = cfg.SizeOf();
  unsigned char *area = new unsigned char[DB_AUTH_STATIC_AREA_SIZE];
  unsigned char *cfg_buf = new unsigned char[cfg_len];
  int rv = 0;
  if(f->Read(cfg_buf, cfg_len, (FAU_t)f->FileHeaderSize()) != gxDBASE_NO_ERROR ||
     f->Read(area, DB_AUTH_STATIC_AREA_SIZE, (FAU_t)f->FileHeaderSize() + DB_CONFIG_STATIC_AREA_SIZE) !=
     gxDBASE_NO_ERROR) {
    err << clear << "Error reading the database static areas";
    rv = -1;
  }
  AES_hmac_ctx *h = 0;
  if(rv == 0) {
    h = AES_hmac_new(EPDBKeysetKey(keyset, EPDB_KEY_DB_MAC), EPDB_V2_KEY_LEN);
    if(!h ||
       !AES_hmac_update(h, (const unsigned char *)EPDB_LABEL_DB_MAC, strlen(EPDB_LABEL_DB_MAC) + 1) ||
       !AES_hmac_update(h, hdr, EPDB_V2_MAC_OFFSET) ||
       !AES_hmac_update(h, hdr + EPDB_V2_MAC_OFFSET + EPDB_V2_MAC_LEN,
			EPDB_V2_HEADER_LEN - EPDB_V2_MAC_OFFSET - EPDB_V2_MAC_LEN) ||
       !AES_hmac_update(h, cfg_buf, cfg_len) ||
       !AES_hmac_update(h, area, DB_AUTH_STATIC_AREA_SIZE)) {
      err << clear << "Cannot compute the database MAC";
      rv = -1;
    }
  }
  if(rv == 0) {
    rv = epdb_walk_objects(f, epdb_mac_visit, h, err);
    if(rv != 0 && err.is_null()) err << clear << "Cannot compute the database MAC";
  }
  if(rv == 0 && !AES_hmac_final(h, mac, EPDB_V2_MAC_LEN)) {
    err << clear << "Cannot compute the database MAC";
    rv = -1;
  }
  if(h) AES_hmac_free(h);
  OPENSSL_cleanse(cfg_buf, cfg_len);
  delete[] cfg_buf;
  delete[] area;
  return rv;
}

int EPDBSeal(gxDatabase *f, const MemoryBuffer &keyset, gxString &err)
// Computes and stores the database MAC. Call after every change to a v2
// database. Returns 0 if successful.
{
  unsigned char mac[EPDB_V2_MAC_LEN];
  if(EPDBComputeMAC(f, keyset, mac, err) != 0) return -1;
  if(f->Write(mac, sizeof(mac), epdb_header_address(f) + EPDB_V2_MAC_OFFSET) != gxDBASE_NO_ERROR ||
     f->Flush() != gxDBASE_NO_ERROR) {
    err << clear << "Cannot write the database MAC";
    return -1;
  }
  return 0;
}

// Object addresses for the index check
struct EPDBAddressList {
  EPDBAddressList() { addr = 0; num = size = 0; }
  ~EPDBAddressList() { delete[] addr; }
  int Add(FAU_t a) {
    if(num == size) {
      unsigned n = size ? size * 2 : 256;
      if(n > 4000000) return 0;
      FAU_t *p = new FAU_t[n];
      for(unsigned i = 0; i < num; i++) p[i] = addr[i];
      delete[] addr;
      addr = p;
      size = n;
    }
    addr[num++] = a;
    return 1;
  }
  void Sort() { qsort(addr, num, sizeof(FAU_t), Cmp); }
  static int Cmp(const void *a, const void *b) {
    FAU_t x = *(const FAU_t *)a, y = *(const FAU_t *)b;
    return x < y ? -1 : (x > y ? 1 : 0);
  }
  FAU_t *addr;
  unsigned num, size;
};

static int epdb_addr_visit(void *data, FAU_t, FAU_t object_address, const unsigned char *, unsigned)
{
  return ((EPDBAddressList *)data)->Add(object_address) ? 0 : -1;
}

static int epdb_walk_index(gxBtree *btx, FAU_t node_address, EPDBAddressList &list, unsigned depth)
{
  if(node_address == (FAU_t)0) return 0;
  if(depth > 64) return -1; // Deeper than any valid tree
  BtreeNode n(btx->KeySize(), btx->NodeOrder());
  if(btx->ReadNode(n, node_address) != gxDBASE_NO_ERROR) return -1;
  INFOHOGKEY key;
  for(BtreeKeyCount_t i = 0; i < n.key_count; i++) {
    n.LoadKey(key, (BtreeKeyLocation_t)i);
    if(!list.Add(key.ObjectID())) return -1;
  }
  for(BtreeKeyCount_t i = (BtreeKeyCount_t)-1; i < n.key_count; i++) {
    if(epdb_walk_index(btx, n.GetBranch(i), list, depth + 1) != 0) return -1;
  }
  return 0;
}

int EPDBVerify(POD *pod, const MemoryBuffer &keyset, gxString &err)
// Checks the database MAC and that the index lists exactly the records
// in the data file, so records cannot be hidden by removing them from
// the index. Returns 0 if the database is intact.
{
  gxDatabase *f = pod->OpenDataFile();
  unsigned char mac[EPDB_V2_MAC_LEN], hdr[EPDB_V2_HEADER_LEN];
  if(EPDBReadHeader(f, hdr) != 1) {
    err << clear << "The database is not a v2 database";
    return -1;
  }
  if(EPDBComputeMAC(f, keyset, mac, err) != 0) return -1;
  if(CRYPTO_memcmp(mac, hdr + EPDB_V2_MAC_OFFSET, sizeof(mac)) != 0) {
    err << clear << "The database integrity check failed: the database MAC does not match. "
	<< "The file may have been changed or tampered with outside this program.";
    return 1;
  }
  EPDBAddressList data_list, index_list;
  if(epdb_walk_objects(f, epdb_addr_visit, &data_list, err) != 0) return -1;
  if(epdb_walk_index(pod->Index(), pod->Index()->Root(), index_list, 0) != 0) {
    err << clear << "The database integrity check failed: the index file cannot be read";
    return 1;
  }
  data_list.Sort();
  index_list.Sort();
  int same = (data_list.num == index_list.num);
  for(unsigned i = 0; same && i < data_list.num; i++) same = (data_list.addr[i] == index_list.addr[i]);
  if(!same) {
    err << clear << "The database integrity check failed: the index file does not match the data file. "
	<< "Entries may have been removed or added outside this program.";
    return 1;
  }
  return 0;
}

// --------------------------------------------------------------
// 10/25/2026: Upgrade a v1 database to v2
// --------------------------------------------------------------
static int epdb_config_strings(gxDatabaseConfig &c, DBString **list, int max)
// Every DBString in the database config
{
  int n = 0;
  list[n++] = &c.database_name;
  for(int i = 0; i < NumDataMembers; i++) list[n++] = &c.column_names[i];
  list[n++] = &c.grid_label_font_name;
  list[n++] = &c.grid_text_font_name;
  list[n++] = &c.hyperlink_font_name;
  list[n++] = &c.print_doc_name;
  list[n++] = &c.print_doc_custom_header;
  list[n++] = &c.print_header_font_name;
  list[n++] = &c.print_doc_custom_footer;
  list[n++] = &c.print_footer_font_name;
  for(int i = 0; i < NumDataMembers; i++) list[n++] = &c.cell_text_font_names[i];
  for(int i = 0; i < NumDataMembers; i++) list[n++] = &c.label_text_font_names[i];
  return n <= max ? n : -1;
}

static int epdb_reencrypt(DBString &s, MemoryBuffer &from, MemoryBuffer &to, gxString &err)
// Decrypts a string with one key and encrypts it with another. A null
// string stays null.
{
  int e = 0;
  char *p = s.GetString(from, e);
  if(!p || e) {
    err << clear << "Cannot decrypt a database string " << AES_err_string(e);
    return -1;
  }
  if(p == DBStringNULLPtr::DBStringNUllStr) return 0;
  s.SetString(to, e, p);
  OPENSSL_cleanse(p, strlen(p));
  delete[] p;
  if(e) {
    err << clear << "Cannot encrypt a database string " << AES_err_string(e);
    return -1;
  }
  return 0;
}

static int epdb_copy_private(const char *from, const char *to, gxString &err)
// Copies a file to a new file only the user can read, then compares the
// copy with the original. Returns 0 if successful.
{
  FILE *in = fopen(from, "rb");
  if(!in) {
    err << clear << "Cannot read " << from;
    return -1;
  }
  MemoryBuffer data;
  char buf[65536];
  size_t n;
  while((n = fread(buf, 1, sizeof(buf), in)) > 0) data.Cat(buf, (unsigned)n);
  int read_error = ferror(in);
  fclose(in);
  if(read_error) {
    err << clear << "Error reading " << from;
    return -1;
  }
  if(WritePrivateFile(to, (const char *)data.m_buf(), data.length(), err) != 0) return -1;
  // Read the copy back and compare
  FILE *chk = fopen(to, "rb");
  if(!chk) {
    err << clear << "Cannot read the backup " << to;
    return -1;
  }
  MemoryBuffer copy;
  while((n = fread(buf, 1, sizeof(buf), chk)) > 0) copy.Cat(buf, (unsigned)n);
  fclose(chk);
  if(copy.length() != data.length() || memcmp(copy.m_buf(), data.m_buf(), data.length()) != 0) {
    err << clear << "The backup " << to << " does not match " << from;
    return -1;
  }
  return 0;
}

struct EPDBRecordList { // Plain text of every record, in data file order
  gxList<gxString> records;
  MemoryBuffer *key;
};

static int epdb_record_text_visit(void *data, FAU_t, FAU_t, const unsigned char *members,
				  unsigned members_len)
{
  EPDBRecordList *rl = (EPDBRecordList *)data;
  unsigned num = epdb_get32(members), pos = 4;
  gxString rec;
  rec << num;
  for(unsigned i = 0; i < num; i++) {
    unsigned len = epdb_get32(members+pos);
    pos += 4;
    rec << "|" << i << ":";
    if(len == sizeof(DBString)) {
      DBString s;
      memmove(s.GetSPtr(), members+pos, sizeof(DBString));
      int e = 0;
      char *p = s.GetString(*(rl->key), e);
      if(!p || e) return -1;
      if(p != DBStringNULLPtr::DBStringNUllStr) {
	rec << "s" << (unsigned)strlen(p) << ":" << p;
	OPENSSL_cleanse(p, strlen(p));
	delete[] p;
      }
      else {
	rec << "null";
      }
    }
    else {
      rec << "raw" << len;
    }
    pos += len;
  }
  if(pos != members_len) return -1;
  rl->records.Add(rec);
  SecureWipe(rec);
  return 0;
}

static void epdb_wipe_list(gxList<gxString> &l)
{
  gxListNode<gxString> *p = l.GetHead();
  while(p) {
    SecureWipe(p->data);
    p = p->next;
  }
  l.ClearList();
}

static POD *epdb_open_pod(const gxString &data_file, const gxString &index_file,
			  gxDatabaseAccessMode mode, gxString &err)
{
  POD *pod = new POD;
  InfoHogKey<DBString> key_type;
  FAU_t static_data_size = (FAU_t)(DB_CONFIG_STATIC_AREA_SIZE + DB_AUTH_STATIC_AREA_SIZE);
  gxDatabaseError e = pod->Open(data_file.c_str(), index_file.c_str(), key_type,
				InfoHogNodeOrder, mode, InfoHogUseIndexFile,
				static_data_size, InfoHogNumTrees, 'E', 'E');
  if(e != gxDBASE_NO_ERROR) {
    err << clear << gxDatabaseExceptionMessage(e);
    delete pod;
    return 0;
  }
  return pod;
}

static int epdb_upgrade_copy(POD *old_pod, POD *new_pod, MemoryBuffer &secret, MemoryBuffer &keyset,
			     gxString &err)
// Writes the v2 database: static areas, v2 header, config and records
{
  gxDatabase *of = old_pod->OpenDataFile();
  gxDatabase *nf = new_pod->OpenDataFile();
  FAU_t areas = (FAU_t)nf->FileHeaderSize();
  const unsigned area_size = DB_CONFIG_STATIC_AREA_SIZE + DB_AUTH_STATIC_AREA_SIZE;

  // Random fill of both static areas, then the auth area from the v1
  // file with the user block versions changed to v2
  unsigned char *area = new unsigned char[area_size];
  AES_fillrand(area, area_size);
  unsigned char *auth = area + DB_CONFIG_STATIC_AREA_SIZE;
  int rv = 0;
  if(of->Read(auth, DB_AUTH_STATIC_AREA_SIZE, (FAU_t)of->FileHeaderSize() + DB_CONFIG_STATIC_AREA_SIZE) !=
     gxDBASE_NO_ERROR) {
    err << clear << "Error reading the auth area";
    rv = -1;
  }
  if(rv == 0) {
    DatabaseUserAuth db_auth;
    db_auth.f = of;
    if(db_auth.LoadStaticDataBlocks() != 0) {
      err << clear << db_auth.err;
      rv = -1;
    }
    else {
      // Blocks are stored one after another from the start of the area
      unsigned offset = 0;
      gxListNode<StaticDataBlock> *p = db_auth.static_block_list.GetHead();
      while(p && rv == 0) {
	StaticDataBlockHdr hdr;
	const unsigned hdr_len = sizeof(hdr);
	while(offset + hdr_len <= DB_AUTH_STATIC_AREA_SIZE) {
	  memmove(&hdr, auth + offset, hdr_len);
	  if(((hdr.version == STATIC_DATA_BLOCK_VERSION) || (hdr.version == STATIC_DATA_BLOCK_VERSION_V2)) &&
	     hdr.checkword == 0xFEFE) break;
	  offset += hdr_len;
	}
	if(offset + hdr_len > DB_AUTH_STATIC_AREA_SIZE) {
	  err << clear << "Cannot find a user block in the auth area";
	  rv = -1;
	  break;
	}
	hdr.version = STATIC_DATA_BLOCK_VERSION_V2;
	memmove(auth + offset, &hdr, hdr_len);
	offset += hdr_len + (unsigned)(__ULWORD__)hdr.ciphertext_len + AES_MAX_HMAC_LEN +
	  (unsigned)(__ULWORD__)hdr.username_len;
	p = p->next;
      }
    }
  }
  if(rv == 0 && nf->Write(area, area_size, areas) != gxDBASE_NO_ERROR) {
    err << clear << "Error writing the static areas";
    rv = -1;
  }
  delete[] area;
  if(rv != 0) return rv;

  // v2 header and keys
  if(EPDBInitV2(nf, secret, keyset, err) != 0) return -1;

  // Config strings
  gxDatabaseConfig cfg;
  if(!cfg.ReadConfig(of)) {
    err << clear << "Error reading the database config";
    return -1;
  }
  DBString *strings[64];
  int ns = epdb_config_strings(cfg, strings, 64);
  for(int i = 0; i < ns; i++) {
    if(epdb_reencrypt(*strings[i], secret, keyset, err) != 0) return -1;
  }
  if(!cfg.WriteConfig(nf)) {
    err << clear << "Error writing the database config";
    return -1;
  }

  // Records in data file order. New records are written with the v2
  // keyset, which the index insert also uses to compare keys.
  EPDBAddressList list;
  if(epdb_walk_objects(of, epdb_addr_visit, &list, err) != 0) return -1;
  MemoryBuffer saved_key = DBStringConfig::crypt_key;
  int saved_case = DBStringConfig::DBStringCaseCompare;
  DBStringConfig::crypt_key = keyset;
  DBStringConfig::DBStringCaseCompare = 0; // As the GUI builds the index
  for(unsigned r = 0; rv == 0 && r < list.num; r++) {
    INFOHOG in(old_pod);
    if(!in.ReadObject(list.addr[r])) {
      err << clear << "Error reading a database record";
      rv = -1;
      break;
    }
    INFOHOG out(new_pod, in.GetNumMembers());
    for(unsigned i = 0; rv == 0 && i < in.GetNumMembers(); i++) {
      unsigned len = in.GetMemberLen(i);
      if(len == 0) continue;
      if(len == sizeof(DBString)) {
	DBString s;
	memmove(s.GetSPtr(), in.GetMember(i), sizeof(DBString));
	if(epdb_reencrypt(s, secret, keyset, err) != 0) { rv = -1; break; }
	out.SetMember(&s, sizeof(s), i);
      }
      else {
	out.SetMember(in.GetMember(i), len, i);
      }
    }
    if(rv == 0 && !out.WriteObject()) {
      err << clear << "Error writing a database record, the database may have two entries with the same name";
      rv = -1;
    }
  }
  DBStringConfig::crypt_key = saved_key;
  DBStringConfig::DBStringCaseCompare = saved_case;
  saved_key.Clear(1);
  if(rv != 0) return rv;

  new_pod->Flush();
  return EPDBSeal(nf, keyset, err);
}

int EPDBUpgrade(const char *data_file, const MemoryBuffer &secret_in,
		gxString &backup_data_file, gxString &err)
// Upgrades a v1 database to v2. secret is the v1 database secret. The
// original files are copied to a timestamped backup first. The v2
// database is written to temporary files, checked against the original
// and then renamed over it. If any step fails the original is left as
// it was and no backup or temporary files are left. Returns 0 if
// successful.
{
  err.Clear();
  backup_data_file.Clear();
  MemoryBuffer secret(secret_in);
  gxString dfile(data_file);
  gxString ifile = ReplaceFileExtension(dfile, ".ehx");
  if(dfile.length() < 5 || CaseICmp(gxString(dfile.c_str() + dfile.length() - 4), ".ehd") != 0) {
    err << clear << "The database file name must end in .ehd";
    return -1;
  }
  if(EPDBIsKeyset(secret)) {
    err << clear << "The database is already a v2 database";
    return -1;
  }

  // Check the database and key
  POD *old_pod = epdb_open_pod(dfile, ifile, gxDBASE_READONLY, err);
  if(!old_pod) return -1;
  if(EPDBIsV2(old_pod->OpenDataFile())) {
    delete old_pod;
    err << clear << "The database is already a v2 database";
    return -1;
  }
  int aes_error = 0;
  if(VerifyDatabaseKey(old_pod->OpenDataFile(), secret, err, aes_error) != 0) {
    delete old_pod;
    return -1;
  }

  // Backup copies of both files
  char stamp[32];
  time_t now = time(0);
  struct tm *t = localtime(&now);
  strftime(stamp, sizeof(stamp), "%Y%m%d-%H%M%S", t);
  gxString ext;
  ext << clear << ".v1-backup-" << stamp << ".ehd";
  gxString bdfile = ReplaceFileExtension(dfile, ext.c_str());
  ext << clear << ".v1-backup-" << stamp << ".ehx";
  gxString bifile = ReplaceFileExtension(dfile, ext.c_str());
  gxString tdfile = ReplaceFileExtension(dfile, ".v2-upgrade.ehd");
  gxString tifile = ReplaceFileExtension(dfile, ".v2-upgrade.ehx");
  if(futils_exists(bdfile.c_str()) || futils_exists(bifile.c_str()) ||
     futils_exists(tdfile.c_str()) || futils_exists(tifile.c_str())) {
    delete old_pod;
    err << clear << "A backup or temporary file from another upgrade exists: " << tdfile.c_str();
    return -1;
  }
  int rv = 0;
  if(epdb_copy_private(dfile.c_str(), bdfile.c_str(), err) != 0 ||
     epdb_copy_private(ifile.c_str(), bifile.c_str(), err) != 0) {
    rv = -1;
  }

  // Write and check the v2 database
  MemoryBuffer keyset;
  if(rv == 0) {
    POD *new_pod = epdb_open_pod(tdfile, tifile, gxDBASE_READWRITE, err);
    if(!new_pod) rv = -1;
    else {
#if defined (__UNIX__)
      chmod(tdfile.c_str(), 0600);
      chmod(tifile.c_str(), 0600);
#endif
      rv = epdb_upgrade_copy(old_pod, new_pod, secret, keyset, err);
      delete new_pod;
    }
  }
  if(rv == 0) {
    POD *chk = epdb_open_pod(tdfile, tifile, gxDBASE_READONLY, err);
    MemoryBuffer keyset2;
    if(!chk) rv = -1;
    if(rv == 0 && EPDBUnlock(chk->OpenDataFile(), secret, keyset2, err, aes_error) != 0) rv = -1;
    if(rv == 0 && EPDBVerify(chk, keyset2, err) != 0) rv = -1;
    if(rv == 0) {
      // Every record and config string must match the v1 database
      EPDBRecordList a, b;
      a.key = &secret;
      b.key = &keyset2;
      if(epdb_walk_objects(old_pod->OpenDataFile(), epdb_record_text_visit, &a, err) != 0 ||
	 epdb_walk_objects(chk->OpenDataFile(), epdb_record_text_visit, &b, err) != 0) {
	err << clear << "Cannot read the records to check the upgrade";
	rv = -1;
      }
      gxListNode<gxString> *pa = a.records.GetHead(), *pb = b.records.GetHead();
      while(rv == 0 && (pa || pb)) {
	if(!pa || !pb || pa->data != pb->data) {
	  err << clear << "The upgraded records do not match the original records";
	  rv = -1;
	  break;
	}
	pa = pa->next;
	pb = pb->next;
      }
      epdb_wipe_list(a.records);
      epdb_wipe_list(b.records);
      gxDatabaseConfig c1, c2;
      if(rv == 0 && (!c1.ReadConfig(old_pod->OpenDataFile()) || !c2.ReadConfig(chk->OpenDataFile()))) {
	err << clear << "Cannot read the config to check the upgrade";
	rv = -1;
      }
      DBString *s1[64], *s2[64];
      int n1 = epdb_config_strings(c1, s1, 64);
      epdb_config_strings(c2, s2, 64);
      for(int i = 0; rv == 0 && i < n1; i++) {
	int e1 = 0, e2 = 0;
	char *p1 = s1[i]->GetString(secret, e1);
	char *p2 = s2[i]->GetString(keyset2, e2);
	int same = p1 && p2 && !e1 && !e2 && strcmp(p1, p2) == 0;
	if(p1 && p1 != DBStringNULLPtr::DBStringNUllStr) { OPENSSL_cleanse(p1, strlen(p1)); delete[] p1; }
	if(p2 && p2 != DBStringNULLPtr::DBStringNUllStr) { OPENSSL_cleanse(p2, strlen(p2)); delete[] p2; }
	if(!same) {
	  err << clear << "The upgraded database config does not match the original";
	  rv = -1;
	}
      }
    }
    keyset2.Clear(1);
    if(chk) delete chk;
  }
  delete old_pod;
  keyset.Clear(1);

  // Replace the original files
  if(rv == 0) {
    if(rename(tifile.c_str(), ifile.c_str()) != 0 || rename(tdfile.c_str(), dfile.c_str()) != 0) {
      err << clear << "Cannot replace the database files. The original database is in the backup files "
	  << bdfile.c_str() << " and " << bifile.c_str();
      backup_data_file = bdfile;
      secret.Clear(1);
      return -1;
    }
    backup_data_file = bdfile;
    secret.Clear(1);
    return 0;
  }

  // Failed: remove the temporary and backup files, the original is unchanged
  gxString keep_err = err;
  futils_remove(tdfile.c_str());
  futils_remove(tifile.c_str());
  futils_remove(bdfile.c_str());
  futils_remove(bifile.c_str());
  err = keep_err;
  secret.Clear(1);
  return -1;
}

// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
