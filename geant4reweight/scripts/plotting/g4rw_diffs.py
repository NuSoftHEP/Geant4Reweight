import ROOT as RT
from argparse import ArgumentParser as ap
from array import array

parser = ap()

parser.add_argument("-m", type=str, help='MC File', default = "")
parser.add_argument("-o", type=str, help='Output', default = "")
parser.add_argument("-n", type=int, help='Number of vars', default = 1)
parser.add_argument("-b", type=str, help='Binning', default = "(100, 0, 100)")
parser.add_argument("-w", type=float, help='Width', default = 10.)
parser.add_argument("-a", type=int, help='Use alt vars', default = 1)
parser.add_argument("-t", type=str, help='Which PDG', default = '211')
parser.add_argument("-x", type=int, help='Extra check', default=0)
args = parser.parse_args()


RT.gStyle.SetOptStat(0)
RT.gROOT.SetBatch(1)

fMC = RT.TFile(args.m, "OPEN")
tMC = fMC.Get("pionana/beamana")

if args.a:
  check = "@g4rw_alt_primary_plus_sigma_weight.size() > 0 && true_beam_PDG == " + args.t
else: 
  check = "@g4rw_primary_plus_sigma_weight.size() > 0 && true_beam_PDG == " + args.t
tMC.Draw("true_beam_endZ>>hNom" + args.b, check)
hNom = RT.gDirectory.Get("hNom")


plus_hists = []
minus_hists = []
plus_ints = []
minus_ints = []
for i in range(0, args.n):
  #if i == 2: continue
  if args.a:
    plus_weight = "g4rw_alt_primary_plus_sigma_weight[" + str(i) + "]"
  else:
    plus_weight = "g4rw_primary_plus_sigma_weight[" + str(i) + "]"
  tMC.Draw("true_beam_endZ>>hPlus" + str(i+1) + args.b, plus_weight + "*(" +check +")")

  if args.a:
    minus_weight = "g4rw_alt_primary_minus_sigma_weight[" + str(i) + "]"
  else:
    minus_weight = "g4rw_primary_minus_sigma_weight[" + str(i) + "]"
  tMC.Draw("true_beam_endZ>>hMinus" + str(i+1) + args.b, minus_weight + "*(" +check +")")

  plus_hist = RT.gDirectory.Get("hPlus" + str(i+1))
  minus_hist = RT.gDirectory.Get("hMinus" + str(i+1))

  plus_hists.append(plus_hist)
  minus_hists.append(minus_hist)

  plus_ints.append(plus_hist.Integral()) 
  minus_ints.append(minus_hist.Integral()) 
  print(plus_hist.Integral(), minus_hist.Integral())

nom_int = hNom.Integral()
plus_ints = [i/nom_int for i in plus_ints]
minus_ints = [i/nom_int for i in minus_ints]
minus_ints.reverse()

all_ints = minus_ints + [1.] + plus_ints

xs = [1. + i*args.w for i in range(-1*len(plus_ints), len(plus_ints)+1)]
print(len(xs), len(all_ints))
gr = RT.TGraph(len(all_ints), array("d", xs), array("d", all_ints) )
 

fout = RT.TFile(args.o, "RECREATE")
fout.cd()
gr.Write("vars")
for h1, h2 in zip(plus_hists, minus_hists):
  h1.SetLineColor(RT.kBlack)
  h2.SetLineColor(RT.kRed)
  h1.Write()
  h2.Write()
hNom.Write()
fout.Close()
#c1 = RT.TCanvas("c1", "c1", 500, 400)
#gr.Draw("AP")
#c1.SaveAs(args.o)
