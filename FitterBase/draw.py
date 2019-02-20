from ROOT import * 
import sys

filename = sys.argv[1]
f = TFile( filename )

theDir = sys.argv[2]

exp = sys.argv[3]

cut = sys.argv[4]

data = f.Get("Data/" + exp + "/" + cut)
data.SetMarkerColor(1)
data.SetMarkerStyle(20)

MC = f.Get(theDir + "/" + exp + "/" + cut)
chi2_val = f.Get(theDir + "/" + exp + "/" + cut + "_chi2")

c1 = TCanvas("500", "400")
c1.SetTickx(1)
c1.SetTicky(1)

MC.SetMinimum(0.)
data.SetMinimum(0.)
MC.SetLineColor(2)
MC.SetMarkerColor(2)
MC.SetMarkerStyle(20)

leg = TLegend( .55, .68, .88, .88 )
leg.AddEntry( None, "#chi^{2} = " + "{:.2f}".format( chi2_val(0) ), "" )
leg.AddEntry( data, "Data", "lp" )
leg.AddEntry( MC, "Geant", "lp" )


data_points = []
MC_points = []
for i in range(0, data.GetN()):
  data_points.append( data.GetY()[i] + data.GetEY()[i] )
  MC_points.append( MC.GetY()[i] )

if( max(data_points) > max(MC_points) ):
  data.SetMaximum( max(data_points) * 1.5 )
else: 
  data.SetMaximum( max(MC_points) * 1.5 )

data.SetTitle( ";Momentum (MeV/c);#sigma (mb)" )
data.GetXaxis().SetTitleSize(.06)
data.GetXaxis().SetTitleOffset(.75)
data.GetYaxis().SetTitleSize(.06)
data.GetYaxis().SetTitleOffset(.75)
data.GetXaxis().SetLabelSize(.04)
data.GetYaxis().SetLabelSize(.04)
data.Draw("AP")
MC.Draw("PC same")
#else:
#  MC.SetMaximum( max(data_points) * 1.5 )
#  MC.SetTitle( str(chi2_val(0)) )
#  MC.Draw("APC")
#  data.Draw("P same")

leg.Draw("same")

c1.SaveAs(sys.argv[5])
