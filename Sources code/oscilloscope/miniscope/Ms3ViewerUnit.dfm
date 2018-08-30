object frmMs3Viewer: TfrmMs3Viewer
  Left = 0
  Top = 0
  Align = alClient
  BorderStyle = bsNone
  Caption = 'frmMs3Viewer'
  ClientHeight = 370
  ClientWidth = 136
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  ShowHint = True
  OnClose = FormClose
  DesignSize = (
    136
    370)
  PixelsPerInch = 96
  TextHeight = 13
  object lblSampling: TLabel
    Left = 8
    Top = 32
    Width = 42
    Height = 13
    Caption = 'Sampling'
  end
  object lblSamplingVal: TLabel
    Left = 117
    Top = 48
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
  end
  object lblTriggerType: TLabel
    Left = 8
    Top = 64
    Width = 59
    Height = 13
    Caption = 'Trigger type'
  end
  object lblCouplingType: TLabel
    Left = 8
    Top = 96
    Width = 41
    Height = 13
    Caption = 'Coupling'
  end
  object lblTimestamp: TLabel
    Left = 8
    Top = 128
    Width = 51
    Height = 13
    Caption = 'Timestamp'
  end
  object lblTriggerTypeVal: TLabel
    Left = 117
    Top = 80
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
  end
  object lblCouplingTypeVal: TLabel
    Left = 117
    Top = 112
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
  end
  object lblTimestampVal: TLabel
    Left = 117
    Top = 144
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
  end
  object lblSamplesCount: TLabel
    Left = 8
    Top = 160
    Width = 69
    Height = 13
    Caption = 'Samples count'
  end
  object lblSamplesCountVal: TLabel
    Left = 117
    Top = 176
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
  end
  object lblFrame: TLabel
    Left = 8
    Top = 192
    Width = 30
    Height = 13
    Caption = 'Frame'
  end
  object lblFrameVal: TLabel
    Left = 117
    Top = 208
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 329
    Width = 136
    Height = 41
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object btnExitViewer: TButton
      Left = 8
      Top = 8
      Width = 120
      Height = 25
      Align = alCustom
      Caption = 'Exit MS3 Viewer'
      TabOrder = 0
      OnClick = btnExitViewerClick
    end
  end
  object edFilename: TEdit
    Left = 8
    Top = 8
    Width = 120
    Height = 18
    TabStop = False
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 0
    Text = '< file name >'
  end
  object btnPrevFrame: TButton
    Left = 8
    Top = 232
    Width = 33
    Height = 25
    Hint = 'Previous frame'
    Caption = '<--'
    TabOrder = 2
    OnClick = btnPrevFrameClick
  end
  object btnNextFrame: TButton
    Left = 95
    Top = 232
    Width = 33
    Height = 25
    Hint = 'Next frame'
    Caption = '-->'
    TabOrder = 4
    OnClick = btnNextFrameClick
  end
  object btnFrameList: TBitBtn
    Left = 47
    Top = 232
    Width = 42
    Height = 25
    Hint = 'List of data frames'
    TabOrder = 3
    OnClick = btnFrameListClick
    Glyph.Data = {
      7E000000424D7E000000000000003E0000002800000010000000100000000100
      010000000000400000000000000000000000020000000000000000000000FFFF
      FF00FFFF0000C0030000FFFF0000FFFF0000C0030000FFFF0000FFFF0000C003
      0000FFFF0000FFFF0000C0030000FFFF0000FFFF0000C0030000FFFF0000FFFF
      0000}
  end
end
