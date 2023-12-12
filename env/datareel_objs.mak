# Rules to compile Datareel library components 
# ===============================================================
asprint.o:	$(DATAREEL_SRC_PATH)asprint.cpp $(ASPRINT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)asprint.cpp

bstreei.o:	$(DATAREEL_SRC_PATH)bstreei.cpp $(GXSTREEI_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)bstreei.cpp

btcache.o:	$(DATAREEL_SRC_PATH)btcache.cpp $(BTCACHE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)btcache.cpp

btnode.o:	$(DATAREEL_SRC_PATH)btnode.cpp $(BTNODE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)btnode.cpp

btstack.o:	$(DATAREEL_SRC_PATH)btstack.cpp $(BTSTACK_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)btstack.cpp

cdate.o:	$(DATAREEL_SRC_PATH)cdate.cpp $(CDATE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)cdate.cpp

dbfcache.o:	$(DATAREEL_SRC_PATH)dbfcache.cpp $(DBFCACHE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)dbfcache.cpp

dbasekey.o:	$(DATAREEL_SRC_PATH)dbasekey.cpp $(DBASEKEY_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)dbasekey.cpp

dbugmgr.o:	$(DATAREEL_SRC_PATH)dbugmgr.cpp $(DBUGMGR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)dbugmgr.cpp

devcache.o:	$(DATAREEL_SRC_PATH)devcache.cpp $(DEVCACHE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)devcache.cpp

dfileb.o:	$(DATAREEL_SRC_PATH)dfileb.cpp $(DFILEB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)dfileb.cpp

ehandler.o:	$(DATAREEL_SRC_PATH)ehandler.cpp $(EHANDLER_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)ehandler.cpp

fstring.o:	$(DATAREEL_SRC_PATH)fstring.cpp $(FSTRING_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)fstring.cpp

futils.o:	$(DATAREEL_SRC_PATH)futils.cpp $(FUTILS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)futils.cpp

gpersist.o:	$(DATAREEL_SRC_PATH)gpersist.cpp $(GPERSIST_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gpersist.cpp

gthreadt.o:	$(DATAREEL_SRC_PATH)gthreadt.cpp $(GTHREADT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gthreadt.cpp

gxbtree.o:	$(DATAREEL_SRC_PATH)gxbtree.cpp $(GXTREE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxbtree.cpp

gxcond.o:	$(DATAREEL_SRC_PATH)gxcond.cpp $(GXCOND_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxcond.cpp

gxconfig.o:	$(DATAREEL_SRC_PATH)gxconfig.cpp $(GXCONFIG_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxconfig.cpp

gxcrc32.o:	$(DATAREEL_SRC_PATH)gxcrc32.cpp $(GXCRC32_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxcrc32.cpp

gxdatagm.o:	$(DATAREEL_SRC_PATH)gxdatagm.cpp $(GXDATAGM_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdatagm.cpp

gxdbase.o:    $(DATAREEL_SRC_PATH)gxdbase.cpp $(GXDBASE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdbase.cpp

gxderror.o:	$(DATAREEL_SRC_PATH)gxderror.cpp $(GXDERROR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxderror.cpp

gxdfp64.o:    $(DATAREEL_SRC_PATH)gxdfp64.cpp $(GXDFP64_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdfp64.cpp

gxdfptr.o:    $(DATAREEL_SRC_PATH)gxdfptr.cpp $(GXDFPTR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdfptr.cpp

gxdlcode.o:	$(DATAREEL_SRC_PATH)gxdlcode.cpp $(GXDLCODE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdlcode.cpp

gxdstats.o:	$(DATAREEL_SRC_PATH)gxdstats.cpp $(GXDSTATS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdstats.cpp

gxfloat.o:	$(DATAREEL_SRC_PATH)gxfloat.cpp $(GXFLOAT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxfloat.cpp

gxint16.o:	$(DATAREEL_SRC_PATH)gxint16.cpp $(GXINT16_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxint16.cpp

gxint32.o:	$(DATAREEL_SRC_PATH)gxint32.cpp $(GXINT32_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxint32.cpp

gxint64.o:	$(DATAREEL_SRC_PATH)gxint64.cpp $(GXINT64_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxint64.cpp

gxip32.o:	$(DATAREEL_SRC_PATH)gxip32.cpp $(GXIP32_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxip32.cpp

gxlistb.o:	$(DATAREEL_SRC_PATH)gxlistb.cpp $(GXLISTB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxlistb.cpp

gxmac48.o:	$(DATAREEL_SRC_PATH)gxmac48.cpp $(GXMAC48_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxmac48.cpp

gxmutex.o:	$(DATAREEL_SRC_PATH)gxmutex.cpp $(GXMUTEX_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxmutex.cpp

gxrdbdef.o:	$(DATAREEL_SRC_PATH)gxrdbdef.cpp $(GXRDBDEF_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxrdbdef.cpp

gxrdbhdr.o:	$(DATAREEL_SRC_PATH)gxrdbhdr.cpp $(GXRDBHDR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxrdbhdr.cpp

gxrdbms.o:	$(DATAREEL_SRC_PATH)gxrdbms.cpp $(GXRDBMS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxrdbms.cpp

gxrdbsql.o:	$(DATAREEL_SRC_PATH)gxrdbsql.cpp $(GXRDBSQL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxrdbsql.cpp

gxscomm.o:	$(DATAREEL_SRC_PATH)gxscomm.cpp $(GXSCOMM_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxscomm.cpp

gxsema.o:	$(DATAREEL_SRC_PATH)gxsema.cpp $(GXSEMA_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsema.cpp

gxsftp.o:	$(DATAREEL_SRC_PATH)gxsftp.cpp $(GXSFTP_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsftp.cpp

gxshtml.o:	$(DATAREEL_SRC_PATH)gxshtml.cpp $(GXSHTML_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxshtml.cpp

gxsrss.o:	$(DATAREEL_SRC_PATH)gxsrss.cpp $(GXSRSS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsrss.cpp

gxsxml.o:	$(DATAREEL_SRC_PATH)gxsxml.cpp $(GXSXML_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsxml.cpp

gxshttp.o:	$(DATAREEL_SRC_PATH)gxshttp.cpp $(GXSHTTP_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxshttp.cpp

gxshttpc.o:	$(DATAREEL_SRC_PATH)gxshttpc.cpp $(GXSHTTPC_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxshttpc.cpp

gxsmtp.o:	$(DATAREEL_SRC_PATH)gxsmtp.cpp $(GXSMTP_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsmtp.cpp

gxsocket.o:	$(DATAREEL_SRC_PATH)gxsocket.cpp $(GXSOCKET_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsocket.cpp

gxsping.o:	$(DATAREEL_SRC_PATH)gxsping.cpp $(GXSPING_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsping.cpp

gxspop3.o:	$(DATAREEL_SRC_PATH)gxspop3.cpp $(GXSPOP3_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxspop3.cpp

gxssl.o:	$(DATAREEL_SRC_PATH)gxssl.cpp $(GXSSL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxssl.cpp

gxstream.o:	$(DATAREEL_SRC_PATH)gxstream.cpp $(GXSTREAM_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxstream.cpp

gxsurl.o:	$(DATAREEL_SRC_PATH)gxsurl.cpp $(GXSURL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsurl.cpp

gxsutils.o:	$(DATAREEL_SRC_PATH)gxsutils.cpp $(GXSUTILS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsutils.cpp

gxs_b64.o:	$(DATAREEL_SRC_PATH)gxs_b64.cpp $(GXS_B64_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxs_b64.cpp

gxtelnet.o:	$(DATAREEL_SRC_PATH)gxtelnet.cpp $(GXTELNET_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxtelnet.cpp

gxthread.o:	$(DATAREEL_SRC_PATH)gxthread.cpp $(GXTHREAD_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxthread.cpp

gxuint16.o:	$(DATAREEL_SRC_PATH)gxuint16.cpp $(GXUINT16_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxuint16.cpp

gxuint32.o:	$(DATAREEL_SRC_PATH)gxuint32.cpp $(GXUINT32_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxuint32.cpp

gxuint64.o:	$(DATAREEL_SRC_PATH)gxuint64.cpp $(GXUINT64_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxuint64.cpp

htmldrv.o:	$(DATAREEL_SRC_PATH)htmldrv.cpp $(HTMLDRV_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)htmldrv.cpp

httpgrab.o:	$(DATAREEL_SRC_PATH)httpgrab.cpp $(HTTPGRAB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)httpgrab.cpp

leaktest.o:	$(DATAREEL_SRC_PATH)leaktest.cpp $(LEAKTEST_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)leaktest.cpp

logfile.o:	$(DATAREEL_SRC_PATH)logfile.cpp $(LOGFILE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)logfile.cpp

memblock.o:	$(DATAREEL_SRC_PATH)memblock.cpp $(MEMBLOCK_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)memblock.cpp

membuf.o:	$(DATAREEL_SRC_PATH)membuf.cpp $(MEMBUF_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)membuf.cpp

ostrbase.o:	$(DATAREEL_SRC_PATH)ostrbase.cpp $(OSTRBASE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)ostrbase.cpp

pod.o:	$(DATAREEL_SRC_PATH)pod.cpp $(POD_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)pod.cpp

pscript.o:	$(DATAREEL_SRC_PATH)pscript.cpp $(PSCRIPT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)pscript.cpp

scomserv.o:	$(DATAREEL_SRC_PATH)scomserv.cpp $(SCOMSERV_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)scomserv.cpp

stdafx.o:	$(DATAREEL_SRC_PATH)stdafx.cpp $(STDAFX_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)stdafx.cpp

strutil.o:	$(DATAREEL_SRC_PATH)strutil.cpp $(STRUTIL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)strutil.cpp

systime.o:	$(DATAREEL_SRC_PATH)systime.cpp $(SYSTIME_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)systime.cpp

terminal.o:	$(DATAREEL_SRC_PATH)terminal.cpp $(TERMINAL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)terminal.cpp

thelpers.o:	$(DATAREEL_SRC_PATH)thelpers.cpp $(THELPERS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)thelpers.cpp

thrapiw.o:	$(DATAREEL_SRC_PATH)thrapiw.cpp $(THRAPIW_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)thrapiw.cpp

thrpool.o:	$(DATAREEL_SRC_PATH)thrpool.cpp $(THRPOOL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)thrpool.cpp

ustring.o:	$(DATAREEL_SRC_PATH)ustring.cpp $(USTRING_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)ustring.cpp

wserror.o:	$(DATAREEL_SRC_PATH)wserror.cpp $(WSERROR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)wserror.cpp

# DLL entry point objects
DATAREEL_DLL_OBJECTS = stdafx.o \
	gxdlcode.o

# Core database library components
DATAREEL_DATABASE_CORE_OBJECTS =  gxint32.o \
	gxint64.o \
	gxuint32.o \
	gxuint64.o \
	gxcrc32.o \
	gxdbase.o \
	gxderror.o \
	gxdfp64.o \
	gxdfptr.o

# RDBMS library components
DATAREEL_RDBMS_OBJECTS =  gxrdbdef.o \
	gxrdbhdr.o \
	gxrdbms.o \
	gxrdbsql.o

# Extra database library components
DATAREEL_DATABASE_EX_OBJECTS = btstack.o \
	btcache.o \
	btnode.o \
	dbasekey.o \
	gpersist.o \
	gxbtree.o \
	pod.o \
	gxint16.o \
	gxuint16.o \
	gxfloat.o \
	dbfcache.o \
	dbugmgr.o

# Core socket library components
DATAREEL_SOCKET_CORE_OBJECTS = gxsocket.o

# Extra socket library components
DATAREEL_SOCKET_EX_OBJECTS = gxshttp.o \
	gxshttpc.o \
	gxsmtp.o \
	gxsping.o \
	gxspop3.o \
	gxsftp.o \
	gxshtml.o \
	gxsurl.o \
	gxsutils.o \
	gxs_b64.o \
	gxtelnet.o \
	wserror.o

# Database socket library components
DATAREEL_SOCKET_DB_OBJECTS = gxdatagm.o \
	gxstream.o

# Core serial comm library components
DATAREEL_SERIAL_CORE_OBJECTS = gxscomm.o

# Database serial com library components
DATAREEL_SERIAL_DB_OBJECTS = scomserv.o

# Thread library components
DATAREEL_THREAD_OBJECTS = gthreadt.o \
	gxmutex.o \
	thelpers.o \
	thrapiw.o \
	gxthread.o \
	gxsema.o \
	gxcond.o \
	thrpool.o

# General purpose library components
DATAREEL_GP_OBJECTS = asprint.o \
	bstreei.o  \
	cdate.o  \
	devcache.o \
	dfileb.o \
	fstring.o \
	futils.o \
	gxconfig.o \
	gxip32.o \
	gxlistb.o \
	gxmac48.o \
	htmldrv.o \
	logfile.o \
	memblock.o \
	membuf.o \
	ostrbase.o \
	pscript.o  \
	strutil.o \
	systime.o \
	ustring.o 

# Optional debug objects
# NOTE: The leak test functions requires the /D__MSVC_DEBUG__ compiler
# flag and the /MDd or /MTd compiler flag. 
DATAREEL_DEBUG_OBJECTS = leaktest.o

# Term I/O objects
DATAREEL_TERM_OBJECTS = terminal.o

# Console/GUI messaging gxcode objects
DATAREEL_MSG_OBJECTS = ehandler.o \
	gxdstats.o

DATAREEL_OBJECTS = $(DATAREEL_DLL_OBJECTS) \
		 $(DATAREEL_DATABASE_CORE_OBJECTS) \
		 $(DATAREEL_RDBMS_OBJECTS) \
		 $(DATAREEL_DATABASE_EX_OBJECTS) \
		 $(DATAREEL_SOCKET_CORE_OBJECTS) \
		 $(DATAREEL_SOCKET_EX_OBJECTS) \
		 $(DATAREEL_SOCKET_DB_OBJECTS) \
		 $(DATAREEL_SERIAL_CORE_OBJECTS) \
		 $(DATAREEL_SERIAL_DB_OBJECTS) \
		 $(DATAREEL_THREAD_OBJECTS) \
		 $(DATAREEL_GP_OBJECTS) \
		 $(DATAREEL_DEBUG_OBJECTS) \
		 $(DATAREEL_TERM_OBJECTS) \
		 $(DATAREEL_MSG_OBJECTS)
