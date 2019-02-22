from ROOT import * 
import sys
gROOT.SetBatch(1)

filename = sys.argv[1]
f = TFile( filename )

theDir = sys.argv[2]
theDirs = []
if( theDir == "all" ):
  for k in f.GetListOfKeys():
    if (k.GetName() == "Data" or k.GetName() == "chi2_surf"): continue

    theDirs.append( k.GetName() )
else: theDirs.append( theDir )

print theDirs

dataDir = f.Get("Data")
exps = [k.GetName() for k in dataDir.GetListOfKeys()]
##experiments
for exp in exps:
  expDir = f.Get("Data/" + exp)
  cuts = [k.GetName() for k in expDir.GetListOfKeys()]
  for cut in cuts:
    if "cov" in cut: continue
    print exp, cut
    data = f.Get("Data/" + exp + "/" + cut)
    data.SetMarkerColor(1)
    data.SetMarkerStyle(20)
    
    MCs = []
    chi2_vals = []
    for Dir in theDirs:
      MC = f.Get(Dir + "/" + exp + "/" + cut)
      MC.SetLineColor(2)
      MC.SetMarkerColor(2)
      MC.SetMarkerStyle(20)

      MCs.append(MC)
      chi2_vals.append(f.Get(Dir + "/" + exp + "/" + cut + "_chi2"))
    
    c1 = TCanvas("500", "400")
    c1.SetTickx(1)
    c1.SetTicky(1)
    
    #MC.SetMinimum(0.)
    #data.SetMinimum(0.)
    
    leg = TLegend( .55, .75, .88, .88 )
    for MC_chi2,subdir in zip( zip(MCs, chi2_vals), theDirs):
      leg.AddEntry( MC_chi2[0], subdir + " #chi^{2} = " + "{:.2f}".format( MC_chi2[1](0) ), "lp" )
    leg.AddEntry( data, "Data", "lp" )
    
    
    data_points = []
    data_points_low = []
    MC_points = []
    for i in range(0, data.GetN()):
      data_points.append( data.GetY()[i] + data.GetEY()[i] )
      data_points_low.append( data.GetY()[i] - data.GetEY()[i] )
      for MC in MCs: 
        MC_points.append( MC.GetY()[i] )

    print "max data:", max(data_points)
    print "max MC:", max(MC_points)
    print "min data:", min(data_points_low)
    print "min MC:", min(MC_points)

    
    if( max(data_points) > max(MC_points) ):
      data.SetMaximum( max(data_points) * 1.5 )
    else: 
      data.SetMaximum( max(MC_points) * 1.5 )

    if( min(data_points_low) < min(MC_points) ):
      data.SetMinimum( min(data_points_low) * .75 )
    else: 
      data.SetMinimum( min(MC_points) * .75 )
    
    data.SetTitle( exp + " " + cut + " ;Momentum (MeV/c);#sigma (mb)" )
    data.GetXaxis().SetTitleSize(.06)
    data.GetXaxis().SetTitleOffset(.75)
    data.GetYaxis().SetTitleSize(.06)
    data.GetYaxis().SetTitleOffset(.75)
    data.GetXaxis().SetLabelSize(.04)
    data.GetYaxis().SetLabelSize(.04)
    data.Draw("AP")
    for MC in MCs:
      MC.Draw("PL same")
    
    leg.Draw("same")
    
    c1.SaveAs(exp + "_" + cut + ".pdf")
