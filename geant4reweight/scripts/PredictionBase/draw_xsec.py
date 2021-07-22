import ROOT as RT
import sys
f = RT.TFile( sys.argv[1] )

RT.gROOT.SetBatch(1)


if sys.argv[2] == "momentum":
  grtotal = f.Get( "total_momentum" )
  grinel = f.Get( "inel_momentum" )
  grel   = f.Get( "el_momentum" )
else:
  grtotal = f.Get( "total_KE" )
  grinel = f.Get( "inel_KE" )
  grel   = f.Get( "el_KE" )


maxtot = max( [grtotal.GetY()[i] for i in range(0, grtotal.GetN())] )

grtotal.SetMaximum(1.2 * maxtot)
grtotal.SetMinimum(0.)
grtotal.GetXaxis().SetRangeUser(0, int(sys.argv[4]))
grinel.GetXaxis().SetRangeUser(0, int(sys.argv[4]))
grel.GetXaxis().SetRangeUser(0, int(sys.argv[4]))
#grtotal.SetTitle( sys.argv[2] + ";" + sys.argv[3] +" Kinetic Energy (MeV);#sigma (mb)")
if sys.argv[2] == "momentum":
  grtotal.SetTitle( ";" + sys.argv[3] +" Momentum (MeV/c);#sigma (mb)")
  grinel.SetTitle( ";" + sys.argv[3] +" Momentum (MeV/c);#sigma (mb)")
  grel.SetTitle( ";" + sys.argv[3] +" Momentum (MeV/c);#sigma (mb)")
else:
  grtotal.SetTitle( ";" + sys.argv[3] +" Kinetic Energy (MeV);#sigma (mb)")
grinel.GetXaxis().SetTitleSize(.05)
grinel.GetYaxis().SetTitleSize(.05)
grinel.GetXaxis().SetTitleOffset(.9)
grinel.GetYaxis().SetTitleOffset(1.125)
grinel.GetXaxis().SetLabelSize(.04)
grinel.GetYaxis().SetLabelSize(.04)


grtotal.SetLineColor(4)
grel.SetLineColor(2)

c1 = RT.TCanvas("c1","c1",500,400)
c1.SetTickx(1)
c1.SetTicky(1)
RT.gStyle.SetOptStat(0)
RT.gPad.SetLeftMargin( RT.gPad.GetLeftMargin() * 1.20 )


#grtotal.Draw("AC")
#grinel.Draw("same C")
grinel.Draw("AC")
grel.Draw("same C")

leg = RT.TLegend(.6,.6,.85,.85)
#leg.AddEntry( grtotal,  "Total",  "l")
leg.AddEntry( grinel, "Inelastic",  "l")
leg.AddEntry( grel, "Elastic",  "l")

leg.Draw("same")
RT.gPad.RedrawAxis()
c1.SaveAs( sys.argv[5])
