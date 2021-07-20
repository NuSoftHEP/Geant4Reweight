import ROOT as RT
from array import array
import math
import sys

def mom_to_ke( grin ):
  xs = [math.sqrt(grin.GetX()[i]*grin.GetX()[i] + 139.57*139.57 ) - 139.57 for i in range(0,grin.GetN())] 
  ys = [grin.GetY()[i] for i in range(0,grin.GetN())] 
  print(xs)
  grout = RT.TGraph( len(xs), array("d", xs), array("d", ys) )
  return grout

f = RT.TFile( sys.argv[1] )

RT.gROOT.SetBatch(1)
grabs =  f.Get( "abs" ) 
grcex =  f.Get( "cex" ) 
grinel = f.Get( "inel" )
grprod = f.Get( "prod" )
grdcex = f.Get( "dcex" )




grabs.SetMaximum(1.2)
grabs.SetMinimum(0.)
grabs.GetXaxis().SetRangeUser(0.,2000.)
grabs.SetTitle(";Pion Momentum (MeV/c);Fraction")
grabs.GetXaxis().SetTitleSize(.05)
grabs.GetYaxis().SetTitleSize(.05)
grabs.GetXaxis().SetTitleOffset(.9)
grabs.GetYaxis().SetTitleOffset(.9)
grabs.GetXaxis().SetLabelSize(.04)
grabs.GetYaxis().SetLabelSize(.04)


grabs.SetLineColor(13)
grinel.SetLineColor(2)
grcex.SetLineColor(4)
grdcex.SetLineColor(8)
grprod.SetLineColor(46)

c1 = RT.TCanvas("c1","c1",500,400)
c1.SetTickx(1)
c1.SetTicky(1)
RT.gStyle.SetOptStat(0)
RT.gPad.SetLeftMargin( RT.gPad.GetLeftMargin() * 1.10 )


grabs.Draw("AC")
grcex.Draw("same C")
grinel.Draw("same C")
grdcex.Draw("same C")
grprod.Draw("same C")

grtotal = RT.TGraph()
grtotal.SetLineColor(1)

pos = [float(i) for i in sys.argv[3].split(",")]
#.6,.6,.85,.85
leg = RT.TLegend(pos[0], pos[1], pos[2], pos[3])
#leg.AddEntry( grtotal, "Total Inelastic", "l")
leg.AddEntry( grabs,  "Absorption",  "l")
leg.AddEntry( grinel, "Quasielastic",  "l")
leg.AddEntry( grcex,  "Charge Exchange",  "l")
leg.AddEntry( grdcex, "Double Charge Exchange",  "l")
leg.AddEntry( grprod, "Pion Production",  "l")

leg.Draw("same")
RT.gPad.RedrawAxis()

title = sys.argv[2]
ext = title.split(".")[1]
title = title.split(".")[0]
c1.SaveAs( title + "." + ext)

tpt = RT.TPaveText( .15, .15, .95, .87, "brNDC")

texts = ["Absorption",
         "Inelastic/Quasi-elastic",
         "Charge Exchange",
         "Double Charge Exchange",
         "Pion Production"]
text_colors = [ 13, 2, 4, 8, 46 ]
for t,c in zip( texts, text_colors ):
  text = tpt.AddText( t )
  text.SetTextColor( c )

tpt.SetTextSize(.06)
tpt.SetFillColor(0)
tpt.SetBorderSize(0)
c2 = RT.TCanvas("c2","c2",500,400)
tpt.Draw()
RT.gPad.SetLeftMargin( RT.gPad.GetRightMargin() )
c2.SaveAs( title + "_leg." + ext)

