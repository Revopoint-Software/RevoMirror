//core setup script

var
  m_isReleased, m_isInitialized, m_isProductInstalledBefore, m_shouldStartupOnSetupFinished : boolean;
  m_labelInstallProgress, m_labelProductAlreadyInstalled, m_labelLicenseDesc, m_labelLicense, m_labelSetupSettings, m_labelSetupPath, m_labelAnd, m_labelPrivacyPolicy: TLabel;
  m_editTargetPath : TEdit;
  m_btnMinimize, m_btnExit, m_btnInstall, m_btnBrowse, m_btnCustomizeSetup, m_btnUncustomizeSetup, m_checkboxStartup , m_checkboxLicense : hwnd;
  m_imageCustomizeBackground, m_imageFormBackground, m_progressbarBackground, m_progressbarForeground, PBOldProc : longint;


const
  WM_SYSCOMMAND = $0112;
  ID_BUTTON_ON_CLICK_EVENT = 1;
  WIZARDFORM_WIDTH_NORMAL = 600;
  WIZARDFORM_HEIGHT_NORMAL = 503;
  WIZARDFORM_HEIGHT_MORE = 570;
  
type
  TBtnEventProc = procedure(h : hwnd);
  TPBProc = function(h : hWnd; Msg, wParam, lParam : longint) : longint;
  
function  ImgLoad(h : hwnd; FileName : PAnsiChar; Left, Top, Width, Height : integer; Stretch, IsBkg : boolean) : longint; external 'ImgLoad@files:botva2.dll STDCALL DELAYLOAD';
procedure ImgSetVisibility(img : longint; Visible : boolean); external 'ImgSetVisibility@files:botva2.dll STDCALL DELAYLOAD';
procedure ImgApplyChanges(h : hwnd); external 'ImgApplyChanges@files:botva2.dll STDCALL DELAYLOAD';
procedure ImgSetPosition(img : longint; NewLeft, NewTop, NewWidth, NewHeight : integer); external 'ImgSetPosition@files:botva2.dll STDCALL DELAYLOAD';
procedure ImgRelease(img : longint); external 'ImgRelease@files:botva2.dll STDCALL DELAYLOAD';
procedure CreateFormFromImage(h : hwnd; FileName : PAnsiChar); external 'CreateFormFromImage@files:botva2.dll STDCALL DELAYLOAD';
procedure gdipShutdown();  external 'gdipShutdown@files:botva2.dll STDCALL DELAYLOAD';
function  WrapBtnCallback(Callback : TBtnEventProc; ParamCount : integer) : longword; external 'wrapcallback@files:innocallback.dll STDCALL DELAYLOAD';
function  BtnCreate(hParent : hwnd; Left, Top, Width, Height : integer; FileName : PAnsiChar; ShadowWidth : integer; IsCheckBtn : boolean) : hwnd;  external 'BtnCreate@files:botva2.dll STDCALL DELAYLOAD';
procedure BtnSetVisibility(h : hwnd; Value : boolean); external 'BtnSetVisibility@files:botva2.dll STDCALL DELAYLOAD';
procedure BtnSetEvent(h : hwnd; EventID : integer; Event : longword); external 'BtnSetEvent@files:botva2.dll STDCALL DELAYLOAD';
procedure BtnSetEnabled(h : hwnd; Value : boolean); external 'BtnSetEnabled@files:botva2.dll STDCALL DELAYLOAD';
function  BtnGetChecked(h : hwnd) : boolean; external 'BtnGetChecked@files:botva2.dll STDCALL DELAYLOAD';
procedure BtnSetChecked(h : hwnd; Value : boolean); external 'BtnSetChecked@files:botva2.dll STDCALL DELAYLOAD';
procedure BtnSetPosition(h : hwnd; NewLeft, NewTop, NewWidth, NewHeight : integer);  external 'BtnSetPosition@files:botva2.dll STDCALL DELAYLOAD';
function  SetWindowLong(h : HWnd; Index : integer; NewLong : longint) : longint; external 'SetWindowLongA@user32.dll STDCALL';
function  PBCallBack(P : TPBProc; ParamCount : integer) : longword; external 'wrapcallback@files:innocallback.dll STDCALL DELAYLOAD';
function  CallWindowProc(lpPrevWndFunc : longint; h : hwnd; Msg : uint; wParam, lParam : longint) : longint; external 'CallWindowProcA@user32.dll STDCALL';
procedure ImgSetVisiblePart(img : longint; NewLeft, NewTop, NewWidth, NewHeight : integer); external 'ImgSetVisiblePart@files:botva2.dll STDCALL DELAYLOAD';
function  ReleaseCapture() : longint; external 'ReleaseCapture@user32.dll STDCALL';
function  CreateRoundRectRgn(p1, p2, p3, p4, p5, p6 : integer) : THandle; external 'CreateRoundRectRgn@gdi32.dll STDCALL';
function  SetWindowRgn(h : hwnd; hRgn : THandle; bRedraw : boolean) : integer; external 'SetWindowRgn@user32.dll STDCALL';


function IsEnoughFreeSpace(const Path: string; MinSpace: Cardinal): Boolean;
var
  FreeSpace, TotalSpace: Cardinal;
begin
  // the second parameter set to True means that the function operates with
  // megabyte units; if you set it to False, it will operate with bytes; by
  // the chosen units you must reflect the value of the MinSpace paremeter
  if GetSpaceOnDisk(Path, True, FreeSpace, TotalSpace) then
    Result := FreeSpace >= MinSpace
  else
    RaiseException('Failed to check free space.');
end;

procedure MyBtnSetEvent(h : hwnd; Callback : TBtnEventProc);
begin
	BtnSetEvent(h, 1, WrapBtnCallback(Callback, 1));
end;

function ProgressbarProcedure(h : hWnd; Msg, wParam, lParam : longint) : longint;
var
  percent, value, range : EXTendED;
  barWidth : integer;
begin
  Result := CallWindowProc(PBOldProc, h, Msg, wParam, lParam);
  if ((Msg = $402) and (WizardForm.ProgressGauge.Position > WizardForm.ProgressGauge.Min)) then
  begin
    value := WizardForm.ProgressGauge.Position - WizardForm.ProgressGauge.Min;
    range := WizardForm.ProgressGauge.Max - WizardForm.ProgressGauge.Min;
    percent := (value * 100) / range;
    m_labelInstallProgress.Caption := Format('%d', [Round(percent)]) + '%';
    barWidth := Round((560 * percent) / 100);
    ImgSetPosition(m_progressbarForeground, ScaleX(20), ScaleY(374), ScaleX(barWidth), ScaleY(6));
    //这个设置函数，数值反而不需要根据DPI进行缩放，用原始值。否则进度条会显示出错
    ImgSetVisiblePart(m_progressbarForeground, 0, 0, barWidth, 6);
    ImgApplyChanges(WizardForm.Handle);
  end;
end;

procedure on_mouseDown(Sender : TObject; Button : TMouseButton; Shift : TShiftState; X, Y : integer);
begin
  ReleaseCapture();
  SendMessage(WizardForm.Handle, WM_SYSCOMMAND, $F012, 0);
end;

procedure on_editTargetPath_changed(Sender : TObject);
begin
  WizardForm.DirEdit.Text := m_editTargetPath.Text;
end;

//响应查看用户协议详情
procedure on_urlLabelOnClick(Sender : TObject);
var
  ErrorCode : integer;
begin
  //MsgBox(ExpandConstant(ACtiveLanguage), mbCriticalError, MB_OK);
  if ( ExpandConstant(ACtiveLanguage) = 'chinesesimplified') then
    begin
      if (Sender = m_labelLicense) then
        ShellExec('open', 'https://www.revopoint3d.com.cn/index.php?c=category&id=114', '', '', SW_SHOW, ewNoWait, ErrorCode);
      if (Sender = m_labelPrivacyPolicy) then
        ShellExec('open', 'https://www.revopoint3d.com.cn/index.php?c=category&id=113', '', '', SW_SHOW, ewNoWait, ErrorCode);
    end else
    begin
      if (Sender = m_labelLicense) then
        ShellExec('open', 'https://www.revopoint3d.com/pages/revo-mirror-user-agreement', '', '', SW_SHOW, ewNoWait, ErrorCode);
      if (Sender = m_labelPrivacyPolicy) then
        ShellExec('open', 'https://www.revopoint3d.com/pages/revo-mirror-privacy-policy', '', '', SW_SHOW, ewNoWait, ErrorCode);
    end


end;

procedure on_btnExit_clicked(hBtn : hwnd);
begin
  WizardForm.CancelButton.OnClick(WizardForm);
end;

procedure on_btnMinimize_clicked(hBtn : hwnd);
begin
  SendMessage(WizardForm.Handle, WM_SYSCOMMAND, 61472, 0);
end;

procedure on_btnInstall_clicked(hBtn : hwnd);
begin

  if not IsEnoughFreeSpace(ExtractFileDrive(WizardDirValue), 1024) then
      begin
        MsgBox(ExpandConstant('{cm:RS5DiskSpaceWarning}'), mbCriticalError, MB_OK);
        // in this input parameter you can pass your own exit code which can have
        // some meaningful value indicating that the setup process exited because
        // of the not enough space reason
        //ExitProcess(666);
      end else
      begin
        WizardForm.NextButton.OnClick(WizardForm);
      end
  
end;

procedure on_btnBrowse_clicked(hBtn : hwnd);
begin
  WizardForm.DirBrowseButton.OnClick(WizardForm);
  m_editTargetPath.Text := WizardForm.DirEdit.Text;
end;

procedure on_btnCustomizeSetup_clicked(hBtn : hwnd);
begin
  if m_editTargetPath.Visible then
  begin
    m_editTargetPath.Hide();
    m_labelProductAlreadyInstalled.Hide();
    BtnSetVisibility(m_btnBrowse, false);
    WizardForm.Height := ScaleY(WIZARDFORM_HEIGHT_NORMAL);
    ImgSetVisibility(m_imageCustomizeBackground, false);
    ImgSetVisibility(m_imageFormBackground, true);
    BtnSetVisibility(m_btnCustomizeSetup, true);
    BtnSetVisibility(m_btnUncustomizeSetup, false);
  end else
  begin
    WizardForm.Height := ScaleY(WIZARDFORM_HEIGHT_MORE);
    ImgSetVisibility(m_imageCustomizeBackground, true);
    ImgSetVisibility(m_imageFormBackground, false);
    m_editTargetPath.Show();
    BtnSetVisibility(m_btnBrowse, true);
    BtnSetVisibility(m_btnCustomizeSetup, false);
    BtnSetVisibility(m_btnUncustomizeSetup, true);
    
    if m_isProductInstalledBefore then
    begin
      m_editTargetPath.Enabled := false;
      BtnSetEnabled(m_btnBrowse, false);
      m_labelProductAlreadyInstalled.Show();
    end;
  end;
  ImgApplyChanges(WizardForm.Handle);
end;

procedure on_checkboxStartup_clicked(hBtn : hwnd);
begin
  m_shouldStartupOnSetupFinished := BtnGetChecked(m_checkboxStartup);
end;

//解压安装程序运行需要的临时文件
procedure extractTempFiles();
begin
  ExtractTemporaryFile('button_customize_setup.png');
  ExtractTemporaryFile('button_uncustomize_setup.png');
  ExtractTemporaryFile(ExpandConstant('button_finish{cm:RS5SetupImageSuffix}.png'));
  ExtractTemporaryFile(ExpandConstant('button_install{cm:RS5SetupImageSuffix}.png'));
  ExtractTemporaryFile('background_welcome.png');
  ExtractTemporaryFile('background_welcome_more.png');
  ExtractTemporaryFile('button_browse.png');
  ExtractTemporaryFile('progressbar_background.png');
  ExtractTemporaryFile('progressbar_foreground.png');
  ExtractTemporaryFile('background_installing.png');
  ExtractTemporaryFile('background_finish.png');
  ExtractTemporaryFile('button_close.png');
  ExtractTemporaryFile('button_minimize.png');
  ExtractTemporaryFile('checkbox_startup.png');
  ExtractTemporaryFile('icon_install_check.png');
  

  ExtractTemporaryFile(ExpandConstant('License{cm:RS5SetupImageSuffix}.txt'));
end;

//优化为 对Process name 进行检查。（通过cmd从进程中匹配exe）
function IsAppRunning(const ProcessName : string) : Boolean;
var
  ResultCode: Integer;
  ErrorCode: Integer;
  OutputLines: TArrayOfString;
  I: Integer;
begin
  Result := false;
  if Exec('cmd.exe','/C tasklist > C:\Windows\System32\outputProcess.txt','',SW_HIDE,ewWaitUntilTerminated,ResultCode) then
  begin
    if LoadStringsFromFile(ExpandConstant('C:\Windows\System32\outputProcess.txt'),OutputLines) then
    begin
      for I := 0 to GetArrayLength(OutputLines) - 1 do
      begin
        if Pos(ProcessName,OutputLines[I]) > 0 then
        begin
          Result := true;
          break;
        end;
      end;
    end;
  end;
  DeleteFile(ExpandConstant('C:\Windows\System32\outputProcess.txt'));
  //Result := (FindWindowByWindowName(AppName) <> 0);
end;

function IsAppInstalled() : Boolean;
begin
  Result := RegKeyExists(HKLM64, 'SOFTWARE\{#Define_AppPublisher}\{#Define_AppName}');
end;
// 勾选查看 license
procedure checkboxLicenseClick(hBtn:HWND);
  begin
    if BtnGetChecked(m_checkboxLicense)=true then
    begin
      BtnSetEnabled(m_btnInstall,true)
    end else
    begin
      BtnSetEnabled(m_btnInstall,false)
    end
  end;

function InitializeSetup(): Boolean;
begin
  m_isProductInstalledBefore := false;
  Result := true;
  if IsAppRunning('{#Define_MainExeName}') then
  begin
    m_isProductInstalledBefore := TRUE;
    Msgbox(ExpandConstant('{cm:RS5AppRunningInstallTips}'), mbInformation, MB_OK);
    Result := false;
  end;
  if Result and IsAppInstalled() then
  begin
    m_isProductInstalledBefore := TRUE;
    Result := (Msgbox(ExpandConstant('{cm:RS5AppReinstallTips}'), mbConfirmation, MB_YESNO) = IDYES);
  end;
end;

function InitializeUninstall(): Boolean;
begin
  Result:= true;
  if  IsAppRunning('{#Define_MainExeName}') then
  begin
    MsgBox(ExpandConstant('{cm:RS5AppRunningInstallTips}'), mbInformation, MB_OK)
    Result:= false;
  end;
end;

procedure InitializeWizard();
var
  windowTitle,labelToMove : TLabel;
begin
  m_isInitialized := true;
  m_isReleased := false;
  //do NOT execute the program by default
  m_shouldStartupOnSetupFinished := false;

  extractTempFiles();
  WizardForm.InnerNotebook.Hide();
  WizardForm.OuterNotebook.Hide();
  WizardForm.Bevel.Hide();
  with WizardForm do
  begin
    BorderStyle := bsNone;
    Width := ScaleX(WIZARDFORM_WIDTH_NORMAL);
    Height := ScaleY(WIZARDFORM_HEIGHT_NORMAL);
    Position := poDesktopCenter;
    Color := clWhite;
    NextButton.Width := 0;
    CancelButton.Width := 0;
    BackButton.Visible := false;
  end;
  windowTitle := TLabel.Create(WizardForm);
  with windowTitle do
  begin
    Parent := WizardForm;
    AutoSize := false;
    Left := ScaleX(10);
    Top := ScaleY(5);
    Width := ScaleX(400);
    Height := ScaleY(20);
    Font.Name := 'Microsoft YaHei';
    Font.Size := 9;
    Font.Color := clGray;
    Caption := ExpandConstant('{cm:RS5AppSetupTitle}');
    Transparent := true;
    OnMouseDown := @on_mouseDown;
  end;
  labelToMove := TLabel.Create(WizardForm);
  with labelToMove do
  begin
    Parent := WizardForm;
    AutoSize := false;
    Left := ScaleX(0);
    Top := ScaleY(0);
    Width := WizardForm.Width;
    Height := WizardForm.Height;
    Caption := '';
    Transparent := true;
    OnMouseDown := @on_mouseDown;
  end;
  
  m_btnExit := BtnCreate(WizardForm.Handle, ScaleX(570), ScaleY(0), ScaleX(30), ScaleY(30), ExpandConstant('{tmp}\button_close.png'), 0, false);
  MyBtnSetEvent(m_btnExit, @on_btnExit_clicked);
  
  m_btnMinimize := BtnCreate(WizardForm.Handle, ScaleX(540), ScaleY(0), ScaleX(30), ScaleY(30), ExpandConstant('{tmp}\button_minimize.png'), 0, false);
  MyBtnSetEvent(m_btnMinimize, @on_btnMinimize_clicked);
  
  ImgApplyChanges(WizardForm.Handle);
end;

procedure CurPageChanged(CurPageID : integer);
begin
  if (CurPageID = wpWelcome) then
  begin
    //MsgBox(ExpandConstant('{tmp}\button_install{cm:RS5SetupImageSuffix}.png'), mbInformation, MB_OK)
    m_btnInstall := BtnCreate(WizardForm.Handle, ScaleX(174), ScaleY(334), ScaleX(253), ScaleY(48), ExpandConstant('{tmp}\button_install{cm:RS5SetupImageSuffix}.png'), 0, false);
    MyBtnSetEvent(m_btnInstall, @on_btnInstall_clicked);

        // license 提示
    //m_checkboxLicense:=BtnCreate(WizardForm.Handle,220,400,16,16,ExpandConstant('{tmp}\checkbox_startup.png'),1,true)
    m_checkboxLicense := BtnCreate(WizardForm.Handle, ScaleX(130), ScaleY(410), ScaleX(16), ScaleY(16), ExpandConstant('{tmp}\icon_install_check.png'), 0, True);
    MyBtnSetEvent(m_checkboxLicense, @checkboxLicenseClick);
         // 默认不勾选 阅读并同意
    BtnSetChecked(m_checkboxLicense,false)
	BtnSetEnabled(m_btnInstall,false)
    //User License Description
    m_labelLicenseDesc := TLabel.Create(WizardForm);
    with m_labelLicenseDesc do
    begin
      Parent := WizardForm;
      Left := ScaleX(155);
      Top := ScaleY(410);
      Height := ScaleY(40);
      Font.Name := 'Microsoft YaHei';
      Font.Size := 10;
      Font.Color := clWhite;
      Caption := ExpandConstant('{cm:RS5LicenseDescription}');
      Transparent := true;
      AutoSize := true;
    end;

    if ( ExpandConstant(ACtiveLanguage) = 'english') then
    begin
      BtnSetPosition(m_checkboxLicense,  ScaleX(80), ScaleY(410), ScaleX(16), ScaleY(16));
      m_labelLicenseDesc.Left := ScaleX(110);
    end;

    //User License URL 
    m_labelLicense := TLabel.Create(WizardForm);
    with m_labelLicense do
    begin
      Parent := WizardForm;
      Left := m_labelLicenseDesc.Left + m_labelLicenseDesc.Width + ScaleY(10);
      Top := m_labelLicenseDesc.Top;
      Height := ScaleY(40);
      Font.Name := 'Microsoft YaHei';
      Font.Size := 10;
      Font.Color := 16756480;
      Font.Style := Font.Style + [fsUnderline];
      Caption := ExpandConstant('{cm:RS5UserLicenseDetail}');
      Transparent := true;
      //鼠标移上去时显示为手形
      Cursor := crHand;    
      OnClick := @on_urlLabelOnClick;//绑定点击事件
      AutoSize := true;
    end;


    //和这个字 
    m_labelAnd := TLabel.Create(WizardForm);
    with m_labelAnd do
    begin
      Parent := WizardForm;
      Left := m_labelLicense.Left + m_labelLicense.Width + ScaleY(10);
      Top := m_labelLicense.Top;
      Height := ScaleY(40);
      Font.Name := 'Microsoft YaHei';
      Font.Size := 10;
      Font.Color := clWhite;
      Caption := ExpandConstant('{cm:RS5Add}');
      Transparent := true;
      AutoSize := true;
    end;

      //隐私协议 
    m_labelPrivacyPolicy := TLabel.Create(WizardForm);
    with m_labelPrivacyPolicy do
    begin
      Parent := WizardForm;
      AutoSize := false;
      Left := m_labelAnd.Left + m_labelAnd.Width + ScaleY(10);
      Top := m_labelAnd.Top;
      Height := ScaleY(40);
      Font.Name := 'Microsoft YaHei';
      Font.Size := 10;
      Font.Color := 16756480;
      Font.Style := Font.Style + [fsUnderline];  
      Caption := ExpandConstant('{cm:RS5PrivacyPolicy}');
      Transparent := true;
      //鼠标移上去时显示为手形
      Cursor := crHand;    
      OnClick := @on_urlLabelOnClick;//绑定点击事件
      AutoSize := true;
    end;



    //Setup settings Label
    m_labelSetupSettings := TLabel.Create(WizardForm);
    with m_labelSetupSettings do
    begin
      Parent := WizardForm;
      AutoSize := false;
      Left := ScaleX(400);
      Top := ScaleY(464);
      Width := ScaleX(157);
      Height := ScaleY(24);
      WordWrap := false;
      Font.Name := 'Microsoft YaHei';
      Font.Size := 10;
      Font.Color := clWhite;
      //Font.Style := Font.Style + [fsBold];
      Caption := ExpandConstant('{cm:RS5SetupSettings}');
      Alignment := taRightJustify;
      Transparent := true;
    end;


    m_btnCustomizeSetup := BtnCreate(WizardForm.Handle, ScaleX(400), ScaleY(464), ScaleX(180), ScaleY(24), ExpandConstant('{tmp}\button_customize_setup.png'), 0, false);
    MyBtnSetEvent(m_btnCustomizeSetup, @on_btnCustomizeSetup_clicked);
    
    m_btnUncustomizeSetup := BtnCreate(WizardForm.Handle, ScaleX(400), ScaleY(464), ScaleX(180), ScaleY(24), ExpandConstant('{tmp}\button_uncustomize_setup.png'), 0, false);
    MyBtnSetEvent(m_btnUncustomizeSetup, @on_btnCustomizeSetup_clicked);
    BtnSetVisibility(m_btnUncustomizeSetup, false);

    //Setup Path Label
    m_labelSetupPath := TLabel.Create(WizardForm);
    with m_labelSetupPath do
    begin
      Parent := WizardForm;
      AutoSize := false;
      Left := ScaleX(22);
      Top := ScaleY(512);
      Width := ScaleX(120);
      Height := ScaleY(20);
      WordWrap := false;
      Font.Name := 'Microsoft YaHei';
      Font.Size := 9;
      Font.Color := clWhite;
      Caption := ExpandConstant('{cm:RS5SetupPath}');
      Alignment := taRightJustify;
      Transparent := true;
    end;
    
    m_editTargetPath:= TEdit.Create(WizardForm);
    with m_editTargetPath do
    begin
      Parent := WizardForm;
      Text := WizardForm.DirEdit.Text;
      Font.Name := 'Microsoft YaHei';
      Font.Size := 9;
      Font.Color := clWhite;
      BorderStyle := bsNone;
      SetBounds(ScaleX(152),ScaleY(512),ScaleX(362),ScaleY(20));
      OnChange := @on_editTargetPath_changed;
      //InnoColorValue=rValue+gValue<<8+bValue<<16
      Color := 4473924;
      TabStop := false;
    end;
    m_editTargetPath.Hide();
    
    m_labelProductAlreadyInstalled := TLabel.Create(WizardForm);
    with m_labelProductAlreadyInstalled do
    begin
      Parent := WizardForm;
      AutoSize := false;
      Left := ScaleX(30);
      Top := m_editTargetPath.Top + m_editTargetPath.ClientHeight + ScaleY(10);
      Width := ScaleX(550);
      Height := ScaleY(30);
      WordWrap := true;
      Alignment:= taCenter;
      Font.Name := 'Microsoft YaHei';
      Font.Size := 9;
      Font.Color := clGray;
      Caption := ExpandConstant('{cm:RS5DirChangeEnableTips}');
      Transparent := true;
    end;
    m_labelProductAlreadyInstalled.Hide();
	
    PBOldProc := SetWindowLong(WizardForm.ProgressGauge.Handle, -4, PBCallBack(@ProgressbarProcedure, 4));
    
    m_btnBrowse := BtnCreate(WizardForm.Handle, ScaleX(516), ScaleY(512), ScaleX(20), ScaleY(20), ExpandConstant('{tmp}\button_browse.png'), 0, false);
    MyBtnSetEvent(m_btnBrowse, @on_btnBrowse_clicked);
    BtnSetVisibility(m_btnBrowse, false);
    
    m_imageFormBackground := ImgLoad(WizardForm.Handle, ExpandConstant('{tmp}\background_welcome.png'), ScaleX(0), ScaleY(0), ScaleX(WIZARDFORM_WIDTH_NORMAL), ScaleY(WIZARDFORM_HEIGHT_NORMAL), true, true);
    m_imageCustomizeBackground := ImgLoad(WizardForm.Handle, ExpandConstant('{tmp}\background_welcome_more.png'), ScaleX(0), ScaleY(0), ScaleX(WIZARDFORM_WIDTH_NORMAL), ScaleY(WIZARDFORM_HEIGHT_MORE), true, true);
    ImgSetVisibility(m_imageCustomizeBackground, false);
    WizardForm.Width := ScaleX(WIZARDFORM_WIDTH_NORMAL);
    WizardForm.Height := ScaleY(WIZARDFORM_HEIGHT_NORMAL);
    ImgApplyChanges(WizardForm.Handle);
  end;


  if (CurPageID = wpInstalling) then
  begin
    //Hide something
    BtnSetVisibility(m_checkboxLicense, False);
    m_labelAnd.Hide();
    m_labelPrivacyPolicy.Hide();
    m_labelLicenseDesc.Hide();
    m_labelLicense.Hide();

    m_labelSetupPath.Hide();
    m_editTargetPath.Hide();
    BtnSetVisibility(m_btnBrowse, false);

    m_labelSetupSettings.Hide();
    BtnSetVisibility(m_btnCustomizeSetup, false);
    BtnSetVisibility(m_btnUncustomizeSetup, false);

    m_labelProductAlreadyInstalled.Hide();


    WizardForm.Height := ScaleY(WIZARDFORM_HEIGHT_NORMAL);
    
    m_labelInstallProgress := TLabel.Create(WizardForm);
    with m_labelInstallProgress do
    begin
      Parent := WizardForm;
      AutoSize := false;
      Left := ScaleX(532);
      Top := ScaleY(345);
      Width := ScaleX(50);
      Height := ScaleY(30);
      Font.Name := 'Microsoft YaHei';
      Font.Size := 14;
      Font.Color := clWhite;
      Caption := '';
      Transparent := true;
      Alignment := taRightJustify;
      OnMouseDown := @on_mouseDown;
    end;
    BtnSetEnabled(m_btnExit, false);
    m_imageFormBackground := ImgLoad(WizardForm.Handle, ExpandConstant('{tmp}\background_installing.png'), ScaleX(0), ScaleY(0), ScaleX(WIZARDFORM_WIDTH_NORMAL), ScaleY(WIZARDFORM_HEIGHT_NORMAL), true, true);
    m_progressbarBackground := ImgLoad(WizardForm.Handle, ExpandConstant('{tmp}\progressbar_background.png'), ScaleX(20), ScaleY(374), ScaleX(560), ScaleY(6), true, true);
    m_progressbarForeground := ImgLoad(WizardForm.Handle, ExpandConstant('{tmp}\progressbar_foreground.png'), ScaleX(20), ScaleY(374), ScaleX(560), ScaleY(6), true, true);
    BtnSetVisibility(m_btnInstall, false);
    ImgApplyChanges(WizardForm.Handle);
  end;

  if (CurPageID = wpFinished) then
  begin
    ImgSetVisibility(m_imageFormBackground, false);
    ImgSetVisibility(m_progressbarBackground, false);
    ImgSetVisibility(m_progressbarForeground, false);
    m_labelInstallProgress.Hide();
    BtnSetEnabled(m_btnExit, true);


    //****Don't need to start the exe after isntallation 

    //m_checkboxStartup := BtnCreate(WizardForm.Handle, 248, 280, 110, 17, ExpandConstant('{tmp}\checkbox_startup.png'), 0, True);
    //MyBtnSetEvent(m_checkboxStartup, @on_checkboxStartup_clicked);
    //BtnSetChecked(m_checkboxStartup, True);
    
    m_btnInstall := BtnCreate(WizardForm.Handle, ScaleX(174), ScaleY(334), ScaleX(253), ScaleY(48), ExpandConstant('{tmp}\button_finish{cm:RS5SetupImageSuffix}.png'), 0, false);
    
    m_imageFormBackground := ImgLoad(WizardForm.Handle, ExpandConstant('{tmp}\background_finish.png'), ScaleX(0), ScaleY(0), ScaleX(WIZARDFORM_WIDTH_NORMAL), ScaleY(WIZARDFORM_HEIGHT_NORMAL), true, true);
    MyBtnSetEvent(m_btnInstall, @on_btnInstall_clicked);
    MyBtnSetEvent(m_btnExit, @on_btnInstall_clicked);
    
    ImgApplyChanges(m_btnInstall);
    ImgApplyChanges(WizardForm.Handle);
  end;
end;

function ShouldSkipPage(PageID : integer) : boolean;
begin
  if (PageID = wpLicense) then Result := true;
  if (PageID = wpPassword) then Result := true;
  if (PageID = wpInfoBefore) then Result := true;
  if (PageID = wpUserInfo) then Result := true;
  if (PageID = wpSelectDir) then Result := true;
  if (PageID = wpSelectComponents) then Result := true;
  if (PageID = wpSelectProgramGroup) then Result := true;
  if (PageID = wpSelectTasks) then Result := true;
  if (PageID = wpReady) then Result := true;
  if (PageID = wpPreparing) then Result := true;
  if (PageID = wpInfoAfter) then Result := true;
end;

procedure releaseInstallerAfterInit();
begin
  WizardForm.Release();
end;

procedure releaseInstaller();
begin
  gdipShutdown();
  releaseInstallerAfterInit
end;

procedure CancelButtonClick(CurPageID : integer; var Cancel, Confirm: boolean);
begin
  Cancel := false;
  Confirm := false;
  if MsgBox(ExpandConstant('{cm:RS5ExitSetupTips}'), mbInformation, MB_YESNO) = IDYES then
  begin
    releaseInstaller();
    Cancel := true;
    m_isReleased := true;
  end;
end;

procedure DeinitializeSetup();
begin
  if (m_isReleased = false) then
  begin
    gdipShutdown();
    if m_isInitialized then
    begin
      releaseInstallerAfterInit();
    end;
  end;
end;

procedure CurStepChanged(CurStep : TSetupStep);
var
  ErrorCode : integer;
begin
  if (CurStep = ssDone) then
  begin
    if m_shouldStartupOnSetupFinished then
    begin
      if not ShellExec('', ExpandConstant('{app}\{#Define_MainExeName}'), '', '', SW_SHOW, ewNoWait, ErrorCode) then
      begin
        MsgBox(ExpandConstant('{cm:RS5AppStartupFailedTips}'), mbError, MB_OK);
      end;
    end;
    m_isReleased := true;
    releaseInstaller();
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usDone then
  begin
    DelTree(ExpandConstant('{app}'), True, True, True);
  end;
end;

