from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Ashery_946.root", "RECREATE")

infos = [
  ["o_cx_piM_Ashery.csv", ["O", "cex", "piminus"] ],
  ["o_cx_piP_Ashery.csv", ["O", "cex", "piplus"] ],
  ["c_cx_piM_Ashery.csv",  ["C", "cex", "piminus"] ],
  ["c_cx_piP_Ashery.csv",  ["C", "cex", "piplus"] ],
  ["pb_cx_piM_Ashery.csv", ["Pb", "cex", "piminus"] ],
  ["pb_cx_piP_Ashery.csv", ["Pb", "cex", "piplus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()
