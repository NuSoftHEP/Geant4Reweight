from ROOT import * 
from array import array 
from make_graph import make_graph

f = TFile("Ashery_2173.root", "RECREATE")

infos = [
  ["fe_abscx_piM_Ashery.csv", ["Fe", "abscx", "piminus"] ],
  ["fe_abscx_piP_Ashery.csv", ["Fe", "abscx", "piplus"] ],
  ["c_abscx_piM_Ashery.csv",  ["C", "abscx", "piminus"] ],
  ["c_abscx_piP_Ashery.csv",  ["C", "abscx", "piplus"] ],
  ["al_abscx_piM_Ashery.csv", ["Al", "abscx", "piminus"] ],
  ["al_abscx_piP_Ashery.csv", ["Al", "abscx", "piplus"] ],

  ["fe_inel_piM_Ashery.csv", ["Fe", "inel", "piminus"] ],
  ["fe_inel_piP_Ashery.csv", ["Fe", "inel", "piplus"] ],
  ["c_inel_piM_Ashery.csv",  ["C", "inel", "piminus"] ],
  ["c_inel_piP_Ashery.csv",  ["C", "inel", "piplus"] ],
  ["al_inel_piM_Ashery.csv", ["Al", "inel", "piminus"] ],
  ["al_inel_piP_Ashery.csv", ["Al", "inel", "piplus"] ]
]

f.cd()
for info in infos:
  gr = make_graph( info )
  gr.Write()
