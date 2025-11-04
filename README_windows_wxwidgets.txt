For the cross platform GUI library we will be using wxWidgets.

For the wxWidgets build you will need to download the release you with
to use from: 

https://github.com/wxWidgets/wxWidgets
https://github.com/wxWidgets/wxWidgets/releases

In this document we will be using version 3.2.4

E:
E:\>cd 3plibs
E:\3plibs>mkdir e:\3plibs\wxWidgets32-3.2.4
E:\3plibs>cd e:\3plibs\distrib
E:\3plibs\distrib>unzip -d e:\3plibs\wxWidgets32-3.2.4 wxWidgets-3.2.4.zip
E:\3plibs\distrib>cd e:\3plibs\wxWidgets32-3.2.4\build\msw
E:\3plibs\wxWidgets32-3.2.4\build\msw>"C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars32.bat"
E:\3plibs\wxWidgets32-3.2.4\build\msw>nmake -f makefile.vc BUILD=release SHARED=0 UNICODE=1

To test the build:

E:\3plibs\wxWidgets32-3.2.4\build\msw>cd ..\..\samples\minimal
E:\3plibs\wxWidgets32-3.2.4\samples\minimal>nmake -f makefile.vc BUILD=release SHARED=0 UNICODE=1

NOTE: To build a debug version of wxWigets use the following options:

> nmake -f makefile.vc BUILD=debug SHARED=0 UNICODE=1

