object frmJsonViewer: TfrmJsonViewer
  Left = 0
  Top = 0
  Align = alClient
  BorderStyle = bsNone
  ClientHeight = 370
  ClientWidth = 136
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  DesignSize = (
    136
    370)
  PixelsPerInch = 96
  TextHeight = 13
  object lblSensitivity: TLabel
    Left = 8
    Top = 232
    Width = 49
    Height = 13
    Caption = 'Sensitivity'
    Visible = False
  end
  object lblSampling: TLabel
    Left = 8
    Top = 32
    Width = 42
    Height = 13
    Caption = 'Sampling'
  end
  object lblSensitivityVal: TLabel
    Left = 95
    Top = 232
    Width = 33
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '0 V/div'
    Visible = False
    ExplicitLeft = 79
  end
  object lblSamplingVal: TLabel
    Left = 90
    Top = 48
    Width = 38
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '300 kHz'
    ExplicitLeft = 74
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
    Left = 113
    Top = 80
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
    ExplicitLeft = 97
  end
  object lblCouplingTypeVal: TLabel
    Left = 113
    Top = 112
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
    ExplicitLeft = 97
  end
  object lblTimestampVal: TLabel
    Left = 113
    Top = 144
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
    ExplicitLeft = 97
  end
  object lblSamplesCount: TLabel
    Left = 8
    Top = 160
    Width = 69
    Height = 13
    Caption = 'Samples count'
  end
  object lblSamplesCountVal: TLabel
    Left = 113
    Top = 176
    Width = 15
    Height = 13
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = '???'
    ExplicitLeft = 97
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 329
    Width = 136
    Height = 41
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object btnExitViewer: TButton
      Left = 8
      Top = 8
      Width = 120
      Height = 25
      Align = alCustom
      Caption = 'Exit JSON Viewer'
      TabOrder = 0
      OnClick = btnExitViewerClick
    end
  end
  object edFilename: TEdit
    Left = 8
    Top = 8
    Width = 120
    Height = 18
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 1
    Text = '< file name >'
  end
end
