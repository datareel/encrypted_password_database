# WIN32/UNIX common project dependencies, objects, and libraries
# ===============================================================

# Additional library components
SRC_PATH = ..$(PATHSEP)..$(PATHSEP)src$(PATHSEP)
INCLUDE_PATH = ..$(PATHSEP)..$(PATHSEP)include$(PATHSEP)

# Build dependency rules
# ===============================================================
include ../../env/datareel_deps.mak
include ../../env/fcrypt_deps.mak

AS2PRINT_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_compress.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

DB_AUTH_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_compress.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

DBSTRING_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_compress.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

M_COMPRESS = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_compress.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

M_DBASE_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_compress.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

M_GLOBALS_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_compress.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

PROJECT_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_compress.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h


# ===============================================================
include ../../env/datareel_objs.mak
include ../../env/fcrypt_objs.mak

as2print$(OBJ_EXT):	$(SRC_PATH)as2print.cpp $(AS2PRINT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)as2print.cpp

db_auth$(OBJ_EXT):	$(SRC_PATH)db_auth.cpp $(DB_AUTH_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)db_auth.cpp

dbstring$(OBJ_EXT):	$(SRC_PATH)dbstring.cpp $(DBSTRING_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)dbstring.cpp

m_compress$(OBJ_EXT):	$(SRC_PATH)m_compress.cpp $(M_COMPRESS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)m_compress.cpp

m_dbase$(OBJ_EXT):	$(SRC_PATH)m_dbase.cpp $(M_DBASE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)m_dbase.cpp

m_globals$(OBJ_EXT):	$(SRC_PATH)m_globals.cpp $(M_GLOBALS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)m_globals.cpp

$(PROJECT)$(OBJ_EXT):	../$(PROJECT).cpp $(PROJECT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) ../$(PROJECT).cpp

OBJECTS = $(PROJECT)$(OBJ_EXT) as2print$(OBJ_EXT) db_auth$(OBJ_EXT) \
	dbstring$(OBJ_EXT) m_compress$(OBJ_EXT) m_dbase$(OBJ_EXT) \
	m_globals$(OBJ_EXT) $(DATAREEL_OBJECTS) $(FCRYPT_CRYPTO_OBJECTS)
# ===============================================================
