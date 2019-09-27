from ROOT import * 
from array import array 

def make_graph( theInput ):
  print theInput

  inFileName = theInput[0]
  target = theInput[1][0]
  cut = theInput[1][1]
  probe = theInput[1][2]

  infile = file(inFileName, "r")
  moms = []
  xsecs = []
  errs = []

  for line in infile:
    if(line[0] == "#"): continue

    data = [float(i) for i in line.split()]
    moms.append(data[0])
    xsecs.append(data[1])
    errs.append(data[2])

  gr = TGraphErrors( len(moms), array("d", moms), array("d", xsecs), array("d", [0]*len(moms)), array("d", errs) )
  gr.SetName( target + "_xsec_" + cut + "_" + probe )
  return gr

