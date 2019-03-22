from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Saunders.root", "RECREATE")

infos = [
  ["c_reac_piM_Saunders.csv",  ["C", "reac", "piminus"] ],
  ["c_reac_piP_Saunders.csv",  ["C", "reac", "piplus"] ],
  ["al_reac_piM_Saunders.csv", ["Al", "reac", "piminus"] ],
  ["al_reac_piP_Saunders.csv", ["Al", "reac", "piplus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()
