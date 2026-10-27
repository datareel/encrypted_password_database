@echo off
rem Builds output\encrypted_password_database_<v_v>_windows_setup.exe from
rem an unzipped release zip folder and vc_redist.x64.exe.
rem Usage: build_installer.bat <unzipped release folder> <vc_redist.x64.exe>
rem vc_redist.x64.exe is in the Visual Studio folder VC\Redist\MSVC\<version>
if "%~2"=="" (
  echo Usage: %0 release_folder vc_redist.x64.exe
  exit /b 1
)
cd /d %~dp0
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" "/DStageDir=%~f1" "/DRedist=%~f2" epdb.iss
