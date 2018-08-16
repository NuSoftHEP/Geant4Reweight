from subprocess import call
from os import listdir as ls
import sys

for folder in sys.argv[1:]:
  print folder
  samps = ls(folder)
  for samp in samps:
    if ( "_50MeV" in samp):
      print "Sending sample", samp
      call(["jobsub_submit","-N","5","-M","--OS=SL6","--group=dune","--memory=1GB","--timeout=600s","--resource-provides=usage_model=OPPORTUNISTIC","file:///dune/app/users/calcuttj/geant/GeantReweight/subs/"+folder+"/"+samp])

    
