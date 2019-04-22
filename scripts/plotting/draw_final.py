from ROOT import * 
import sys
from draw_utils import * 

#def getChi2(w, v):
#  result = []  
#  chi2 = 0.
#  nbins = 0
#  for i in range(1,w.GetNbinsX()+1):
#    if(  (w.GetBinError(i) == 0) and (v.GetBinError(i) == 0) ) :
#      continue
#    
#    a = ( (w.GetBinContent(i) - v.GetBinContent(i))**2 ) / ( w.GetBinError(i)**2 + v.GetBinError(i)**2  ) 
#    chi2 = chi2 + a 
#    nbins = nbins + 1
#  result.append(chi2)
#  print nbins
#  result.append(nbins)
#  return result
#  
#
#def getMax(n, w, v):
#  nMax = n.GetMaximum()
#  wMax = w.GetMaximum()
#  vMax = v.GetMaximum()
#  if (nMax > wMax and nMax > vMax):
#    return [ [n,""], [w,"same hist e"], [v,"same hist e"] ]
#  elif (wMax > nMax and wMax > vMax):
#    return [ [w,"hist e"], [n,"same"], [v,"same hist e"] ]
#  elif (vMax > nMax and vMax > wMax):
#    return [ [v,"hist e"], [n,"same"], [w,"same hist e"] ]

f1 = TFile(sys.argv[1],"READ")
f2 = TFile(sys.argv[2],"READ")
#gStyle.SetOptStat(0)


nom = f1.Get("tree")
var = f2.Get("tree")

draws = ["nPiPlus", "nPiMinus", "nPi0", "nProton", "nNeutron"]

for draw in draws:
  nom.Draw(draw + ">>n" + draw, "")
  nom.Draw(draw + ">>w" + draw, "weight*elastWeight")
  var.Draw(draw + ">>v" + draw, "")

  n = gDirectory.Get("n" + draw)
  w = gDirectory.Get("w" + draw)
  v = gDirectory.Get("v" + draw)


  chi2_result = getChi2(w,v)
  
  print chi2_result[0]/chi2_result[1]

  w.SetLineColor(2)
  w.SetMarkerColor(2)
  w.SetMarkerStyle(20)
  v.SetLineColor(1)
  n.SetMarkerColor(4)
  n.SetMarkerStyle(25)

  drawThis = getMax(n,w,v) 
  drawThis[0][0].SetTitle("#sigma_{inel}x" + str(sys.argv[4]) + ", #sigma_{el}x" +str(sys.argv[5]))
  drawThis[0][0].SetXTitle(draw)

  XMax = getXMax(n,w,v)
  n.GetXaxis().SetRangeUser(0., XMax) 
  w.GetXaxis().SetRangeUser(0., XMax) 
  v.GetXaxis().SetRangeUser(0., XMax) 

  c1 = TCanvas()

  drawThis[0][0].SetMinimum(0.)

  drawThis[0][0].Draw(drawThis[0][1])
  drawThis[1][0].Draw(drawThis[1][1])
  drawThis[2][0].Draw(drawThis[2][1])
  
  
  l = TLegend(.6,.6,.9,.9)
  l.AddEntry(n, "Nominal","lpf")
  l.AddEntry(w, "Weighted", "lpef")
  l.AddEntry(v, "Varied", "lpef")
  #l.AddEntry(None, "#chi^{2}/bin = " + str(chi2_result[0]/chi2_result[1])[0:5] ,"")
  
  l.Draw("same")
  
  c1.SaveAs(sys.argv[3] + "_" + draw + ".pdf")


