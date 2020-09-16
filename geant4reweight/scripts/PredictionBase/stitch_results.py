#from ROOT import *
import ROOT as RT
import sys
from array import array

fout = RT.TFile(sys.argv[1], "RECREATE")

files = sys.argv[3:]

print("Files:")
for f in files: print(f)

trees = []

if int(sys.argv[2]) == 211:
  graphs = [
    "abs",
    "cex",
    "dcex",
    "inel",
    "prod"
  ]
elif int(sys.argv[2]) == 2212:
  graphs = [
    "0n0p", "0n1p", "1n0p", "1n1p", "Other"
  ]

for gr_name in graphs: 
  print(gr_name)
  new_xs = []
  new_ys = []
  for f in files:
    fin = RT.TFile(f, "OPEN")
    gr = fin.Get( gr_name )
    for i in range( 0, gr.GetN() ):
      new_xs.append( gr.GetX()[i] )
      new_ys.append( gr.GetY()[i] )
    fin.Close()
       

#  print new_xs
#  print new_ys
  new_gr = RT.TGraph( len( new_xs ), array("f", new_xs), array("f", new_ys) )
  fout.cd()
  new_gr.Write( gr_name )

#tree_list = TList();
#
#newTree = TTree("tree","")
#for f in files:
#  print "Adding tree from", f
#  fin = TFile(f, "OPEN")
#  newTree.CopyAddresses(fin.Get("tree"))
#  newTree.CopyEntries(fin.Get("tree"))
#
#fout.cd()
##newTree = TTree.MergeTrees(tree_list)
##newTree.SetName("tree")
#newTree.Write()

fout.Close()
