from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Miller.root", "RECREATE")

infos = [
  ["pb_abscx_piM_Miller.csv", ["Pb", "abscx", "piminus"] ],
  ["c_abscx_piM_Miller.csv",  ["C", "abscx", "piminus"] ],
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()

