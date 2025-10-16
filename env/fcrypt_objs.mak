# Rules to compile Fcrypt library components 
# ===============================================================
cryptdb$(OBJ_EXT):	$(FCRYPT_SRC_PATH)cryptdb.cpp $(CRYPTDB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)cryptdb.cpp

aesdb$(OBJ_EXT):	$(FCRYPT_SRC_PATH)aesdb.cpp $(AESDB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)aesdb.cpp

rsadb$(OBJ_EXT):	$(FCRYPT_SRC_PATH)rsadb.cpp $(RSADB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)rsadb.cpp

smart_card$(OBJ_EXT):	$(FCRYPT_SRC_PATH)smart_card.cpp $(SMART_CARD_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)smart_card.cpp

globals$(OBJ_EXT):	$(FCRYPT_SRC_PATH)globals.cpp $(GLOBALS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)globals.cpp

c_config$(OBJ_EXT):	$(FCRYPT_SRC_PATH)c_config.cpp $(C_CONFIG_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)c_config.cpp

c_thread$(OBJ_EXT):	$(FCRYPT_SRC_PATH)c_thread.cpp $(C_THREAD_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(FCRYPT_SRC_PATH)c_thread.cpp

FCRYPT_CRYPTO_OBJECTS = cryptdb$(OBJ_EXT) \
	       aesdb$(OBJ_EXT) \
	       rsadb$(OBJ_EXT) \
	       smart_card$(OBJ_EXT)

FCRYPT_OBJECTS = globals$(OBJ_EXT) \
	       c_config$(OBJ_EXT) \
	       c_thread$(OBJ_EXT)
