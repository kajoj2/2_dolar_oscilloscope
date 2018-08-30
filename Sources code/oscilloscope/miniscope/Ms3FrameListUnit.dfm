object frmMs3FrameList: TfrmMs3FrameList
  Left = 0
  Top = 0
  Caption = 'Data frame list'
  ClientHeight = 217
  ClientWidth = 541
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lvFrames: TListView
    Left = 0
    Top = 0
    Width = 541
    Height = 217
    Align = alClient
    Columns = <
      item
        Caption = 'ID'
        Width = 40
      end
      item
        Caption = 'Timestamp'
        Width = 140
      end
      item
        Caption = 'Channels/Coupling'
        Width = 120
      end
      item
        Caption = 'Sampling'
        Width = 70
      end
      item
        Caption = 'Samples'
        Width = 60
      end
      item
        Caption = 'Trigger'
        Width = 80
      end>
    HideSelection = False
    MultiSelect = True
    OwnerData = True
    ReadOnly = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnChange = lvFramesChange
    OnData = lvFramesData
    ExplicitWidth = 508
  end
  object tmrRefresh: TTimer
    Enabled = False
    Interval = 20
    OnTimer = tmrRefreshTimer
    Left = 8
    Top = 24
  end
end
