from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Bellotti.root", "RECREATE")

Ts = [130.]

mom = array( "d", [ get_p(T) for T in Ts ])
print mom

moms = {
  "C_xsec_abs_piplus":  mom,
  "C_xsec_cex_piplus":  mom
}


xsecs = {
  "C_xsec_abs_piplus":  array("d", [189.]), 
  "C_xsec_cex_piplus":  array("d", [36.])
}


errs = {
  "C_xsec_abs_piplus":  array("d", [19.]), 
  "C_xsec_cex_piplus":  array("d", [5.])
}

exs = {
  "C_xsec_abs_piplus":  array("d", [0.]), 
  "C_xsec_cex_piplus":  array("d", [0.]) 
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
