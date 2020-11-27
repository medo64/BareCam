#define AppName        GetStringFileInfo('..\..\bin\BareCam.exe', 'ProductName')
#define AppVersion     GetStringFileInfo('..\..\bin\BareCam.exe', 'ProductVersion')
#define AppFileVersion GetStringFileInfo('..\..\bin\BareCam.exe', 'FileVersion')
#define AppCompany     GetStringFileInfo('..\..\bin\BareCam.exe', 'CompanyName')
#define AppCopyright   GetStringFileInfo('..\..\bin\BareCam.exe', 'LegalCopyright')
#define AppBase        LowerCase(StringChange(AppName, ' ', ''))
#define AppVersion3    Copy(AppVersion, 1, RPos('.', AppVersion) - 1)
#define AppSetupFile   AppBase + '-' + AppVersion3


[Setup]
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppCompany}
AppPublisherURL=https://www.medo64.com/{#AppBase}/
AppCopyright={#AppCopyright}
VersionInfoProductVersion={#AppVersion}
VersionInfoProductTextVersion={#AppVersion3}
VersionInfoVersion={#AppFileVersion}
DefaultDirName={autopf}\{#AppCompany}\{#AppName}
OutputBaseFilename={#AppSetupFile}
SourceDir=..\..\bin
OutputDir=..\dist
AppId=Medo64_BareCam
CloseApplications="yes"
RestartApplications="no"
AppMutex=Medo64_BareCam
UninstallDisplayIcon={app}\BareCam.exe
AlwaysShowComponentsList=no
ArchitecturesInstallIn64BitMode=x64
DisableProgramGroupPage=yes
MergeDuplicateFiles=yes
MinVersion=0,6.0
PrivilegesRequired=lowest
ShowLanguageDialog=no
SolidCompression=yes
ChangesAssociations=no
DisableWelcomePage=yes
LicenseFile=../package/win/License.rtf


[Messages]
SetupAppTitle=Setup {#AppName} {#AppVersion3}
SetupWindowTitle=Setup {#AppName} {#AppVersion3}
BeveledLabel=medo64.com

[Dirs]
Name: "{userappdata}\Medo64\BareCam";  Flags: uninsalwaysuninstall

[Files]
Source: "BareCam.exe";                              DestDir: "{app}";  Flags: ignoreversion;
Source: "libgcc_s_seh-1.dll";                       DestDir: "{app}";  Flags: ignoreversion;
Source: "libstdc++-6.dll";                          DestDir: "{app}";  Flags: ignoreversion;
Source: "libstdc++-6.dll";                          DestDir: "{app}";  Flags: ignoreversion;
Source: "libwinpthread-1.dll";                      DestDir: "{app}";  Flags: ignoreversion;
Source: "Qt5Core.dll";                              DestDir: "{app}";  Flags: ignoreversion;
Source: "Qt5Gui.dll";                               DestDir: "{app}";  Flags: ignoreversion;
Source: "Qt5Multimedia.dll";                        DestDir: "{app}";  Flags: ignoreversion;
Source: "Qt5MultimediaWidgets.dll";                 DestDir: "{app}";  Flags: ignoreversion;
Source: "Qt5Network.dll";                           DestDir: "{app}";  Flags: ignoreversion;
Source: "Qt5Widgets.dll";                           DestDir: "{app}";  Flags: ignoreversion;
Source: "libcrypto-1_1-x64.dll";                    DestDir: "{app}";  Flags: ignoreversion;
Source: "libssl-1_1-x64.dll";                       DestDir: "{app}";  Flags: ignoreversion;
Source: "mediaservice/dsengine.dll";                DestDir: "{app}/mediaservice";  Flags: ignoreversion;
Source: "mediaservice/qtmedia_audioengine.dll";     DestDir: "{app}/mediaservice";  Flags: ignoreversion;
Source: "platforms/qwindows.dll";                   DestDir: "{app}/platforms";  Flags: ignoreversion;
Source: "styles/qwindowsvistastyle.dll";            DestDir: "{app}/styles";     Flags: ignoreversion;
Source: "..\README.md";   DestName: "README.txt";   DestDir: "{app}";  Flags: overwritereadonly uninsremovereadonly;  Attribs: readonly;
Source: "..\LICENSE.md";  DestName: "LICENSE.txt";  DestDir: "{app}";  Flags: overwritereadonly uninsremovereadonly;  Attribs: readonly;

[Icons]
Name: "{userstartmenu}\BareCam";  Filename: "{app}\BareCam.exe"

[Run]
Description: "Launch application now";  Filename: "{app}\BareCam.exe";  Parameters: "/setup";  Flags: postinstall nowait skipifsilent runasoriginaluser shellexec
Description: "View ReadMe.txt";         Filename: "{app}\ReadMe.txt";                          Flags: postinstall nowait skipifsilent runasoriginaluser shellexec unchecked


[Code]

procedure InitializeWizard;
begin
  WizardForm.LicenseAcceptedRadio.Checked := True;
end;


function PrepareToInstall(var NeedsRestart: Boolean): String;
var
    ResultCode: Integer;
begin
    Exec(ExpandConstant('{sys}\taskkill.exe'), '/f /im barecam.exe', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
    Result := Result;
end;
