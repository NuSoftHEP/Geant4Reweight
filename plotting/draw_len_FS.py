from ROOT import * 
import sys


f = TFile(sys.argv[1], "OPEN")

print sys.argv[3]
print type(sys.argv[3])

tree = f.Get("tree")

cuts = {
  "reac": "(sqrt(Energy*Energy - 139.57*139.57) < 300. && preFinalP > 200. && int == \"pi+Inelastic\")",
  "inel": "(sqrt(Energy*Energy - 139.57*139.57) < 300. && preFinalP > 200. && int == \"pi+Inelastic\" && nPiPlus == 1 && (nPi0 + nPiMinus) == 0 )",
  "abs":  "(sqrt(Energy*Energy - 139.57*139.57) < 300. && preFinalP > 200. && int == \"pi+Inelastic\" && (nPiPlus + nPi0 + nPiMinus) == 0 )",
  "cex":  "(sqrt(Energy*Energy - 139.57*139.57) < 300. && preFinalP > 200. && int == \"pi+Inelastic\" && nPi0 == 1 && (nPiPlus + nPiMinus) == 0 )",
  "dcex": "(sqrt(Energy*Energy - 139.57*139.57) < 300. && preFinalP > 200. && int == \"pi+Inelastic\" && nPiMinus == 1 && (nPi0 + nPiPlus) == 0 )",
  "prod": "(sqrt(Energy*Energy - 139.57*139.57) < 300. && preFinalP > 200. && int == \"pi+Inelastic\" && (nPiPlus + nPi0 + nPiMinus) > 1 )",

}


gROOT.SetBatch(1)

gStyle.SetOptStat(0)

if ( "0" in sys.argv[3]): weight_str = "*weight*finalStateWeight"
else: weight_str = "*altFSWeight"
print weight_str

for name,cut in cuts.iteritems():
  tree.Draw("len>>n" + name + "(50,0,100)", cut, "goff")
  tree.Draw("len>>w" + name + "(50,0,100)", cut + weight_str, "goff")

  n = gDirectory.Get("n" + name) 
  w = gDirectory.Get("w" + name) 

  if n.Integral() == 0: continue

  n.Scale( 1./ n.Integral() )
  w.Scale( 1./ w.Integral() )

  w.SetLineColor(2) 
  w.SetMinimum(0.)
  w.Draw()
  n.Draw("same")

  c1.SaveAs(name + "_" + sys.argv[2])

  w.Divide(n)
  w.Draw("hist")

  print name, "ratio:", w.Integral()
  c1.SaveAs("ratio_" + name  + "_1D_"+ sys.argv[2])


  tree.Draw("len:sqrt(Energy*Energy - 139.57*139.57)>>n" + name + "2D", cut, "goff colz")
  tree.Draw("len:sqrt(Energy*Energy - 139.57*139.57)>>w" + name + "2D", cut + weight_str, "goff colz")
  n2D = gDirectory.Get("n" + name + "2D") 
  w2D = gDirectory.Get("w" + name + "2D") 


  n2D.Draw("colz")
  c1.SaveAs("nom_" + name + "_2D_" + sys.argv[2])
  w2D.Draw("colz")
  c1.SaveAs("w_" + name + "_2D_" + sys.argv[2])

  w2D.Divide(n2D)
  w2D.Draw("colz")
  c1.SaveAs("ratio_" + name + "_2D_" + sys.argv[2])
