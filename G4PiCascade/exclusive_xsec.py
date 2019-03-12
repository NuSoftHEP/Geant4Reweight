from ROOT import * 
from array import array
import sys

fout = TFile(sys.argv[3], "RECREATE")

total_file = TFile(sys.argv[1], "OPEN")
frac_file  = TFile(sys.argv[2], "OPEN")

total_inel = total_file.Get("inel_momentum")
total_xs = [ total_inel.GetX()[i] for i in range(0,total_inel.GetN()) ]
total_ys = [ total_inel.GetY()[i] for i in range(0,total_inel.GetN()) ]

cuts = ["abs","inel","cex","dcex","prod"]

fracs = dict()
xsecs = dict()
for cut in cuts:
  frac = frac_file.Get( cut )
  xsec_xs = []
  xsec_ys = []

  for x,y in zip(total_xs,total_ys): 
    if( x > frac.GetX()[frac.GetN() - 1] ): break
    if( x < frac.GetX()[0] ): continue
    xsec_xs.append( x )
    if( y < 0. ): print "warning", x,y
    if( frac.Eval( x ) < 0. ): print "eval", x,frac.Eval( x )
    xsec_ys.append( y * frac.Eval( x ) )
  xsecs[ cut ] = TGraph(len(xsec_xs), array("d", xsec_xs), array("d", xsec_ys))
  fout.cd()
  xsecs[ cut ].Write( cut )
  

fout.cd()
total_inel.Write("total_inel")
fout.Close()

  
