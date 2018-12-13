from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Saunders.root", "RECREATE")

Ts = [42., 45., 46.5, 48., 49.5, 50., 54., 65.]

mom = array( "d", [ get_p(T) for T in Ts ])
print mom

moms = {
  "C_xsec_reac_piplus":   mom,
  "C_xsec_reac_piminus":  mom[5:6],
  "Al_xsec_reac_piplus":  mom[5:6],
  "Al_xsec_reac_piminus": mom[5:6]
}


xsecs = {
  "C_xsec_reac_piplus":   array("d", [125., 135., 137., 140., 158., 152., 147., 202.]), 
  "C_xsec_reac_piminus":  array("d", [177.6]), 
  "Al_xsec_reac_piplus":  array("d", [359.1]), 
  "Al_xsec_reac_piminus": array("d", [550.8]) 
}


errs = {
  "C_xsec_reac_piplus":   array("d", [14., 15., 14., 20., 19., 14., 14., 17.]), 
  "C_xsec_reac_piminus":  array("d", [24.]), 
  "Al_xsec_reac_piplus":  array("d", [35.1]), 
  "Al_xsec_reac_piminus": array("d", [59.4]) 
}

exs = {
  "C_xsec_reac_piplus":   array("d", [0.]*8), 
  "C_xsec_reac_piminus":  array("d", [0.]), 
  "Al_xsec_reac_piplus":  array("d", [0.]), 
  "Al_xsec_reac_piminus": array("d", [0.]) 
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
