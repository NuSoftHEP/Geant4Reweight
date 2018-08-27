from xml.etree.ElementTree import parse
from argparse import ArgumentParser 
from glob import glob as ls
from subprocess import Popen
def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-x', type=str, help='XML file')
  parser.add_argument('-f', type=str, help='Search path')
  parser.add_argument('-o', type=str, help='Output file path')
  return parser

args = init_parser().parse_args()
xml_file = args.x
out_file = args.o
folder = args.f

print "Config file: ", xml_file 
theXML = parse(xml_file)

#out = file(out_file,"w")

variations = [["1","1"]]

for sub in theXML.findall('Sub'):
  print sub.get('Name')
  #out.write(sub.get('Name') + " ")
  theSub = sub.get('Name')
  theType = sub.get('Type')
  theSample = sub.get('Sample')
  fileName = theSub.replace("T","t") + "_inel" + variations[0][0] + "_elast" + variations[0][1] +"_"
#  out.write(fileName + " ")

  command = ["hadd", "-f", out_file + fileName + "full.root"] 

  for job in sub.findall('Job'):
    print job.get('ID'), job.get('N')
    theID = job.get('ID')
    theN = job.get('N')   

#    files = [f.split("/")[-1] for f in ls(folder + "/" + theID + "/" + fileName + "*")]
    files = ls(folder + "/" + theID + "/" + fileName + theN + ".root")
    print files    
    if len(files) > 0:
      #out.write(files[0] + " ")
      command.append(files[0])
    else:
      print "ERROR"
  print command
  Popen(command)
      
#  out.write("\n")
 
