from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Ingram.root", "RECREATE")

Ts = [114., 163., 240.]
mom = array( "d", [ get_p(T) for T in Ts ])
print mom
moms = {
  "O_xsec_inel_piplus": mom,
  "O_xsec_cex_piplus":  mom,
  "O_xsec_dcex_piplus": mom[2:],
  "O_xsec_abs_piplus":  mom
}


exs = {
  "O_xsec_inel_piplus": array("d", [0.]*3),
  "O_xsec_cex_piplus":  array("d", [0.]*3),
  "O_xsec_dcex_piplus": array("d", [0.]),
  "O_xsec_abs_piplus":  array("d", [0.]*3)
}


reacs = {
  "O_xsec_inel_piplus": array("d", [191., 259., 249.]),
  "O_xsec_cex_piplus":  array("d", [58., 63., 62.]),
  "O_xsec_dcex_piplus": array("d", [6.]),
  "O_xsec_abs_piplus":  array("d", [206., 188., 89.])
}

errs = {
  "O_xsec_inel_piplus": array("d", [12., 17., 16.]),
  "O_xsec_cex_piplus":  array("d", [17., 19., 19.]),
  "O_xsec_dcex_piplus": array("d", [1.]),
  "O_xsec_abs_piplus":  array("d", [33., 36., 35.])
}



f.cd()
for name, reac in zip(reacs.keys(), reacs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], reac, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
