from ROOT import * 
import sys

fin = TFile(sys.argv[1], "OPEN")
fout = TFile(sys.argv[2], "RECREATE")

tree = fin.Get("tree")

fout.cd()

cuts = {
  "inel": "int == \"pi+Inelastic\" && nPiPlus == 1 && (nPi0 + nPiMinus) == 0",
  "abs":  "int == \"pi+Inelastic\" && (nPiPlus + nPi0 + nPiMinus) == 0",
  "cex":  "int == \"pi+Inelastic\" && nPi0 == 1 && (nPiPlus + nPiMinus) == 0",
  "dcex": "int == \"pi+Inelastic\" && nPiMinus == 1 && (nPiPlus + nPi0) == 0",
  "prod": "int == \"pi+Inelastic\" && (nPiPlus + nPi0 + nPiMinus) > 1"
 
}

for name,cut in cuts.iteritems():
  tree.Draw("sqrt(Energy*Energy - 139.57*139.57)>>" + name + "(10,200,300)", cut, "goff")
  h = gDirectory.Get(name)
  h.Write()

fout.Close()
