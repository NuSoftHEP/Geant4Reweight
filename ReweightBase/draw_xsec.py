from ROOT import * 
import sys
from draw_utils import * 
from math import log

def LogIt(h):
  for i in range(1,h.GetNbinsX() + 1):
    content = h.GetBinContent(i)
#    if(content == 0): continue
    content = log(1. / ( 1. - content) )
    h.SetBinContent(i,content)

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

f1 = TFile(sys.argv[1],"READ")
f2 = TFile(sys.argv[2],"READ")


nom = f1.Get("tree")
var = f2.Get("tree")

nom.Draw("(sliceEnergy - 139.57)>>nd(55,0,1100)")
nom.Draw("(sliceEnergy - 139.57)>>wd(55,0,1100)", "weight*elastWeight")
var.Draw("(sliceEnergy - 139.57)>>vd(55,0,1100)")

nom.Draw("(sliceEnergy - 139.57)>>nn(55,0,1100)", "(sliceInts > 0)")
nom.Draw("(sliceEnergy - 139.57)>>wn(55,0,1100)", "weight*elastWeight*(sliceInts > 0)")
var.Draw("(sliceEnergy - 139.57)>>vn(55,0,1100)", "(sliceInts > 0)")

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

#LogIt(nr)
#LogIt(vr)
#LogIt(wr)

nr.Scale( 1./ ( float(sys.argv[6]) * 1.390 * 6.022E23 / 39.95 ) )
vr.Scale( 1./ ( float(sys.argv[6]) * 1.390 * 6.022E23 / 39.95 ) )
wr.Scale( 1./ ( float(sys.argv[6]) * 1.390 * 6.022E23 / 39.95 ) )


chi2_result = getChi2(wr,vr)

#print chi2_result[0]/chi2_result[1]
vr.SetTitle("#sigma_{inel}x" + str(sys.argv[4]) + ", #sigma_{el}x" +str(sys.argv[5]))
vr.SetXTitle("Pion Kinetic Energy (MeV)")
vr.SetYTitle("XSec (cm^{2})")
wr.SetTitle("#sigma_{inel}x" + str(sys.argv[4]) + ", #sigma_{el}x" +str(sys.argv[5]))
wr.SetXTitle("Pion Kinetic Energy (MeV)")
wr.SetYTitle("XSec (cm^{2})")
#wr.SetLineColor(2)
#vr.SetLineColor(1)
drawStyle(nr,wr,vr)

c1 = TCanvas()
gStyle.SetOptStat(0)

drawThis = getMax(nr,wr,vr)
#drawThis[0][0].GetYaxis().SetRangeUser(0, 0.15)
drawThis[0][0].Draw(drawThis[0][1])
drawThis[1][0].Draw(drawThis[1][1])
drawThis[2][0].Draw(drawThis[2][1])


l = TLegend(.6,.7,.9,.9)
l.AddEntry(nr, "Nominal","lpf")
l.AddEntry(wr, "Weighted", "lpef")
l.AddEntry(vr, "Varied", "lpef")
#l.AddEntry(None, "#chi^{2}/bin = " + str(chi2_result[0]/chi2_result[1])[0:5] ,"")

l.Draw("same")

c1.SaveAs(sys.argv[3])


