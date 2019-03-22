from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Bellotti.root", "RECREATE")

infos = [
  ["c_abs_piP_Belloti.csv", ["C", "abs", "piplus"] ],
  ["c_cx_piP_Belloti2.csv", ["C", "cex", "piplus"] ],
]
f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()

