import xml.etree.ElementTree
from argparse import ArgumentParser
def init_parser():
  parser = ArgumentParser()
  parser.add_argument('--config', type=str, help='Detector config XML file (full path)', default='/mnt/research/NuInt/DUNEPrismTools/jake-contrib/try.xml')
  parser.add_argument('--DIR', type=str, help='Directory for plots', default='')
  parser.add_argument('--setting', type=str, help='Horn Current (FD?)', default='FHC')
  parser.add_argument('--veto', type=float, help='Amount of Had energy in veto to cut on (GeV)',default=.01)
  parser.add_argument('--extra', type=str, help="extra dir", default="")
  parser.add_argument('--o', type=str, help='outputfile', default='')

  return parser

#print a.config

