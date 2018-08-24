from subprocess import call
from os import listdir as ls
from os import environ as env 
from os import system as syst 
from os import remove as rm
import sys
from argparse import ArgumentParser 

def init_parser():
  parser = ArgumentParser()
  parser.add_argument("-f", type=str, help="folder") 
  return parser

args = init_parser().parse_args()
folder = args.f

energies = ["50MeV", "100MeV", "150MeV", "200MeV", "250MeV",
            "300MeV", "400MeV", "500MeV", "600MeV", "700MeV", 
            "800MeV", "1000MeV", "1200MeV"]
nomvar = ["nom", "var"]            
samples = ["inel1.5_elast1_", "inel1_elast1.5_", "inel1.5_elast1.5_"]

#basedir="/pnfs/dune/scratch/users/calcuttj/GeantReweight/Thin/"
basedir=folder


found = []
found_samples = {}

if "Sim" in basedir:
  first = "Ar_"
else:
  first = "thin_"

for energy in energies:
  for it in nomvar:
    for samp in samples:
      print first+energy+"_"+it+"_"+samp
      found_samples[first+energy+"_"+it+"_"+samp] = []



for outdir in ls(basedir): 
  for f in ls(basedir + outdir):
    print f[0:-6]
    if(f.split("_")[1] in "var" or f.split("_")[1] in "nom"): continue 
    print f[0:-6]
    
    found_samples[f[0:-6]].append(outdir)
    print "Appended"

  
outfile = file("hadd_locs.txt","w")

for sample, outlist in found_samples.items():
  print sample, len(outlist)
  line = sample
  for out in outlist: line = line + " " + out
  outfile.write(line + "\n")


