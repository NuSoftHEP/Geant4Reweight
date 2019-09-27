from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Navon.root", "RECREATE")

infos = [
#  ["fe_abscx_piM_Navon.csv", ["Fe", "abscx", "piminus"] ],
  ["fe_abscx_piP_Navon.csv", ["Fe", "abscx", "piplus"] ],
#  ["c_abscx_piM_Navon.csv",  ["C", "abscx", "piminus"] ],
  ["c_abscx_piP_Navon.csv",  ["C", "abscx", "piplus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()
