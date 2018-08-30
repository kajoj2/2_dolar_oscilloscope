object frmControl: TfrmControl
  Left = 0
  Top = 0
  Align = alClient
  BorderStyle = bsNone
  Caption = 'frmControl'
  ClientHeight = 428
  ClientWidth = 137
  Color = clBtnFace
  Constraints.MinHeight = 428
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object pnlOscControl: TPanel
    Left = 0
    Top = 0
    Width = 137
    Height = 428
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object pnlOscControlBottom: TPanel
      Left = 0
      Top = 110
      Width = 137
      Height = 318
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
      DesignSize = (
        137
        318)
      object lblSampling: TLabel
        Left = 6
        Top = 6
        Width = 42
        Height = 13
        Caption = 'Sampling'
      end
      object lblSamplingVal: TLabel
        Left = 97
        Top = 6
        Width = 38
        Height = 13
        Alignment = taRightJustify
        Anchors = [akTop, akRight]
        Caption = '300 kHz'
        ExplicitLeft = 87
      end
      object lblTriggerType: TLabel
        Left = 6
        Top = 136
        Width = 59
        Height = 13
        Caption = 'Trigger type'
      end
      object lblTriggerLevel: TLabel
        Left = 6
        Top = 172
        Width = 59
        Height = 13
        Caption = 'Trigger level'
        Visible = False
      end
      object lblTriggerLevelVal: TLabel
        Left = 118
        Top = 172
        Width = 17
        Height = 13
        Alignment = taRightJustify
        Anchors = [akTop, akRight]
        Caption = '0%'
        Visible = False
      end
      object lblBuffer: TLabel
        Left = 6
        Top = 52
        Width = 30
        Height = 13
        Caption = 'Buffer'
      end
      object lblInterval: TLabel
        Left = 6
        Top = 245
        Width = 38
        Height = 13
        Caption = 'Interval'
      end
      object lblSeconds: TLabel
        Left = 105
        Top = 245
        Width = 5
        Height = 13
        Caption = 's'
      end
      object lblTriggerSource: TLabel
        Left = 6
        Top = 117
        Width = 69
        Height = 13
        Caption = 'Trigger source'
      end
      object lblTriggerPosition: TLabel
        Left = 6
        Top = 214
        Width = 52
        Height = 13
        Caption = 'Pre-trigger'
        Visible = False
      end
      object lblTriggerPositionVal: TLabel
        Left = 112
        Top = 214
        Width = 17
        Height = 13
        Alignment = taRightJustify
        Anchors = [akTop, akRight]
        Caption = '0%'
        Visible = False
      end
      object lblTriggerMode: TLabel
        Left = 6
        Top = 93
        Width = 26
        Height = 13
        Caption = 'Mode'
      end
      object trbarSampling: TTrackBar
        Left = 0
        Top = 19
        Width = 135
        Height = 28
        Max = 6
        Position = 6
        TabOrder = 0
        OnKeyUp = TrackbarKeyUp
      end
      object cbTriggerType: TComboBox
        Left = 6
        Top = 150
        Width = 126
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 1
        OnChange = cbTriggerTypeChange
      end
      object trbarTriggerLevel: TTrackBar
        Left = 0
        Top = 186
        Width = 134
        Height = 28
        Max = 127
        Min = -128
        Frequency = 13
        Position = 6
        TabOrder = 2
        Visible = False
        OnKeyUp = TrackbarKeyUp
      end
      object btnStart: TButton
        Left = 5
        Top = 282
        Width = 63
        Height = 25
        Caption = 'Run'
        TabOrder = 3
        OnClick = btnStartClick
      end
      object btnStop: TButton
        Left = 68
        Top = 282
        Width = 63
        Height = 25
        Caption = 'Stop'
        Enabled = False
        TabOrder = 4
        OnClick = btnStopClick
      end
      object btnManualTrigger: TButton
        Left = 5
        Top = 257
        Width = 126
        Height = 25
        Caption = 'Trigger now!'
        TabOrder = 5
        Visible = False
        OnClick = btnManualTriggerClick
      end
      object cbBufferSize: TComboBox
        Left = 53
        Top = 49
        Width = 79
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 6
        OnChange = cbBufferSizeChange
      end
      object ProgressBarFrameRx: TProgressBar
        Left = 6
        Top = 71
        Width = 126
        Height = 15
        Hint = 'Buffer receiving progress'
        Smooth = True
        TabOrder = 7
      end
      object edInterval: TEdit
        Left = 50
        Top = 242
        Width = 47
        Height = 21
        TabOrder = 8
        Text = '10'
      end
      object btnRecorderStart: TButton
        Left = 6
        Top = 282
        Width = 39
        Height = 25
        Caption = 'Start'
        TabOrder = 9
        OnClick = btnRecorderStartClick
      end
      object btnRecorderStop: TButton
        Left = 49
        Top = 282
        Width = 39
        Height = 25
        Caption = 'Stop'
        Enabled = False
        TabOrder = 10
        OnClick = btnRecorderStopClick
      end
      object btnRecorderClear: TButton
        Left = 92
        Top = 282
        Width = 39
        Height = 25
        Caption = 'Clear'
        TabOrder = 11
        OnClick = btnRecorderClearClick
      end
      object cbTriggerSource: TComboBox
        Left = 92
        Top = 113
        Width = 40
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        ItemIndex = 0
        TabOrder = 12
        Text = 'A'
        OnChange = cbTriggerTypeChange
        Items.Strings = (
          'A'
          'B')
      end
      object trbarTriggerPosition: TTrackBar
        Left = -2
        Top = 228
        Width = 134
        Height = 28
        Max = 100
        Frequency = 10
        Position = 50
        TabOrder = 13
        Visible = False
        OnKeyUp = TrackbarKeyUp
      end
      object cbTriggerMode: TComboBox
        Left = 36
        Top = 90
        Width = 96
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 14
        OnChange = cbTriggerModeChange
      end
    end
    object tabOscControlTop: TTabControl
      Left = 0
      Top = 0
      Width = 137
      Height = 110
      Align = alTop
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      OwnerDraw = True
      ParentFont = False
      Style = tsButtons
      TabOrder = 1
      Tabs.Strings = (
        'A'
        'B'
        'C'
        'D'
        'E'
        'F')
      TabIndex = 0
      TabWidth = 22
      OnDrawTab = tabOscControlTopDrawTab
      DesignSize = (
        137
        110)
      object lblSensitivity: TLabel
        Left = 7
        Top = 43
        Width = 49
        Height = 13
        Caption = 'Sensitivity'
      end
      object lblSensitivityVal: TLabel
        Left = 97
        Top = 43
        Width = 33
        Height = 13
        Alignment = taRightJustify
        Anchors = [akTop, akRight]
        Caption = '0 V/div'
      end
      object lblCouplingType: TLabel
        Left = 7
        Top = 89
        Width = 41
        Height = 13
        Caption = 'Coupling'
      end
      object trbarSensitivity: TTrackBar
        Left = 2
        Top = 57
        Width = 135
        Height = 28
        Max = 6
        Position = 6
        TabOrder = 1
        OnKeyUp = TrackbarKeyUp
      end
      object cbCouplingType: TComboBox
        Left = 75
        Top = 86
        Width = 59
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 2
        OnChange = cbCouplingTypeChange
      end
      object chbChannelEnable: TCheckBox
        Left = 6
        Top = 25
        Width = 123
        Height = 17
        Caption = 'Enable channel'
        TabOrder = 0
        OnClick = chbChannelEnableClick
        OnKeyPress = chbChannelEnableKeyPress
      end
    end
  end
  object tmrRecorder: TTimer
    Enabled = False
    Interval = 10000
    OnTimer = tmrRecorderTimer
    Left = 104
    Top = 80
  end
end
