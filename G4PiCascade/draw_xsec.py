from ROOT import * 
import sys
f = TFile( sys.argv[1] )

gROOT.SetBatch(1)
grtotal = f.Get( "total_KE" )
grinel = f.Get( "inel_KE" )
grel   = f.Get( "el_KE" )


maxtot = max( [grtotal.GetY()[i] for i in range(0, grtotal.GetN())] )

grtotal.SetMaximum(1.2 * maxtot)
grtotal.SetMinimum(0.)
grtotal.SetTitle( sys.argv[2] + ";" + sys.argv[3] +" Kinetic Energy (MeV);#sigma (mb)")
grtotal.GetXaxis().SetTitleSize(.065)
grtotal.GetYaxis().SetTitleSize(.065)
grtotal.GetXaxis().SetTitleOffset(.7)
grtotal.GetYaxis().SetTitleOffset(.75)
grtotal.GetXaxis().SetLabelSize(.04)
grtotal.GetYaxis().SetLabelSize(.04)


grinel.SetLineColor(4)
grel.SetLineColor(2)

c1 = TCanvas("c1","c1",500,400)
c1.SetTickx(1)
c1.SetTicky(1)
gStyle.SetOptStat(0)
gPad.SetLeftMargin( gPad.GetLeftMargin() * 1.10 )


grtotal.Draw()
grinel.Draw("same")
grel.Draw("same") 

leg = TLegend(.6,.6,.85,.85)
leg.AddEntry( grtotal,  "Total",  "lp")
leg.AddEntry( grinel, "Inelastic",  "lp")
leg.AddEntry( grel, "Elastic",  "lp")

leg.Draw("same")

c1.SaveAs( sys.argv[4] + ".pdf")
c1.SaveAs( sys.argv[4] + ".png")
