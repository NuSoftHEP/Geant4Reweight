from ROOT import * 
import sys
from os import listdir as ls
from draw_utils import * 
from argparse import ArgumentParser 
from ROOT import * 

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-i', type=str, help='Inelastic variation factor', default=1.0)
  parser.add_argument('-e', type=str, help='Elastic variation factor', default=1.0)
  parser.add_argument('--loc', type=str, help='Location of samples')
  parser.add_argument('-d', type=str, help='Location of dataset')
#  parser.add_argument('--energy', type=str, help='Sample to ', default=1.0)
  return parser


 
args = init_parser().parse_args()
inel = args.i
elast = args.e
loc = args.loc

outfile = TFile("thin_xsec_inel" + inel + "_elast" + elast + ".root","RECREATE")
dataset = args.d
infile = file(dataset,"r")
x = []
y = []
for line in infile:
  x.append(float(line.split(",")[0]))
  y.append(float(line.split(",")[1])*1.E-3)

print zip(x,y)

#gr = TGraph(len(x),array('d',x),array('d',y))
gr = TGraph(len(x))
i = 1
for px,py in zip(x,y):
  gr.SetPoint(i,px,py)
  i = i + 1
gr.SetMarkerStyle(27)
gr.SetMarkerColor(6)
gr.SetName("data")
gr.Write()

finalN = TH1F("thin_xsec_n","", 900,0,900)
finalV = TH1F("thin_xsec_v","", 900,0,900)
finalW = TH1F("thin_xsec_w","", 900,0,900)

samps = ["50MeV", "100MeV", "150MeV", "200MeV", "250MeV",
        "300MeV", "400MeV", "500MeV", "600MeV", "700MeV", "800MeV"]
bins = [50, 100, 150, 200, 250,
         300, 400, 500, 600, 700, 800]
i = 0

for samp in samps:
  print samp    
  fileName = loc + "/thin_" + samp + "_nom_" + "inel" + inel + "_elast" + elast + "_full.root"
#  nomFiles[samp] = TFile(fileName,"READ")
#  nomTrees[samp] = nomFiles[samp].Get("tree")
  nomFile = TFile(fileName,"READ")
  nomTree = nomFile.Get("tree")

  fileName = loc + "/thin_" + samp + "_var_" + "inel" + inel + "_elast" + elast + "_full.root"
#  varFiles[samp] = TFile(fileName,"READ")
#  varTrees[samp] = varFiles[samp].Get("tree")
  varFile = TFile(fileName,"READ")
  varTree = varFile.Get("tree")

  nomTree.Draw("Energy>>nd","","goff")
  nomTree.Draw("Energy>>wd","weight*elastWeight","goff")
  varTree.Draw("Energy>>vd","","goff")

  nd = gDirectory.Get("nd") 
  wd = gDirectory.Get("wd") 
  vd = gDirectory.Get("vd") 

  nomTree.Draw("Energy>>nn", "(int==\"pi+Inelastic\")","goff")
  nomTree.Draw("Energy>>wn","weight*elastWeight*(int==\"pi+Inelastic\")","goff")
  varTree.Draw("Energy>>vn", "(int==\"pi+Inelastic\")","goff")
  
  nn = gDirectory.Get("nn") 
  wn = gDirectory.Get("wn") 
  vn = gDirectory.Get("vn") 
  print nn.GetMaximum()
  nr = nn.Clone()
  wr = wn.Clone()
  vr = vn.Clone()

  nr.Divide(nd)
  wr.Divide(wd)
  vr.Divide(vd)

  nr.Scale( 1.E24/ (.5 * 1.390 * 6.022E23 / 39.95 ) )
  wr.Scale( 1.E24/ (.5 * 1.390 * 6.022E23 / 39.95 ) )
  vr.Scale( 1.E24/ (.5 * 1.390 * 6.022E23 / 39.95 ) )

  print nr.GetMaximum(), vr.GetMaximum(), wr.GetMaximum() 
  print nr.GetMaximumBin(), vr.GetMaximumBin(), wr.GetMaximumBin()
  n = nr.GetBinContent( nr.GetMaximumBin() )
  w = wr.GetBinContent( wr.GetMaximumBin() )
  v = vr.GetBinContent( vr.GetMaximumBin() )
  print bins[i], n, w, v
  finalN.SetBinContent( finalN.FindBin(bins[i]), n )
  finalW.SetBinContent( finalW.FindBin(bins[i]), w )
  finalV.SetBinContent( finalV.FindBin(bins[i]), v )

  i = i + 1

finalN.SetMarkerColor(4)
finalW.SetMarkerColor(2)
finalV.SetMarkerColor(1)

finalN.SetMarkerStyle(2)
finalW.SetMarkerStyle(2)
finalV.SetMarkerStyle(2)

def SetStyle(h):
  h.SetTitle("Pi+ Ar - Thin Target Scattering")
  h.SetXTitle("KE (MeV)")
  h.SetYTitle("#sigma (barn)")
  h.GetXaxis().SetTitleSize(.05)
  h.GetYaxis().SetTitleSize(.05)
  h.GetYaxis().SetTitleOffset(.8)
  h.GetXaxis().SetTitleOffset(.8)

SetStyle(finalN)
SetStyle(finalV)
SetStyle(finalW)

outfile.cd()
finalN.Write()
finalW.Write()
finalV.Write()

leg = TLegend(.55,.7,.9,.9)
leg.AddEntry(finalN,"Nominal","pf")
leg.AddEntry(finalW,"Weighted","pf")
leg.AddEntry(finalV,"Varied","pf")
leg.AddEntry(data,"Nutini","pf")
leg.SetName("leg")
leg.Write()
