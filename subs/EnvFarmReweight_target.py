from subprocess import call, Popen, PIPE
from os import listdir as ls
from os import environ
import sys
from argparse import ArgumentParser 
from xml.etree.ElementTree import parse, TreeBuilder, tostring, fromstring, ElementTree
import xml.dom.minidom as xdm

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-t', type=str, help='Timeout Limit', default='8h')
  parser.add_argument('-m', type=str, help='Memory Limit', default='2000')
  parser.add_argument('-o', type=str, help='Name of makeup file list')
  parser.add_argument('-b', type=str, help='Batch File')
  parser.add_argument('--file', type=str, help='File name')
  parser.add_argument('--filedir', type=str, help='File location')
  return parser


args = init_parser().parse_args()
timeout = args.t
memory = args.m
batch = args.b
theFile = args.file
theFileDir = args.filedir

my_env = environ
my_env["FILE"] = theFile
my_env["FILEDIR"] = theFileDir
command = ["jobsub_submit","-N","5","-M","--OS=SL6","--group=dune","--memory="+memory,"--timeout="+timeout,"--resource-provides=usage_model=OPPORTUNISTIC", "-e", "FILE", "-e", "FILEDIR", "file:///dune/app/users/calcuttj/geant/GeantReweight/subs/" + batch]
p = Popen(command, env = my_env, stdout=PIPE, stderr=PIPE)
stdout, stderr = p.communicate()
if not stderr: 
  for line in stdout.split("\n"):
    if "first job" in line:
      jobid = (line.split(":")[1]).strip(" ")
    if "job(s)" in line:
      nJobs = (line.split(" ")[0]).strip(" ")
  print "Found jobid:", jobid
  print "nJobs: ", nJobs
  print jobid.split("@")
else: print "Error\n", stderr            

