from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Levenson.root", "RECREATE")

Ts = [100., 160., 220., 300.]
mom = array( "d", [ get_p(T) for T in Ts ])
print mom
moms = {
  "C_xsec_inel_piplus":  mom
}


exs = {
  "C_xsec_inel_piplus":  array("d", [0.]*4),
}


xsecs = {
  "C_xsec_inel_piplus":  array("d", [150., 280., 240., 169.]),
}

errs = {
  "C_xsec_inel_piplus":  array("d", [45., 56., 48., 50.7 ]),
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
