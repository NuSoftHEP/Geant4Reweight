from ROOT import * 
import sys
f = TFile( sys.argv[1] )

gROOT.SetBatch(1)
grabs = f.Get( "abs" )
grcex = f.Get( "cex" )
grinel = f.Get( "inel" )
grprod = f.Get( "prod" )
grdcex = f.Get( "dcex" )
grtotal = f.Get( "total_inel" )




grtotal.SetMaximum(1200.)
grtotal.SetMinimum(0.)
grtotal.SetTitle("Exclusive Cross Sections;Pion Momentum (MeV/c);#sigma (mb)")
grtotal.GetXaxis().SetTitleSize(.065)
grtotal.GetYaxis().SetTitleSize(.065)
grtotal.GetXaxis().SetTitleOffset(.75)
grtotal.GetYaxis().SetTitleOffset(.7)
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


grtotal.Draw()
grabs.Draw("same")
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
leg.AddEntry( grtotal, "Total", "lp")

leg.Draw("same")

c1.SaveAs( sys.argv[2] + ".pdf")
c1.SaveAs( sys.argv[2] + ".png")
