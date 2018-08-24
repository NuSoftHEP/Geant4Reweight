from subprocess import call
from os import listdir as ls
from os import environ as env 
from os import system as syst 
from os import remove as rm
import sys
import glob
basedir="/pnfs/dune/scratch/users/calcuttj/GeantReweight/Sim/"
target = "/pnfs/dune/scratch/users/calcuttj/GeantReweight/Sim_Full/"
infile = file("hadd_locs.txt","r")

for line in infile:
  print line.split()
  print target+line.split()[0]
  for d in line.split()[1:]: 
    print glob.glob(basedir+d+"/"+line.split()[0]+"*")[0]
  command = ["hadd","-f", line.split()[0] + "full.root"]  
  command = command + [ glob.glob(basedir+d+"/"+line.split()[0]+"*")[0] for d in line.split()[1:] ] 
  call(command)

  command = ["mv", line.split()[0]+"full.root", target]
  call(command)

