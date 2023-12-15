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
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_crypto.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

DB_AUTH_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_crypto.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

DBSTRING_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_crypto.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

M_CRYPTO = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_crypto.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

M_DBASE_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_crypto.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

M_GLOBALS_DEP = $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_crypto.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h

PROJECT_DEP = $(APP_PATH)$(PATHSEP)$(PROJECT).h \
	    $(INCLUDE_PATH)as2print.h $(INCLUDE_PATH)db_auth.h \
	    $(INCLUDE_PATH)dbstring.h  $(INCLUDE_PATH)m_crypto.h \
 	    $(INCLUDE_PATH)m_dbase.h $(INCLUDE_PATH)m_globals.h \
	    $(APP_PATH)$(PATHSEP)app_defs.h \
	    $(APP_PATH)$(PATHSEP)a_panel.h \
	    $(APP_PATH)$(PATHSEP)c_panel.h \
	    $(APP_PATH)$(PATHSEP)d_panel.h \
	    $(APP_PATH)$(PATHSEP)e_panel.h \
	    $(APP_PATH)$(PATHSEP)f_panel.h \
	    $(APP_PATH)$(PATHSEP)g_thread.h \
	    $(APP_PATH)$(PATHSEP)lprinter.h \
	    $(APP_PATH)$(PATHSEP)lp_panel.h \
	    $(APP_PATH)$(PATHSEP)mciplay.h \
	    $(APP_PATH)$(PATHSEP)mswpage.h \
	    $(APP_PATH)$(PATHSEP)mswprint.h \
	    $(APP_PATH)$(PATHSEP)m_doc.h \
	    $(APP_PATH)$(PATHSEP)m_frame.h \
	    $(APP_PATH)$(PATHSEP)m_grid.h \
	    $(APP_PATH)$(PATHSEP)m_text.h \
	    $(APP_PATH)$(PATHSEP)pspage.h \
	    $(APP_PATH)$(PATHSEP)psprint.h \
	    $(APP_PATH)$(PATHSEP)p_config.h \
	    $(APP_PATH)$(PATHSEP)startdoc.h \
	    $(APP_PATH)$(PATHSEP)st_frame.h \
	    $(APP_PATH)$(PATHSEP)st_panel.h \
	    $(APP_PATH)$(PATHSEP)winapp.h \
	    $(APP_PATH)$(PATHSEP)wx_incs.h \
	    $(APP_PATH)$(PATHSEP)new_panel.h \
	    $(APP_PATH)$(PATHSEP)open_panel.h \
	    $(APP_PATH)$(PATHSEP)a_panel.cpp \
	    $(APP_PATH)$(PATHSEP)backup.cpp \
	    $(APP_PATH)$(PATHSEP)compare.cpp \
	    $(APP_PATH)$(PATHSEP)c_panel.cpp \
	    $(APP_PATH)$(PATHSEP)d_panel.cpp \
	    $(APP_PATH)$(PATHSEP)export.cpp \
	    $(APP_PATH)$(PATHSEP)e_panel.cpp \
	    $(APP_PATH)$(PATHSEP)f_panel.cpp \
	    $(APP_PATH)$(PATHSEP)g_thread.cpp \
	    $(APP_PATH)$(PATHSEP)htmprint.cpp \
	    $(APP_PATH)$(PATHSEP)import.cpp \
	    $(APP_PATH)$(PATHSEP)lprinter.cpp \
	    $(APP_PATH)$(PATHSEP)lp_panel.cpp \
	    $(APP_PATH)$(PATHSEP)mciplay.cpp \
	    $(APP_PATH)$(PATHSEP)merge.cpp \
	    $(APP_PATH)$(PATHSEP)mswpage.cpp \
	    $(APP_PATH)$(PATHSEP)mswprint.cpp \
	    $(APP_PATH)$(PATHSEP)m_doc.cpp \
	    $(APP_PATH)$(PATHSEP)m_frame.cpp \
	    $(APP_PATH)$(PATHSEP)m_grid.cpp \
	    $(APP_PATH)$(PATHSEP)m_tbar.cpp \
	    $(APP_PATH)$(PATHSEP)m_text.cpp \
	    $(APP_PATH)$(PATHSEP)pspage.cpp \
	    $(APP_PATH)$(PATHSEP)psprint.cpp \
	    $(APP_PATH)$(PATHSEP)p_config.cpp \
	    $(APP_PATH)$(PATHSEP)rebuild.cpp \
	    $(APP_PATH)$(PATHSEP)startdoc.cpp \
	    $(APP_PATH)$(PATHSEP)stats.cpp \
	    $(APP_PATH)$(PATHSEP)st_frame.cpp \
	    $(APP_PATH)$(PATHSEP)st_panel.cpp \
	    $(APP_PATH)$(PATHSEP)template.cpp \
	    $(APP_PATH)$(PATHSEP)txtprint.cpp \
	    $(APP_PATH)$(PATHSEP)winapp.cpp \
	    $(APP_PATH)$(PATHSEP)new_panel.cpp \
	    $(APP_PATH)$(PATHSEP)open_panel.cpp
# ===============================================================
include ../../env/datareel_objs.mak
include ../../env/fcrypt_objs.mak

as2print$(OBJ_EXT):	$(SRC_PATH)as2print.cpp $(AS2PRINT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)as2print.cpp

db_auth$(OBJ_EXT):	$(SRC_PATH)db_auth.cpp $(DB_AUTH_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)db_auth.cpp

dbstring$(OBJ_EXT):	$(SRC_PATH)dbstring.cpp $(DBSTRING_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)dbstring.cpp

m_crypto$(OBJ_EXT):	$(SRC_PATH)m_crypto.cpp $(M_CRYPTO_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)m_crypto.cpp

m_dbase$(OBJ_EXT):	$(SRC_PATH)m_dbase.cpp $(M_DBASE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)m_dbase.cpp

m_globals$(OBJ_EXT):	$(SRC_PATH)m_globals.cpp $(M_GLOBALS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(SRC_PATH)m_globals.cpp

$(PROJECT)$(OBJ_EXT):	$(APP_PATH)$(PATHSEP)$(PROJECT).cpp $(PROJECT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(APP_PATH)$(PATHSEP)$(PROJECT).cpp

$(PROJECT)$.res:	$(APP_PATH)$(PATHSEP)$(PROJECT)$.rc $(WXDIR)\include\wx\msw\wx.rc
	$(RC) -r /i$(WXDIR)\include -fo$@ $(APP_PATH)$(PATHSEP)$(PROJECT)$.rc

OBJECTS = $(PROJECT)$(OBJ_EXT) as2print$(OBJ_EXT) db_auth$(OBJ_EXT) \
	dbstring$(OBJ_EXT) m_crypto$(OBJ_EXT) m_dbase$(OBJ_EXT) \
	m_globals$(OBJ_EXT) $(DATAREEL_OBJECTS) $(FCRYPT_CRYPTO_OBJECTS)

RCS = $(PROJECT).res
# ===============================================================
