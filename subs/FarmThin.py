from subprocess import call
from os import listdir as ls
import sys
from argparse import ArgumentParser 

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-f', type=str, help='Comma separated list: Folders')
  parser.add_argument('-s', type=str, help='Comma separated list: Samples')
  return parser

def check_samples(samples, samp):
  for sample in samples:
    if sample in samp:
      return True
  return False


args = init_parser().parse_args()
folders = args.f
folders = folders.split(",")
samples = args.s
samples = samples.split(",")
#for folder in sys.argv[1:]:
for folder in folders:
  print folder
  samps = ls(folder)
  for samp in samps:
    if ("makeup" in samp): continue
#    if ( "_50MeV" in samples):
    if(check_samples(samples,samp)):
      print "Sending sample", samp
      call(["jobsub_submit","-N","5","-M","--OS=SL6","--group=dune","--memory=1GB","--timeout=2000s","--resource-provides=usage_model=OPPORTUNISTIC","file:///dune/app/users/calcuttj/geant/GeantReweight/subs/"+folder+"/"+samp])

    
