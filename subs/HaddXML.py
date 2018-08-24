from xml.etree.ElementTree import parse
from argparse import ArgumentParser 
from glob import glob as ls
def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-x', type=str, help='XML file')
  parser.add_argument('-f', type=str, help='Search path')
  parser.add_argument('-o', type=str, help='Output file name')
  return parser

args = init_parser().parse_args()
xml_file = args.x
out_file = args.o
folder = args.f

print "Config file: ", xml_file 
theXML = parse(xml_file)

bad_samples = {}

out = file(out_file,"w")

for sub in theXML.findall('Sub'):
  print sub.get('Name')
  out.write(sub.get('Name') + " ")
  for job in sub.findall('Job'):
    print job.get('ID'), job.get('N'), job.get('Sub'), job.get('Type')
    theID = job.get('ID')
    theN = job.get('N')
    theType = job.get('Type')

    files = ls(folder + "/" + theID + "/*")
    
    if len(files) > 0:
      if "nom" in theType :
        if not ( len(files) == 4): 
          out.write(theN+" ")
      elif "var" in theType:
        if not ( len(files) == 2): 
          out.write(theN+" ")
    else:
      out.write(theN+" ")
  out.write("\n")
 
