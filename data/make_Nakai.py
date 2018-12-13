from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Nakai.root", "RECREATE")

Ts = [23., 37., 52., 68., 83., 100., 125., 151., 187., 233., 280.]

mom = array( "d", [ get_p(T) for T in Ts ])
print mom

moms = {
  "Al_xsec_abs_piplus":   mom,
  "Al_xsec_abs_piminus":  mom[1:],
  "Cu_xsec_abs_piplus":   mom,
  "Cu_xsec_abs_piminus":  mom[1:]
}


xsecs = {
  "Al_xsec_abs_piplus":   array("d", [102., 148., 220., 260., 282., 275., 256., 259., 256., 211., 187.]), 
  "Al_xsec_abs_piminus":  array("d", [253., 307., 304., 303., 306., 292., 267., 217., 225., 170.]), 
  "Cu_xsec_abs_piplus":   array("d", [332., 379., 490., 521., 554., 552., 571., 557., 540., 463., 412.]), 
  "Cu_xsec_abs_piminus":  array("d", [736., 758., 712., 695., 683., 655., 584., 495., 449., 400.]) 
}


errs = {
  "Al_xsec_abs_piplus":   array("d", [15., 15., 15., 20., 20., 29., 20., 29., 20., 20., 25.]), 
  "Al_xsec_abs_piminus":  array("d", [20., 20., 20., 20., 25., 15., 20., 35., 25., 15.]), 
  "Cu_xsec_abs_piplus":   array("d", [35., 29., 25., 25., 20., 20., 30., 25., 30., 45., 30.]), 
  "Cu_xsec_abs_piminus":  array("d", [45., 20., 20., 15., 20., 20., 25., 30., 45., 45.])  
}

exs = {
  "Al_xsec_abs_piplus":   array("d", [0.]*len(mom)), 
  "Al_xsec_abs_piminus":  array("d", [0.]*(len(mom)-1)), 
  "Cu_xsec_abs_piplus":   array("d", [0.]*len(mom)), 
  "Cu_xsec_abs_piminus":  array("d", [0.]*(len(mom)-1))  
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
