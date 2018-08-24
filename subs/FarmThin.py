from subprocess import call, Popen, PIPE
from os import listdir as ls
import sys
from argparse import ArgumentParser 

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-f', type=str, help='Comma separated list: Folders')
  parser.add_argument('-s', type=str, help='Comma separated list: Samples')
  parser.add_argument('-t', type=str, help='Timeout Limit')
  parser.add_argument('-m', type=str, help='Memory Limit')
  parser.add_argument('-o', type=str, help='Name of makeup file list')
  return parser

def check_samples(samples, samp):
  for sample in samples:
    if sample in samp:
      return sample
  return None 


args = init_parser().parse_args()
folders = args.f
folders = folders.split(",")
samples = args.s
samples = samples.split(",")
timeout = args.t
memory = args.m
output = file(args.o,"w")

output.write("<Jobs>\n")

for folder in folders:
  print folder
  samps = ls(folder)
  for samp in samps:
    if ("makeup" in samp): continue
#    if ( "_50MeV" in samples):
    check = check_samples(samples,samp)
    if(check):
      print "Sending sample", samp
      theSub = samp.replace("batch","makeup")
      p = Popen(["jobsub_submit","-N","5","-M","--OS=SL6","--group=dune","--memory="+memory,"--timeout="+timeout,"--resource-provides=usage_model=OPPORTUNISTIC","file:///dune/app/users/calcuttj/geant/GeantReweight/subs/"+folder+"/"+samp], stdout=PIPE, stderr=PIPE)
      stdout, stderr = p.communicate()
      if not stderr: 
        for line in stdout.split("\n"):
          if "first job" in line:
            jobid = (line.split(":")[1]).strip(" ")
          if "job(s)" in line:
            nJobs = (line.split(" ")[0]).strip(" ")
        print theSub 
        print "Found jobid:", jobid
        print "nJobs: ", nJobs
        nomvar = "nom" if ("nom" in theSub) else "var"
        print nomvar
        jobid_pre = jobid.split("@")[0]
        print jobid.split("@")
        output.write(' <Sub Name="' + theSub +'" Sample="' + check + '">\n')
        for i in range(0,int(nJobs)):
          jobid_end_num = int(jobid_pre[-1]) + i 
          new_jobid = jobid_pre[0:-1] + str(jobid_end_num)
          outline = '\t<Job ID="' + new_jobid + '" N="' + str(i) + '" Type="' + nomvar  + '"/>\n '
          output.write(outline)
        output.write("</Sub>\n")
      else: print "Error\n", stderr            
output.write("</Jobs>") 
