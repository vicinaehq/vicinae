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
; the self updater checks this against the version announced by the release feed
VersionInfoVersion={#AppVersion}
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
RestartApplications=no
UninstallDisplayName=Vicinae
UninstallDisplayIcon={app}\bin\vicinae-server.exe

[Tasks]
Name: "autostart"; Description: "Start Vicinae when you log in"

[Files]
Source: "{#StageDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Registry]
; URL scheme handlers. Also self-registered by the server at startup; declared
; here so uninstall cleans them up.
Root: HKCU; Subkey: "Software\Classes\vicinae"; ValueType: string; ValueData: "URL:vicinae"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Classes\vicinae"; ValueType: string; ValueName: "URL Protocol"; ValueData: ""
Root: HKCU; Subkey: "Software\Classes\vicinae\DefaultIcon"; ValueType: string; ValueData: "{app}\bin\vicinae-server.exe,0"
Root: HKCU; Subkey: "Software\Classes\vicinae\shell\open\command"; ValueType: string; ValueData: """{app}\bin\vicinae-url-handler.exe"" ""%1"""
Root: HKCU; Subkey: "Software\Classes\raycast"; ValueType: string; ValueData: "URL:raycast"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Classes\raycast"; ValueType: string; ValueName: "URL Protocol"; ValueData: ""
Root: HKCU; Subkey: "Software\Classes\raycast\DefaultIcon"; ValueType: string; ValueData: "{app}\bin\vicinae-server.exe,0"
Root: HKCU; Subkey: "Software\Classes\raycast\shell\open\command"; ValueType: string; ValueData: """{app}\bin\vicinae-url-handler.exe"" ""%1"""
Root: HKCU; Subkey: "Software\Classes\com.raycast"; ValueType: string; ValueData: "URL:com.raycast"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Classes\com.raycast"; ValueType: string; ValueName: "URL Protocol"; ValueData: ""
Root: HKCU; Subkey: "Software\Classes\com.raycast\DefaultIcon"; ValueType: string; ValueData: "{app}\bin\vicinae-server.exe,0"
Root: HKCU; Subkey: "Software\Classes\com.raycast\shell\open\command"; ValueType: string; ValueData: """{app}\bin\vicinae-url-handler.exe"" ""%1"""

[Icons]
Name: "{autoprograms}\Vicinae"; Filename: "{app}\bin\vicinae-server.exe"

[Run]
Filename: "{app}\bin\vicinae-server.exe"; Description: "Launch Vicinae"; Flags: nowait postinstall skipifsilent
; relaunch after a self update (the server runs setup with /AUTOUPDATE=1)
Filename: "{app}\bin\vicinae-server.exe"; Flags: nowait; Check: IsAutoUpdate

[UninstallRun]
Filename: "{sys}\taskkill.exe"; Parameters: "/f /im vicinae-server.exe"; Flags: runhidden; RunOnceId: "KillServer"

[Code]
const
  AutostartTaskName = 'Vicinae';

function IsAutoUpdate: Boolean;
begin
  Result := ExpandConstant('{param:AUTOUPDATE|0}') = '1';
end;

procedure RemoveAutostart;
var
  ResultCode: Integer;
begin
  Exec(ExpandConstant('{sys}\schtasks.exe'),
       '/Delete /F /TN "' + AutostartTaskName + '"',
       '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
end;

procedure CreateAutostart;
var
  User, Cmd, Server: string;
  ResultCode: Integer;
begin
  Server := ExpandConstant('{app}\bin\vicinae-server.exe');

  // schtasks.exe doesn't allow adding ONLOGON tasks without elevation, even for current user only.
  User := GetUserNameString;
  Cmd := '$ErrorActionPreference=''Stop''; Register-ScheduledTask -TaskName ''' + AutostartTaskName + ''' ' +
         '-Action (New-ScheduledTaskAction -Execute ''' + Server + ''') ' +
         '-Trigger (New-ScheduledTaskTrigger -AtLogOn -User ''' + User + ''') ' +
         '-Principal (New-ScheduledTaskPrincipal -UserId ''' + User +
         ''' -LogonType Interactive -RunLevel Limited) -Force';
  Exec(ExpandConstant('{sys}\WindowsPowerShell\v1.0\powershell.exe'),
       '-NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -Command "' + Cmd + '"',
       '', SW_HIDE, ewWaitUntilTerminated, ResultCode);

  if ResultCode <> 0 then
    Log('Failed to create the autostart scheduled task (exit code ' + IntToStr(ResultCode) + ')');
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep = ssPostInstall) and WizardIsTaskSelected('autostart') then
    CreateAutostart;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then
    RemoveAutostart;
end;
