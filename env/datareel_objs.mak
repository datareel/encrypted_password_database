# Rules to compile Datareel library components 
# ===============================================================
asprint$(OBJ_EXT):	$(DATAREEL_SRC_PATH)asprint.cpp $(ASPRINT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)asprint.cpp

bstreei$(OBJ_EXT):	$(DATAREEL_SRC_PATH)bstreei.cpp $(GXSTREEI_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)bstreei.cpp

btcache$(OBJ_EXT):	$(DATAREEL_SRC_PATH)btcache.cpp $(BTCACHE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)btcache.cpp

btnode$(OBJ_EXT):	$(DATAREEL_SRC_PATH)btnode.cpp $(BTNODE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)btnode.cpp

btstack$(OBJ_EXT):	$(DATAREEL_SRC_PATH)btstack.cpp $(BTSTACK_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)btstack.cpp

cdate$(OBJ_EXT):	$(DATAREEL_SRC_PATH)cdate.cpp $(CDATE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)cdate.cpp

dbfcache$(OBJ_EXT):	$(DATAREEL_SRC_PATH)dbfcache.cpp $(DBFCACHE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)dbfcache.cpp

dbasekey$(OBJ_EXT):	$(DATAREEL_SRC_PATH)dbasekey.cpp $(DBASEKEY_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)dbasekey.cpp

dbugmgr$(OBJ_EXT):	$(DATAREEL_SRC_PATH)dbugmgr.cpp $(DBUGMGR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)dbugmgr.cpp

devcache$(OBJ_EXT):	$(DATAREEL_SRC_PATH)devcache.cpp $(DEVCACHE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)devcache.cpp

dfileb$(OBJ_EXT):	$(DATAREEL_SRC_PATH)dfileb.cpp $(DFILEB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)dfileb.cpp

ehandler$(OBJ_EXT):	$(DATAREEL_SRC_PATH)ehandler.cpp $(EHANDLER_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)ehandler.cpp

fstring$(OBJ_EXT):	$(DATAREEL_SRC_PATH)fstring.cpp $(FSTRING_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)fstring.cpp

futils$(OBJ_EXT):	$(DATAREEL_SRC_PATH)futils.cpp $(FUTILS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)futils.cpp

gpersist$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gpersist.cpp $(GPERSIST_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gpersist.cpp

gthreadt$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gthreadt.cpp $(GTHREADT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gthreadt.cpp

gxbtree$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxbtree.cpp $(GXTREE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxbtree.cpp

gxcond$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxcond.cpp $(GXCOND_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxcond.cpp

gxconfig$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxconfig.cpp $(GXCONFIG_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxconfig.cpp

gxcrc32$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxcrc32.cpp $(GXCRC32_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxcrc32.cpp

gxdatagm$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxdatagm.cpp $(GXDATAGM_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdatagm.cpp

gxdbase$(OBJ_EXT):    $(DATAREEL_SRC_PATH)gxdbase.cpp $(GXDBASE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdbase.cpp

gxderror$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxderror.cpp $(GXDERROR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxderror.cpp

gxdfp64$(OBJ_EXT):    $(DATAREEL_SRC_PATH)gxdfp64.cpp $(GXDFP64_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdfp64.cpp

gxdfptr$(OBJ_EXT):    $(DATAREEL_SRC_PATH)gxdfptr.cpp $(GXDFPTR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdfptr.cpp

gxdlcode$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxdlcode.cpp $(GXDLCODE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdlcode.cpp

gxdstats$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxdstats.cpp $(GXDSTATS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxdstats.cpp

gxfloat$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxfloat.cpp $(GXFLOAT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxfloat.cpp

gxint16$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxint16.cpp $(GXINT16_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxint16.cpp

gxint32$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxint32.cpp $(GXINT32_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxint32.cpp

gxint64$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxint64.cpp $(GXINT64_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxint64.cpp

gxip32$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxip32.cpp $(GXIP32_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxip32.cpp

gxlistb$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxlistb.cpp $(GXLISTB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxlistb.cpp

gxmac48$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxmac48.cpp $(GXMAC48_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxmac48.cpp

gxmutex$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxmutex.cpp $(GXMUTEX_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxmutex.cpp

gxrdbdef$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxrdbdef.cpp $(GXRDBDEF_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxrdbdef.cpp

gxrdbhdr$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxrdbhdr.cpp $(GXRDBHDR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxrdbhdr.cpp

gxrdbms$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxrdbms.cpp $(GXRDBMS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxrdbms.cpp

gxrdbsql$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxrdbsql.cpp $(GXRDBSQL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxrdbsql.cpp

gxscomm$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxscomm.cpp $(GXSCOMM_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxscomm.cpp

gxsema$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxsema.cpp $(GXSEMA_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsema.cpp

gxsftp$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxsftp.cpp $(GXSFTP_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsftp.cpp

gxshtml$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxshtml.cpp $(GXSHTML_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxshtml.cpp

gxsrss$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxsrss.cpp $(GXSRSS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsrss.cpp

gxsxml$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxsxml.cpp $(GXSXML_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsxml.cpp

gxshttp$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxshttp.cpp $(GXSHTTP_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxshttp.cpp

gxshttpc$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxshttpc.cpp $(GXSHTTPC_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxshttpc.cpp

gxsmtp$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxsmtp.cpp $(GXSMTP_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsmtp.cpp

gxsocket$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxsocket.cpp $(GXSOCKET_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsocket.cpp

gxsping$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxsping.cpp $(GXSPING_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsping.cpp

gxspop3$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxspop3.cpp $(GXSPOP3_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxspop3.cpp

gxssl$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxssl.cpp $(GXSSL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxssl.cpp

gxstream$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxstream.cpp $(GXSTREAM_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxstream.cpp

gxsurl$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxsurl.cpp $(GXSURL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsurl.cpp

gxsutils$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxsutils.cpp $(GXSUTILS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxsutils.cpp

gxs_b64$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxs_b64.cpp $(GXS_B64_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxs_b64.cpp

gxtelnet$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxtelnet.cpp $(GXTELNET_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxtelnet.cpp

gxthread$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxthread.cpp $(GXTHREAD_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxthread.cpp

gxuint16$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxuint16.cpp $(GXUINT16_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxuint16.cpp

gxuint32$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxuint32.cpp $(GXUINT32_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxuint32.cpp

gxuint64$(OBJ_EXT):	$(DATAREEL_SRC_PATH)gxuint64.cpp $(GXUINT64_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)gxuint64.cpp

htmldrv$(OBJ_EXT):	$(DATAREEL_SRC_PATH)htmldrv.cpp $(HTMLDRV_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)htmldrv.cpp

httpgrab$(OBJ_EXT):	$(DATAREEL_SRC_PATH)httpgrab.cpp $(HTTPGRAB_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)httpgrab.cpp

leaktest$(OBJ_EXT):	$(DATAREEL_SRC_PATH)leaktest.cpp $(LEAKTEST_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)leaktest.cpp

logfile$(OBJ_EXT):	$(DATAREEL_SRC_PATH)logfile.cpp $(LOGFILE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)logfile.cpp

memblock$(OBJ_EXT):	$(DATAREEL_SRC_PATH)memblock.cpp $(MEMBLOCK_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)memblock.cpp

membuf$(OBJ_EXT):	$(DATAREEL_SRC_PATH)membuf.cpp $(MEMBUF_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)membuf.cpp

ostrbase$(OBJ_EXT):	$(DATAREEL_SRC_PATH)ostrbase.cpp $(OSTRBASE_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)ostrbase.cpp

pod$(OBJ_EXT):	$(DATAREEL_SRC_PATH)pod.cpp $(POD_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)pod.cpp

pscript$(OBJ_EXT):	$(DATAREEL_SRC_PATH)pscript.cpp $(PSCRIPT_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)pscript.cpp

scomserv$(OBJ_EXT):	$(DATAREEL_SRC_PATH)scomserv.cpp $(SCOMSERV_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)scomserv.cpp

stdafx$(OBJ_EXT):	$(DATAREEL_SRC_PATH)stdafx.cpp $(STDAFX_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)stdafx.cpp

strutil$(OBJ_EXT):	$(DATAREEL_SRC_PATH)strutil.cpp $(STRUTIL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)strutil.cpp

systime$(OBJ_EXT):	$(DATAREEL_SRC_PATH)systime.cpp $(SYSTIME_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)systime.cpp

terminal$(OBJ_EXT):	$(DATAREEL_SRC_PATH)terminal.cpp $(TERMINAL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)terminal.cpp

thelpers$(OBJ_EXT):	$(DATAREEL_SRC_PATH)thelpers.cpp $(THELPERS_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)thelpers.cpp

thrapiw$(OBJ_EXT):	$(DATAREEL_SRC_PATH)thrapiw.cpp $(THRAPIW_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)thrapiw.cpp

thrpool$(OBJ_EXT):	$(DATAREEL_SRC_PATH)thrpool.cpp $(THRPOOL_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)thrpool.cpp

ustring$(OBJ_EXT):	$(DATAREEL_SRC_PATH)ustring.cpp $(USTRING_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)ustring.cpp

wserror$(OBJ_EXT):	$(DATAREEL_SRC_PATH)wserror.cpp $(WSERROR_DEP)
	$(CPP) $(COMPILE_ONLY) $(COMPILE_FLAGS) \
	$(DATAREEL_SRC_PATH)wserror.cpp

# DLL entry point objects
DATAREEL_DLL_OBJECTS = stdafx$(OBJ_EXT) \
	gxdlcode$(OBJ_EXT)

# Core database library components
DATAREEL_DATABASE_CORE_OBJECTS =  gxint32$(OBJ_EXT) \
	gxint64$(OBJ_EXT) \
	gxuint32$(OBJ_EXT) \
	gxuint64$(OBJ_EXT) \
	gxcrc32$(OBJ_EXT) \
	gxdbase$(OBJ_EXT) \
	gxderror$(OBJ_EXT) \
	gxdfp64$(OBJ_EXT) \
	gxdfptr$(OBJ_EXT)

# RDBMS library components
DATAREEL_RDBMS_OBJECTS =  gxrdbdef$(OBJ_EXT) \
	gxrdbhdr$(OBJ_EXT) \
	gxrdbms$(OBJ_EXT) \
	gxrdbsql$(OBJ_EXT)

# Extra database library components
DATAREEL_DATABASE_EX_OBJECTS = btstack$(OBJ_EXT) \
	btcache$(OBJ_EXT) \
	btnode$(OBJ_EXT) \
	dbasekey$(OBJ_EXT) \
	gpersist$(OBJ_EXT) \
	gxbtree$(OBJ_EXT) \
	pod$(OBJ_EXT) \
	gxint16$(OBJ_EXT) \
	gxuint16$(OBJ_EXT) \
	gxfloat$(OBJ_EXT) \
	dbfcache$(OBJ_EXT) \
	dbugmgr$(OBJ_EXT)

# Core socket library components
DATAREEL_SOCKET_CORE_OBJECTS = gxsocket$(OBJ_EXT)

# Extra socket library components
DATAREEL_SOCKET_EX_OBJECTS = gxshttp$(OBJ_EXT) \
	gxshttpc$(OBJ_EXT) \
	gxsmtp$(OBJ_EXT) \
	gxsping$(OBJ_EXT) \
	gxspop3$(OBJ_EXT) \
	gxsftp$(OBJ_EXT) \
	gxshtml$(OBJ_EXT) \
	gxsurl$(OBJ_EXT) \
	gxsutils$(OBJ_EXT) \
	gxs_b64$(OBJ_EXT) \
	gxtelnet$(OBJ_EXT) \
	wserror$(OBJ_EXT)

# Database socket library components
DATAREEL_SOCKET_DB_OBJECTS = gxdatagm$(OBJ_EXT) \
	gxstream$(OBJ_EXT)

# Core serial comm library components
DATAREEL_SERIAL_CORE_OBJECTS = gxscomm$(OBJ_EXT)

# Database serial com library components
DATAREEL_SERIAL_DB_OBJECTS = scomserv$(OBJ_EXT)

# Thread library components
DATAREEL_THREAD_OBJECTS = gthreadt$(OBJ_EXT) \
	gxmutex$(OBJ_EXT) \
	thelpers$(OBJ_EXT) \
	thrapiw$(OBJ_EXT) \
	gxthread$(OBJ_EXT) \
	gxsema$(OBJ_EXT) \
	gxcond$(OBJ_EXT) \
	thrpool$(OBJ_EXT)

# General purpose library components
DATAREEL_GP_OBJECTS = asprint$(OBJ_EXT) \
	bstreei$(OBJ_EXT)  \
	cdate$(OBJ_EXT)  \
	devcache$(OBJ_EXT) \
	dfileb$(OBJ_EXT) \
	fstring$(OBJ_EXT) \
	futils$(OBJ_EXT) \
	gxconfig$(OBJ_EXT) \
	gxip32$(OBJ_EXT) \
	gxlistb$(OBJ_EXT) \
	gxmac48$(OBJ_EXT) \
	htmldrv$(OBJ_EXT) \
	logfile$(OBJ_EXT) \
	memblock$(OBJ_EXT) \
	membuf$(OBJ_EXT) \
	ostrbase$(OBJ_EXT) \
	pscript$(OBJ_EXT)  \
	strutil$(OBJ_EXT) \
	systime$(OBJ_EXT) \
	ustring$(OBJ_EXT) 

# Optional debug objects
# NOTE: The leak test functions requires the /D__MSVC_DEBUG__ compiler
# flag and the /MDd or /MTd compiler flag. 
DATAREEL_DEBUG_OBJECTS = leaktest$(OBJ_EXT)

# Term I/O objects
DATAREEL_TERM_OBJECTS = terminal$(OBJ_EXT)

# Console/GUI messaging gxcode objects
DATAREEL_MSG_OBJECTS = ehandler$(OBJ_EXT) \
	gxdstats$(OBJ_EXT)

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
