VERSION 5.00
Object = "{5E9E78A0-531B-11CF-91F6-C2863C385E30}#1.0#0"; "msflxgrd.ocx"
Begin VB.Form Form1 
   Caption         =   "Thermodynamic Data"
   ClientHeight    =   7130
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   9680
   LinkTopic       =   "Form1"
   ScaleHeight     =   7130
   ScaleWidth      =   9680
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command2 
      Caption         =   "Pipeline Density"
      Height          =   730
      Left            =   8040
      TabIndex        =   12
      Top             =   4260
      Width           =   1570
   End
   Begin VB.TextBox TXTTEMPERATURE 
      Height          =   300
      Left            =   360
      TabIndex        =   8
      Text            =   "21"
      Top             =   4920
      Width           =   490
   End
   Begin VB.TextBox txtStoragePressure 
      Height          =   300
      Left            =   360
      TabIndex        =   6
      Text            =   "70"
      Top             =   4560
      Width           =   610
   End
   Begin VB.CheckBox chkHalon 
      Caption         =   "Halon 1301 Calculation"
      Height          =   435
      Left            =   7440
      TabIndex        =   4
      Top             =   5160
      Visible         =   0   'False
      Width           =   1995
   End
   Begin VB.TextBox txtFillDensity 
      Height          =   300
      Left            =   360
      TabIndex        =   3
      Text            =   "1.4"
      Top             =   4170
      Width           =   555
   End
   Begin MSFlexGridLib.MSFlexGrid GRDtHERMO 
      Height          =   3680
      Left            =   240
      TabIndex        =   2
      Top             =   240
      Width           =   9080
      _ExtentX        =   15998
      _ExtentY        =   6473
      _Version        =   393216
      Cols            =   8
   End
   Begin VB.CommandButton Command1 
      Caption         =   "E&xit"
      Height          =   615
      Index           =   1
      Left            =   6000
      TabIndex        =   1
      Top             =   4320
      Width           =   1635
   End
   Begin VB.CommandButton Command1 
      Caption         =   "&Calculate Recession"
      Height          =   615
      Index           =   0
      Left            =   3600
      TabIndex        =   0
      Top             =   4320
      Width           =   1635
   End
   Begin VB.Label Label6 
      Caption         =   "Label6"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   790
      Left            =   300
      TabIndex        =   13
      Top             =   6240
      Width           =   9250
   End
   Begin VB.Label Label4 
      AutoSize        =   -1  'True
      Caption         =   "Label4"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   300
      Left            =   360
      TabIndex        =   10
      Top             =   5820
      Width           =   9130
      WordWrap        =   -1  'True
   End
   Begin VB.Label Label5 
      Alignment       =   1  'Right Justify
      Caption         =   "Recession Output File"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   610
      Left            =   240
      TabIndex        =   11
      Top             =   5520
      Width           =   2290
   End
   Begin VB.Label Label3 
      Caption         =   "Storage Temp (C)"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   1080
      TabIndex        =   9
      Top             =   4920
      Width           =   1335
   End
   Begin VB.Label Label2 
      Caption         =   "Pressure BAR"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   250
      Left            =   1080
      TabIndex        =   7
      Top             =   4560
      Width           =   1570
   End
   Begin VB.Label Label1 
      Caption         =   "KG/L Fill Density"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   370
      Left            =   960
      TabIndex        =   5
      Top             =   4200
      Width           =   1690
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Command2_Click()
If referenceTemp > 999 Then
MsgBox "Please select data line nearest to 50% discharged from pressure recession results."
Exit Sub
End If
TN2 = referenceTemp
PN2 = referenceNitroPressure
Unload frmResults
frmResults.Caption = "Pipeline Density"
frmResults.Text1.Visible = False
frmResults.Label1.Visible = False
Call CALCEXPANSION(PN2, TN2)
End Sub

Private Sub Form_Load()
Dim IND As Integer
Dim IND1 As Integer
Rem T(X, 0) TEMPERATURE DEG F
Rem T(X, 1) NOVEC PRESSURE PSI
Rem T(X, 2) SPECIFIC VOLUME LIQUID
Rem T(X, 3) SPECIFICE VOLUME VAPOR
Rem T(X, 4) LIQUID ENTHALPY
Rem T(X, 5) VAPOR ENTHALPY
Rem T(X, 6) LIQUID ENTROPY
Rem T(X, 7) VAPOR ENTROPY
Rem T(X, 8) HENRY'S LAW CONSTANT
ChDrive App.Path
ChDir App.Path
referenceTemp = 9999
GRDtHERMO.Cols = 9
GRDtHERMO.Rows = 71
GRDtHERMO.Row = 0
GRDtHERMO.Col = 0
GRDtHERMO.RowHeight(0) = 480:  Rem T(X, 0) TEMPERATURE DEG F
GRDtHERMO.Text = "°F"
GRDtHERMO.Col = 1
GRDtHERMO.Text = "PSIA"
GRDtHERMO.Col = 2
GRDtHERMO.Text = "SP VOL LIQ"
GRDtHERMO.ColWidth(2) = 1080
GRDtHERMO.Col = 3
GRDtHERMO.Text = "SP VOL VAPOR"
GRDtHERMO.WordWrap = True
GRDtHERMO.Col = 4
GRDtHERMO.Text = "ENTHALPY LIQ"
GRDtHERMO.ColWidth(4) = 1080
GRDtHERMO.ColWidth(5) = 1080
GRDtHERMO.Col = 5
GRDtHERMO.Text = "ENTHALPY VAPOR"
GRDtHERMO.Col = 6
GRDtHERMO.Text = "ENTROPY LIQ"
GRDtHERMO.Col = 7
GRDtHERMO.Text = "ENTROPY VAPOR"
GRDtHERMO.Col = 8
GRDtHERMO.Text = "Henry's Law Constant"
'MsgBox App.Path
Close
     Open App.Path & "/NOVECthermo.CSV" For Input As #1

     For IND = 1 To 70
     For IND1 = 0 To 7
     Input #1, T(IND, IND1)
          GRDtHERMO.Col = IND1
     GRDtHERMO.Row = IND
     GRDtHERMO = Format(Str$(T(IND, IND1)), "###.####")
     Next IND1
     Next IND
      For IND = 1 To 70
T(IND, 8) = (0.9286 * IND + 3494.6)
     GRDtHERMO.Col = 8
     GRDtHERMO.Row = IND
     GRDtHERMO = T(IND, 8)
          Next IND
 SUPERPRESSURE = 1015 - T(70, 1)
 Close
 Me.Label4.Caption = App.Path & "NOVEC_" + txtStoragePressure + "_Rec_" + Form1.txtFillDensity.Text + ".csv"
 Me.Label6.Caption = App.Path & "NovecPipeline" + Form1.txtFillDensity.Text + ".csv"
End Sub

Private Sub Command1_Click(Index As Integer)
Dim IND As Integer
Dim IND1 As Integer
frmResults.Caption = "Pressure Recession in Cylinder"
referenceTemp = 9999
Select Case Index
Case Is = 0
FILLDEN = Val(txtFillDensity) * 62.4269
StorPressure = Val(txtStoragePressure.Text) * 14.5
STORTEMP = 32 + Val(Me.TXTTEMPERATURE) * 9 / 5
STORTEMP = CInt(STORTEMP)
bHalon = False
frmResults.Text1.Visible = True
frmResults.Label1.Visible = True

CALCRECESSION
Case Is = 1
Unload Me
Case Is = 2
For IND = 1 To 70
     For IND1 = 0 To 8
     GRDtHERMO.Col = IND1
     GRDtHERMO.Row = IND
     GRDtHERMO = Format(Str$(T(IND, IND1)), "###.####")
     Next IND1
     Next IND

End Select
End Sub

Private Sub Text1_Change()

End Sub

Private Sub txtFillDensity_KeyUp(KeyCode As Integer, Shift As Integer)
 Me.Label4.Caption = App.Path & "NOVEC_" + txtStoragePressure + "_Rec_" + Form1.txtFillDensity.Text + ".csv"
 Me.Label6.Caption = App.Path & "NovecPipeline" + Form1.txtFillDensity.Text + ".csv"

End Sub

Private Sub txtStoragePressure_Change()
 Me.Label4.Caption = App.Path & "NOVEC_" + txtStoragePressure + "_Rec_" + Form1.txtFillDensity.Text + ".csv"
 Me.Label6.Caption = App.Path & "NovecPipeline_Density_" + Form1.txtFillDensity.Text + ".csv"

End Sub
