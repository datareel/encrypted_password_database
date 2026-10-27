For the cross platform GUI library we use wxWidgets 3.3.4, built as
static libraries with the dynamic C runtime (/MD), the same as the
OpenSSL and zlib builds are linked.

Download the release from:

https://github.com/wxWidgets/wxWidgets/releases

The builds used for EPDB were made from wxWidgets-3.3.4.tar.bz2
(sha256 815c16cafa4fcbfcf25c5c0a3418309c62257b8a3b6f91d85a1367a134f98c98)
with Visual Studio 2026 and the default include\wx\msw\setup.h
(wxUSE_NANOSVG 1, which the SVG icons need). They are archived on the
AWS server in ~/3plibs/wxwindows/builds, see README.txt.

64-bit x64 build (EPDB env\msvc64.env uses 3plibs\wxWidgets64-3.3.4):

E:\>mkdir E:\3plibs\wxWidgets64-3.3.4
E:\>tar -xjf E:\3plibs\distrib\wxWidgets-3.3.4.tar.bz2 -C E:\3plibs\wxWidgets64-3.3.4 --strip-components=1
E:\>"C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
E:\>cd /d E:\3plibs\wxWidgets64-3.3.4\build\msw
E:\3plibs\wxWidgets64-3.3.4\build\msw>nmake -f makefile.vc BUILD=release SHARED=0 RUNTIME_LIBS=dynamic TARGET_CPU=X64
E:\3plibs\wxWidgets64-3.3.4\build\msw>nmake -f makefile.vc BUILD=debug SHARED=0 RUNTIME_LIBS=dynamic TARGET_CPU=X64

The libraries are in lib\vc_x64_lib (mswu for release, mswud for debug).

32-bit x86 build (EPDB env\msvc.env uses 3plibs\wxWidgets32-3.3.4):

E:\>mkdir E:\3plibs\wxWidgets32-3.3.4
E:\>tar -xjf E:\3plibs\distrib\wxWidgets-3.3.4.tar.bz2 -C E:\3plibs\wxWidgets32-3.3.4 --strip-components=1
E:\>"C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars32.bat"
E:\>cd /d E:\3plibs\wxWidgets32-3.3.4\build\msw
E:\3plibs\wxWidgets32-3.3.4\build\msw>nmake -f makefile.vc BUILD=release SHARED=0 RUNTIME_LIBS=dynamic
E:\3plibs\wxWidgets32-3.3.4\build\msw>nmake -f makefile.vc BUILD=debug SHARED=0 RUNTIME_LIBS=dynamic

The libraries are in lib\vc_lib.

To test a build:

E:\3plibs\wxWidgets64-3.3.4\build\msw>cd ..\..\samples\minimal
E:\3plibs\wxWidgets64-3.3.4\samples\minimal>nmake -f makefile.vc BUILD=release SHARED=0 RUNTIME_LIBS=dynamic TARGET_CPU=X64

The earlier wxWidgets 3.2.4 builds (3plibs\wxWidgets64-3.2.4 and
wxWidgets32-3.2.4) can still be used with: nmake WXVER=324
