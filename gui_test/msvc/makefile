#######################
#### Start of File ####
#######################
# Makefile Contents: Datareel wxWidgets makefile for VS 2026

# Define a name for the executable
#PROJECT = 
#FINAL=
#64BITCFG=

!IF "$(FINAL)" == "1"
LINK_LIB_EXT = wx.lib
!ELSE
LINK_LIB_EXT = dwx.lib
!ENDIF

!IF "$(64BITCFG)" == "1"
GXLINK_LIB = ../../lib/gxcode64$(LINK_LIB_EXT)
!ELSE
GXLINK_LIB = ../../lib/gxcode32$(LINK_LIB_EXT)
!ENDIF

# Installation directory for the application and config files
INSTALL_DIR = ..\..\bin\win32

# Set path to wxWidgets library here
WXDIR = ../../../3plibs/wxWidgets32-3.2.4

# GXD/GXS/GXT library path
GCODE_LIB_DIR = ../../../3plibs/datareel
AES_LIB_DIR = ../../../3plibs/aes
ZCODE_LIB_DIR = ../../../3plibs/zlib

# Use DLL gui libraries
!include ..\..\env\msvc.env
!include $(AES_LIB_DIR)\env\aesdeps.mak
!include $(ZCODE_LIB_DIR)\env\zlibdeps.mak

# Setup additional paths for includes and source code
APP_PATH = ..

ADD_INC_PATHS = -I../../include -I../../src -I../

# Compile the files and build the executable
# ===============================================================
all:	$(PROJECT).exe

!include ../project.mak
!include $(AES_LIB_DIR)\env\aesobjs.mak
!include $(ZCODE_LIB_DIR)\env\zlibobjs.mak

AES_OBJECTS = aescrypt$(OBJ_EXT) \
	aeskey$(OBJ_EXT) \
	aestab$(OBJ_EXT) \
	fileenc$(OBJ_EXT) \
	hmac$(OBJ_EXT) \
	prng$(OBJ_EXT) \
	pwd2key$(OBJ_EXT) \
	sha1$(OBJ_EXT)


ZLIB_OBJECTS = adler32$(OBJ_EXT) \
	compress$(OBJ_EXT) \
	crc32$(OBJ_EXT) \
	deflate$(OBJ_EXT) \
	gzio$(OBJ_EXT) \
	inflate$(OBJ_EXT) \
	inftrees$(OBJ_EXT) \
	inffast$(OBJ_EXT) \
	trees$(OBJ_EXT) \
	uncompr$(OBJ_EXT) \
	zutil$(OBJ_EXT)

$(PROJECT).exe:	$(OBJECTS) $(AES_OBJECTS) $(ZLIB_OBJECTS) $(RCS)
	$(LINKER) $(LINKER_FLAGS) \
	$(OBJECTS) $(AES_OBJECTS) $(ZLIB_OBJECTS) $(RCS) $(LINK_LIBRARIES) $(GXLINK_LIB) /OUT:$@ 
# ===============================================================

# Install the binaries to the bin directory
# ===============================================================
#install:
#	@echo Installing $(PROJECT) binaries to the bin directory...
#	@if exist $(PROJECT).exe copy $(PROJECT).exe $(INSTALL_DIR)
# ===============================================================

# Remove OBJS, debug files, and executable after running nmake 
# ===============================================================
clean:
	@echo Removing all .SBR files from working directory...
	if exist *.sbr del *.sbr  

	@echo Removing all .VCW files from working directory...
	if exist *.vcw del *.vcw 

	@echo Removing all .PDB files from working directory...
	if exist *.pdb del *.pdb 

	@echo Removing all .WSP files from working directory...
	if exist *.wsp del *.wsp 

	@echo Removing all .BSC files from working directory...
	if exist *.bsc del *.bsc 

	@echo Removing all .SBT files from working directory...
	if exist *.sbt del *.sbt 

	@echo Removing all .ILK files from working directory...
	if exist *.ilk del *.ilk 

	@echo Removing all .IDB files from working directory...
	if exist *.idb del *.idb 

	@echo Removing all .MDP files from working directory...
	if exist *.mdp del *.mdp 

	@echo Removing all .PCH files from working directory...
	if exist *.pch del *.pch 

	@echo Removing all .NCB files from working directory...
	if exist *.ncb del *.ncb 

	@echo Removing all .RES files from working directory...
	if exist *.res del *.res 

	@echo Removing all $(OBJ_EXT) files from working directory...
	if exist *.obj del *.obj

	@echo Removing the EXECUTABLE file from working directory
	if exist $(PROJECT).exe del $(PROJECT).exe 

	@echo Removing all database files from working directory
	if exist *.ihd del *.idh
	if exist *.ihx del *.idx
	if exist *.hsd del *.hsd
	if exist *.hsx del *.hsx
	if exist *.ds1 del *.ds1
	if exist *.log del *.log
# --------------------------------------------------------------- 
#####################
#### End of File ####
#####################
