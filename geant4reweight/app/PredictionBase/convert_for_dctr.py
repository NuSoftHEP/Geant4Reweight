import numpy as np, ROOT as RT, h5py as h5
from argparse import ArgumentParser as ap

if __name__ == '__main__':
  parser = ap()
  parser.add_argument('-i', type=str, required=True)
  parser.add_argument('-o', type=str, default='converted_cascade.h5')
  parser.add_argument('--max_parts', type=int, default=20)
  parser.add_argument('--max_events', type=int, default=-1)
  args = parser.parse_args()
  f = RT.TFile.Open(args.i)
  t = f.Get('tree')

  with h5.File(args.o, 'w') as fout:
    dt = np.dtype('float32')
    X = fout.create_dataset(
      'X',
      (t.GetEntries(), 20, 5),
      dtype=dt,
      maxshape=(None, 20, 5)
    )
    Y = fout.create_dataset(
      'Y',
      (t.GetEntries()),
      dtype=dt,
      maxshape=(None)
    )

    a = 0
    for e in t:
      if args.max_events > 0 and a >= args.max_events: break
      if not (a % 1000): print(f'{a}/{t.GetEntries()}', end='\r')
      for i in range(len(e.c_pdg)):
        if i >= args.max_parts: break
        #print(e.c_pdg[i])
        X[a,i] = np.array(
          [e.c_momentum_x[i], e.c_momentum_y[i], e.c_momentum_z[i],
           e.c_pdg[i], e.radius_trailing]
        )
        Y[a] = e.is_varied
      a += 1

  f.Close()
