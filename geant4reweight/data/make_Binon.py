from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Binon.root", "RECREATE")

Ts = [
 69.5, 87.5, 120., 150., 180., 
 200., 230., 260., 280.
]

moms = array( "d", [ sqrt( (T+139.57)*(T+139.57) - 139.57*139.57) for T in Ts ])


exs = array("d", [0.]*9)
reacs = array("d", [323., 358., 429., 441., 423., 396., 359., 324., 311.]) 
errs = array("d", [36., 28., 18., 18., 13., 13., 18., 19., 19.]) 

f.cd()

gr = TGraphErrors(len(moms), moms, reacs, exs, errs)
gr.SetName("C_xsec_reac_piminus")
gr.Write()

