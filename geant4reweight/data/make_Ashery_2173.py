from ROOT import * 
from array import array 
from math import sqrt
from get_p import get_p

f = TFile("Ashery_2173.root", "RECREATE")

Ts = [85., 125., 165., 205., 245., 315.]


mom = array( "d", [ get_p(T) for T in Ts ])
print mom

#elastic cross sections added by C Thorpe

moms = {
  "C_xsec_elast_piplus":   mom,
  "C_xsec_elast_piminus":  mom[1:3],
  "C_xsec_abscx_piplus":   mom,
  "C_xsec_abscx_piminus":  mom[1:3],
  "C_xsec_inel_piplus":    mom,
  "C_xsec_inel_piminus":   mom[1:3],
   "Al_xsec_elast_piplus":   mom,
  "Al_xsec_elast_piminus":  mom[1:3],  
"Al_xsec_abscx_piplus":  mom,
  "Al_xsec_abscx_piminus": mom[1:3],
  "Al_xsec_inel_piplus":   mom,
  "Al_xsec_inel_piminus":  mom[1:3],

  "Al_xsec_total_piminus": mom[1:3],

   "Fe_xsec_elast_piplus":   mom,
   "Fe_xsec_elast_piminus":  mom[1:3],  
"Fe_xsec_abscx_piplus":  mom,
  "Fe_xsec_abscx_piminus": mom[1:3],
  "Fe_xsec_inel_piplus":   mom,
  "Fe_xsec_inel_piminus":  mom[1:3],

 "Fe_xsec_total_piminus": mom[1:3]
}


xsecs = {
 "C_xsec_elast_piplus": array("d", [178., 221., 223., 214., 186., 136.]),
 "C_xsec_elast_piminus": array("d", [244., 242.]),
 "C_xsec_abscx_piplus":   array("d", [144., 204., 240., 202., 142., 109.]),
  "C_xsec_abscx_piminus":  array("d", [253., 249.]),
  "C_xsec_inel_piplus":    array("d", [143., 213., 207., 210., 224., 200.]),
  "C_xsec_inel_piminus":   array("d", [176., 193.]),
 "Al_xsec_elast_piplus": array("d", [372., 400., 411., 411., 382., 311.]),
 "Al_xsec_elast_piminus": array("d", [465., 466.]),
  "Al_xsec_abscx_piplus":  array("d", [307., 398., 410., 385., 282., 192.]),
  "Al_xsec_abscx_piminus": array("d", [464., 449.]),
  "Al_xsec_inel_piplus":   array("d", [331., 342., 321., 269., 296., 307.]),
  "Al_xsec_inel_piminus":  array("d", [205., 207.]),
  "Al_xsec_total_piminus":  array("d", [1134., 1122.]),  
 "Fe_xsec_elast_piplus": array("d", [616., 661., 686., 700., 674., 593.]),
 "Fe_xsec_elast_piminus": array("d", [822., 823.]),
 "Fe_xsec_abscx_piplus":  array("d", [500., 610., 680., 690., 506., 418.]),
  "Fe_xsec_abscx_piminus": array("d", [728., 741.]),
  "Fe_xsec_inel_piplus":   array("d", [784., 644., 474., 360., 430., 389.]),
  "Fe_xsec_inel_piminus":  array("d", [398., 304.]),
  "Fe_xsec_total_piminus": array("d", [1948., 1868.])

}


errs = {
  "C_xsec_elast_piplus":   array("d", [18., 22., 22., 21., 20., 14.]), 
  "C_xsec_elast_piminus":  array("d", [24.,24.]), 
 "C_xsec_abscx_piplus":   array("d", [15., 23., 28., 29., 22., 16.]),
  "C_xsec_abscx_piminus":  array("d", [24., 26.]),
  "C_xsec_inel_piplus":    array("d", [26., 33., 33., 51., 30., 22.]),
  "C_xsec_inel_piminus":   array("d", [37., 37.]),
  "Al_xsec_elast_piplus":   array("d", [37., 40., 41., 41., 38., 32.]), 
  "Al_xsec_elast_piminus":  array("d", [47.,47.]), 
  "Al_xsec_abscx_piplus":  array("d", [29., 40., 44., 48., 34., 25.]),
  "Al_xsec_abscx_piminus": array("d", [39., 47.]),
  "Al_xsec_inel_piplus":   array("d", [56., 70., 72., 72., 62., 54.]),
  "Al_xsec_inel_piminus":  array("d", [73., 78.]),
  "Al_xsec_total_piminus": array("d",[40., 40.]),
  "Fe_xsec_elast_piplus":   array("d", [61., 70., 70., 70., 70., 60.]), 
  "Fe_xsec_elast_piminus":  array("d", [82.,82.]), 
  "Fe_xsec_abscx_piplus":  array("d", [55., 61., 71., 76., 50., 36.]),
  "Fe_xsec_abscx_piminus": array("d", [61., 73.]),
  "Fe_xsec_inel_piplus":   array("d", [115., 123., 130., 125., 110., 100.]),
  "Fe_xsec_inel_piminus":  array("d", [130., 136.]),
  "Fe_xsec_total_piminus": array("d", [80., 80.])
}

exs = {
 "C_xsec_elast_piplus":  array("d",[0.]*6),
  "C_xsec_elast_piminus": array("d",[0.]*2),  
"C_xsec_abscx_piplus":   array("d", [0.]*6),
  "C_xsec_abscx_piminus":  array("d", [0.]*2),
  "C_xsec_inel_piplus":    array("d", [0.]*6),
  "C_xsec_inel_piminus":   array("d", [0.]*2),
 "Al_xsec_elast_piplus":  array("d",[0.]*6),
  "Al_xsec_elast_piminus": array("d",[0.]*2),  
   "Al_xsec_abscx_piplus":  array("d", [0.]*6),
  "Al_xsec_abscx_piminus": array("d", [0.]*2),
  "Al_xsec_inel_piplus":   array("d", [0.]*6),
  "Al_xsec_inel_piminus":  array("d", [0.]*2),
  "Al_xsec_total_piminus": array("d",[0.]*2), 
"Fe_xsec_elast_piplus":  array("d",[0.]*6),
  "Fe_xsec_elast_piminus": array("d",[0.]*2),
  "Fe_xsec_abscx_piplus":  array("d", [0.]*6),
  "Fe_xsec_abscx_piminus": array("d", [0.]*2),
  "Fe_xsec_inel_piplus":   array("d", [0.]*6),
  "Fe_xsec_inel_piminus":  array("d", [0.]*2),
  "Fe_xsec_total_piminus": array("d",[0.]*2)
}



f.cd()
for name, xsec in zip(xsecs.keys(), xsecs.values()):
  ex  = exs[name] 
  err = errs[name]
  gr = TGraphErrors(len(moms[name]), moms[name], xsec, ex, err)
  gr.SetName(name)
  gr.Write()
f.Close()
