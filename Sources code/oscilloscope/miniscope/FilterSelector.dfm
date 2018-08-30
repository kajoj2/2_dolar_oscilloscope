object frmFilterSelector: TfrmFilterSelector
  Left = 0
  Top = 0
  BorderStyle = bsSingle
  Caption = 'Filter chain selector'
  ClientHeight = 222
  ClientWidth = 397
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnlBottom: TPanel
    Left = 0
    Top = 181
    Width = 397
    Height = 41
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object lblNote: TLabel
      Left = 8
      Top = 6
      Width = 167
      Height = 13
      Caption = 'Note: new filters will be used when'
    end
    object lblNote2: TLabel
      Left = 38
      Top = 21
      Width = 101
      Height = 13
      Caption = 'new data is captured'
    end
    object btnApply: TButton
      Left = 231
      Top = 9
      Width = 75
      Height = 25
      Caption = 'Apply'
      ModalResult = 1
      TabOrder = 0
    end
    object btnCancel: TButton
      Left = 315
      Top = 9
      Width = 75
      Height = 25
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
  object grBoxAvailableFilters: TGroupBox
    Left = 0
    Top = 0
    Width = 165
    Height = 181
    Align = alLeft
    Caption = 'Available filters'
    TabOrder = 1
    object lvAvailableFilters: TListView
      Left = 2
      Top = 15
      Width = 161
      Height = 164
      Align = alClient
      Columns = <
        item
          AutoSize = True
        end>
      HideSelection = False
      OwnerData = True
      ReadOnly = True
      RowSelect = True
      ShowColumnHeaders = False
      TabOrder = 0
      ViewStyle = vsReport
      OnData = lvAvailableFiltersData
      OnDblClick = lvAvailableFiltersDblClick
    end
  end
  object grboxUsedFilters: TGroupBox
    Left = 232
    Top = 0
    Width = 165
    Height = 181
    Align = alRight
    Caption = 'Used filters chain'
    TabOrder = 2
    object pnlUsedFiltersBottom: TPanel
      Left = 2
      Top = 146
      Width = 161
      Height = 33
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 0
      object btnRemove: TButton
        Left = 109
        Top = 4
        Width = 45
        Height = 25
        Caption = 'Remove'
        TabOrder = 0
        OnClick = btnRemoveClick
      end
      object btnUp: TButton
        Left = 6
        Top = 4
        Width = 45
        Height = 25
        Caption = 'Up'
        TabOrder = 1
        OnClick = btnUpClick
      end
      object btnDown: TButton
        Left = 57
        Top = 4
        Width = 45
        Height = 25
        Caption = 'Down'
        TabOrder = 2
        OnClick = btnDownClick
      end
    end
    object lvFilterChain: TListView
      Left = 2
      Top = 15
      Width = 161
      Height = 131
      Align = alClient
      Columns = <
        item
          AutoSize = True
        end>
      HideSelection = False
      OwnerData = True
      ReadOnly = True
      RowSelect = True
      ShowColumnHeaders = False
      TabOrder = 1
      ViewStyle = vsReport
      OnData = lvFilterChainData
    end
  end
  object btnAdd: TButton
    Left = 171
    Top = 80
    Width = 53
    Height = 25
    Caption = 'Add ->'
    TabOrder = 3
    OnClick = btnAddClick
  end
end
