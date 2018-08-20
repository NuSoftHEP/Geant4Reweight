from subprocess import call
from os import listdir as ls
from os import environ as env 
from os import system as syst 
from os import remove as rm
import sys
from argparse import ArgumentParser 

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-f', type=str, help='Folder')
  return parser

args = init_parser().parse_args()
folder = args.f 

good_samples = {}
bad_samples = {}
found = []
found_samples = {}

for sample in ls(folder):
  if "makeup" in sample.split("_"): continue
  good_samples[sample] = []
  bad_samples[sample] = ["0","1","2","3","4"]
#  if folder == "nom":
  if "nom" in folder:
    print sample.split("_")[-2], sample.split("_")[-1].replace(".sh","")   
    energy = sample.split("_")[-2]
    samp = sample.split("_")[-1].replace(".sh","")
#  elif folder == "var": 
  elif "var" in folder:
    energy = sample.split("_")[-4]
    samp = "_".join(sample.split("_")[-3:]).replace(".sh","") + "_"
    print "SAMPLE:", samp
  for outdir in ls("/pnfs/dune/scratch/users/calcuttj/GeantReweight/Sim/"): 
    if outdir in found: continue
    if ( len( ls("/pnfs/dune/scratch/users/calcuttj/GeantReweight/Sim/" + outdir)) == 0 ): continue
    this_file = ls("/pnfs/dune/scratch/users/calcuttj/GeantReweight/Sim/" + outdir)[0]
    print this_file
    if (energy in this_file) and (samp in this_file): 
      print this_file.replace(".root","")[-1]
      found.append(outdir)      
      good_samples[sample].append(this_file.replace(".root","")[-1])
      bad_samples[sample].remove(this_file.replace(".root","")[-1])

print "Found", len(found), "Good", folder, "samples"

outfile = file(folder.replace("/","_")+"bad_samples.txt",'w')

print "Good samples:" 
for sample, goods in good_samples.items():
  print sample, goods
print "Bad samples:"
for sample, bads in bad_samples.items():
  print sample, bads
  line = sample.replace("batch","makeup")
  for bad in bads: line = line + " " + bad
  outfile.write(line+"\n")

