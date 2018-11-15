from ROOT import * 
import sys
from os import listdir as ls
from draw_utils import * 
from argparse import ArgumentParser 
from ROOT import * 
from math import log, sqrt

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('--loc',      type=str,  help='Location of samples')
  parser.add_argument('--nom',      type=str,  help='nom File name')
  parser.add_argument('--cmd',      type=str,  help='Which command to execute ', default=" ")
  parser.add_argument('-f',         type=str,  help='Which input file')
  parser.add_argument('--plot',     type=str,  help='Plot name')
  parser.add_argument('--nbins',    type=int,  help='How many bins', default=75)
  parser.add_argument('--bin_high', type=float, help='Upper boundary', default=450.)
  parser.add_argument('--bin_low',  type=float, help='Lower boundary',default=0.)
  return parser

def SetStyle(h):
  h.SetTitle("Pi+ Ar - Thin Target Scattering")
  h.SetXTitle("KE (MeV)")
  h.SetYTitle("#sigma (barn)")
  h.GetXaxis().SetTitleSize(.05)
  h.GetYaxis().SetTitleSize(.05)
  h.GetYaxis().SetTitleOffset(.8)
  h.GetXaxis().SetTitleOffset(.8)

gStyle.SetLabelFont(62,"XYZ")
gStyle.SetTitleFont(62,"XYZ")
 
args = init_parser().parse_args()
loc = args.loc
cmd = args.cmd
plot = args.plot
gStyle.SetPadTickX(1)
gStyle.SetPadTickY(1)

nbins = args.nbins
bin_high = args.bin_high
bin_low = args.bin_low

names = ["abs", "cex", "dcex", "prod", "inel"] 

titles = {"abs":"Pion Absorption",
          "cex":"Single Charge Exchange",
          "dcex":"Double Charge Exchange",
          "prod":"Pion Production",
          "inel":"Inelastic"}

if (cmd == "Draw" or cmd == "draw"):
  inFile = TFile(loc + "/" + args.f, "UPDATE") 

  for name in names:
    c = TCanvas(name, name)
    nhist = inFile.Get("n"+name) 
    nhist.SetFillColor(0)
    whist = inFile.Get("w"+name) 
    vhist = inFile.Get("v"+name) 
    whist.SetFillColor(0)
    vhist.SetFillColor(0)

    nhist.SetTitle(titles[name])
    vhist.SetTitle(titles[name])
    whist.SetTitle(titles[name])

    nhist.SetXTitle("Pion Momentum (MeV)")
    vhist.SetXTitle("Pion Momentum (MeV)")
    whist.SetXTitle("Pion Momentum (MeV)")

    nhist.SetYTitle("Fraction")
    vhist.SetYTitle("Fraction")
    whist.SetYTitle("Fraction")

    nhist.SetMarkerStyle(20) 
    whist.SetMarkerStyle(21)
    vhist.SetMarkerStyle(22)
    nhist.SetMarkerColor(4) 
    whist.SetMarkerColor(2)
    vhist.SetMarkerColor(1)
    
    leg = TLegend(.55,.7,.9,.9)
    leg.AddEntry(nhist, "Nominal", "lp")
    leg.AddEntry(whist, "Weighted", "lp")
    leg.AddEntry(vhist, "Varied", "lp")
   

    whist.SetLineColor(2)
    vhist.SetLineColor(1)

    vhist.SetMaximum(.6)
    vhist.SetMinimum(0.)
    vhist.Draw("pE hist")
    whist.Draw("pE same hist")
    nhist.Draw("pE same")
    leg.Draw("same")

    c.Write()
    c.SaveAs(name+"_"+plot)



else:

  reac_cut = "(int == \"pi+Inelastic\")"
  abs_cut =  "(int == \"pi+Inelastic\" && ( (nPi0 + nPiPlus + nPiMinus)  == 0) )"
  inel_cut = "(int == \"pi+Inelastic\" && ( (nPi0 + nPiMinus) == 0 ) && (nPiPlus == 1))"
  cex_cut =  "(int == \"pi+Inelastic\" && ( (nPiPlus + nPiMinus) == 0 ) && (nPi0 == 1))"
  dcex_cut = "(int == \"pi+Inelastic\" && ( (nPiPlus + nPi0) == 0 ) && (nPiMinus == 1))"
  prod_cut = "(int == \"pi+Inelastic\" && ( (nPiPlus + nPi0 + nPiMinus) > 1) )"


  cuts = {"abs":abs_cut,
    "inel":inel_cut,
    "cex":cex_cut,
    "dcex":dcex_cut,
    "prod":prod_cut
  }

#  scale =1.E24/ (.5 * 2.266 * 6.022E23 / 12.01 )

  nhists = dict()
  whists = dict()

  outfile = TFile(loc+"/"+args.f,"RECREATE")
  fileName = loc + "/" + args.nom 

  nomFile = TFile(fileName,"READ")
  nomTree = nomFile.Get("tree")
  nIncident = nomTree.GetEntries()
  
  outfile.cd()
  
  nomTree.Draw("preFinalP>>reac(" + str(nbins) + "," + str(bin_low) + "," + str(bin_high) + ")",reac_cut,"goff")
  reac = gDirectory.Get("reac")

  reac.Sumw2() 

  reac.Write() 

  for name in names:
    cut = cuts[name]
    print name, cut
    nomTree.Draw("preFinalP>>"+name+"(" + str(nbins) + "," + str(bin_low) + "," + str(bin_high) + ")", cut,"goff")
  
    nhists[name] = gDirectory.Get(name) 
    nhists[name].Sumw2()
    nhists[name].Write() 

  outfile.Close()


