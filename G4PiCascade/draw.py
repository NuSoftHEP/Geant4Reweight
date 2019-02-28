from ROOT import * 
import sys
f = TFile( sys.argv[1] )

gROOT.SetBatch(1)
grabs = f.Get( "abs" )
grcex = f.Get( "cex" )
grinel = f.Get( "inel" )
grprod = f.Get( "prod" )
grdcex = f.Get( "dcex" )




grabs.SetMaximum(1.2)
grabs.SetMinimum(0.)
grabs.SetTitle("Cascade Results " + sys.argv[2] + ";Pion Momentum (MeV/c);Fraction")
grabs.GetXaxis().SetTitleSize(.065)
grabs.GetYaxis().SetTitleSize(.065)
grabs.GetXaxis().SetTitleOffset(.75)
grabs.GetYaxis().SetTitleOffset(.7)
grabs.GetXaxis().SetLabelSize(.04)
grabs.GetYaxis().SetLabelSize(.04)


grinel.SetLineColor(2)
grcex.SetLineColor(4)
grdcex.SetLineColor(8)
grprod.SetLineColor(46)

c1 = TCanvas("c1","c1",500,400)
c1.SetTickx(1)
c1.SetTicky(1)
gStyle.SetOptStat(0)


grabs.Draw()
grcex.Draw("same")
grinel.Draw("same")
grdcex.Draw("same")
grprod.Draw("same")

leg = TLegend(.6,.6,.85,.85)
leg.AddEntry( grabs,  "Abs",  "lp")
leg.AddEntry( grinel, "Inel",  "lp")
leg.AddEntry( grcex,  "Cex",  "lp")
leg.AddEntry( grdcex, "DCex",  "lp")
leg.AddEntry( grprod, "Prod",  "lp")

leg.Draw("same")

c1.SaveAs( sys.argv[2] + ".pdf")
c1.SaveAs( sys.argv[2] + ".png")
