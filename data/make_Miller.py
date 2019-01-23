from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Miller.root", "RECREATE")

Ts = [150.]

mom = array( "d", [ get_p(T) for T in Ts ])
print mom

moms = {
  "C_xsec_abscx_piminus":  mom,
  "Pb_xsec_abscx_piminus": mom
}


xsecs = {
  "C_xsec_abscx_piminus":  array("d", [192.]), 
  "Pb_xsec_abscx_piminus": array("d", [380.]) 
}


errs = {
  "C_xsec_abscx_piminus":  array("d", [34.]), 
  "Pb_xsec_abscx_piminus": array("d", [310.]) 
}

exs = {
  "C_xsec_abscx_piminus":  array("d", [0.]), 
  "Pb_xsec_abscx_piminus": array("d", [0.]) 
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
