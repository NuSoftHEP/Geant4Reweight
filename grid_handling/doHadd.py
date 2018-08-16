from subprocess import call
from os import listdir as ls
from os import environ as env 
from os import system as syst 
from os import remove as rm
import sys
import glob
basedir="/pnfs/dune/scratch/users/calcuttj/GeantReweight/Thin/"
infile = file("hadd_locs.txt","r")

for line in infile:
  print line.split()
  command = ["hadd", line.split()[0] + "full.root"]  
  command = command + [ glob.glob(basedir+d+"/"+line.split()[0]+"*")[0] for d in line.split()[1:] ] 
  call(command)

