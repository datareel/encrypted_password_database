#######################
#### Start of File ####
#######################
# --------------------------------------------------------------- 
# Makefile Contents: Common settings for Linux console applications

# Define the following before including this makefile
#PROJECT=
#FINAL=
#64BITCFG=

ifeq ($(FINAL),1)
LINK_LIB_EXT = wx.a
else
LINK_LIB_EXT = dwx.a
endif

# Define this macro in the main makefile for this application
# Installation directory for the application and config files
INSTALL_DIR = ../../bin/linux

# Datareel library path
DATAREEL_PATH = ../../fcrypt/3plibs/datareel
DATAREEL_SRC_PATH = $(DATAREEL_PATH)/src/
DATAREEL_INCLUDE_PATH = $(DATAREEL_PATH)/include/

FCRYPT_PATH = ../../fcrypt
FCRYPT_SRC_PATH = $(FCRYPT_PATH)/src/
FCRYPT_INCLUDE_PATH = $(FCRYPT_PATH)/include/

# Use Linux dynamic libraries
include ../../env/linux.env

# Define the $(PROJECT_DIR) macro in the main makefile for this application
# Setup path for the project directory
PROJECT_DIR = ..

# Setup additional paths for includes and source code
APP_PATH = $(PROJECT_DIR)

ADD_INC_PATHS = -I../../include -I../../src -I../ -I$(DATAREEL_INCLUDE_PATH) -I$(FCRYPT_INCLUDE_PATH)

# Compile the files and build the executable
# ===============================================================
all:	$(PROJECT)

include ../project.mak

LIBRARIES = 

$(PROJECT):	$(OBJECTS)
	$(CPP) $(LINKER_FLAGS) $(COMPILE_FLAGS) $(OBJECTS) $(LINK_LIBRARIES) \
	$(LIBRARIES) $(OUTPUT) $(PROJECT)
# ===============================================================

# Install the binaries to the bin directory
# ===============================================================
install:
	echo Installing $(PROJECT) binaries to $(INSTALL_DIR)
	mkdir -pv $(INSTALL_DIR)
	cp $(PROJECT) $(INSTALL_DIR)/.
# ===============================================================

# Remove object files and the executable after running make 
# ===============================================================
clean:
	echo Removing all OBJECT files from working directory...
	rm -f *.o 

	echo Removing all database files from working directory...
	rm -f *.hsx
	rm -f *.hsd
	rm -f *.ihd
	rm -f *.ihx 
	rm -f *.ds1
	rm -f *.log
	rm -f *~
	rm -f *.d

	echo Removing EXECUTABLE file from working directory...
	rm -f $(PROJECT)
# --------------------------------------------------------------- 
#####################
#### End of File ####
#####################
