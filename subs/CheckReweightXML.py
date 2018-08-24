from xml.etree.ElementTree import parse
from argparse import ArgumentParser 
from glob import glob as ls
from ROOT import * 
def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-x', type=str, help='XML file')
  parser.add_argument('-f', type=str, help='Search path')
  parser.add_argument('-m', type=str, help='Makeup file name')
  parser.add_argument('-r', type=str, help='Reweight file name')
  return parser

args = init_parser().parse_args()
xml_file = args.x
makeup_file = args.m
makeup = file(makeup_file,"w")
reweight_file = args.r
reweight = file(reweight_file,"w")

folder = args.f

print "Config file: ", xml_file 
theXML = parse(xml_file)


variations = ["inel1.5_elast1_", "inel1_elast1.5_", "inel1.5_elast1.5_"]

all_dirs = []


for sub in theXML.findall('Sub'):
  print sub.get('Name')
  makeup.write(sub.get('Name') + " ")
  for job in sub.findall('Job'):
    print job.get('ID'), job.get('N'), job.get('Type')
    theID = job.get('ID')
    all_dirs.append(theID)
    theN = job.get('N')
    theType = job.get('Type')
    files = ls(folder + "/" + theID + "/*")
    trimmed_files = [f.split("/")[-1] for f in files]  
    newline = True
    if "nom" in theType:
      if len(trimmed_files) > 0:
        template = [] 
        for f in trimmed_files:
          if(theType in f.split("_")[1]):
            template = f.split("_")
 
        for v in variations:
          check = False
  
          for f in trimmed_files:
            if(theType in f.split("_")[1]): continue 
  
            if v in f:                
              check = True
  
          if not check: 
            #form v
            split_v = v.split("_")
            inel = split_v[0].replace("inel","")
            elast = split_v[1].replace("elast","")
  
            reweight.write(theID + " " + ("_").join(template) + " " + inel + " " + elast + " ")
            newline = False
            print "Missing variation", v 
  
            filename = "_".join([template[0],template[2],template[1],v[0:-1],template[3]])
            reweight.write(filename + "\n")
      else:
        makeup.write(theN)
        print "Missing Sim file" 
    elif "var" in theType:
      if len(trimmed_files) > 0:
        template = [] 
        for f in trimmed_files:
          if(theType in f.split("_")[1]):
            template = f.split("_")
        
        for v in variations:
          if v in (sub.get('Name')).replace(".sh","_"):
            check = False
      
            for f in trimmed_files:
              if(theType in f.split("_")[1]): continue 
      
              if v in f:                
                check = True
      
            if not check: 
              #form v
              split_v = v.split("_")
              inel = split_v[0].replace("inel","")
              elast = split_v[1].replace("elast","")
      
              reweight.write(theID + " " + ("_").join(template) + " " + inel + " " + elast + " ")
              newline = False
              print "Missing variation", v 
      
              filename = "_".join([template[0],template[4],template[1],v[0:-1],template[-1]])
              reweight.write(filename + "\n")
            break
      else:
        makeup.write(theN + " ")
        print "Missing Sim file", theN
  
  makeup.write("\n")

