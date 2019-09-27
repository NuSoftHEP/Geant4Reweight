from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Jones.root", "RECREATE")


infos = [
  ["c_cx_piP_Jones.csv",  ["C", "cex", "piplus"] ],
#  ["pb_cx_piP_Jones.csv", ["Pb", "cex", "piplus"] ],

  ["c_inel_piP_Jones.csv",  ["C", "inel", "piplus"] ],
  ["pb_inel_piP_Jones.csv", ["Pb", "inel", "piplus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()


