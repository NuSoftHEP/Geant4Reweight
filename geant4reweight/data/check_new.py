from ROOT import * 
import sys

f1 = TFile(sys.argv[1])
f2 = TFile("pion_data/" + sys.argv[1])


for grname in [i.GetName() for i in f2.GetListOfKeys()]:
  print grname

  gr1 = f1.Get( grname )
  oldxs = [gr1.GetX()[i] for i in range(0,gr1.GetN())]
  oldys = [gr1.GetY()[i] for i in range(0,gr1.GetN())]
  oldes = [gr1.GetEY()[i] for i in range(0,gr1.GetN())]

  gr2 = f2.Get( grname )
  newxs = [gr2.GetX()[i] for i in range(0,gr2.GetN())]
  newys = [gr2.GetY()[i] for i in range(0,gr2.GetN())]
  newes = [gr2.GetEY()[i] for i in range(0,gr2.GetN())]

  print "X:"
  print oldxs
  print newxs

  print "Y:"
  print oldys
  print newys

  print "EY:"
  print oldes
  print newes

