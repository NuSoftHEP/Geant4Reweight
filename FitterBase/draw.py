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

c1 = TCanvas("500", "400")
c1.SetTickx(1)
c1.SetTicky(1)

MC.SetMinimum(0.)
data.SetMinimum(0.)
MC.SetLineColor(2)
MC.SetMarkerColor(2)
MC.SetMarkerStyle(20)

data_points = []
MC_points = []
for i in range(0, data.GetN()):
  data_points.append( data.GetY()[i] + data.GetEY()[i] )
  MC_points.append( MC.GetY()[i] )

if( max(data_points) > max(MC_points) ):
  data.Draw("AP")
  MC.Draw("PC same")
else:
  MC.Draw("APC")
  data.Draw("P same")

c1.SaveAs("draw.pdf")
