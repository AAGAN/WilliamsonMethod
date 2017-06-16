Attribute VB_Name = "Module2"
Option Explicit
Sub CALCEXPANSION(PN2, TN2)
Dim calden(71, 8) As Single
Dim IND, IND2 As Integer
Dim e7, e4, e8, e9, p9, T9, D9, p2, p3, a1, a2, a3, a4, a5, e1, e2, e3, e5, e6, r1, r2, r3 As Single
Dim Liq, vAp, L1, L2, v1, v2, nStart2, vaPr   As Single
Dim filestring As String
Dim Hstart As Single
'2000 Data 101.5, 59, 72.8, 45: Rem INSERT DATA FROM PRESSURRE RECESSION TABLE
'     Print "INPUT N2 PRESSURE AND TEMP FROM CYLINDER PRESSURE RECESSION"
'     INPUT PN2:INPUT tn2
'     S = S + 1
     nStart2 = PN2 / T(TN2, 8)
     calden(TN2, 1) = PN2 + T(TN2, 1)
     calden(TN2, 2) = 1
     calden(TN2, 3) = 0
     calden(TN2, 4) = PN2
     calden(TN2, 5) = (1 + nStart2) / ((1 + 0.053 * PN2 / T(TN2, 8)) * T(TN2, 2))
     p2 = PN2 - 1
     Liq = 1
     vaPr = 0
     For IND = TN2 To 2 Step -1
     T(IND, 8) = T(IND, 8) * 0.01
     Next IND
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

Hstart = T(TN2, 4) / T(TN2, 2)


      For IND = TN2 - 1 To 2 Step -1
2161      L2 = (Hstart - T(IND, 5) / T(IND, 3)) / ((T(IND, 4) / T(IND, 2)) - (T(IND, 5) / T(IND, 3)))
      r2 = (T(IND, 4) / T(IND, 2))
      r3 = Hstart - r2
      r2 = r3 / T(IND, 5)
      If L2 > 1 Then L2 = 0.99
            r3 = L2 * (T(IND, 4) / T(IND, 2)) + (1 - L2) * (T(IND, 5) / T(IND, 3))

      
      
      
'     e1 = (1 + 0.053 * PN2 / T(IND, 8)) * T(IND, 4)
'     e2 = (1 + PN2 / T(IND, 1)) * T(IND, 5)
'     e6 = (1 + 0.01 * PN2 / T(IND, 8))
'     e7 = (1 + 0.0886 * PN2 / T(IND, 1))
2160 e3 = (1 + 0.053 * p2 / T(IND - 1, 8)) * T(IND - 1, 4)
'     e4 = (1 + p2 / T(IND - 1, 1)) * T(IND - 1, 5)
'     e8 = (1 + 0.01 * p2 / T(IND - 1, 8))
'     e9 = 1 + 0.0886 * p2 / T(IND - 1, 1)
'     L2 = (Liq * (e1 * e9 - e6 * e4) + vaPr * (e2 * e9 - e7 * e4)) / (e3 * e9 - e8 * e4)
     v2 = 1 - L2
     If v2 < 0 Then v2 = 0
     p3 = nStart2 / (L2 * 0.01 / T(IND - 1, 8) + v2 * 0.0886 / T(IND - 1, 1))
'     On Key(11) GoSub 2228
'     KEY(11)ON
If IND = 2 Then GoTo 2330
'2228 Stop
2230  If Abs(p3 - p2) <= 1.1 Then GoTo 2330
If p3 < p2 - 0.5 Then GoTo 2270
     If p3 > p2 + 0.5 Then GoTo 2290
     If p3 < p2 - 0.1 Then GoTo 2310
     GoTo 2330
2270 p2 = p2 - 1
     GoTo 2161
2290 p2 = p2 + 0.1
     GoTo 2161
2310 p2 = p2 - 0.1
     GoTo 2161
2330 PN2 = p2
     Liq = L2
     vaPr = v2
     calden(IND - 1, 1) = PN2 + T(IND - 1, 1)
     calden(IND - 1, 2) = Liq
     calden(IND - 1, 3) = vaPr
     calden(IND - 1, 4) = PN2
     calden(IND - 1, 5) = (1 + nStart2) / (Liq * (1 + 0.053 * PN2 / T(IND - 1, 8)) * T(IND - 1, 2) + vaPr * T(IND - 1, 3))
     Next IND
'     OPEN "o",#1,N$
'     Print "CONSTANT ENTHALPY EXPANSION FOR H-1301 PLUS N2"
'     LPRINT TAB(15) "CONSTANT ENTHALPY EXPANSION FOR H-1301 PLUS N2"
'     Print "storage pressure"; p9; "at"; T9; "degrees f"; D9; "lbs/cu ft fill density"
'     LPRINT"STORAGE PRESSURE"P9"AT"T9"DEGREES F"D9"LBS/CU FT FILL DENSITY"
'     Print
'     LPRINT
frmResults.grdResults.Clear
frmResults.grdResults.Row = 0
For IND = 1 To 7
frmResults.grdResults.Col = IND - 1
Select Case IND
Case Is = 1
frmResults.grdResults = "TEMP"
Case Is = 2
frmResults.grdResults = "PSIA"
Case Is = 3
frmResults.grdResults = "LIQ"
Case Is = 4
frmResults.grdResults = "VAP"
Case Is = 5
frmResults.grdResults = "N-PSI"
Case Is = 6
frmResults.grdResults = "DENSITY"
Case Is = 7
frmResults.grdResults = ""
Case Is = 8
frmResults.grdResults = ""

End Select
Next IND
For IND2 = TN2 To 2 Step -1
If IND2 = TN2 - 1 Then GoTo 999
frmResults.grdResults.Row = frmResults.grdResults.Row + 1
For IND = 1 To 6
frmResults.grdResults.Col = IND - 1
Select Case IND
Case Is = 1
frmResults.grdResults = IND2
Case Is = 2
frmResults.grdResults = calden(IND2, 1)
Case Is = 3
frmResults.grdResults = calden(IND2, 2)
Case Is = 4
frmResults.grdResults = calden(IND2, 3)
Case Is = 5
frmResults.grdResults = calden(IND2, 4)
Case Is = 6
frmResults.grdResults = calden(IND2, 5)
End Select
Next IND
999 Next IND2
For IND = TN2 To 2 Step -1
     T(IND, 8) = T(IND, 8) * 100
     Next IND
'     Print "   TEMP   PSIA     LIQ    VAP    N-PSI    DENSITY"
'     LPRINT TAB(15)"   TEMP   PSIA     LIQ    VAP    N-PSI    DENSITY"
ChDrive App.Path
ChDir App.Path

filestring = "70BarNovec" + Format(Str(StorPressure), "0") + "Pipeline" + Format(Str(FILLDEN), "00") + ".csv"
Open filestring For Output As #4
    For IND = TN2 To 2 Step -1
'     Print USING; "  ###   ###.#   #.###    .###   ###.##  ###.##"; ind, calden(ind, 1), calden(ind, 2), calden(ind, 3), calden(ind, 4), calden(ind, 5)
     Write #4, IND, calden(IND, 1), calden(IND, 5)
'     Next ind
'     Close #1
'     T8 = Timer - T8
'     Print T8; "SEC"
'     Print Date$, Time$
'     For ind = tn2 To 2 Step -1
'     LPRINT USING "                 ###   ###.#   #.###    .###   ###.##  ###.##";ind,calden(ind,1),calden(ind,2),calden(ind,3),calden(ind,4),calden(ind,5)
     Next IND
     Print #4, FILLDEN
     Close
     frmResults.Show vbModal

'     LPRINT T8"SEC"
'     LPRINT Date$, Time$
'     End
End Sub
