from xml.etree.ElementTree import parse
from argparse import ArgumentParser 
from glob import glob as ls


def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-x', type=str, help='XML file')
  parser.add_argument('-i', type=str, help='input file')
  parser.add_argument('-f', type=str, help='Search path')
  parser.add_argument('-o', type=str, help='Output file')
  return parser

args = init_parser().parse_args()
xml_file = args.x
theXML = parse(xml_file)
root = theXML.getroot()
inFile = file(args.i, "r")
out = file(args.o,"w")
folder = args.f

#build olds
olds = []
#for sub in theXML.findall('Sub'):
#  for job in sub.findall('Job'):
#    print job.get("ID")
#    olds.append(job.get("ID"))

print olds

for line in inFile:
  if len(line.split(" ")) > 2:
#    print line.split(" ")[0] 
    
    bads = line.split(" ")[1:-1]
    print bads
#    for sub in theXML.findall('Sub'):
    for sub in root.findall('Sub'):
      if line.split(" ")[0] in sub.get('Name'):
#        print "found"
        Name = sub.get('Name')
        sample = [ "_" + Name.split("_")[2], Name.split("_")[3].replace(".sh","") ]
        for bad in bads:
#          print bad
          for job in sub.findall('Job'):
            if bad in job.get('N'):
#              print "found"
              
              #now search through the dirs 
              for DIR in ls(folder+"/*"):
                if DIR not in olds:
#                  print DIR

                  newFiles = ls(DIR + "/*")
#                  print newFiles
                  if len(newFiles) == 0: continue
                  if sample[0] in newFiles[0] and sample[1] in newFiles[0]:
                    if job.get('N') in newFiles[0].replace(".root","").split("_")[-1]:
                      print bad, "found in", DIR
                      job.set('ID',DIR.split("/")[-1])
#                      print newFiles

theXML.write(args.o)
