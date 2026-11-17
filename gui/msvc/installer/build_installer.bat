@echo off
rem Builds output\encrypted_password_database_<v_v>_windows_setup.exe from
rem an unzipped release zip folder and vc_redist.x64.exe, or with "x86" as
rem the third argument output\encrypted_password_database_<v_v>_windows_32bit_setup.exe
rem from the unzipped 32-bit release zip folder and vc_redist.x86.exe.
rem Usage: build_installer.bat <unzipped release folder> <vc_redist.x64.exe> [x86]
rem vc_redist.x64.exe and vc_redist.x86.exe are in the Visual Studio folder VC\Redist\MSVC\<version>
if "%~2"=="" (
  echo Usage: %0 release_folder vc_redist.exe [x86]
  exit /b 1
)
cd /d %~dp0
if /i "%~3"=="x86" (
  "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" /DArch32 "/DStageDir=%~f1" "/DRedist=%~f2" epdb.iss
) else (
  "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" "/DStageDir=%~f1" "/DRedist=%~f2" epdb.iss
)
