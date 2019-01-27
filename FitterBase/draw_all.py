from ROOT import *
import sys
from array import array
import operator

f = TFile( sys.argv[1] )
sample = sys.argv[2]
print sample



MC_all = dict() 
###Getting MC graphs


##variations
for top_key in f.GetListOfKeys(): 
  if "Data" in top_key.GetName() or "chi2" in top_key.GetName():
    continue

  MC_sets = dict()
  theDir = f.Get(top_key.GetName())
  keys = theDir.GetListOfKeys()
  
  ##experiments
  for key in keys:
    if (sample in key.GetName()) or ("DUET" in key.GetName() and sample == "C_PiPlus"):
      print key.GetName()
      graph_keys = f.Get("abs1.00_cex1.00/" + key.GetName()).GetListOfKeys() 
      for gr_key in graph_keys: 
        print gr_key.GetName()
  
        if not (gr_key.GetName() in MC_all.keys()):
          print "Making"
          MC_all[gr_key.GetName()] = []

        if not (gr_key.GetName() in MC_sets.keys()):
          MC_sets[gr_key.GetName()] = []
  
        gr = f.Get(top_key.GetName() + "/" + key.GetName() + "/" + gr_key.GetName())
  
        for i in range(0, gr.GetN()):
          print gr.GetX()[i] , gr.GetY()[i]
          MC_sets[gr_key.GetName()].append( ( gr.GetX()[i] , gr.GetY()[i] ) )

  print "Got", top_key.GetName()
  for name,MC_set in MC_sets.iteritems():
    print name, MC_set
    MC_all[ name ].append( MC_set )      
  print
#######################

for name, the_sets in MC_all.iteritems():
  print name, the_sets
print


###Geting data graphs
data_all = dict()
data_maxs = dict()
theDir = f.Get("Data")
keys = theDir.GetListOfKeys()

for key in keys:
  if (sample in key.GetName()):
    graph_keys = f.Get("Data/" + key.GetName()).GetListOfKeys() 
    for gr_key in graph_keys: 
      print gr_key.GetName()

      if not (gr_key.GetName() in data_all.keys()):
        data_all[gr_key.GetName()] = []
        data_maxs[gr_key.GetName()] = 0 

      gr = f.Get("Data/" + key.GetName() + "/" + gr_key.GetName() )

      data_all[ gr_key.GetName() ].append( (gr, key.GetName().replace("_" + sample, "" )  ) )
          
      ys  = gr.GetY()
      eys = gr.GetEY()
      n   = gr.GetN()

      tops = []
      for i in range(0,n):
        y  = ys[i]
        ey = eys[i]
        tops.append( y + ey )
      
      if( max(tops) > data_maxs[ gr_key.GetName() ] ):
        data_maxs[ gr_key.GetName() ] = max(tops)

  elif ("DUET" in key.GetName() and sample == "C_PiPlus"):
    duet_graphs = ["xsec_abs", "xsec_cex"]
    
    for dgr in duet_graphs:
      print dgr 

      if not ( dgr[-3:] in data_all.keys()):
        data_all[dgr[-3:]] = []
        data_maxs[dgr[-3:]] = 0 

      
      gr = f.Get("Data/" + key.GetName() + "/" + dgr )

      data_all[ dgr[-3:] ].append( ( gr, "DUET" ) )

      ys  = gr.GetY()
      eys = gr.GetEY()
      n   = gr.GetN()

      tops = []
      for i in range(0,n):
        y  = ys[i]
        ey = eys[i]
        tops.append( y + ey )
      
      if( max(tops) > data_maxs[ gr_key.GetName() ] ):
        data_maxs[ gr_key.GetName() ] = max(tops)

#######################

cut_to_names = {
  "reac"  : "Reactive",
  "abs"   : "Absorption",
  "cex"   : "Charge Exchange",
  "abscx" : "Absorption + Charge Exchange",
  "inel"  : "Inelastic (Quasi-Elastic)"
}

sample_to_type = {
  "C_PiPlus" : "#pi^{+},C - ",
  "C_PiMinus" : "#pi^{-},C - ",
}


    
for name,cut in MC_all.iteritems():  
  print name

  cut_max = 0.

  cut_grs = []

  for MC_set in cut: 

    MC_set.sort(key=operator.itemgetter(0))
    print MC_set
  
    new_xs = array("d", zip(*MC_set)[0])
    new_ys = array("d", zip(*MC_set)[1])
  
    new_gr = TGraph( len(new_xs), new_xs, new_ys )
    new_gr.SetMarkerStyle(20)
    new_gr.SetMarkerColor(2)
    new_gr.SetLineColor(2)
    new_gr.SetMinimum(0.)
  
    cut_grs.append( new_gr )
    print max(new_ys)
    if max(new_ys) > cut_max:
      cut_max = max(new_ys)

  c1 = TCanvas("500", "400")
  c1.SetTickx(1)
  c1.SetTicky(1)
  gPad.SetLeftMargin(gPad.GetLeftMargin()*1.10) 
  gPad.SetBottomMargin(gPad.GetBottomMargin()*1.10) 
#  leg = TLegend()

  if( len( cut_grs ) > 0 ):
    if( cut_max < data_maxs[name]*1.2 ):
      cut_grs[0].SetMaximum( data_maxs[name]*1.2 )
    
    cut_grs[0].Draw("APL")
    cut_grs[0].SetTitle( sample_to_type[ sample ] + cut_to_names[ name ] + ";Pion Momentum (MeV/c);#sigma (mb)" )
    cut_grs[0].GetXaxis().SetTitleSize(.06)
    cut_grs[0].GetYaxis().SetTitleSize(.06)
    cut_grs[0].GetXaxis().SetTitleOffset(.90)
    cut_grs[0].GetYaxis().SetTitleOffset(.90)
    cut_grs[0].GetXaxis().SetLabelSize(.05)
    cut_grs[0].GetYaxis().SetLabelSize(.05)
    cut_grs[0].GetXaxis().SetNdivisions(505)
    cut_grs[0].GetYaxis().SetNdivisions(505)

    for gr in cut_grs[1:]: 
      gr.SetMarkerStyle(20)
      gr.SetMarkerColor(2)
      gr.SetLineColor(2)
      gr.Draw("PL same")


    for gr,exp_name in data_all[ name ]:
      gr.SetLineColor(1)
      gr.SetMarkerColor(1)
      gr.SetMarkerStyle(20)
      gr.Draw("PE same")

    c1.SaveAs(sample + "_" + name + ".pdf")
  print 
