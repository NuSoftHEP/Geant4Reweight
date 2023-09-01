import ROOT as RT
from argparse import ArgumentParser as ap
#from array import array
import numpy as np

def add_graphs(grs):
  if len(grs) == 0: return None
  xs = np.array([x for x in grs[0].GetX()])
  ys = np.array([[y for y in gr.GetY()] for gr in grs])
  gr = RT.TGraph(len(xs), xs, ys.sum(axis=0))
  return gr

if __name__ == '__main__':
  parser = ap()
  parser.add_argument('-i', required=True)
  parser.add_argument('-o', required=True)
  args = parser.parse_args()

  fin = RT.TFile.Open(args.i)

  gr_abs = fin.Get('abs')
  gr_cex = fin.Get('cex')
  gr_inel = fin.Get('inel')
  gr_dcex = fin.Get('dcex')
  gr_prod = fin.Get('prod')

  gr_other = add_graphs([gr_inel, gr_dcex, gr_prod])

  fout = RT.TFile(args.o, 'recreate')
  gr_other.Write('other')
  gr_abs.Write('abs')
  gr_cex.Write('cex')
  gr_total_test = add_graphs([gr_abs, gr_cex, gr_other])
  gr_total_test.Write('test')

  fout.Close()
  fin.Close()
