from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Levenson.root", "RECREATE")

infos = [
  ["c_inel_piP_Levenson.csv",  ["C", "inel", "piplus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()

