from xml.etree.ElementTree import parse
from argparse import ArgumentParser 
from glob import glob as ls
from ROOT import * 
def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-x', type=str, help='Original XML file')
  parser.add_argument('-n', type=str, help='New XML file')
  return parser

args = init_parser().parse_args()
xml_file = args.x
new_file = args.n

theXML = parse(xml_file)
oldRoot = theXML.getroot()

theNew = parse(new_file)




for sub in theNew.findall('Sub'):
  theName = sub.get('Name')
  theSample = sub.get('Sample')
  print theName, theSample 

  for job in sub.findall('Job'):
    oldID = job.get('ID')
    newID = job.get('New')
    print oldID, "------->", newID 

    for oldSub in oldRoot:
      if oldSub.get('Name') in theName and oldSub.get('Sample') in theSample:
        print "Found Match"
        
        for oldJob in oldSub:
          if oldJob.get('ID') in oldID:
            oldJob.set('ID', newID)
theXML.write(xml_file)
