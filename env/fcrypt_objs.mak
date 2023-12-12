# Rules to compile Fcrypt library components 
# ===============================================================
cryptdb.o:	$(FCRYPT_SRC_PATH)cryptdb.cpp $(CRYPTDB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)cryptdb.cpp

aesdb.o:	$(FCRYPT_SRC_PATH)aesdb.cpp $(AESDB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)aesdb.cpp

rsadb.o:	$(FCRYPT_SRC_PATH)rsadb.cpp $(RSADB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)rsadb.cpp

smart_card.o:	$(FCRYPT_SRC_PATH)smart_card.cpp $(SMART_CARD_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)smart_card.cpp

globals.o:	$(FCRYPT_SRC_PATH)globals.cpp $(GLOBALS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)globals.cpp

c_config.o:	$(FCRYPT_SRC_PATH)c_config.cpp $(C_CONFIG_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)c_config.cpp

c_thread.o:	$(FCRYPT_SRC_PATH)c_thread.cpp $(C_THREAD_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)c_thread.cpp

FCRYPT_CRYPTO_OBJECTS = cryptdb.o \
	       aesdb.o \
	       rsadb.o \
	       smart_card.o

FCRYPT_OBJECTS = globals.o \
	       c_config.o \
	       c_thread.o
