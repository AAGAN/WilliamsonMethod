VERSION 5.00
Object = "{5E9E78A0-531B-11CF-91F6-C2863C385E30}#1.0#0"; "msflxgrd.ocx"
Begin VB.Form frmResults 
   Caption         =   "Pipeline Density Versus Pressure"
   ClientHeight    =   5480
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   10320
   LinkTopic       =   "Form2"
   ScaleHeight     =   5480
   ScaleWidth      =   10320
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox Text1 
      Height          =   315
      Left            =   4380
      TabIndex        =   2
      Text            =   "Text1"
      Top             =   4740
      Width           =   1035
   End
   Begin VB.CommandButton Command1 
      Caption         =   "&EXIT"
      Height          =   555
      Left            =   2340
      TabIndex        =   1
      Top             =   4650
      Width           =   1335
   End
   Begin MSFlexGridLib.MSFlexGrid grdResults 
      Height          =   4220
      Left            =   300
      TabIndex        =   0
      Top             =   240
      Width           =   9620
      _ExtentX        =   16969
      _ExtentY        =   7444
      _Version        =   393216
      Cols            =   8
      WordWrap        =   -1  'True
      AllowUserResizing=   1
   End
   Begin VB.Label Label1 
      Caption         =   "initial kgs of nitrogen / kg NOVEC 1230"
      Height          =   380
      Left            =   5460
      TabIndex        =   3
      Top             =   4740
      Width           =   1640
   End
End
Attribute VB_Name = "frmResults"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private Sub Command1_Click()
Unload Me
End Sub

Private Sub Form_Load()
grdResults.Rows = 100
grdResults.Cols = 8
grdResults.Row = 0
frmResults.grdResults.RowHeight(0) = 480
End Sub

Private Sub grdResults_Click()
Dim iCol As Integer
Dim iRow As Integer
grdResults.Col = 0
referenceTemp = Val(grdResults)
'MsgBox referenceTemp
grdResults.Col = 5
referenceNitroPressure = Val(grdResults)
'MsgBox referenceNitroPressure

End Sub
