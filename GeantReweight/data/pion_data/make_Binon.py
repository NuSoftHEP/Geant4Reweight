from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Binon.root", "RECREATE")


infos = [
  [ "c_reac_piM_Binon.csv", ["C", "reac", "piminus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()

