import numpy as np, ROOT as RT, h5py as h5
from argparse import ArgumentParser as ap

if __name__ == '__main__':
  parser = ap()
  parser.add_argument('-i', type=str, required=True)
  parser.add_argument('-o', type=str, default='converted_cascade.h5')
  parser.add_argument('--max', type=int, default=20)
  args = parser.parse_args()
  f = RT.TFile.Open(args.i)
  t = f.Get('tree')

  with h5.File(args.o, 'w') as fout:
    dt = np.dtype('float32')
    dset = fout.create_dataset(
      'X',
      (t.GetEntries(), 20, 5),
      dtype=dt,
      maxshape=(None, 20, 5)
    )

    a = 0
    for e in t:
      for i in range(len(e.c_pdg)):
        if i >= args.max: break
        print(e.c_pdg[i])
        vals = np.array(
          [e.c_momentum_x[i], e.c_momentum_y[i], e.c_momentum_z[i],
           e.c_pdg[i], e.radius_trailing]
        )
        #dset[a][i] = np.array(
        #  [e.c_momentum_x[i], e.c_momentum_y[i], e.c_momentum_z[i],
        #   e.c_pdg[i], e.radius_trailing]
        #)
        print(vals)
        dset[a][i] = vals
        print(dset[a][i])
      break
      a += 1

  f.Close()
