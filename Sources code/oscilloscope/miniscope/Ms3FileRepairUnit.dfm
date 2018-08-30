object frmMs3FileRepair: TfrmMs3FileRepair
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'MS3 file repair'
  ClientHeight = 115
  ClientWidth = 337
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lblInfo: TLabel
    Left = 8
    Top = 27
    Width = 312
    Height = 13
    Caption = 'New file will be created. Make sure you have enough HDD space.'
  end
  object lblErrorType: TLabel
    Left = 8
    Top = 8
    Width = 53
    Height = 13
    Caption = 'Error type:'
  end
  object lblErrorTypeVal: TLabel
    Left = 80
    Top = 8
    Width = 15
    Height = 13
    Caption = '???'
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 74
    Width = 337
    Height = 41
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object btnClose: TButton
      Left = 229
      Top = 8
      Width = 101
      Height = 25
      Caption = 'Close'
      TabOrder = 0
      OnClick = btnCloseClick
    end
  end
  object ProgressBar: TProgressBar
    Left = 8
    Top = 46
    Width = 215
    Height = 25
    TabOrder = 1
  end
  object btnCreateFixedFile: TButton
    Left = 229
    Top = 46
    Width = 101
    Height = 25
    Caption = 'Create fixed file'
    TabOrder = 2
    OnClick = btnCreateFixedFileClick
  end
  object SaveDialog: TSaveDialog
    Left = 304
    Top = 8
  end
end
