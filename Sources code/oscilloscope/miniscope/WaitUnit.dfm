object frmWait: TfrmWait
  Left = 0
  Top = 0
  BorderStyle = bsNone
  ClientHeight = 33
  ClientWidth = 327
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poDesktopCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel: TBevel
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 321
    Height = 27
    Align = alClient
    Shape = bsFrame
    ExplicitLeft = 0
    ExplicitTop = 0
    ExplicitWidth = 300
    ExplicitHeight = 32
  end
  object Label1: TLabel
    Left = 24
    Top = 8
    Width = 245
    Height = 16
    Caption = 'Loading input device dll, please wait...'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
end
