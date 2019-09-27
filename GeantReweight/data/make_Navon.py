from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Navon.root", "RECREATE")

Ts = [50.]

mom = array( "d", [ get_p(T) for T in Ts ])
print mom

moms = {
  "C_xsec_abscx_piplus":   mom,
  "C_xsec_abscx_piminus":  mom,
  "Fe_xsec_abscx_piplus":  mom,
  "Fe_xsec_abscx_piminus": mom
}


xsecs = {
  "C_xsec_abscx_piplus":    array("d", [106.]), 
  "C_xsec_abscx_piminus":   array("d", [238.]), 
  "Fe_xsec_abscx_piplus":   array("d", [478.]), 
  "Fe_xsec_abscx_piminus":  array("d", [1232.]) 
}


errs = {
  "C_xsec_abscx_piplus":    array("d", [21.]), 
  "C_xsec_abscx_piminus":   array("d", [24.]), 
  "Fe_xsec_abscx_piplus":   array("d", [48.]), 
  "Fe_xsec_abscx_piminus":  array("d", [92.])  
}

exs = {
  "C_xsec_abscx_piplus":    array("d", [0.]), 
  "C_xsec_abscx_piminus":   array("d", [0.]), 
  "Fe_xsec_abscx_piplus":   array("d", [0.]), 
  "Fe_xsec_abscx_piminus":  array("d", [0.])  
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
