from ROOT import * 
import sys
f = TFile( sys.argv[1] )

gROOT.SetBatch(1)
grabs = f.Get( "abs_" + sys.argv[2] )
grcex = f.Get( "cex_" + sys.argv[2] )
grinel = f.Get( "inel_" + sys.argv[2] )
grprod = f.Get( "prod_" + sys.argv[2] )
grdcex = f.Get( "dcex_" + sys.argv[2] )
grtotal = f.Get( "total_inel_" + sys.argv[2] )

title = ""
if( sys.argv[2] == "momentum" ):
  title = "Momentum (MeV/c)"
elif( sys.argv[2] == "KE" ):
  title = "Kinetic Energy (MeV)"


grtotal.SetMaximum(500.)
grtotal.SetMinimum(0.)
grtotal.GetXaxis().SetRangeUser( 0., [grabs.GetX()[i] for i in range(0, grabs.GetN())][-1] )
grtotal.SetTitle(";Pion " + title + ";#sigma (mb)")
grtotal.GetXaxis().SetTitleSize(.05)
grtotal.GetYaxis().SetTitleSize(.05)
grtotal.GetXaxis().SetTitleOffset(.9)
grtotal.GetYaxis().SetTitleOffset(.9)
grtotal.GetXaxis().SetLabelSize(.04)
grtotal.GetYaxis().SetLabelSize(.04)


#grtotal.SetLineColor(10)
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

grtotal.Draw()
grabs.Draw("same")
grcex.Draw("same")
grinel.Draw("same")
grdcex.Draw("same")
grprod.Draw("same")

leg = TLegend(.6,.6,.85,.85)
leg.AddEntry( grtotal, "Total", "lp")
leg.AddEntry( grabs,  "Absorption",  "lp")
leg.AddEntry( grinel, "Inelastic/Quasi-Elastic",  "lp")
leg.AddEntry( grcex,  "Charge Exchange",  "lp")
leg.AddEntry( grdcex, "Double Charge Exchange",  "lp")
leg.AddEntry( grprod, "Pion Production",  "lp")

leg.Draw("same")

c1.SaveAs( sys.argv[3] + ".pdf")
c1.SaveAs( sys.argv[3] + ".png")
