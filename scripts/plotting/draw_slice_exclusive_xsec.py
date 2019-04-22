from ROOT import * 
import sys
from draw_utils import * 
from math import log, sqrt
from argparse import ArgumentParser 
from glob import glob as ls

gROOT.SetBatch(1)

def LogIt(h):
  for i in range(1,h.GetNbinsX() + 1):
    content = h.GetBinContent(i)
#    if(content == 0): continue
    content = log(1. / ( 1. - content) )
    h.SetBinContent(i,content)

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-i', type=str, help='Inelastic variation factor', default="1")
  parser.add_argument('-e', type=str, help='Elastic variation factor', default="1")
  parser.add_argument('--loc', type=str, help='Location of samples')
  parser.add_argument('-d', type=str, help='Location of dataset')
  parser.add_argument('-t', type=str, help='Location of total dataset')
  parser.add_argument('--slice', type=float, help='Size of slice')
  parser.add_argument('-n', type=int, help='Which portion')
  parser.add_argument('--cmd', type=str, help='Which command to execute ', default=" ")
  parser.add_argument('--nom', type=str, help='which input file')
  parser.add_argument('-f', type=str, help='Which input file')
  parser.add_argument('--plot',type=str, help='Plot name')
  parser.add_argument('-x', type=str, help='XML File')
  parser.add_argument('--title',type=str, help='Append to title')
  parser.add_argument('--alt', type=int, help='Which weight to apply', default=1)
  return parser

def ratio_scale_errors(n, d, scale, name="r"):
  r = n.Clone(name)
  r.Divide(d)
  r.Scale(scale)

  for ix in range(1, n.GetNbinsX() + 1):
    if(n.GetBinContent(ix) == 0): continue
    if(d.GetBinContent(ix) == 0): continue
    error =  r.GetBinContent(ix) * sqrt( 1/n.GetBinContent(ix) + 1/d.GetBinContent(ix) )
    r.SetBinError(ix, error)

  return r

gStyle.SetPadTickX(1)
gStyle.SetPadTickY(1)

args = init_parser().parse_args()

names = ["reac", "abs", "cex", "dcex", "inel", "prod"]

titles = {"abs":"Pion Absorption",
          "cex":"Single Charge Exchange",
          "dcex":"Double Charge Exchange",
          "prod":"Pion Production",
          "inel":"Inelastic",
          "reac":"Reactive"}


if (args.cmd == "Draw" or args.cmd == "draw"):
  inFile = TFile(args.f, "READ") 

  for name in names:

    n = inFile.Get("n"+name)  
    w = inFile.Get("w"+name)  
    n.SetMarkerStyle(20) 
    w.SetMarkerStyle(21)
    n.SetMarkerSize(.4) 
    w.SetMarkerSize(.4)
    n.SetMarkerColor(4)
    w.SetMarkerColor(2)
    n.SetLineColor(4)
    w.SetLineColor(2)


    n.SetTitle(titles[name])
    w.SetTitle(titles[name])


    leg = TLegend(.55,.7,.9,.9)  
    leg.AddEntry(n, "Nominal", "lp")
    leg.AddEntry(w, "Weighted", "lp")


    c1 = TCanvas()

    n.SetMinimum(0.)  
    w.SetMinimum(0.)

    maxes = [n.GetMaximum(), w.GetMaximum()]

    n.SetMaximum( 1.25*max(maxes) )
    w.SetMaximum( 1.25*max(maxes) )

    w.Draw("pE")
    n.Draw("pE same")
    leg.Draw("same")
    c1.SaveAs(name +"_"+args.plot)

else:  
  scale =1.E24/ (args.slice * 1.390 * 6.022E23 / 39.95 )
  
  infile = TFile(args.loc + "/" + args.nom, "READ")

  nom = infile.Get("tree")  

  ###Momentum


  reac_cut = "(sliceIntsInelastic > 0)"
  abs_cut =  "(sliceIntsInelastic > 0 && (nPi0 + nPiPlus + nPiMinus) == 0)"
  inel_cut = "(sliceIntsInelastic > 0 && (nPi0 + nPiMinus) == 0 && (nPiPlus == 1))"
  cex_cut =  "(sliceIntsInelastic > 0 && (nPiPlus + nPiMinus) == 0 && (nPi0 == 1))"
  dcex_cut = "(sliceIntsInelastic > 0 && (nPiPlus + nPi0) == 0 && (nPiMinus == 1))"
  prod_cut = "(sliceIntsInelastic > 0 && (nPiPlus + nPi0 + nPiMinus) > 1)"


  cuts = {"abs":abs_cut,
    "inel":inel_cut,
    "cex":cex_cut,
    "dcex":dcex_cut,
    "prod":prod_cut,
    "reac":reac_cut
  }

  nhists = dict()
  whists = dict()

  outfile = TFile(args.loc+"/"+args.f,"RECREATE")
  outfile.cd()

  if( args.alt ): weight_str = "altFSWeight"
  else: weight_str = "finalStateWeight*weight*elastWeight"
  print weight_str

  ##Total
  nom.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>nTotal(52,0,1300)","","goff")
  nom.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>wTotal(52,0,1300)", weight_str,"goff")

  nTotal = gDirectory.Get("nTotal")
  wTotal = gDirectory.Get("wTotal")
  nTotal.Write()
  wTotal.Write()
  
  for name in names: 

    cut = cuts[name]

    nom.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>n" + name + "(52,0,1300)", cut,"goff")
    nom.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>w" + name + "(52,0,1300)", weight_str + "*" + cut,"goff")
    nhists[name] = gDirectory.Get("n" + name)
    whists[name] = gDirectory.Get("w" + name)

    nhists[name].Write()
    whists[name].Write()

    nr = ratio_scale_errors(nhists[name], nTotal, scale, "n"+name)
    wr = ratio_scale_errors(whists[name], wTotal, scale, "w"+name)

    nr.Write()
    wr.Write()

  outfile.Close()

