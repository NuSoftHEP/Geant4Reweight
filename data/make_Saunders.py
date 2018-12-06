from ROOT import * 
from array import array 
from math import sqrt

f = TFile("Saunders.root", "RECREATE")

Ts = [42., 45., 46.5, 48., 49.5, 50., 54., 65.]
moms = array( "d", [ sqrt( (T+139.57)*(T+139.57) - 139.57*139.57) for T in Ts ])


exs = array("d", [0.]*8)
reacs = array("d", [125., 135., 137., 140., 158., 152., 147., 202.]) 
errs = array("d", [14., 15., 14., 20., 19., 14., 14., 17.])  



f.cd()

gr = TGraphErrors(len(moms), moms, reacs, exs, errs)
gr.SetName("C_xsec_reac_piplus")
gr.Write()

moms = array("d", [sqrt( (50.+139.57)*(50.+139.57) - 139.57*139.57)])
reacs = array("d", [12*14.8])
exs = array("d", [0])
errs = array("d", [24.])

gr = TGraphErrors(1, moms, reacs, exs, errs)
gr.SetName("C_xsec_reac_piminus")
gr.Write()

