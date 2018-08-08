from ROOT import * 
from draw_utils import *
import sys

f1 = TFile(sys.argv[1],"READ")
f2 = TFile(sys.argv[2],"READ")


nom = f1.Get("tree")
var = f2.Get("tree")

nom.Draw("len>>n(25,0,500)")
nom.Draw("len>>w(25,0,500)", "weight*elastWeight")
var.Draw("len>>v(25,0,500)")

n = gDirectory.Get("n")
w = gDirectory.Get("w")
v = gDirectory.Get("v")

#chi2 = 0.
#nbins = 0
#chi2_bins = []
#for i in range(1,w.GetNbinsX()+1):
#  if(  (w.GetBinError(i) == 0) or (v.GetBinError(i) == 0) ) :
#    continue
#  
#  a = ( (w.GetBinContent(i) - v.GetBinContent(i))**2 ) / ( w.GetBinError(i)**2 + v.GetBinError(i)**2  ) 
#  chi2_bins.append(a)
#  chi2 = chi2 + a 
#  nbins = nbins + 1

chi2_result = getChi2(w,v)
print chi2_result[0]/chi2_result[1]
#n.SetTitle("#chi^{2}/bin = " + str(chi2/nbins))

#w.SetLineColor(2)
#v.SetLineColor(1)


c1 = TCanvas()
gStyle.SetOptStat(0)

drawStyle(n,w,v)

drawThis = getMax(n,w,v) 
drawThis[0][0].SetTitle("#sigma_{inel}x" + str(sys.argv[5]) + ", #sigma_{el}x" +str(sys.argv[6]))
drawThis[0][0].SetXTitle("Track Length (cm)")
drawThis[0][0].Draw(drawThis[0][1])
drawThis[1][0].Draw(drawThis[1][1])
drawThis[2][0].Draw(drawThis[2][1])
#w.Draw(" hist e")
#n.Draw("same")
#v.Draw("same hist e")

l = TLegend(.6,.6,.9,.9)
l.AddEntry(n, "Nominal","lpf")
l.AddEntry(w, "Weighted", "lpef")
l.AddEntry(v, "Varied", "lpef")
l.AddEntry(None, "#chi^{2}/bin = " + str(chi2_result[0]/chi2_result[1])[0:5] ,"")

l.Draw("same")

c1.SaveAs(sys.argv[3])

f = TF1("line", "1", 0, 32.5)
f.SetLineColor(1)

r = w.Clone("")
r.Rebin(5)
d = v.Clone("")
d.Rebin(5)
r.Divide(d)

r.SetMarkerSize(15)
r.SetTitle("#sigma_{inel}x" + str(sys.argv[5]) + ", #sigma_{el}x" +str(sys.argv[6]))
r.GetYaxis().SetRangeUser(0.,2.)
r.Draw()
f.Draw("same")
c1.SaveAs(sys.argv[4])
