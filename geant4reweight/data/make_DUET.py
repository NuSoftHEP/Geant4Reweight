from ROOT import * 
from array import array 
#from numpy import sign
from math import sqrt

f = TFile("DUET.root", "RECREATE")

x = array("d", [201.6, 216.6, 237.2, 265.6, 295.1])
ex = array("d", [0.]*5)

y_abs = array("d", [153.8, 182.1, 160.8, 161.4, 159.4])
ey_abs = array("d", [12.0, 19.2, 16.6, 15.7, 15.3])

y_cex = array("d", [44.0, 33.8, 55.8, 63.5, 52.0])
ey_cex = array("d", [7.9, 10.2, 10.8, 10.8, 9.3])


f.cd()

g_abs = TGraphErrors(5, x, y_abs, ex, ey_abs)
g_cex = TGraphErrors(5, x, y_cex, ex, ey_cex)

g_abs.SetName("xsec_abs")
g_cex.SetName("xsec_cex")

g_abs.Write()
g_cex.Write()


#taken from DUET paper. The commented block includes the correlation matrix portion

cov_vals = [ 
 # .19, .19, .23, .30, .41, -.26, .13, -.04, -.13, -.28,
 # .10, .30, .17, .19, .23, -.08, -.06, -.13, -.14, -.16,
 # .09, .10, .19, .40, .42, -.11, -.15, -.29, -.28, -.25,
 # .10, .10, .11, .17, .47, -.20, -.14, -.27, -.35, -.23,
 # .12, .11, .12, .12, .17, -.23, -.04, -.27, -.29, -.46,
 # -.06, -.04, -.04, -.05, -.06, .08, .08, .22, .32, .35,
 # .05, -.04, -.05, -.05, -.03, .03, .10, .36, .32, .09,
 # -.03, -.06, -.07, -.07, -.07, .04, .06, .10, .44, .33,
 # -.05, -.06, -.07, -.08, -.07, .05, .06, .06, .10, .35,
 # -.07, -.07, -.07, -.06, -.09, .05, .03, .06, .06, .10  
  .19, .10, .09, .10, .12, -.06, .05, -.03, -.05, -.07,
  .10, .30, .10, .10, .11, -.04, -.04, -.06, -.06, -.07,
  .09, .10, .19, .11, .12, -.04, -.05, -.07, -.07, -.07,
  .10, .10, .11, .17, .12, -.05, -.05, -.07, -.08, -.06,
  .12, .11, .12, .12, .17, -.06, -.03, -.07, -.07, -.09,
  -.06, -.04, -.04, -.05, -.06, .08, .03, .04, .05, .05,
  .05, -.04, -.05, -.05, -.03, .03, .10, .06, .06, .03,
  -.03, -.06, -.07, -.07, -.07, .04, .06, .10, .06, .06,
  -.05, -.06, -.07, -.08, -.07, .05, .06, .06, .10, .06,
  -.07, -.07, -.07, -.06, -.09, .05, .03, .06, .06, .10  
]

##need to square it
cov_vals = [(c/sqrt(c*c))*c*c for c in cov_vals]

#sign not currently working, find replacement

#s = c/sqrt(c*c)

#also, need to convert from fractional to full covariance:
for i in range(0,10):
  if( i < 5 ): i_val = y_cex[i]
  else: i_val = y_abs[i - 5]

  for j in range(0,10):
    if( j < 5 ): j_val = y_cex[j]
    else: j_val = y_abs[j - 5]
    cov_vals[i + j*10] = cov_vals[i+j*10]*i_val*j_val


cov = TMatrixD(10,10)
cov.SetMatrixArray( array("d", cov_vals) )
cov.Write("cov")
