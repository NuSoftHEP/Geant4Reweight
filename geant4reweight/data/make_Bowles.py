from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Bowles.root", "RECREATE")

Ts = [50., 100.]

mom = array( "d", [ get_p(T) for T in Ts ])
print mom

moms = {
  "O_xsec_cex_piplus":   mom,
}


xsecs = {
  "O_xsec_cex_piplus":   array("d", [21., 66.])
}


errs = {
  "O_xsec_cex_piplus":   array("d", [3., 10.])
}

exs = {
  "O_xsec_cex_piplus":   array("d", [0., 0.])
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
