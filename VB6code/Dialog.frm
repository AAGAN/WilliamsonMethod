VERSION 5.00
Begin VB.Form Dialog 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Pipeline Expansion Input Parameters"
   ClientHeight    =   3200
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   6030
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3200
   ScaleWidth      =   6030
   ShowInTaskbar   =   0   'False
   Begin VB.TextBox Text2 
      Height          =   735
      Left            =   480
      TabIndex        =   3
      Text            =   "TEMP"
      Top             =   1410
      Width           =   1635
   End
   Begin VB.TextBox Text1 
      Height          =   645
      Left            =   480
      TabIndex        =   2
      Text            =   "N PSI"
      Top             =   270
      Width           =   1695
   End
   Begin VB.CommandButton CancelButton 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   4680
      TabIndex        =   1
      Top             =   600
      Width           =   1215
   End
   Begin VB.CommandButton OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   4680
      TabIndex        =   0
      Top             =   120
      Width           =   1215
   End
   Begin VB.Label Label2 
      Caption         =   "REFERENCE TEMPERATURE F"
      Height          =   735
      Left            =   2520
      TabIndex        =   5
      Top             =   1500
      Width           =   1635
   End
   Begin VB.Label Label1 
      Caption         =   "NITROGEN PSI"
      Height          =   645
      Left            =   2460
      TabIndex        =   4
      Top             =   360
      Width           =   1695
   End
End
Attribute VB_Name = "Dialog"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Option Explicit

Private Sub CancelButton_Click()
Unload Me
End Sub

Private Sub OKButton_Click()
PN2 = Val(Text1)
TN2 = Val(Text2)
Call CALCEXPANSION(PN2, TN2)
Unload Me

End Sub
