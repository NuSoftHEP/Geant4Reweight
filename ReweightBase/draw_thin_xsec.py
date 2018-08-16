from ROOT import * 
import sys
from os import listdir as ls
from draw_utils import * 
from argparse import ArgumentParser 
from ROOT import * 
from math import log

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-i', type=str, help='Inelastic variation factor', default=1.0)
  parser.add_argument('-e', type=str, help='Elastic variation factor', default=1.0)
  parser.add_argument('--loc', type=str, help='Location of samples')
  parser.add_argument('-d', type=str, help='Location of dataset')
  parser.add_argument('-t', type=str, help='Location of total dataset')
  parser.add_argument('--cmd', type=str, help='Which command to execute ', default=" ")
  parser.add_argument('-f', type=str, help='Which input file')
  parser.add_argument('--plot',type=str, help='Plot name')
  return parser

def SetStyle(h):
  h.SetTitle("Pi+ Ar - Thin Target Scattering")
  h.SetXTitle("KE (MeV)")
  h.SetYTitle("#sigma (barn)")
  h.GetXaxis().SetTitleSize(.05)
  h.GetYaxis().SetTitleSize(.05)
  h.GetYaxis().SetTitleOffset(.8)
  h.GetXaxis().SetTitleOffset(.8)


 
args = init_parser().parse_args()
inel = args.i
elast = args.e
loc = args.loc
cmd = args.cmd

gStyle.SetPadTickX(1)
gStyle.SetPadTickY(1)

if (cmd == "Draw" or cmd == "draw"):
  inFile = TFile(args.f, "WRITE") 
  thin_xsec_reactive_n = inFile.Get("thin_reactive_xsec_n")  
  thin_xsec_reactive_w = inFile.Get("thin_reactive_xsec_w")  
  thin_xsec_reactive_v = inFile.Get("thin_reactive_xsec_v")  
  thin_xsec_reactive_n.SetMarkerStyle(24) 
  thin_xsec_reactive_w.SetMarkerStyle(26)
  thin_xsec_reactive_v.SetMarkerStyle(25)

  thin_xsec_total_n = inFile.Get("thin_total_xsec_n")  
  thin_xsec_total_w = inFile.Get("thin_total_xsec_w")  
  thin_xsec_total_v = inFile.Get("thin_total_xsec_v")  
  thin_xsec_total_n.SetMarkerStyle(20) 
  thin_xsec_total_w.SetMarkerStyle(21)
  thin_xsec_total_v.SetMarkerStyle(22)

  reactive_data = inFile.Get("data")
  reactive_data.SetMarkerStyle(27)
  total_data = inFile.Get("total_data")
  total_data.SetMarkerStyle(29)

  leg = inFile.Get("leg")

  c1 = TCanvas()

  thin_xsec_total_v.Draw("p")
  thin_xsec_total_w.Draw("p same")
  thin_xsec_total_n.Draw("p same")
  thin_xsec_reactive_v.Draw("p same")
  thin_xsec_reactive_w.Draw("p same")
  thin_xsec_reactive_n.Draw("p same")
  total_data.Draw("p same")
  reactive_data.Draw("p same")
  leg.Draw("same")
  c1.SaveAs(args.plot) 

else:
  scale =1.E24/ (.5 * 1.390 * 6.022E23 / 39.95 )
  surv_cut = "!(int != \"pi+Inelastic\" && nElast == 0)" 
  int_cut  = "(int == \"pi+Inelastic\")"
  
  outfile = TFile(loc+"/thin_xsec_inel" + inel + "_elast" + elast + ".root","RECREATE")
  
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
  
  finalNReactive = TH1F("thin_reactive_xsec_n","", 900,0,900)
  finalVReactive = TH1F("thin_reactive_xsec_v","", 900,0,900)
  finalWReactive = TH1F("thin_reactive_xsec_w","", 900,0,900)
  
  finalNTotal = TH1F("thin_total_xsec_n","", 900,0,900)
  finalVTotal = TH1F("thin_total_xsec_v","", 900,0,900)
  finalWTotal = TH1F("thin_total_xsec_w","", 900,0,900)
  
  samps = ["50MeV", "100MeV", "150MeV", "200MeV", "250MeV",
          "300MeV", "400MeV", "500MeV", "600MeV", "700MeV", "800MeV"]
  bins = [50, 100, 150, 200, 250,
           300, 400, 500, 600, 700, 800]
  
  i = 0
  for samp in samps:
    print samp    
    fileName = loc + "/thin_" + samp + "_nom_" + "inel" + inel + "_elast" + elast + "_full.root"
    nomFile = TFile(fileName,"READ")
    nomTree = nomFile.Get("tree")
  
    fileName = loc + "/thin_" + samp + "_var_" + "inel" + inel + "_elast" + elast + "_full.root"
    varFile = TFile(fileName,"READ")
    varTree = varFile.Get("tree")
  
    nIncident = nomTree.GetEntries()
  
    #nomTree.Draw("Energy>>nd","","goff")
    #nomTree.Draw("Energy>>wd","weight*elastWeight","goff")
    #varTree.Draw("Energy>>vd","","goff")
  
    #nd = gDirectory.Get("nd") 
    #wd = gDirectory.Get("wd") 
    #vd = gDirectory.Get("vd") 
  
    nomTree.Draw("Energy>>nn", int_cut,"goff")
    nomTree.Draw("Energy>>wn","weight*elastWeight*" + int_cut,"goff")
    varTree.Draw("Energy>>vn", int_cut,"goff")
    
    nn = gDirectory.Get("nn") 
    wn = gDirectory.Get("wn") 
    vn = gDirectory.Get("vn") 
  
    nr = nn.Clone()
    wr = wn.Clone()
    vr = vn.Clone()
  
  #  nr.Divide(nd)
  #  wr.Divide(wd)
  #  vr.Divide(vd)
  
    nr.Scale(scale/nIncident)
    wr.Scale(scale/nIncident)
    vr.Scale(scale/nIncident)
  
    print nr.GetMaximum(), vr.GetMaximum(), wr.GetMaximum() 
    print nr.GetMaximumBin(), vr.GetMaximumBin(), wr.GetMaximumBin()
  
    nBin = finalNReactive.FindBin(bins[i]) 
    vBin = finalVReactive.FindBin(bins[i]) 
    wBin = finalWReactive.FindBin(bins[i]) 
  
    n = nr.GetMaximum()    
    w = wr.GetMaximum()   
    v = vr.GetMaximum()   
  
    finalNReactive.SetBinContent(nBin, n)
    finalWReactive.SetBinContent(wBin, w)
    finalVReactive.SetBinContent(vBin, v)
  
    ##Total XSec
    nomTree.Draw("Energy>>nSurvTotal", surv_cut,"goff")
    nomTree.Draw("Energy>>wSurvTotal","weight*elastWeight*" + surv_cut,"goff")
    varTree.Draw("Energy>>vSurvTotal", surv_cut,"goff")
  
    nSurvTotal = gDirectory.Get("nSurvTotal")
    wSurvTotal = gDirectory.Get("wSurvTotal")
    vSurvTotal = gDirectory.Get("vSurvTotal")
  
    nSurv = nSurvTotal.GetMaximum()
    wSurv = wSurvTotal.GetMaximum()
    vSurv = vSurvTotal.GetMaximum()
    
    theBin = finalNTotal.FindBin(bins[i]) 
  
    #finalNTotal.SetBinContent(theBin, log(nIncident / nSurv) * scale)
    #finalWTotal.SetBinContent(theBin, log(nIncident / wSurv) * scale)
    #finalVTotal.SetBinContent(theBin, log(nIncident / vSurv) * scale)
    finalNTotal.SetBinContent(theBin, (nSurv / nIncident) * scale)
    finalWTotal.SetBinContent(theBin, (wSurv / nIncident) * scale)
    finalVTotal.SetBinContent(theBin, (vSurv / nIncident) * scale)
  
    ###End Total XSec section 
    
    i = i + 1
  
  ####End Samps Loop
  
  finalNReactive.SetMarkerColor(4)
  finalWReactive.SetMarkerColor(2)
  finalVReactive.SetMarkerColor(1)
  
  finalNReactive.SetMarkerStyle(24)
  finalWReactive.SetMarkerStyle(26)
  finalVReactive.SetMarkerStyle(25)
  
  finalNTotal.SetMarkerColor(4)
  finalWTotal.SetMarkerColor(2)
  finalVTotal.SetMarkerColor(1)
  
  finalNTotal.SetMarkerStyle(20)
  finalWTotal.SetMarkerStyle(21)
  finalVTotal.SetMarkerStyle(22)
  
  
  
  SetStyle(finalNReactive)
  SetStyle(finalVReactive)
  SetStyle(finalWReactive)
  
  SetStyle(finalNTotal)
  SetStyle(finalVTotal)
  SetStyle(finalWTotal)
  
  outfile.cd()
  
  finalNReactive.Write()
  finalWReactive.Write()
  finalVReactive.Write()
  
  finalNTotal.Write()
  finalWTotal.Write()
  finalVTotal.Write()
  
  leg = TLegend(.55,.7,.9,.9)
  leg.AddEntry(finalNReactive,"Nominal","p")
  leg.AddEntry(finalWReactive,"Weighted","p")
  leg.AddEntry(finalVReactive,"Varied","p")
  leg.AddEntry(data,"Nutini Reactive","p")
  leg.AddEntry(total_data,"Nutini Total","p")
  leg.SetName("leg")
  leg.Write()
