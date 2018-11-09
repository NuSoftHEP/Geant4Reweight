from ROOT import * 
from draw_utils import *
import sys

def getChi2(w, v):
  result = []  
  chi2 = 0.
  nbins = 0
  for i in range(1,w.GetNbinsX()+1):
    if(  (w.GetBinError(i) == 0) and (v.GetBinError(i) == 0) ) :
      continue
    
    a = ( (w.GetBinContent(i) - v.GetBinContent(i))**2 ) / ( w.GetBinError(i)**2 + v.GetBinError(i)**2  ) 
    chi2 = chi2 + a 
    nbins = nbins + 1
  result.append(chi2)
  print nbins
  result.append(nbins)
  return result
  


f1 = TFile(sys.argv[1],"READ")
f2 = TFile(sys.argv[2],"READ")


nom = f1.Get("tree")
var = f2.Get("tree")

###First elastic
nom.Draw("elastDists[0]>>n(100,0,50)", "(nElast > 0)")
nom.Draw("elastDists[0]>>w(100,0,50)", "weight*elastWeight*(nElast > 0)")
var.Draw("elastDists[0]>>v(100,0,50)", "(nElast > 0)")

n = gDirectory.Get("n")
w = gDirectory.Get("w")
v = gDirectory.Get("v")


chi2_result = getChi2(w,v)

print chi2_result[0]/chi2_result[1]
#w.SetLineColor(2)
#w.SetMarkerColor(2)
#w.SetMarkerStyle(20)
#v.SetLineColor(1)
drawStyle(n,w,v)

c1 = TCanvas()
gStyle.SetOptStat(0)

drawThis = getMax(n,w,v) 
drawThis[0][0].SetMinimum(0.)
drawThis[0][0].SetTitle("#sigma_{inel}x" + str(sys.argv[5]) + ", #sigma_{el}x" +str(sys.argv[6]))
drawThis[0][0].SetXTitle("Dist to 1st Elastic Scatter (cm)")
drawThis[0][0].Draw(drawThis[0][1])
drawThis[1][0].Draw(drawThis[1][1])
drawThis[2][0].Draw(drawThis[2][1])

#w.Draw("e")
#n.Draw("same")
#v.Draw("same hist e")

l = TLegend(.6,.6,.9,.9)
l.AddEntry(n, "Nominal","lpf")
l.AddEntry(w, "Weighted", "lpef")
l.AddEntry(v, "Varied", "lpef")
l.AddEntry(None, "#chi^{2}/bin = " + str(chi2_result[0]/chi2_result[1])[0:5] ,"")

l.Draw("same")

c1.SaveAs(sys.argv[3])
####################End first elastic

###Second elastic
nom.Draw("elastDists[1]>>n2(50,0,50)", "(nElast > 1)")
nom.Draw("elastDists[1]>>w2(50,0,50)", "weight*elastWeight*(nElast > 1)")
var.Draw("elastDists[1]>>v2(50,0,50)", "(nElast > 1)")

n2 = gDirectory.Get("n2")
w2 = gDirectory.Get("w2")
v2 = gDirectory.Get("v2")


new_chi2_result = getChi2(w2,v2)

print new_chi2_result[0]/new_chi2_result[1]

drawThis = getMax(n2w2v2 
drawThis[0][0].SetMinimum(0.)
drawThis[0][0].SetTitle("#sigma_{inel}x" + str(sys.argv[5]) + ", #sigma_{el}x" +str(sys.argv[6]))
drawThis[0][0].SetXTitle("Dist to 2nd Elastic Scatter (cm)")
drawThis[0][0].Draw(drawThis[0][1])
drawThis[1][0].Draw(drawThis[1][1])
drawThis[2][0].Draw(drawThis[2][1])

#w2.SetLineColor(2)
#w2.SetMarkerColor(2)
#w2.SetMarkerStyle(20)
#v2.SetLineColor(1)
drawStyle(n,w,v)

c1 = TCanvas()
gStyle.SetOptStat(0)

#w2.SetMinimum(0.)
#w2.Draw("e")
#n2.Draw("same")
#v2.Draw("same hist e")

l = TLegend(.6,.6,.9,.9)
l.AddEntry(n2, "Nominal","lpf")
l.AddEntry(w2, "Weighted", "lpef")
l.AddEntry(v2, "Varied", "lpef")
l.AddEntry(None, "#chi^{2}/bin = " + str(new_chi2_result[0]/new_chi2_result[1])[0:5] ,"")

l.Draw("same")

c1.SaveAs(sys.argv[4])
####################End second elastic

