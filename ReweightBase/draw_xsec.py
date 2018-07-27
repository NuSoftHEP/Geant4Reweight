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


nom = f1.Get("tree")
var = f2.Get("tree")

nom.Draw("sliceEnergy>>nd(65,0,1300)")
nom.Draw("sliceEnergy>>wd(65,0,1300)", "weight*elastWeight")
var.Draw("sliceEnergy>>vd(65,0,1300)")

nom.Draw("sliceEnergy>>nn(65,0,1300)", "(sliceInts > 0)")
nom.Draw("sliceEnergy>>wn(65,0,1300)", "weight*elastWeight*(sliceInts > 0)")
var.Draw("sliceEnergy>>vn(65,0,1300)", "(sliceInts > 0)")

nd = gDirectory.Get("nd")
wd = gDirectory.Get("wd")
vd = gDirectory.Get("vd")
vd.Sumw2()

nn = gDirectory.Get("nn")
wn = gDirectory.Get("wn")
vn = gDirectory.Get("vn")
vn.Sumw2()

nr = nn.Clone()
nr.Divide(nd)

wr = wn.Clone()
wr.Divide(wd)

vr = vn.Clone()
vr.Divide(vd)

chi2_result = getChi2(wr,vr)

#print chi2_result[0]/chi2_result[1]
vr.SetTitle("#sigma_{inel}x" + str(sys.argv[4]) + ", #sigma_{el}x" +str(sys.argv[5]))
vr.SetXTitle("KE (MeV)")
vr.SetYTitle("XSec (arb units)")
wr.SetLineColor(2)
vr.SetLineColor(1)

c1 = TCanvas()
gStyle.SetOptStat(0)

vr.GetYaxis().SetRangeUser(0, 0.15)
vr.Draw("hist e")
wr.Draw("same hist e")
nr.Draw("same")


l = TLegend(.1,.6,.4,.9)
l.AddEntry(nr, "Nominal","lpf")
l.AddEntry(wr, "Weighted", "lpef")
l.AddEntry(vr, "Varied", "lpef")
#l.AddEntry(None, "#chi^{2}/bin = " + str(chi2_result[0]/chi2_result[1])[0:5] ,"")

l.Draw("same")

c1.SaveAs(sys.argv[3])


