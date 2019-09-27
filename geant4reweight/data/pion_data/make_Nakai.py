from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Nakai.root", "RECREATE")

infos = [
  ["cu_abs_piM_Nakai.csv",  ["Cu", "abs", "piminus"] ],
  ["cu_abs_piP_Nakai.csv",  ["Cu", "abs", "piplus"] ],
  ["al_abs_piM_Nakai.csv", ["Al", "abs", "piminus"] ],
  ["al_abs_piP_Nakai.csv", ["Al", "abs", "piplus"] ]

]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()
