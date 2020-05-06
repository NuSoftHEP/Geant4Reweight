import ROOT as RT
from argparse import ArgumentParser as ap

RT.gROOT.SetBatch(1)

parser = ap()

parser.add_argument('-x', type = str, help = 'Which xsec file', default = '')
parser.add_argument('-r', type = str, help = 'Which rate file', default = '')
parser.add_argument('-o', type = str, help = 'Output file', default = '')
parser.add_argument('--lowX', type = float, help = 'X Binning low end', default = 0.)
parser.add_argument('--highX', type = float, help = 'X Binning high end', default = 100.)
parser.add_argument('--nX', type = int, help = 'Number of X bins', default = 100)
parser.add_argument('--lowY', type = float, help = 'Y Binning low end', default = 0.)
parser.add_argument('--highY', type = float, help = 'Y Binning high end', default = 100.)
parser.add_argument('--nY', type = int, help = 'Number of Y bins', default = 100)
parser.add_argument('-t', type = str, help = 'KE vs p', default = 'KE')
args = parser.parse_args()

if not (args.x and args.r and args.o):
  print("Error: need to provide xsec, rate, and output files")
  print("Using '-x', '-r', and '-o'")
  exit()

if not (args.t == 'KE' or args.t == 'p'):
  print("Need to provide -t with 'KE' or 'p'")
  exit()

fXSec = RT.TFile(args.x)
fRates = RT.TFile(args.r)

if args.t == 'p':
  xsec_graph = fXSec.Get("el_momentum")
  
  x_draw_cmd = "sqrt(initialMomentumX*initialMomentumX"
  x_draw_cmd += " + initialMomentumY*initialMomentumY"
  x_draw_cmd += " + initialMomentumZ*initialMomentumZ)"
else:
  xsec_graph = fXSec.Get("el_KE")
  x_draw_cmd = + "initialKE"

draw_cmd = "(initialKE - finalKE):" + x_draw_cmd

binning = "(" + str(args.nX) + "," + str(args.lowX) + "," + str(args.highX)
binning += "," + str(args.nY) + "," + str(args.lowY) + "," + str(args.highY) + ")"

x_binning = "(" + str(args.nX) + "," + str(args.lowX) + "," + str(args.highX) + ")"

rate_tree = fRates.Get("tree")
rate_tree.Draw(draw_cmd + ">>h2D" + binning)
rate_tree.Draw(x_draw_cmd + ">>h1D" + x_binning)

h2D = RT.gDirectory.Get("h2D")
h1D = RT.gDirectory.Get("h1D")

for i in range(1, h2D.GetNbinsX() + 1):
  n1D = h1D.GetBinContent(i)
  if not n1D:
    print("Found bin with 0 events:", i)
    continue
  xsec = xsec_graph.GetY()[i - 1]
  for j in range(1, h2D.GetNbinsY() + 1):
    n2D = h2D.GetBinContent(i, j)
    h2D.SetBinContent(i, j, xsec * n2D / n1D);

fOut = RT.TFile(args.o, "RECREATE")
fOut.cd()
h2D.Write("diff_xsec_" + args.t)

dir_1D = fOut.mkdir("1D_Plots")
dir_1D.cd()
for i in range(h2D.GetNbinsX() + 1):
  proj = h2D.ProjectionY("proj" + str(i), i, i)
  proj.Write()

fOut.Close()
