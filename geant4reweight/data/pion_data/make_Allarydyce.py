from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Allarydyce.root", "RECREATE")


infos = [
  ["pb_reac_piM_Allardyce.csv", ["Pb", "reac", "piminus"] ],
  ["pb_reac_piP_Allardyce.csv", ["Pb", "reac", "piplus"] ],
  ["c_reac_piM_Allardyce.csv",  ["C", "reac", "piminus"] ],
  ["c_reac_piP_Allardyce.csv",  ["C", "reac", "piplus"] ],
  ["al_reac_piM_Allardyce.csv", ["Al", "reac", "piminus"] ],
  ["al_reac_piP_Allardyce.csv", ["Al", "reac", "piplus"] ]
]
  
f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()
