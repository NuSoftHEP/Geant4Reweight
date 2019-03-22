from ROOT import * 
from array import array 
from make_graph import make_graph


f = TFile("Ingram.root", "RECREATE")

infos = [
  ["o_inel_piP_Ingram.csv", ["O", "inel", "piplus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()

