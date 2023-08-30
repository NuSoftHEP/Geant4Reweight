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

  total_ke = fin.Get('total_inel_KE')
  gr_abs_ke = fin.Get('abs_KE')
  gr_cex_ke = fin.Get('cex_KE')
  gr_inel_ke = fin.Get('inel_KE')
  gr_dcex_ke = fin.Get('dcex_KE')
  gr_prod_ke = fin.Get('prod_KE')

  total_momentum = fin.Get('total_inel_momentum')
  gr_abs_momentum = fin.Get('abs_momentum')
  gr_cex_momentum = fin.Get('cex_momentum')
  gr_inel_momentum = fin.Get('inel_momentum')
  gr_dcex_momentum = fin.Get('dcex_momentum')
  gr_prod_momentum = fin.Get('prod_momentum')

  gr_other_ke = add_graphs([gr_inel_ke, gr_dcex_ke, gr_prod_ke])
  gr_other_momentum = add_graphs([gr_inel_momentum, gr_dcex_momentum, gr_prod_momentum])

  fout = RT.TFile(args.o, 'recreate')
  gr_other_ke.Write('other_KE')
  gr_abs_ke.Write('abs_KE')
  gr_cex_ke.Write('cex_KE')
  total_ke.Write('total_inel_KE')

  gr_abs_momentum.Write('abs_momentum')
  gr_cex_momentum.Write('cex_momentum')
  total_momentum.Write('total_inel_momentum')
  gr_other_momentum.Write('other_momentum')

  gr_total_test = add_graphs([gr_abs_ke, gr_cex_ke, gr_other_ke])
  gr_total_test.Write('test_KE')

  gr_total_test = add_graphs([gr_abs_momentum, gr_cex_momentum, gr_other_momentum])
  gr_total_test.Write('test_momentum')
  
  fout.Close()
  fin.Close()
