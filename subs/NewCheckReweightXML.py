from xml.etree.ElementTree import parse, TreeBuilder, tostring, fromstring, ElementTree
import xml.dom.minidom as xdm
from argparse import ArgumentParser 
from glob import glob as ls

def make_it_pretty(builder):
  root = builder.close()
  rough = tostring(root, 'utf-8')
  reparsed = xdm.parseString(rough) 
  tree = ElementTree(fromstring(reparsed.toprettyxml(indent=" ")))
  return tree

def init_parser():
  parser = ArgumentParser()
  parser.add_argument('-x', type=str, help='XML file')
  parser.add_argument('-f', type=str, help='Search path')
  parser.add_argument('-m', type=str, help='Makeup file name')
  parser.add_argument('-r', type=str, help='Reweight file name')
  return parser

args = init_parser().parse_args()
xml_file = args.x

folder = args.f

print "Config file: ", xml_file 
theXML = parse(xml_file)


variations = [["1","1"]] #[["1.5","1"], ["1","1.5"], ["1.5","1.5"]]

all_dirs = []

builder = TreeBuilder()
builder.start("Makeups",{})

rw_builder = TreeBuilder()
rw_builder.start("Reweights",{})

for sub in theXML.findall('Sub'):
  print sub.get('Name'), sub.get('Sample'), sub.get('Type')
  if('nom' in sub.get('Type')):
    builder.start('Sub', {'Name':sub.get('Name'),'Sample':sub.get('Sample'),'Type':sub.get('Type')})
    rw_builder.start('Sub', {'Name':sub.get('Name'),'Sample':sub.get('Sample'),'Type':sub.get('Type')})
  elif('var' in sub.get('Type')):
    builder.start('Sub', {'Name':sub.get('Name'),'Sample':sub.get('Sample'),'Type':sub.get('Type'),'Inel':sub.get('Inel'),'Elast':sub.get('Elast')})
    rw_builder.start('Sub', {'Name':sub.get('Name'),'Sample':sub.get('Sample'),'Type':sub.get('Type'),'Inel':sub.get('Inel'),'Elast':sub.get('Elast')})
  for job in sub.findall('Job'):
    print job.get('ID'), job.get('N')
    theType = sub.get('Type')
    theID = job.get('ID')
    theN = job.get('N')
    files = ls(folder + "/" + theID + "/*")
    trimmed_files = [f.split("/")[-1] for f in files]  
    if "nom" in theType:
      if len(trimmed_files) > 0:
        template = [] 
        for f in trimmed_files:
          if(theType in f.split("_")[1]):
            template = f.split("_")
             
        for var in variations:
          check = False

          for f in trimmed_files:
            if(theType in f.split("_")[1]): continue 
            
            formed_var = "inel"+var[0]+"_elast"+var[1] +"_"           
            if formed_var in f:                
              check = True

          if not check: 
            #form v
            inel = var[0] 
            elast = var[1] 

            filename = "_".join([template[0],template[2],template[1],formed_var[0:-1],template[3]])
            rw_builder.start("Reweight", {"ID":theID, "Base":("_").join(template), "Inel":inel,"Elast":elast, "Out":filename})
            rw_builder.end("Reweight")
            print "Missing variation", var 


      else:
        builder.start("Makeup",{"ID":theID,"N":theN})
        builder.end("Makeup")
        print "Missing Sim file" 
  builder.end("Sub")
  rw_builder.end("Sub")
builder.end("Makeups")
rw_builder.end("Reweights")

tree = make_it_pretty(builder)
tree.write(args.m)

rw_tree = make_it_pretty(rw_builder)
rw_tree.write(args.r)
