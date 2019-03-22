from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Hilscher.root", "RECREATE")

infos = [
  ["c_cx_piM_Ashery.csv",  ["C", "cex", "piminus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()

