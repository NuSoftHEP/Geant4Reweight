from subprocess import call, Popen, PIPE
from os import listdir as ls
from os import environ
import sys
from argparse import ArgumentParser 
from xml.etree.ElementTree import parse, TreeBuilder, tostring, fromstring, ElementTree
import xml.dom.minidom as xdm
#from xml.dom.minidom import toprettyxml as pretty
#from xml.dom.minidom import parseString as PS 

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-s', type=str, help='Comma separated list: Samples')
  parser.add_argument('-t', type=str, help='Timeout Limit')
  parser.add_argument('-m', type=str, help='Memory Limit')
  parser.add_argument('-o', type=str, help='Name of makeup file list')
  parser.add_argument('-b', type=str, help='Batch File')
  parser.add_argument('--name', type=str, help='Name of submission')
  parser.add_argument('--exe', type=str, help='Which executable')
  return parser

def check_samples(samples, samp):
  for sample in samples:
    if sample in samp:
      return sample
  return None 

#variations = [ ["1.5","1"], ["1","1.5"], ["1.5","1.5"] ]

args = init_parser().parse_args()
samples = args.s
samples = samples.split(",")
timeout = args.t
memory = args.m
batch = args.b
exe = args.exe

builder = TreeBuilder()
builder.start("Jobs",{})

#sample = samples[0]

for var in variations:
  for sample in samples:
    my_env = environ
    my_env["SAMPLE"] = sample 
    my_env["INEL"] = var[0]
    my_env["ELAST"] = var[1]
    my_env["EXE"] = exe
    my_env["NAME"] = args.name
    print "Sending sample", sample, var
    command = ["jobsub_submit","-N","5","-M","--OS=SL6","--group=dune","--memory="+memory,"--timeout="+timeout,"--resource-provides=usage_model=OPPORTUNISTIC", "-e", "SAMPLE", "-e", "NAME", "-e", "EXE", "-e", "INEL", "-e", "ELAST", "file:///dune/app/users/calcuttj/geant/GeantReweight/subs/" + batch]
    p = Popen(command, env = my_env, stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()
    if not stderr: 
      for line in stdout.split("\n"):
        if "first job" in line:
          jobid = (line.split(":")[1]).strip(" ")
        if "job(s)" in line:
          nJobs = (line.split(" ")[0]).strip(" ")
      theSub = args.name 
      print theSub 
      print "Found jobid:", jobid
      print "nJobs: ", nJobs
      nomvar = "var"
      print nomvar
      jobid_pre = jobid.split("@")[0]
      print jobid.split("@")
      builder.start("Sub", {"Name":theSub, "Sample":sample, "Type":"var", "Inel":var[0], "Elast":var[1]})
      for i in range(0,int(nJobs)):
        jobid_end_num = int(jobid_pre[-1]) + i 
        new_jobid = jobid_pre[0:-1] + str(jobid_end_num)
        builder.start("Job", {"ID": new_jobid , "N":str(i)})
        builder.end("Job")
      builder.end("Sub")
    else: print "Error\n", stderr            
builder.end("Jobs")

root = builder.close()
rough = tostring(root, 'utf-8')
reparsed = xdm.parseString(rough) 
tree = ElementTree(fromstring(reparsed.toprettyxml(indent=" ")))
tree.write(args.o)
