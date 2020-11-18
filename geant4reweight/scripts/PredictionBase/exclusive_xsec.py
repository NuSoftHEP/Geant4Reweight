import ROOT as RT
from array import array
import sys
import math


def mom_to_ke( grin ):
  xs = [math.sqrt(grin.GetX()[i]*grin.GetX()[i] + 139.57*139.57 ) - 139.57 for i in range(0,grin.GetN())] 
  ys = [grin.GetY()[i] for i in range(0,grin.GetN())] 
  print(xs)
  grout = RT.TGraph( len(xs), array("d", xs), array("d", ys) )
  return grout

fout = RT.TFile(sys.argv[3], "RECREATE")

total_file = RT.TFile(sys.argv[1], "OPEN")
frac_file  = RT.TFile(sys.argv[2], "OPEN")


grabs = ["momentum", "KE"]

for grab in grabs:
  total_inel = total_file.Get("inel_" + grab)
  total_xs = [ total_inel.GetX()[i] for i in range(0,total_inel.GetN()) ]
  total_ys = [ total_inel.GetY()[i] for i in range(0,total_inel.GetN()) ]
  
  cuts = ["abs","inel","cex","dcex","prod"]
  
  fracs = dict()
  xsecs = dict()
  for cut in cuts:
    if( grab == "momentum" ): frac = frac_file.Get( cut )
    elif( grab == "KE" ): frac = mom_to_ke( frac_file.Get( cut ) )
    xsec_xs = []
    xsec_ys = []
  
    for x,y in zip(total_xs,total_ys): 
      if( x > frac.GetX()[frac.GetN() - 1] ): break
      if( x < frac.GetX()[0] ): continue
      xsec_xs.append( x )
      if( y < 0. ): print("warning", x,y)
      if( frac.Eval( x ) < 0. ): print("eval", x,frac.Eval( x ))
      xsec_ys.append( y * frac.Eval( x ) )
    print(xsec_xs[-1])
    xsecs[ cut ] = RT.TGraph(len(xsec_xs), array("d", xsec_xs), array("d", xsec_ys))
    fout.cd()
    xsecs[ cut ].Write( cut + "_" + grab)
    
  
  fout.cd()
  total_inel.Write("total_inel_" + grab)
fout.Close()

  
