from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Meirav.root", "RECREATE")

Ts = [50., 65., 80.]
mom = array( "d", [ get_p(T) for T in Ts ])
print mom
moms = {
  "C_xsec_reac_piplus":  mom,
  "C_xsec_reac_piminus": mom,
  "O_xsec_reac_piplus":  mom[:1],
  "O_xsec_reac_piminus": mom[:1] 
}


exs = {
  "C_xsec_reac_piplus":  array("d", [0.]*3),
  "C_xsec_reac_piminus": array("d", [0.]*3),
  "O_xsec_reac_piplus":  array("d", [0.]),
  "O_xsec_reac_piminus": array("d", [0.])
}


reacs = {
  "C_xsec_reac_piplus":  array("d", [150., 201., 240.]),
  "C_xsec_reac_piminus": array("d", [193., 251., 280.]),
  "O_xsec_reac_piplus":  array("d", [166.]),
  "O_xsec_reac_piminus": array("d", [242.])
}

errs = {
  "C_xsec_reac_piplus":  array("d", [15., 16., 12.]),
  "C_xsec_reac_piminus": array("d", [10., 20., 20.]),
  "O_xsec_reac_piplus":  array("d", [19.]),
  "O_xsec_reac_piminus": array("d", [21.])
}



f.cd()
for name, reac in zip(reacs.keys(), reacs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], reac, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
