from ROOT import * 
import sys
from os import listdir as ls
from draw_utils import * 
from argparse import ArgumentParser 
from ROOT import * 
from math import log, sqrt
from default import *
from array import array

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

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-i', type=str, help='Inelastic variation factor', default="1")
  parser.add_argument('-e', type=str, help='Elastic variation factor', default="1")
  parser.add_argument('--loc', type=str, help='Location of samples')
  parser.add_argument('-d', type=str, help='Location of dataset')
  parser.add_argument('-t', type=str, help='Location of total dataset')
  parser.add_argument('--cmd', type=str, help='Which command to execute ', default=" ")
  parser.add_argument('-f', type=str, help='Which input file')
  parser.add_argument('--plot',type=str, help='Plot name')
  parser.add_argument('--reactive',type=str, help='reactive')
  parser.add_argument('--total',type=str, help='total')
  parser.add_argument('--tex', type=int, help='Save as tex?')
  return parser

def SetStyle(h):
  h.SetTitle("Pi+ Ar - Thin Target Scattering")
  h.SetXTitle("Pion Kinetic Energy (MeV)")
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

set_default_style()

if (cmd == "Draw" or cmd == "draw"):
  inFile = TFile(args.f, "READ") 
  thin_xsec_reactive_n = inFile.Get("thin_reactive_xsec_n")  
  thin_xsec_reactive_w = inFile.Get("thin_reactive_xsec_w")  
  thin_xsec_reactive_v = inFile.Get("thin_reactive_xsec_v")  
    
  thin_xsec_reactive_n.SetMarkerStyle(24) 
  thin_xsec_reactive_w.SetMarkerStyle(25)
  thin_xsec_reactive_v.SetMarkerStyle(26)

  thin_xsec_total_n = inFile.Get("thin_total_xsec_n")  
  thin_xsec_total_w = inFile.Get("thin_total_xsec_w")  
  thin_xsec_total_v = inFile.Get("thin_total_xsec_v")  

  thin_xsec_total_n.SetMarkerStyle(20) 
  thin_xsec_total_w.SetMarkerStyle(21)
  thin_xsec_total_v.SetMarkerStyle(22)

  thin_xsec_reactive_n.SetTitle("Pi+ Ar - Thin Target Scattering - Reactive")
  thin_xsec_reactive_w.SetTitle("Pi+ Ar - Thin Target Scattering - Reactive")
  thin_xsec_reactive_v.SetTitle("Pi+ Ar - Thin Target Scattering - Reactive")

  thin_xsec_total_n.SetTitle("Pi+ Ar - Thin Target Scattering - Total")
  thin_xsec_total_w.SetTitle("Pi+ Ar - Thin Target Scattering - Total")
  thin_xsec_total_v.SetTitle("Pi+ Ar - Thin Target Scattering - Total")
  
  thin_xsec_reactive_n.SetTitle("")
  thin_xsec_reactive_w.SetTitle("")
  thin_xsec_reactive_v.SetTitle("")

  thin_xsec_total_n.SetTitle("")
  thin_xsec_total_w.SetTitle("")
  thin_xsec_total_v.SetTitle("")

  set_pdf_style(thin_xsec_reactive_n, "Pion Momentum (MeV/c)", "#sigma (barn)" )
  set_pdf_style(thin_xsec_reactive_w, "Pion Momentum (MeV/c)", "#sigma (barn)" )
  set_pdf_style(thin_xsec_reactive_v, "Pion Momentum (MeV/c)", "#sigma (barn)" )
  
  set_pdf_style(thin_xsec_total_n, "Pion Momentum (MeV/c)", "#sigma (barn)" )
  set_pdf_style(thin_xsec_total_w, "Pion Momentum (MeV/c)", "#sigma (barn)" )
  set_pdf_style(thin_xsec_total_v, "Pion Momentum (MeV/c)", "#sigma (barn)" )

  reactive_data = inFile.Get("data")
  reactive_data.SetMarkerStyle(27)
  
  #check for 0
  xCheck = array("d", [0])
  yCheck = array("d", [0])
  reactive_data.GetPoint(0, xCheck, yCheck) 
  print "X:", xCheck
  if(xCheck[0] == 0.): reactive_data.RemovePoint(0)

  total_data = inFile.Get("total_data")
  total_data.SetMarkerStyle(29)

  total_data.GetPoint(0, xCheck, yCheck) 
  if(xCheck[0] == 0.): total_data.RemovePoint(0)

  #total_leg = inFile.Get("total_leg")
  #reactive_leg = inFile.Get("reactive_leg")
  total_leg = TLegend(.55,.7,.85,.85)
  total_leg.AddEntry(thin_xsec_total_n,"Nominal","p")
  total_leg.AddEntry(thin_xsec_total_w,"Weighted","p")
  total_leg.AddEntry(thin_xsec_total_v,"Varied","p")
  total_leg.AddEntry(total_data,"Validation","p")


  reactive_leg = TLegend(.55,.7,.85,.85)
  reactive_leg.AddEntry(thin_xsec_reactive_n,"Nominal","p")
  reactive_leg.AddEntry(thin_xsec_reactive_w,"Weighted","p")
  reactive_leg.AddEntry(thin_xsec_reactive_v,"Varied","p")
  reactive_leg.AddEntry(reactive_data,"Validation","p")

  c1 = TCanvas("c1","c1",500,400)
  set_pad_style()

  thin_xsec_total_v.Draw("pE")
  thin_xsec_total_w.Draw("pE same")
  thin_xsec_total_n.Draw("pE same")
  total_data.Draw("p same")
  total_leg.SetTextFont(42)
  total_leg.Draw("same")
  if(args.tex == 1): c1.Print("total_"+args.plot+".tex") 
  c1.SaveAs("total_"+args.plot+".pdf")

  thin_xsec_reactive_v.Draw("pE")
  thin_xsec_reactive_w.Draw("pE same")
  thin_xsec_reactive_n.Draw("pE same")
  reactive_data.Draw("p same")
  reactive_leg.SetTextFont(42)
  reactive_leg.Draw("same")
  if(args.tex == 1): c1.Print("reactive_"+args.plot+".tex")
  c1.SaveAs("reactive_"+args.plot+".pdf") 

elif (cmd == "ratio" or cmd == "Ratio"):
  inFile = TFile(args.f, "READ") 
  thin_xsec_reactive_w = inFile.Get("thin_reactive_xsec_w")  
  thin_xsec_reactive_v = inFile.Get("thin_reactive_xsec_v")  

  thin_xsec_total_w = inFile.Get("thin_total_xsec_w")  
  thin_xsec_total_v = inFile.Get("thin_total_xsec_v")  

  total_ratio = thin_xsec_total_w.Clone()
  reactive_ratio = thin_xsec_reactive_w.Clone()

  total_ratio.Divide(thin_xsec_total_v)  
  reactive_ratio.Divide(thin_xsec_reactive_v)  

  total_ratio.SetMinimum(0.)
  total_ratio.SetMaximum(2.0)
  reactive_ratio.SetMinimum(0.)
  reactive_ratio.SetMaximum(2.0)

  set_pdf_style(total_ratio, "Pion Momentum (MeV/c)", "Ratio")
  set_pdf_style(reactive_ratio, "Pion Momentum (MeV/c)", "Ratio")
  total_ratio.SetTitle("")
  reactive_ratio.SetTitle("")

  c1 = TCanvas("c1","c1",500,400)
  total_ratio.Draw("p")
  reactive_ratio.Draw("p same")

  f = TF1("line", "1", 0, 900.)
  f.SetLineColor(1)
  f.Draw("same")

  leg = TLegend(.55,.7,.85,.85)
  leg.SetHeader("Weighted/Varied")
  leg.AddEntry(total_ratio, "Total","p")
  leg.AddEntry(reactive_ratio, "Reactive","p")
  leg.SetTextFont(42)
  leg.Draw("same")
  c1.SaveAs(args.plot+".pdf")

elif (cmd == "can_ratio" or cmd == "can_Ratio"):
  totalFile = TFile(args.total, "READ") 
  reacFile = TFile(args.reactive, "READ") 
  total_can = totalFile.Get("c1")
  reactive_can = reacFile.Get("c1")

  thin_xsec_reactive_w = reactive_can.GetPrimitive("thin_reactive_xsec_w")  
  thin_xsec_reactive_v = reactive_can.GetPrimitive("thin_reactive_xsec_v")  

  thin_xsec_total_w = total_can.GetPrimitive("thin_total_xsec_w")  
  thin_xsec_total_v = total_can.GetPrimitive("thin_total_xsec_v")  

  total_ratio = thin_xsec_total_w.Clone()
  reactive_ratio = thin_xsec_reactive_w.Clone()

  total_ratio.Divide(thin_xsec_total_v)  
  reactive_ratio.Divide(thin_xsec_reactive_v)  

  total_ratio.SetMinimum(0.)
  total_ratio.SetMaximum(2.0)
  reactive_ratio.SetMinimum(0.)
  reactive_ratio.SetMaximum(2.0)

  c1 = TCanvas()
  total_ratio.SetTitle("Pi+ Ar - Thin Target Scattering - Ratios")
  total_ratio.Draw("p")
  reactive_ratio.Draw("p same")

  f = TF1("line", "1", 0, 900.)
  f.SetLineColor(1)
  f.Draw("same")

  leg = TLegend(.55,.7,.85,.85)
  leg.SetHeader("#sigma_{inel}x"+inel+", #sigma_{el}x"+elast, "C")
#  leg.SetHeader("#sigma_{inel}x"+inel+", #sigma_{el}x"+elast)
  leg.AddEntry(total_ratio, "Total: Weighted/Varied","p")
  leg.AddEntry(reactive_ratio, "Reactive: Weighted/Varied","p")
  leg.SetTextFont(42)
  leg.Draw("same")
  c1.SaveAs(args.plot)

else:
  scale =1.E24/ (.5 * 1.390 * 6.022E23 / 39.95 )
  surv_cut = "!(int != \"pi+Inelastic\" && nElast == 0)"#(int == pi+inel || nElast > 0) 
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
    
    nError = sqrt(nn.GetMaximum()) 
    wError = sqrt(wn.GetMaximum()) 
    vError = sqrt(vn.GetMaximum()) 

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
 
    finalNReactive.SetBinError(nBin, nError*scale/nIncident)
    finalWReactive.SetBinError(wBin, wError*scale/nIncident)
    finalVReactive.SetBinError(vBin, vError*scale/nIncident)
  
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
    
    nError = sqrt(nSurv)
    wError = sqrt(wSurv)
    vError = sqrt(vSurv)    

    theBin = finalNTotal.FindBin(bins[i]) 
  
    finalNTotal.SetBinContent(theBin, (nSurv / nIncident) * scale)
    finalWTotal.SetBinContent(theBin, (wSurv / nIncident) * scale)
    finalVTotal.SetBinContent(theBin, (vSurv / nIncident) * scale)

    finalNTotal.SetBinError(theBin, (nError / nIncident) * scale)
    finalWTotal.SetBinError(theBin, (wError / nIncident) * scale)
    finalVTotal.SetBinError(theBin, (vError / nIncident) * scale)
  
    ###End Total XSec section 
    
    i = i + 1
  
  ####End Samps Loop
  
  finalNReactive.SetMarkerColor(4)
  finalWReactive.SetMarkerColor(2)
  finalVReactive.SetMarkerColor(1)
  
  finalNReactive.SetMarkerStyle(24)
  finalWReactive.SetMarkerStyle(25)
  finalVReactive.SetMarkerStyle(26)
  
  finalNTotal.SetMarkerColor(4)
  finalWTotal.SetMarkerColor(2)
  finalVTotal.SetMarkerColor(1)
  
  finalNTotal.SetMarkerStyle(20)
  finalWTotal.SetMarkerStyle(21)
  finalVTotal.SetMarkerStyle(22)
  
  
  
  #SetStyle(finalNReactive)
  #SetStyle(finalVReactive)
  #SetStyle(finalWReactive)
  #SetStyle(finalNTotal)
  #SetStyle(finalVTotal)
  #SetStyle(finalWTotal)

  
  outfile.cd()
  
  finalNReactive.Write()
  finalWReactive.Write()
  finalVReactive.Write()
  
  finalNTotal.Write()
  finalWTotal.Write()
  finalVTotal.Write()
  
  leg = TLegend(.55,.7,.85,.85)
  leg.SetName("reactive_leg")
  #leg.SetHeader("#sigma_{inel}x"+inel+", #sigma_{el}x"+elast, "C")
  leg.AddEntry(finalNReactive,"Nominal","p")
  leg.AddEntry(finalWReactive,"Weighted","p")
  leg.AddEntry(finalVReactive,"Varied","p")
  leg.AddEntry(data,"Validation","p")
  leg.Write()


  leg = TLegend(.55,.7,.85,.85)
  leg.SetName("total_leg")
  #leg.SetHeader("#sigma_{inel}x"+inel+", #sigma_{el}x"+elast, "C")
  leg.AddEntry(finalNTotal,"Nominal","p")
  leg.AddEntry(finalWTotal,"Weighted","p")
  leg.AddEntry(finalVTotal,"Varied","p")
  leg.AddEntry(total_data,"Validation","p")
  leg.Write()
