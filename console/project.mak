# WIN32/UNIX common project dependencies, objects, and libraries
# ===============================================================

# Additional library components
SRC_PATH = ..$(PATHSEP)..$(PATHSEP)src$(PATHSEP)
INCLUDE_PATH = ..$(PATHSEP)..$(PATHSEP)include$(PATHSEP)

# Build dependency rules
# ===============================================================
include ../../env/datareel_deps.mak
include ../../env/fcrypt_deps.mak

DB_AUTH_DEP = $(INCLUDE_PATH)db_auth.h

PROJECT_DEP = $(INCLUDE_PATH)db_auth.h

# ===============================================================
include ../../env/datareel_objs.mak
include ../../env/fcrypt_objs.mak

db_auth$(OBJ_EXT):	$(SRC_PATH)db_auth.cpp $(DB_AUTH_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)db_auth.cpp

$(PROJECT)$(OBJ_EXT):	../$(PROJECT).cpp $(PROJECT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) ../$(PROJECT).cpp

OBJECTS = $(PROJECT)$(OBJ_EXT) db_auth$(OBJ_EXT) $(DATAREEL_OBJECTS) $(FCRYPT_CRYPTO_OBJECTS)
# ===============================================================
