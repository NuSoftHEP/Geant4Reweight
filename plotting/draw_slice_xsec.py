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
  parser.add_argument('--title',type=str, help='Append to title')
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
  xsec_total_n.SetMarkerSize(.4) 
  xsec_total_w.SetMarkerSize(.4)
  xsec_total_v.SetMarkerSize(.4)

  xsec_total_n.SetTitle(xsec_total_n.GetTitle() + " - LAr Slice Total XSec")
  xsec_total_w.SetTitle(xsec_total_w.GetTitle() + " - LAr Slice Total XSec")
  xsec_total_v.SetTitle(xsec_total_v.GetTitle() + " - LAr Slice Total XSec")

  reactive_data = inFile.Get("data")
  reactive_data.SetMarkerStyle(27)
  total_data = inFile.Get("total_data")
  total_data.SetMarkerStyle(29)

  total_chi2 = str(total_result[0]/total_result[1])[0:4]

  leg = TLegend(.55,.7,.9,.9)  
  leg.AddEntry(xsec_total_n, "Nominal", "lp")
  leg.AddEntry(xsec_total_w, "Weighted", "lp")
  leg.AddEntry(xsec_total_v, "Varied", "lp")
  leg.AddEntry(total_data, "Validation", "p")

#  total_leg.AddEntry(None, "#chi^{2}/DOF = " + (total_chi2) )

  c1 = TCanvas()

  xsec_total_n.SetMinimum(0.)  
  xsec_total_v.SetMinimum(0.)
  xsec_total_w.SetMinimum(0.)

  xsec_total_v.Draw("pE")
  xsec_total_w.Draw("pE same")
  xsec_total_n.Draw("pE same")
  total_data.Draw("p same")
  leg.Draw("same")
#  reactive_data.Draw("p same")
#  total_leg.SetTextFont()
#  total_leg.Draw("same")
  c1.SaveAs("total_"+args.plot)

elif (args.cmd == "ratio" or args.cmd == "Ratio"):

  inFile = TFile(args.f, "READ") 

  xsec_total_w = inFile.Get("pwr")  
  xsec_total_v = inFile.Get("pvr")  

  total_ratio = xsec_total_w.Clone()

  total_ratio.Divide(xsec_total_v)  

  total_ratio.SetMinimum(.5)
  total_ratio.SetMaximum(1.5)

  total_ratio.SetTitle("Weighted/Varied: " + args.title + ";Pion Momentum (MeV/c); Ratio") 
  total_ratio.GetXaxis().SetTitleSize(.05)
  total_ratio.GetYaxis().SetTitleSize(.05)
  total_ratio.GetXaxis().SetTitleOffset(.75)
  total_ratio.GetYaxis().SetTitleOffset(.75)

#  total_ratio.Fit("pol1")

  c1 = TCanvas()
  total_ratio.Draw("p")

  f = TF1("line", "1", 0, 1300.)
  f.SetLineColor(1)
  f.Draw("same")

  #leg = TLegend(.55,.7,.9,.9)
  #leg.SetHeader("#sigma_{inel}x"+inel+", #sigma_{el}x"+elast, "C")
  #leg.AddEntry(total_ratio, "Total: Weighted/Varied","p")
  #leg.Draw("same")
  c1.SaveAs(args.plot)

else:  
  samps = [s for s in (args.samps).split(",")]
  
  
  nom = TChain("tree")
  var = TChain("tree")
  
  for samp in samps:
    nom_file = ls(args.loc + "/*" + samp + "_nom_inel" +args.i + "_elast"+args.e+"_full.root" )  
    print args.loc + "/*" + samp + "_nom_inel" +args.i + "_elast"+args.e+"_full.root"
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
  vr.SetYTitle("#sigma (barn)")
  wr.SetTitle("#sigma_{inel}x" + args.i + ", #sigma_{el}x" +args.e)
  wr.SetXTitle("Pion Kinetic Energy (MeV)")
  wr.SetYTitle("#sigma (barn)")
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

  ###Momentum
  nom.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>pnd(52,0,1300)","","goff")
  nom.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>pwd(52,0,1300)", "weight*elastWeight","goff")
  var.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>pvd(52,0,1300)","","goff")
  
  nom.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>pnn(52,0,1300)", "(sliceInts > 0)","goff")
  nom.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>pwn(52,0,1300)", "weight*elastWeight*(sliceInts > 0)","goff")
  var.Draw("(sqrt( sliceEnergy*sliceEnergy - 139.57*139.57))>>pvn(52,0,1300)", "(sliceInts > 0)","goff")
  
  pnd = gDirectory.Get("pnd")
  pwd = gDirectory.Get("pwd")
  pvd = gDirectory.Get("pvd")
  pnd.Write()
  pwd.Write()
  pvd.Write()
  pvd.Sumw2()
  
  pnn = gDirectory.Get("pnn")
  pwn = gDirectory.Get("pwn")
  pvn = gDirectory.Get("pvn")
  pnn.Write()
  pwn.Write()
  pvn.Write()
  pvn.Sumw2()
  
  pnr = ratio_scale_errors(pnn, pnd, scale, "pnr")
  pwr = ratio_scale_errors(pwn, pwd, scale, "pwr")
  pvr = ratio_scale_errors(pvn, pvd, scale, "pvr")

  pvr.SetTitle("#sigma_{inel}x" + args.i + ", #sigma_{el}x" +args.e)
  pvr.SetXTitle("Pion Kinetic Energy (MeV)")
  pvr.SetYTitle("#sigma (barn)")
  pwr.SetTitle("#sigma_{inel}x" + args.i + ", #sigma_{el}x" +args.e)
  pwr.SetXTitle("Pion Kinetic Energy (MeV)")
  pwr.SetYTitle("#sigma (barn)")
  drawStyle(pnr,pwr,pvr)
  
  #c1 = TCanvas()
  gStyle.SetOptStat(0)
  
  drawThis = getMax(pnr,pwr,pvr)

  pnr.Write()
  pwr.Write()
  pvr.Write()
  
  outfile.Close()

