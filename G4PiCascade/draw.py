from ROOT import * 
from array import array
import math
import sys

def mom_to_ke( grin ):
  xs = [math.sqrt(grin.GetX()[i]*grin.GetX()[i] + 139.57*139.57 ) - 139.57 for i in range(0,grin.GetN())] 
  ys = [grin.GetY()[i] for i in range(0,grin.GetN())] 
  print xs
  grout = TGraph( len(xs), array("d", xs), array("d", ys) )
  return grout

f = TFile( sys.argv[1] )

gROOT.SetBatch(1)
grabs =  mom_to_ke( f.Get( "abs" ) )
grcex =  mom_to_ke( f.Get( "cex" ) )
grinel = mom_to_ke( f.Get( "inel" ) )
grprod = mom_to_ke( f.Get( "prod" ) )
grdcex = mom_to_ke( f.Get( "dcex" ) )




grabs.SetMaximum(1.2)
grabs.SetMinimum(0.)
grabs.SetTitle("Cascade Results" + sys.argv[2] + ";Pion Kinetic Energy (MeV);Fraction")
grabs.GetXaxis().SetTitleSize(.065)
grabs.GetYaxis().SetTitleSize(.065)
grabs.GetXaxis().SetTitleOffset(.7)
grabs.GetYaxis().SetTitleOffset(.75)
grabs.GetXaxis().SetLabelSize(.04)
grabs.GetYaxis().SetLabelSize(.04)


grabs.SetLineColor(13)
grinel.SetLineColor(2)
grcex.SetLineColor(4)
grdcex.SetLineColor(8)
grprod.SetLineColor(46)

c1 = TCanvas("c1","c1",500,400)
c1.SetTickx(1)
c1.SetTicky(1)
gStyle.SetOptStat(0)
gPad.SetLeftMargin( gPad.GetLeftMargin() * 1.10 )


grabs.Draw()
grcex.Draw("same")
grinel.Draw("same")
grdcex.Draw("same")
grprod.Draw("same")

leg = TLegend(.6,.6,.85,.85)
leg.AddEntry( grabs,  "Absorption",  "lp")
leg.AddEntry( grinel, "Inelastic/Quasi-Elastic",  "lp")
leg.AddEntry( grcex,  "Charge Exchange",  "lp")
leg.AddEntry( grdcex, "Double Charge Exchange",  "lp")
leg.AddEntry( grprod, "Pion Production",  "lp")

leg.Draw("same")

c1.SaveAs( sys.argv[3] + ".pdf")
c1.SaveAs( sys.argv[3] + ".png")
