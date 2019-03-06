from ROOT import * 
import sys
from array import array


def FindMinimum( infile ):
  f = TFile( infile, "OPEN" )
  
  topdirs = [i.GetName() for i in f.GetListOfKeys()]
  print topdirs
  
  topdirs.remove("Data")
  topdirs.remove("FitTree")
  
  topdir = topdirs[0]
  print topdir
  thisdir = f.Get( topdir +  "/Parameters" )
  par_names = [ i.GetName() for i in thisdir.GetListOfKeys() ]
  print par_names
  
  tree = f.Get( "FitTree" )
  Chi2_br = array( "d", [0] )
  tree.SetBranchAddress( "Chi2", Chi2_br )
  pars = dict()
  par_vals = dict()
  for par_name in par_names:
    pars[ par_name ] = array( "d", [0] )
    tree.SetBranchAddress( par_name, pars[ par_name ] )
    par_vals[ par_name ] = []
  
  chi2 = []
  for e in tree:
    chi2.append( Chi2_br[0] )
    for name,val in pars.iteritems():
      print name, val[0]
      par_vals[ name ].append( val[0] )
  
  print chi2
  print par_vals
  
  min_index = chi2.index( min(chi2) )
  print "Minimum Chi2:", min(chi2)
  print "Point:"
  for name,vals in par_vals.iteritems():
    print name, vals[ min_index ]

  return [par_vals, min_index, topdirs[min_index] ]

if __name__ == '__main__':
 it = FindMinimum( sys.argv[1] )
 print it[1], it[2]
