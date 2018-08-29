from ROOT import * 
import sys
from draw_utils import * 
from math import log, sqrt
from argparse import ArgumentParser 
from glob import glob as ls

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
  parser.add_argument('--samps', type=str, help='Which samples to include')
  parser.add_argument('-n', type=int, help='Which portion')
  parser.add_argument('--cmd', type=str, help='Which command to execute ', default=" ")
  parser.add_argument('-f', type=str, help='Which input file')
  parser.add_argument('--plot',type=str, help='Plot name')
  parser.add_argument('-x', type=str, help='XML File')
  return parser

def ratio_scale_errors(n, d, scale, name="r"):
  r = n.Clone(name)
  r.Divide(d)
  r.Scale(scale)

  for ix in range(1, n.GetNbinsX() + 1):
    error =  r.GetBinContent(ix) * sqrt( 1/n.GetBinContent(ix) + 1/d.GetBinContent(ix) )
    r.SetBinError(ix, error)

  return r


args = init_parser().parse_args()
if (args.cmd == "Draw" or args.cmd == "draw"):
  inFile = TFile(args.f, "READ") 

  xsec_total_n = inFile.Get("nr")  
  xsec_total_w = inFile.Get("wr")  
  xsec_total_v = inFile.Get("vr")  
  total_result = getChi2(xsec_total_w,xsec_total_v)
  print "Total chi2: ", total_result[0]/total_result[1]
  xsec_total_n.SetMarkerStyle(20) 
  xsec_total_w.SetMarkerStyle(21)
  xsec_total_v.SetMarkerStyle(22)

  xsec_total_n.SetTitle(xsec_total_n.GetTitle() + " - Total")
  xsec_total_w.SetTitle(xsec_total_w.GetTitle() + " - Total")
  xsec_total_v.SetTitle(xsec_total_v.GetTitle() + " - Total")

  reactive_data = inFile.Get("data")
  reactive_data.SetMarkerStyle(27)
  total_data = inFile.Get("total_data")
  total_data.SetMarkerStyle(29)

  total_chi2 = str(total_result[0]/total_result[1])[0:4]

#  total_leg = inFile.Get("total_leg")
#  total_leg.AddEntry(None, "#chi^{2}/DOF = " + (total_chi2) )

  c1 = TCanvas()

  xsec_total_n.SetMinimum(0.)  
  xsec_total_v.SetMinimum(0.)
  xsec_total_w.SetMinimum(0.)

  xsec_total_v.Draw("pE")
  xsec_total_w.Draw("pE same")
  xsec_total_n.Draw("pE same")
  total_data.Draw("p same")
#  reactive_data.Draw("p same")
#  total_leg.SetTextFont()
#  total_leg.Draw("same")
  c1.SaveAs("total_"+args.plot)

else:  
  samps = [s for s in (args.samps).split(",")]
  
  
  nom = TChain("tree")
  var = TChain("tree")
  
  for samp in samps:
    nom_file = ls(args.loc + "/*" + samp + "_nom_inel" +args.i + "_elast"+args.e+"_full.root" )  
    print nom_file
    nom.Add(nom_file[0])
  
    var_file = ls(args.loc + "/*" + samp + "_var_inel" +args.i + "_elast"+args.e+"_full.root" )  
    print var_file
    var.Add(var_file[0])
  
  scale =1.E24/ (args.slice * 1.390 * 6.022E23 / 39.95 )
  
  outfile = TFile(args.loc+ "/" + args.f,"RECREATE")
  
  ##Getting dataset
  dataset = args.d
  totalset = args.t
  infile = file(dataset,"r")
  totalInfile = file(totalset,"r")
  x = []
  y = []
  for line in infile:
    x.append(float(line.split(",")[0]))
    y.append(float(line.split(",")[1])*1.E-3)
  
  gr = TGraph(len(x))
  i = 1
  for px,py in zip(x,y):
    gr.SetPoint(i,px,py)
    i = i + 1
  gr.SetMarkerStyle(27)
  gr.SetMarkerColor(6)
  gr.SetName("data")
  gr.Write()
  
  x = []
  y = []
  for line in totalInfile:
    print line.split(",")[0]
    x.append(float(line.split(",")[0]))
    y.append(float(line.split(",")[1])*1.E-3)
  
  print zip(x,y)
  
  gr = TGraph(len(x))
  i = 1
  for px,py in zip(x,y):
    gr.SetPoint(i,px,py)
    i = i + 1
  gr.SetMarkerStyle(29)
  gr.SetMarkerColor(6)
  gr.SetName("total_data")
  gr.Write()
  ####
  
  outfile.cd()
  
  
  
  nom.Draw("(sliceEnergy - 139.57)>>nd(16,25,825)","","goff")
  print "Drew nom"
  nom.Draw("(sliceEnergy - 139.57)>>wd(16,25,825)", "weight*elastWeight","goff")
  print "Drew weighted"
  var.Draw("(sliceEnergy - 139.57)>>vd(16,25,825)","","goff")
  print "Drew Varied"
  
  nom.Draw("(sliceEnergy - 139.57)>>nn(16,25,825)", "(sliceInts > 0)","goff")
  nom.Draw("(sliceEnergy - 139.57)>>wn(16,25,825)", "weight*elastWeight*(sliceInts > 0)","goff")
  var.Draw("(sliceEnergy - 139.57)>>vn(16,25,825)", "(sliceInts > 0)","goff")
  
  nd = gDirectory.Get("nd")
  wd = gDirectory.Get("wd")
  vd = gDirectory.Get("vd")
  nd.Write()
  wd.Write()
  vd.Write()
  vd.Sumw2()
  
  nn = gDirectory.Get("nn")
  wn = gDirectory.Get("wn")
  vn = gDirectory.Get("vn")
  nn.Write()
  wn.Write()
  vn.Write()
  vn.Sumw2()
  
  nr = ratio_scale_errors(nn, nd, scale, "nr")
  wr = ratio_scale_errors(wn, wd, scale, "wr")
  vr = ratio_scale_errors(vn, vd, scale, "vr")
  
  
  chi2_result = getChi2(wr,vr)
  
  #print chi2_result[0]/chi2_result[1]
  vr.SetTitle("#sigma_{inel}x" + args.i + ", #sigma_{el}x" +args.e)
  vr.SetXTitle("Pion Kinetic Energy (MeV)")
  vr.SetYTitle("XSec (cm^{2})")
  wr.SetTitle("#sigma_{inel}x" + args.i + ", #sigma_{el}x" +args.e)
  wr.SetXTitle("Pion Kinetic Energy (MeV)")
  wr.SetYTitle("XSec (cm^{2})")
  #wr.SetLineColor(2)
  #vr.SetLineColor(1)
  drawStyle(nr,wr,vr)
  
  #c1 = TCanvas()
  gStyle.SetOptStat(0)
  
  drawThis = getMax(nr,wr,vr)
  
  l = TLegend(.6,.7,.9,.9)
  l.AddEntry(nr, "Nominal","lpf")
  l.AddEntry(wr, "Weighted", "lpef")
  l.AddEntry(vr, "Varied", "lpef")
  #l.AddEntry(None, "#chi^{2}/bin = " + str(chi2_result[0]/chi2_result[1])[0:5] ,"")
  
  #l.Draw("same")
  l.Write()
  
  #c1.SaveAs(args.plot)
  
  #outfile.cd()
  nr.Write()
  wr.Write()
  vr.Write()
  outfile.Close()

