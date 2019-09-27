from ROOT import * 
import sys

f = TFile(sys.argv[1], "READ")

gr = f.Get(sys.argv[2])

for i in range(0, gr.GetN()):
  print "%.3f"%gr.GetX()[i],

print
