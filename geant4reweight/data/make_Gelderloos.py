from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p
f = TFile("Gelderloos.root", "RECREATE")

moms = array("d", [get_p(410.), get_p(464.), get_p(492.)] )
exs = array("d", [0.]*3) 

reacs = {
  "C":     array("d", [255., 244., 224.]), 
  "Al":    array("d", [456., 451., 420.]),
  "Cu":    array("d", [827., 825., 847.]), 
  "Pb":    array("d", [1922., 1888., 2005.]) 
}

'''errs = {
  "C":     array("d", [11., 10., 9.]), 
  "Al":    array("d", [23., 27., 24.]),
  "Cu":    array("d", [35., 36., 34.]), 
  "Pb":    array("d", [90., 86., 140.]) 
}'''

#Alternate errors. See Gelderloos 
errs = {
  "C":     array("d", [15., 14., 12.]), 
  "Al":    array("d", [26., 44., 29.]),
  "Cu":    array("d", [37., 42., 35.]), 
  "Pb":    array("d", [95., 90., 236.]) 
}


f.cd()

for nuc in reacs.keys():
  gr = TGraphErrors(3, moms, reacs[nuc], exs, errs[nuc])
  gr.SetName(nuc + "_xsec_reac_piminus")
  gr.Write()

