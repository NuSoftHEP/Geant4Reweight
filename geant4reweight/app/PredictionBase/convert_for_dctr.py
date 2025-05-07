import numpy as np, ROOT as RT, h5py as h5
from argparse import ArgumentParser as ap

def merge(args):
  with h5.File(args.o, 'w') as fout:

    for i, n in enumerate(args.i):
      with h5.File(n, 'r') as fin:
        xin = fin['X']
        yin = fin['Y']
        if i == 0:
          fout.create_dataset('X', (len(xin), 20, 5), 
                              dtype=np.dtype('float32'),
                              maxshape=(None, 20, 5),
                              chunks=True,
                              data=xin
                             )
          fout.create_dataset('Y', (len(yin)), 
                              dtype=np.dtype('float32'),
                              maxshape=(None,),
                              chunks=True,
                              data=yin
                             )
        else:
          fout['X'].resize(fout['X'].shape[0]+len(xin), axis=0)
          fout['X'][-len(xin):] = xin
          print(fout['X'][-1])

          fout['Y'].resize(fout['Y'].shape[0]+len(yin), axis=0)
          fout['Y'][-len(yin):] = yin

def convert(args):
  f = RT.TFile.Open(args.i[0])
  t = f.Get('tree')

  max_events = args.max_events if args.max_events > 0 else t.GetEntries()
  with h5.File(args.o, 'w') as fout:
    dt = np.dtype('float32')
    X = fout.create_dataset(
      'X',
      (max_events, 20, 5),
      dtype=dt,
      maxshape=(None, 20, 5)
    )
    Y = fout.create_dataset(
      'Y',
      (max_events),
      dtype=dt,
      maxshape=(None)
    )

    for a, e in enumerate(t):
      if args.max_events > 0 and a >= args.max_events: break
      if not (a % 1000): print(f'{a}/{t.GetEntries()}', end='\r')
      for i in range(len(e.c_pdg)):
        if i >= args.max_parts: break
        #print(e.c_pdg[i])
        X[a,i] = np.array(
          [e.c_momentum_x[i], e.c_momentum_y[i], e.c_momentum_z[i],
           e.c_pdg[i], 0.] #e.radius_trailing
        )
        #Y[a] = (1 if args.force_varied else e.is_varied)
        Y[a] = (1 if args.force_varied else 0)


  f.Close()
if __name__ == '__main__':
  parser = ap()
  parser.add_argument('routine', choices=['convert', 'merge'], default='convert', type=str)
  parser.add_argument('-i', type=str, required=True, nargs='+')
  parser.add_argument('-o', type=str, default='converted_cascade.h5')
  parser.add_argument('--max_parts', type=int, default=20)
  parser.add_argument('--max_events', type=int, default=-1)
  parser.add_argument('--force_varied', action='store_true')
  args = parser.parse_args()
  
  routines = {
    'convert':convert,
    'merge':merge,
  }

  routines[args.routine](args)
