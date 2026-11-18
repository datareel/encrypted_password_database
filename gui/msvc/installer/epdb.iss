; Encrypted Password Database installer (Inno Setup 6), 64-bit Windows, or
; 32-bit Windows with /DArch32 (11/17/2026)
;
; The installer packs the exact files of the release zip, it does not
; build anything. Build the release zip first, then from this folder:
;
;   build_installer.bat <unzipped release folder> <vc_redist.x64.exe>
;   build_installer.bat <unzipped 32-bit release folder> <vc_redist.x86.exe> x86
;
; or call ISCC directly:
;
;   "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" /DStageDir=C:\path\encrypted_password_database_2026_102_windows_binaries ^
;       /DRedist=C:\path\vc_redist.x64.exe epdb.iss
;
; The output is output\encrypted_password_database_<v_v>_windows_setup.exe,
; or output\encrypted_password_database_<v_v>_windows_32bit_setup.exe with
; /DArch32. The 32-bit setup has its own AppId and installs in
; Program Files (x86); both use the same user data folder.
;
; User data is never touched: databases, keys and the configuration
; file live in %USERPROFILE%\.encrypted_password_database and are left
; in place by an upgrade and by the uninstaller. Releases before
; 2026.102 were zip files with no installer, so there is no older
; install to remove; the installed program uses the same
; %USERPROFILE%\.encrypted_password_database as the unzipped programs.

#define AppVersion "2026.106"
#define AppVersionFile "2026_106"
#ifdef Arch32
  #define VcArch "x86"
  #define SetupName "windows_32bit_setup"
#else
  #define VcArch "x64"
  #define SetupName "windows_setup"
#endif
#define AppName "Encrypted Password Database"
#define AppExe "encrypted_password_database.exe"

#ifndef StageDir
  #error Define StageDir, the unzipped release zip folder: ISCC /DStageDir=... /DRedist=... epdb.iss
#endif
#ifndef Redist
  #error Define Redist, the path of vc_redist.x64.exe (vc_redist.x86.exe with /DArch32)
#endif

[Setup]
; Keep these AppIds for every release so a new setup upgrades in place
#ifdef Arch32
AppId={{E9916811-6FBD-4705-88F0-CC397BDC9B97}
#else
AppId={{2E131134-605A-4BF4-ADD0-2DFF8FF2A0E3}
#endif
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher=DataReel
AppPublisherURL=https://datareel.com
AppSupportURL=https://datareel.com/encrypted_password_database/help/
VersionInfoVersion={#AppVersion}
VersionInfoCompany=DataReel
DefaultDirName={autopf}\DataReel\Encrypted Password Database
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
PrivilegesRequired=admin
#ifdef Arch32
ArchitecturesAllowed=x86compatible
#else
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
#endif
; UTF-8 text needs Windows 10 version 1903 or later
MinVersion=10.0.18362
OutputBaseFilename=encrypted_password_database_{#AppVersionFile}_{#SetupName}
OutputDir=output
LicenseFile=..\..\..\LICENSE
SetupIconFile=..\..\icons\encrypted_password_database.ico
WizardStyle=modern
Compression=lzma2
SolidCompression=yes
ChangesEnvironment=yes
CloseApplications=yes
UninstallDisplayName={#AppName} {#AppVersion}
UninstallDisplayIcon={app}\{#AppExe}

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "addpath"; Description: "Add the program folder to the system PATH (for epdb_auth_tool)"; GroupDescription: "Command line:"; Flags: unchecked

[Files]
Source: "{#StageDir}\encrypted_password_database.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StageDir}\epdb_auth_tool.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StageDir}\smartcard_list.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StageDir}\README.txt"; DestDir: "{app}"; DestName: "README_windows.txt"; Flags: ignoreversion
Source: "{#StageDir}\sha256sum.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\..\README.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\..\LICENSE"; DestDir: "{app}"; DestName: "LICENSE.txt"; Flags: ignoreversion
; 11/18/2026: The white paper and the AI-assisted guide files, as on the web site
Source: "..\..\..\docs\security_design.html"; DestDir: "{app}\docs"; Flags: ignoreversion
Source: "..\..\..\AGENTS.md"; DestDir: "{app}\docs"; Flags: ignoreversion
Source: "..\..\..\CLAUDE.md"; DestDir: "{app}\docs"; Flags: ignoreversion
Source: "..\..\..\GEMINI.md"; DestDir: "{app}\docs"; Flags: ignoreversion
Source: "{#Redist}"; DestDir: "{tmp}"; DestName: "vc_redist.{#VcArch}.exe"; Flags: deleteafterinstall; Check: VCRuntimeNeeded

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#AppExe}"
Name: "{group}\README"; Filename: "{app}\README.txt"
Name: "{group}\Security Design (white paper)"; Filename: "{app}\docs\security_design.html"
Name: "{group}\List Smart Cards (no PIN)"; Filename: "{app}\smartcard_list.bat"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\{#AppExe}"; Tasks: desktopicon

[Run]
Filename: "{tmp}\vc_redist.{#VcArch}.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing the Microsoft Visual C++ runtime..."; Flags: waituntilterminated; Check: VCRuntimeNeeded
Filename: "{app}\{#AppExe}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall skipifsilent

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; Tasks: addpath; Check: NeedsAddPath(ExpandConstant('{app}'))

[Code]
// The x86 runtime registers in the 32-bit registry view; the PATH lives
// in the native view (not redirected)
#ifdef Arch32
const RtRoot = HKLM32; EnvRoot = HKLM;
#else
const RtRoot = HKLM64; EnvRoot = HKLM64;
#endif

// The programs are built with MSVC 14.52 and need a VC++ 2015-2026
// runtime ({#VcArch}) of that version or newer
function VCRuntimeNeeded: Boolean;
var
  Installed, Minor, Bld: Cardinal;
begin
  Result := True;
  if RegQueryDWordValue(RtRoot, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\{#VcArch}', 'Installed', Installed) and
     (Installed = 1) and
     RegQueryDWordValue(RtRoot, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\{#VcArch}', 'Minor', Minor) and
     RegQueryDWordValue(RtRoot, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\{#VcArch}', 'Bld', Bld) then
    Result := (Minor < 52) or ((Minor = 52) and (Bld < 37025));
end;

function NeedsAddPath(Dir: String): Boolean;
var
  Path: String;
begin
  if not RegQueryStringValue(EnvRoot, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path) then
  begin
    Result := True;
    exit;
  end;
  Result := Pos(';' + Uppercase(Dir) + ';', ';' + Uppercase(Path) + ';') = 0;
end;

// Remove the program folder from the system PATH on uninstall. Nothing
// in %USERPROFILE%\.encrypted_password_database is removed.
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  Path, Dir: String;
  P: Integer;
begin
  if CurUninstallStep <> usUninstall then exit;
  if not RegQueryStringValue(EnvRoot, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path) then exit;
  Dir := ExpandConstant('{app}');
  P := Pos(';' + Uppercase(Dir), Uppercase(Path));
  if P > 0 then
  begin
    Delete(Path, P, Length(Dir) + 1);
    RegWriteExpandStringValue(EnvRoot, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path);
  end;
end;
