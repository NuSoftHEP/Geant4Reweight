from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Hilscher.root", "RECREATE")

Ts = [70.]

mom = array( "d", [ get_p(T) for T in Ts ])
print mom

moms = {
  "C_xsec_cex_piminus":  mom,
  "Al_xsec_cex_piminus": mom,
  "Cu_xsec_cex_piminus": mom,
  "Pb_xsec_cex_piminus": mom
}


xsecs = {
  "C_xsec_cex_piminus":  array("d", [27.5]), 
  "Al_xsec_cex_piminus": array("d", [39.5]), 
  "Cu_xsec_cex_piminus": array("d", [44.3]), 
  "Pb_xsec_cex_piminus": array("d", [58.7]) 
}


errs = {
  "C_xsec_cex_piminus":  array("d", [8.7]), 
  "Al_xsec_cex_piminus": array("d", [12.4]), 
  "Cu_xsec_cex_piminus": array("d", [13.9]), 
  "Pb_xsec_cex_piminus": array("d", [18.5]) 
}
'''errs = {
  "C_xsec_cex_piminus":  array("d", [2.8]), 
  "Al_xsec_cex_piminus": array("d", [3.7]), 
  "Cu_xsec_cex_piminus": array("d", [4.1]), 
  "Pb_xsec_cex_piminus": array("d", [5.7]) 
}'''

exs = {
  "C_xsec_cex_piminus":  array("d", [0.]),
  "Al_xsec_cex_piminus": array("d", [0.]),
  "Cu_xsec_cex_piminus": array("d", [0.]),
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
