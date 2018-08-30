object frmStatistics: TfrmStatistics
  Left = 0
  Top = 0
  Caption = 'Statistics'
  ClientHeight = 227
  ClientWidth = 310
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object pnlToolbar: TPanel
    Left = 0
    Top = 158
    Width = 310
    Height = 69
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object lblSum: TLabel
      Left = 8
      Top = 48
      Width = 20
      Height = 13
      Caption = 'Sum'
    end
    object lblAverage: TLabel
      Left = 155
      Top = 48
      Width = 41
      Height = 13
      Caption = 'Average'
    end
    object btnAddCurrentSegment: TButton
      Left = 8
      Top = 8
      Width = 137
      Height = 25
      Caption = 'Add current segment'
      TabOrder = 0
      OnClick = btnAddCurrentSegmentClick
    end
    object btnDeleteSelected: TButton
      Left = 155
      Top = 8
      Width = 137
      Height = 25
      Caption = 'Delete selected'
      TabOrder = 1
      OnClick = btnDeleteSelectedClick
    end
    object edSum: TEdit
      Left = 48
      Top = 45
      Width = 73
      Height = 21
      ReadOnly = True
      TabOrder = 2
    end
    object edAverage: TEdit
      Left = 216
      Top = 45
      Width = 76
      Height = 21
      ReadOnly = True
      TabOrder = 3
    end
  end
  object pnlCenter: TPanel
    Left = 0
    Top = 0
    Width = 310
    Height = 158
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object lvSegments: TListView
      Left = 0
      Top = 0
      Width = 310
      Height = 158
      Align = alClient
      Columns = <
        item
          Caption = 'ID'
          Width = 40
        end
        item
          Caption = 'X1'
          Width = 80
        end
        item
          Caption = 'X2'
          Width = 80
        end
        item
          Caption = 'delta'
          Width = 80
        end>
      HideSelection = False
      MultiSelect = True
      OwnerData = True
      ReadOnly = True
      RowSelect = True
      TabOrder = 0
      ViewStyle = vsReport
      OnData = lvSegmentsData
    end
  end
end
