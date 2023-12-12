# Build dependency rules for fcrypt library 
# ===============================================================
CRYPTDB_DEP = $(FCRYPT_INCLUDE_PATH)cryptdb.h $(FCRYPT_INCLUDE_PATH)aesdb.h $(FCRYPT_INCLUDE_PATH)rsadb.h $(FCRYPT_INCLUDE_PATH)smart_card.h

AESDB_DEP = $(FCRYPT_INCLUDE_PATH)aesdb.h

RSADB_DEP = $(FCRYPT_INCLUDE_PATH)rsadb.h

SMART_CARD_DEP = $(FCRYPT_INCLUDE_PATH)smart_card.h

GLOBALS_DEP = $(FCRYPT_INCLUDE_PATH)globals.h $(FCRYPT_INCLUDE_PATH)aesdb.h $(FCRYPT_INCLUDE_PATH)rsadb.h $(FCRYPT_INCLUDE_PATH)smart_card.h

C_CONFIG_DEP = $(FCRYPT_INCLUDE_PATH)c_config.h $(FCRYPT_INCLUDE_PATH)globals.h $(FCRYPT_INCLUDE_PATH)c_thread.h \
	     $(FCRYPT_INCLUDE_PATH)aesdb.h $(FCRYPT_INCLUDE_PATH)rsadb.h $(FCRYPT_INCLUDE_PATH)smart_card.h

C_THREAD_DEP = $(FCRYPT_INCLUDE_PATH)c_thread.h $(FCRYPT_INCLUDE_PATH)globals.h \
	$(FCRYPT_INCLUDE_PATH)c_config.h $(FCRYPT_INCLUDE_PATH)aesdb.h $(FCRYPT_INCLUDE_PATH)cryptdb.h \
	$(FCRYPT_INCLUDE_PATH)rsadb.h $(FCRYPT_INCLUDE_PATH)smart_card.h
