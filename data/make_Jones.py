from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Jones.root", "RECREATE")

Ts = [250., 300., 400., 500.]

mom = array( "d", [ get_p(T) for T in Ts ])
print mom

moms = {
  "C_xsec_inel_piplus":  mom[:2],
  "Pb_xsec_inel_piplus": mom[:2],
  "C_xsec_cex_piplus":   mom,
  "Pb_xsec_cex_piplus":  mom
}


xsecs = {
  "C_xsec_inel_piplus":  array("d", [230., 250.]), 
  "Pb_xsec_inel_piplus": array("d", [870., 1000.]), 
  "C_xsec_cex_piplus":   array("d", [46., 55., 59., 91.]), 
  "Pb_xsec_cex_piplus":  array("d", [220., 260., 290., 550.])
}


errs = {
  "C_xsec_inel_piplus":  array("d", [23., 25.]), 
  "Pb_xsec_inel_piplus": array("d", [87., 100.]), 
  "C_xsec_cex_piplus":   array("d", [4.6, 5.5, 5.9, 9.1]), 
  "Pb_xsec_cex_piplus":  array("d", [22., 26., 29., 55.])
}

exs = {
  "C_xsec_inel_piplus":  array("d", [0.]*2), 
  "Pb_xsec_inel_piplus": array("d", [0.]*2), 
  "C_xsec_cex_piplus":   array("d", [0.]*4), 
  "Pb_xsec_cex_piplus":  array("d", [0.]*4)  
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
