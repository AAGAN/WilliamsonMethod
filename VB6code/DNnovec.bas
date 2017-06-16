Attribute VB_Name = "Module1"
Public T(71, 9) As Single
Public bHalon As Boolean

Public FILLDEN As Single
Public StorPressure As Single
Public STORTEMP As Single
Public SUPERPRESSURE As Single
Public referenceTemp As Integer
Public PN2, TN2 As Single
Public referenceNitroPressure As Single



Sub CALCRECESSION()
Dim calden(71, 8) As Single
Dim IND, IND2, S As Integer
Dim filestring As String
Dim temsinglE As Single
Dim nitrogenInLiquid, nitrogenInVapor As Single
Dim p9, T9, D9, p2, p3, a1, a2, a3, a4, a5, e1, e2, e3, e5, e6, r1, r2, r3 As Single
Dim Liq, vAp, L1, L2, v1, v2 As Single
On Error GoTo CalcErr
Rem T(X, 0) TEMPERATURE DEG F
Rem T(X, 1) NOVEC PRESSURE PSI
Rem T(X, 2) SPECIFIC VOLUME LIQUID
Rem T(X, 3) SPECIFICE VOLUME VAPOR
Rem T(X, 4) LIQUID ENTHALPY
Rem T(X, 5) VAPOR ENTHALPY
Rem T(X, 6) LIQUID ENTROPY
Rem T(X, 7) VAPOR ENTROPY
Rem T(X, 8) HENRY'S LAW CONSTANT
Rem Molecular weight NOVEC 1230 316.04
Rem Molecular weight Nitrogen 28
Rem Ratio of Molecular weight N2/Novec = 0.886
1180 Close
ChDir App.Path
filestring = "70BarNOVEC" + Format(Str(StorPressure), "0") + "Rec" + Format(Str(FILLDEN), "00") + ".csv"
Open filestring For Output As #1
  SUPERPRESSURE = StorPressure - T(70, 1)

 STORTEMP = 70
     p9 = SUPERPRESSURE: T9 = STORTEMP: D9 = FILLDEN
     Liq = (1 - FILLDEN * T(STORTEMP, 3)) / (T(STORTEMP, 2) * (1 + 0.0429 * SUPERPRESSURE / T(STORTEMP, 8)) - T(STORTEMP, 3))
     vaPr = FILLDEN - Liq
     n = Liq * 0.01 * SUPERPRESSURE / T(70, 8) + vaPr * 0.0886 * SUPERPRESSURE / T(70, 1)
     frmResults.Text1 = Format(Str(n / FILLDEN), "0.0000")
     For IND = STORTEMP To 2 Step -1
     calden(IND, 5) = SUPERPRESSURE + T(IND, 1)
     calden(IND, 2) = Liq
     calden(IND, 3) = vaPr
     calden(IND, 4) = SUPERPRESSURE
     n = Liq * 0.0886 * SUPERPRESSURE / T(IND, 8) + vaPr * 0.0886 * SUPERPRESSURE / T(IND, 1)
'     nitrogenInVapor = vaPr * 0.0886 * SUPERPRESSURE / T(IND, 1)
'     n = nitrogenInLiquid + nitrogenInVapor
     p2 = SUPERPRESSURE - 1
     p2 = p2 - 1
     e1 = (1 + 0.0429 * SUPERPRESSURE / T(IND, 8)) * (T(IND, 6))
1330 e5 = (1 + 0.0429 * p2 / T(IND - 1, 8)) * T(IND - 1, 2)
     e6 = 1 + 0.0886 * SUPERPRESSURE / T(IND, 8)
     r1 = (1 + 0.0886 * SUPERPRESSURE / T(IND, 1)) / e6
     r2 = (1 + 0.0886 * p2 / T(IND - 1, 8)) / e6
     r3 = (1 + 0.0886 * p2 / T(IND - 1, 1)) / e6
     a1 = r1 * e1 - (1 + SUPERPRESSURE / T(IND, 1)) * T(IND, 7)
     a2 = r2 * e1 - (1 + 0.0429 * p2 / T(IND - 1, 8)) * (T((IND - 1), 6))
     a3 = r3 * e1 - (1 + p2 / T(IND - 1, 1)) * T(IND - 1, 7)
     L2 = (vaPr * a1 * T(IND - 1, 3) - a3) / (a2 * T(IND - 1, 3) - e5 * a3)
     If L2 < 0 Then GoTo 1620
     v2 = (1 - L2 * e5) / T(IND - 1, 3)
     L1 = Liq + vaPr - L2 - v2
     p3 = (n - L1 * 0.0886 * SUPERPRESSURE / T(IND, 8)) / (L2 * 0.0886 / T(IND - 1, 8) + v2 * 0.0886 / T(IND - 1, 1))
     GoTo 1450
1450 If p3 < p2 - 1 Then GoTo 1490
     If p3 > p2 + 1 Then GoTo 1510
     If p3 < p2 - 1 Then GoTo 1530
     GoTo 1550
1490 p2 = p2 - 1
     GoTo 1330
1510 p2 = p2 + 0.1
     GoTo 1330
1530 p2 = p2 - 0.1
     GoTo 1330
1550 If L2 < 0 Then GoTo 1620
     calden(IND - 1, 1) = calden(IND, 1) + L1
     Liq = L2
     vaPr = v2
     SUPERPRESSURE = p2
     S = S + 1
     Next IND
1620

Write #1, FILLDEN; " lb/cu ft  "; FILLDEN / 62.4269; "  kg/L"
     Write #1, "TEMP", "PSIA", "DISC", "LIQ", "VAP", "N-PSI", "LDENS", "DISC%", "BAR"
            frmResults.grdResults.Row = 0

For IND = 1 To 8
frmResults.grdResults.Col = IND - 1
Select Case IND
Case Is = 1
frmResults.grdResults = "TEMP"
Case Is = 2
frmResults.grdResults = "PSIA"
Case Is = 3
frmResults.grdResults = "DISCH"
Case Is = 4
frmResults.grdResults = "LIQ"
Case Is = 5
frmResults.grdResults = "VAP"
Case Is = 6
frmResults.grdResults = "N PSI"
Case Is = 7
frmResults.grdResults = "LIQ DEN"
Case Is = 8
frmResults.grdResults = "PERCENT DISC"

End Select
Next IND



     For IND = STORTEMP To (STORTEMP - S) Step -1
temsinglE = 0.01 * calden(IND, 1) / FILLDEN

  Write #1, IND, calden(IND, 5), calden(IND, 1), calden(IND, 2), calden(IND, 3), calden(IND, 4), (1 + 0.043 * calden(IND, 4) / T(IND, 8)) / (T(IND, 2) * (1 + 0.0429 * calden(IND, 4) / T(IND, 8))), Format(Str(100 * temsinglE), "0.##"), calden(IND, 5) / 14.5
  
     Next IND
     Write #1, frmResults.Text1 & " initial kgs nitrogen/kg NOVEC"
     frmResults.Show
     a1 = 0
          For IND2 = STORTEMP To (STORTEMP - S) Step -1
a1 = a1 + 1
frmResults.grdResults.Rows = a1 + 1
temsinglE = (1 + 0.0886 * calden(IND2, 4) / T(IND2, 8)) / (T(IND2, 2) * (1 + 0.0886 * calden(IND2, 4) / T(IND2, 8)))
       frmResults.grdResults.Row = a1
       For IND = 1 To 8
frmResults.grdResults.Col = IND - 1
Select Case IND
Case Is = 1
frmResults.grdResults = IND2
Case Is = 2
frmResults.grdResults = calden(IND2, 5)
Case Is = 3
frmResults.grdResults = calden(IND2, 1)
Case Is = 4
frmResults.grdResults = calden(IND2, 2)
Case Is = 5
frmResults.grdResults = calden(IND2, 3)
Case Is = 6
frmResults.grdResults = calden(IND2, 4)
Case Is = 7
frmResults.grdResults = Format(Str(temsinglE), "###.#")
Case Is = 8
temsinglE = calden(IND2, 1) / FILLDEN
frmResults.grdResults = Format(Str(100 * temsinglE), "###.##")


End Select
Next IND
Next IND2

Close
Exit Sub
CalcErr: MsgBox Err.Description

Close
End Sub

