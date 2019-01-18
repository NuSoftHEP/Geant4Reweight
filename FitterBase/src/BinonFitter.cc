#include "BinonFitter.hh"

BinonFitter::BinonFitter(TFile * output_file){ 
  fOutputFile = output_file;

  points = {155.661, 179.112, 218.853, 253.714, 287.481, 309.561, 342.202, 374.401, 395.676};

  binning = "(25,150,400)";

  cuts = {
    {"reac", "(int == \"pi-Inelastic\")"}
  };

  graph_names = {
    {"reac", "C_xsec_reac_piminus"}
  };

  fExperimentName = "Binon";

  scale = 1.E27 / (.5 * 2.266 * 6.022E23 / 12.01 );
  fDataFileName = "/dune/app/users/calcuttj/geant/GeantReweight/data/Binon.root"; 
}
