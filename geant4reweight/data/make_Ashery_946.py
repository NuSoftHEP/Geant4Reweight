from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Ashery_946.root", "RECREATE")

Ts = [160.]

mom = array( "d", [ get_p(T) for T in Ts ])
print mom

moms = {
  "C_xsec_cex_piplus":   mom,
  "C_xsec_cex_piminus":  mom,
  "O_xsec_cex_piplus":   mom,
  "O_xsec_cex_piminus":  mom,
  "Pb_xsec_cex_piplus":  mom,
  "Pb_xsec_cex_piminus": mom
}


xsecs = {
  "C_xsec_cex_piplus":   array("d", [64.]), 
  "C_xsec_cex_piminus":  array("d", [64.]), 
  "O_xsec_cex_piplus":   array("d", [80.]), 
  "O_xsec_cex_piminus":  array("d", [80.]), 
  "Pb_xsec_cex_piplus":  array("d", [252.]), 
  "Pb_xsec_cex_piminus": array("d", [113.]) 
}


errs = {
  "C_xsec_cex_piplus":   array("d", [10.]), 
  "C_xsec_cex_piminus":  array("d", [10.]), 
  "O_xsec_cex_piplus":   array("d", [12.]), 
  "O_xsec_cex_piminus":  array("d", [12.]), 
  "Pb_xsec_cex_piplus":  array("d", [30.]), 
  "Pb_xsec_cex_piminus": array("d", [16.]) 
}

exs = {
  "C_xsec_cex_piplus":   array("d", [0.]), 
  "C_xsec_cex_piminus":  array("d", [0.]), 
  "O_xsec_cex_piplus":   array("d", [0.]), 
  "O_xsec_cex_piminus":  array("d", [0.]), 
  "Pb_xsec_cex_piplus":  array("d", [0.]), 
  "Pb_xsec_cex_piminus": array("d", [0.]) 
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
