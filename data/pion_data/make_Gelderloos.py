from ROOT import * 
from array import array 
from make_graph import make_graph
f = TFile("Gelderloos.root", "RECREATE")

infos = [
  ["cu_reac_piM_Gelderloos.csv", ["Cu", "reac", "piminus"] ],
  ["c_reac_piM_Gelderloos.csv",  ["C", "reac", "piminus"] ],
  ["al_reac_piM_Gelderloos.csv", ["Al", "reac", "piminus"] ],
  ["pb_reac_piM_Gelderloos.csv", ["Pb", "reac", "piminus"] ]

]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()
