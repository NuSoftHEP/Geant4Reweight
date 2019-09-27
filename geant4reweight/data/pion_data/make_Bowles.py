from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Bowles.root", "RECREATE")

infos = [
  ["o_cx_piP_Bowles.csv", ["O","cex", "piplus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()
