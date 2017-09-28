object frmMain: TfrmMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Anti-WebMiner'
  ClientHeight = 473
  ClientWidth = 719
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object timUpdate: TTimer
    Enabled = False
    Interval = 100
    OnTimer = timUpdateTimer
    Left = 376
    Top = 168
  end
  object pmnLang: TPopupMenu
    Left = 432
    Top = 304
    object constHostsfilepathfoundpathFatalError: TMenuItem
      Caption = 'Hosts file path foundpath! Fatal Error!'
    end
    object constHosstCannotRead: TMenuItem
      Caption = 'Hosts file cannot be read!'
    end
    object constSystemError: TMenuItem
      Caption = 'System Error'
    end
  end
  object MainMenu: TMainMenu
    Left = 280
    Top = 176
    object F1: TMenuItem
      Caption = 'File'
      object mnCheckupdate: TMenuItem
        Caption = 'Check for update'
        OnClick = mnCheckupdateClick
      end
      object mnDisplayLog: TMenuItem
        Caption = 'Display Log File...'
        OnClick = mnDisplayLogClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object mnUninstall: TMenuItem
        Caption = 'Uninstall from Hosts'
        OnClick = mnUninstallClick
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object mnExit: TMenuItem
        Caption = 'Exit'
        OnClick = mnExitClick
      end
    end
    object V1: TMenuItem
      Caption = 'View'
      object mnRefresh: TMenuItem
        Caption = 'Refresh'
        OnClick = mnRefreshClick
      end
    end
    object H1: TMenuItem
      Caption = 'Help'
      object mnVisitHomePage: TMenuItem
        Caption = 'Visit Home Page...'
        OnClick = mnVisitHomePageClick
      end
    end
  end
end
