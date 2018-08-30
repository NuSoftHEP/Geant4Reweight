from xml.etree.ElementTree import parse
from argparse import ArgumentParser 
from glob import glob as ls
from subprocess import Popen
def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-x', type=str, help='XML file')
  parser.add_argument('-f', type=str, help='Search path')
  parser.add_argument('-o', type=str, help='Output file path')
  parser.add_argument('--limit', type=int, help='Max num of samples to include (1->5)', default=5)
  parser.add_argument('--name', type=str, help='What to put at end of name', default='full')
  parser.add_argument('-i', type=str, help='Inel', default='1')
  parser.add_argument('-e', type=str, help='Elast', default='1')
  return parser

args = init_parser().parse_args()
xml_file = args.x
out_file = args.o
folder = args.f

print "Config file: ", xml_file 
theXML = parse(xml_file)

#out = file(out_file,"w")

variations = [args.i,args.e]

for sub in theXML.findall('Sub'):
  print sub.get('Name')
  #out.write(sub.get('Name') + " ")
  theSub = sub.get('Name')
  theType = sub.get('Type')
  theSample = sub.get('Sample')
  if "var" in theType:
    theInel = sub.get('Inel')
    theElast = sub.get('Elast')

    if not (theInel == variations[0] and theElast == variations[1]): 
      continue

  fileName = theSub + "_" + theType  + "_inel" + variations[0] + "_elast" + variations[1] +"_"
  print fileName
#  out.write(fileName + " ")

  command = ["hadd", "-f", out_file + fileName + args.name +".root"] 
  
  for job in sub.findall('Job'):
    print job.get('ID'), job.get('N')
    theID = job.get('ID')
    theN = job.get('N')   
    print folder + "/" + theID + "/" + fileName + theN + ".root"
#    if(theN > args.limit - 1): continue
    files = ls(folder + "/" + theID + "/rw_"  + fileName + theN + ".root")
    print files    
    if len(files) > 0:
      #out.write(files[0] + " ")
      command.append(files[0])
    else:
      print "ERROR"
  print command
  if len(command) > 3:
    print "Good"
    Popen(command)
      
#  out.write("\n")
 
