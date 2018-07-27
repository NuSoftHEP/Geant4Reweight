from ROOT import * 
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
gStyle.SetOptStat(0)


nom = f1.Get("tree")
var = f2.Get("tree")

###First elastic
nom.Draw("nElast>>n", "")
nom.Draw("nElast>>w", "weight*elastWeight")
var.Draw("nElast>>v", "")

n = gDirectory.Get("n")
w = gDirectory.Get("w")
v = gDirectory.Get("v")


chi2_result = getChi2(w,v)

print chi2_result[0]/chi2_result[1]
w.SetTitle("#sigma_{inel}x" + str(sys.argv[4]) + ", #sigma_{el}x" +str(sys.argv[5]))
w.SetXTitle("N Elastic Scatters ")
w.SetLineColor(2)
w.SetMarkerColor(2)
w.SetMarkerStyle(20)
v.SetLineColor(1)

c1 = TCanvas()

w.SetMinimum(0.)
v.Draw("e")
n.Draw("same")
w.Draw("same hist e")

l = TLegend(.6,.6,.9,.9)
l.AddEntry(n, "Nominal","lpf")
l.AddEntry(w, "Weighted", "lpef")
l.AddEntry(v, "Varied", "lpef")
l.AddEntry(None, "#chi^{2}/bin = " + str(chi2_result[0]/chi2_result[1])[0:5] ,"")

l.Draw("same")

c1.SaveAs(sys.argv[3])
####################End first elastic


