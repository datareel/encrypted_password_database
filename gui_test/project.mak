# Linux common project dependencies, objects, and libraries
# ===============================================================

# Additional library components
SRC_PATH = ..$(PATHSEP)..$(PATHSEP)src$(PATHSEP)
INCLUDE_PATH = ..$(PATHSEP)..$(PATHSEP)include$(PATHSEP)

# Build dependency rules
# ===============================================================
include ../../env/datareel_deps.mak
include ../../env/fcrypt_deps.mak

WXAPPFW_DEP = $(APP_PATH)$(PATHSEP)wxappfw.xpm
# ===============================================================
include ../../env/datareel_objs.mak
include ../../env/fcrypt_objs.mak

wxappfw$(OBJ_EXT):	$(APP_PATH)$(PATHSEP)wxappfw.cpp $(WXAPPFW_DEP) $(DATAREEL_OBJECTS) $(FCRYPT_CRYPTO_OBJECTS)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(APP_PATH)$(PATHSEP)wxappfw.cpp

OBJECTS = wxappfw$(OBJ_EXT)
# ===============================================================
