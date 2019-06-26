from ROOT import *
import sys
from array import array

fout = TFile(sys.argv[1], "RECREATE")

files = sys.argv[2:]

print "Files:"
for f in files: print f

trees = []

graphs = [
  "abs",
  "cex",
  "dcex",
  "inel",
  "prod"
]

for gr_name in graphs: 
  print gr_name
  new_xs = []
  new_ys = []
  for f in files:
    fin = TFile(f, "OPEN")
    gr = fin.Get( gr_name )
    for i in range( 0, gr.GetN() ):
      new_xs.append( gr.GetX()[i] )
      new_ys.append( gr.GetY()[i] )
    fin.Close()
       

#  print new_xs
#  print new_ys
  new_gr = TGraph( len( new_xs ), array("f", new_xs), array("f", new_ys) )
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
