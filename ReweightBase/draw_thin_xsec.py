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
#  parser.add_argument('--energy', type=str, help='Sample to ', default=1.0)
  return parser


 
args = init_parser().parse_args()
inel = args.i
elast = args.e
loc = args.loc

outfile = TFile("thin_xsec_inel" + inel + "_elast" + elast + ".root")
finalN = TH1F("thin_xsec_n","", 900,0,900)
finalV = TH1F("thin_xsec_v","", 900,0,900)
finalw = TH1F("thin_xsec_w","", 900,0,900)

samps = ["50MeV", "100MeV", "150MeV", "200MeV", "250MeV",
        "300MeV", "400MeV", "500MeV", "600MeV", "700MeV", "800MeV"]
bins = [50, 100, 150, 200, 250,
         300, 400, 500, 600, 700, 800]
i = 0

for samp in samps:
  
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

  nomTree.Draw("Energy>>nd")
  nomTree.Draw("Energy>>wd","weight*elastWeight")
  varTree.Draw("Energy>>vd")

  nd = gDirectory.Get("nd") 
  wd = gDirectory.Get("wd") 
  vd = gDirectory.Get("vd") 

  nomTree.Draw("Energy>>nn", "(int==\"pi+Inelastic\")")
  nomTree.Draw("Energy>>wn","weight*elastWeight*(int==\"pi+Inelastic\")")
  varTree.Draw("Energy>>vn", "(int==\"pi+Inelastic\")")

  nn = gDirectory.Get("nn") 
  wn = gDirectory.Get("wn") 
  vn = gDirectory.Get("vn") 

  nr = nn.Clone()
  wr = wn.Clone()
  vr = vn.Clone()

  nr.Divide(nd)
  wr.Divide(wd)
  vr.Divide(vd)

  nr.Scale( 1./ (.5 * 1.390 * 6.022E23 / 39.95 ) )
  wr.Scale( 1./ (.5 * 1.390 * 6.022E23 / 39.95 ) )
  vr.Scale( 1./ (.5 * 1.390 * 6.022E23 / 39.95 ) )

  n = nr.GetBinContent( nr.FindBin(bins[i]) )
  w = wr.GetBinContent( wr.FindBin(bins[i]) )
  v = vr.GetBinContent( vr.FindBin(bins[i]) )

  finalN.SetBinContent( finalN.FindBin(bins[i]), n )
  finalW.SetBinContent( finalW.FindBin(bins[i]), w )
  finalV.SetBinContent( finalV.FindBin(bins[i]), v )

  i = i + 1

finalN.Write()
finalW.Write()
finalV.Write()
