from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Meirav.root", "RECREATE")

infos = [
  ["c_reac_piM_Meirav.csv",  ["C", "reac", "piminus"] ],
  ["c_reac_piP_Meirav.csv",  ["C", "reac", "piplus"] ],
  ["o_reac_piM_Meirav.csv",  ["O", "reac", "piminus"] ],
  ["o_reac_piP_Meirav.csv",  ["O", "reac", "piplus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()

