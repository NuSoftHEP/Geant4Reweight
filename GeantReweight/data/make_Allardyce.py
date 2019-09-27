from ROOT import * 
from array import array 

f = TFile("Allardyce.root", "RECREATE")

mom_full = [ 710., 840., 1000., 1360., 1580., 2000. ]

moms = {
  "C":     array("d", mom_full),
  "Al":    array("d", mom_full[1:-1]),
  "Ca":    array("d", mom_full),
  "Ni":    array("d", mom_full),
  "Sn":    array("d", mom_full),
  "Sn120": array("d", mom_full[1:-2]),
  "Ho":    array("d", mom_full[1:-2]),
  "Pb":    array("d", mom_full),
  "Pb208": array("d", mom_full[1:-2])
}


exs = {
  "C":     array("d", [0.]*6), 
  "Al":    array("d", [0.]*4),
  "Ca":    array("d", [0.]*6), 
  "Ni":    array("d", [0.]*6), 
  "Sn":    array("d", [0.]*6), 
  "Sn120": array("d", [0.]*3), 
  "Ho":    array("d", [0.]*3), 
  "Pb":    array("d", [0.]*6), 
  "Pb208": array("d", [0.]*3)
}

reacs = {
  "C":     array("d", [263., 246., 257., 248., 243., 224.]), 
  "Al":    array("d", [429., 451., 437., 435.]),
  "Ca":    array("d", [587., 594., 609., 599., 601., 555.]), 
  "Ni":    array("d", [741., 742., 749., 752., 750., 707.]), 
  "Sn":    array("d", [1187., 1190., 1231., 1217., 1208., 1154.]), 
  "Sn120": array("d", [1221., 1254., 1223.]), 
  "Ho":    array("d", [1543., 1606., 1544.]), 
  "Pb":    array("d", [1754., 1752., 1772., 1743., 1736., 1670.]), 
  "Pb208": array("d", [1764., 1762., 1742.])
}

errs = {
#  "C":     array("d", [3., 3., 2., 3., 3., 2.]), 
  "C":     array("d", [3. + 2.63, 3. + 2.46, 2. + 2.57, 3., 3., 2.]), 
#  "Al":    array("d", [11., 3., 4., 3.]),
  "Al":    array("d", [11. + 4.29, 3. + 4.51, 4., 3.]),
  "Ca":    array("d", [7., 9., 5., 4., 5., 5.]), 
  "Ni":    array("d", [8., 9., 5., 5., 6., 8.]), 
  "Sn":    array("d", [17., 10., 8., 9., 11., 10.]), 
  "Sn120": array("d", [14., 10., 14.]), 
  "Ho":    array("d", [28., 15., 26.]), 
#  "Pb":    array("d", [16., 16., 9., 13., 16., 13.]), 
  "Pb":    array("d", [16. + 17.54, 16. + 17.52, 9. + 17.72, 13., 16., 13.]), 
  "Pb208": array("d", [21., 26., 24.])
}


f.cd()

for nuc in moms.keys():
  gr = TGraphErrors(len(moms[nuc]), moms[nuc], reacs[nuc], exs[nuc], errs[nuc])
  gr.SetName(nuc + "_xsec_reac_piplus")
  gr.Write()

reacs_minus = {
  "C":     array("d", [243., 250., 264., 254., 246., 224.]), 
  "Al":    array("d", [433., 454., 444., 444.]),
  "Ca":    array("d", [608., 607., 624., 611., 598., 550.]), 
  "Ni":    array("d", [764., 764., 772., 771., 758., 710.]), 
  "Sn":    array("d", [1221., 1230., 1249., 1249., 1233., 1164.]), 
  "Sn120": array("d", [1241., 1260., 1258.]), 
  "Ho":    array("d", [1583., 1607., 1594.]), 
  "Pb":    array("d", [1806., 1814., 1808., 1817., 1802., 1703.]), 
  "Pb208": array("d", [1759., 1810., 1806.])
}

errs_minus = {
  "C":     array("d", [3. + 2.43, 2. + 2.5, 1. + 2.64, 2., 2., 2.]), 
  "Al":    array("d", [6. + 4.33, 3. + 4.54, 3., 4.]),
  #"C":     array("d", [3., 2., 1., 2., 2., 2.]), 
  #"Al":    array("d", [6., 3., 3., 4.]),
  "Ca":    array("d", [5., 5., 5., 4., 5., 3.]), 
  "Ni":    array("d", [6., 6., 3., 5., 4., 4.]), 
  "Sn":    array("d", [11., 9., 6., 7., 5., 11.]), 
  "Sn120": array("d", [10., 11., 10.]), 
  "Ho":    array("d", [14., 13., 10.]), 
  #"Pb":    array("d", [15., 12., 6., 9., 7., 16.]), 
  "Pb":    array("d", [15. + 18.06, 12. + 18.14, 6. + 18.08, 9., 7., 16.]), 
  "Pb208": array("d", [40., 9., 16.])
}

for nuc in moms.keys():
  gr = TGraphErrors(len(moms[nuc]), moms[nuc], reacs_minus[nuc], exs[nuc], errs_minus[nuc])
  gr.SetName(nuc + "_xsec_reac_piminus")
  gr.Write()
