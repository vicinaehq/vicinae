; iscc /DStageDir=<staged tree> /DAppVersion=<x.y.z> vicinae.iss

#ifndef StageDir
  #error Pass /DStageDir=<path to a tree staged with cmake --install>
#endif
#ifndef AppVersion
  #define AppVersion "0.0.0"
#endif
#ifndef Arch
  #define Arch "x64"
#endif

[Setup]
AppId={{C698C8E4-B6C9-4C86-A9AA-520A6D2E45A1}
AppName=Vicinae
AppVersion={#AppVersion}
AppPublisher=Vicinae
AppPublisherURL=https://vicinae.com
AppSupportURL=https://github.com/vicinaehq/vicinae
DefaultDirName={autopf}\Vicinae
DefaultGroupName=Vicinae
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputBaseFilename=vicinae-{#Arch}-setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupIconFile=vicinae.ico
CloseApplications=force
UninstallDisplayName=Vicinae
UninstallDisplayIcon={app}\bin\vicinae-server.exe

[Tasks]
Name: "autostart"; Description: "Start Vicinae when you log in"

[Files]
Source: "{#StageDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\Vicinae"; Filename: "{app}\bin\vicinae-server.exe"
Name: "{userstartup}\Vicinae"; Filename: "{app}\bin\vicinae-server.exe"; Tasks: autostart

[Run]
Filename: "{app}\bin\vicinae-server.exe"; Description: "Launch Vicinae"; Flags: nowait postinstall skipifsilent

[UninstallRun]
Filename: "{sys}\taskkill.exe"; Parameters: "/f /im vicinae-server.exe"; Flags: runhidden; RunOnceId: "KillServer"
