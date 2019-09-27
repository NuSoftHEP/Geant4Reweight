from subprocess import call, Popen, PIPE
from os import listdir as ls
from os import environ
import sys
from argparse import ArgumentParser 
from xml.etree.ElementTree import parse, TreeBuilder, tostring, fromstring, ElementTree
import xml.dom.minidom as xdm

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-t', type=str, help='Timeout Limit')
  parser.add_argument('-m', type=str, help='Memory Limit')
  parser.add_argument('-o', type=str, help='Name of makeup file list')
  parser.add_argument('-b', type=str, help='Batch File')
  parser.add_argument('-x', type=str, help='Input XML File')
  return parser

def check_samples(samples, samp):
  for sample in samples:
    if sample in samp:
      return sample
  return None 



args = init_parser().parse_args()
timeout = args.t
memory = args.m
batch = args.b
xml_file = args.x

builder = TreeBuilder()
builder.start("Makeups",{})

theXML = parse(xml_file)

for sub in theXML.findall("Sub"):
 
  sample = sub.get("Sample")

  for job in sub.findall("Makeup"):

    my_env = environ
    my_env["SAMPLE"] = sample 
    my_env["MAKEUP"] = job.get("N")    
    
    print "Sending sample", sample
    command = ["jobsub_submit","-N","1","-M","--OS=SL6","--group=dune","--memory="+memory,"--timeout="+timeout,"--resource-provides=usage_model=OPPORTUNISTIC", "-e", "SAMPLE", "-e", "MAKEUP", "file:///dune/app/users/calcuttj/geant/GeantReweight/subs/" + batch]
    p = Popen(command, env = my_env, stdout=PIPE, stderr=PIPE)

    stdout, stderr = p.communicate()
    if not stderr: 
      for line in stdout.split("\n"):
        if "first job" in line:
          jobid = (line.split(":")[1]).strip(" ")
        if "job(s)" in line:
          nJobs = (line.split(" ")[0]).strip(" ")
      theSub = "Thin_"+sample+"_nom"
      print theSub 
      print "Found jobid:", jobid
      print "nJobs: ", nJobs
      nomvar = "nom" if ("nom" in theSub) else "var"
      print nomvar
      jobid_pre = jobid.split("@")[0]
      print jobid.split("@")
      builder.start("Sub", {"Name":theSub, "Sample":sample, "Type":"nom"})
      builder.start("Job",{"ID":job.get("ID"), "New":jobid_pre,"N":job.get("N")})
      builder.end("Job")
      builder.end("Sub")
    else: print "Error\n", stderr            
builder.end("Makeups")

root = builder.close()
rough = tostring(root, 'utf-8')
reparsed = xdm.parseString(rough) 
tree = ElementTree(fromstring(reparsed.toprettyxml(indent=" ")))
tree.write(args.o)
