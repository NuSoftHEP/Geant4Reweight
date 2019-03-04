from ROOT import * 
import sys
from array import array

f = TFile( sys.argv[1], "OPEN" )

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
pars = dict()
for par_name in par_names:
  pars[ par_name ] = array( "d", [0] )
  tree.SetBranchAddress( par_name, pars[ par_name ] )

for e in tree:
  for name,val in pars.iteritems():
    print name, val[0]
    
