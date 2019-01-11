#ifndef BinonFitter_h
#define BinonFitter_h

#include "G4ReweightFitter.hh"
#include "FitSample.hh"

#include <iostream>
#include <string>
#include <vector>

#include "TH1D.h"
#include "TH2D.h"
#include "TMatrixD.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TFile.h"
#include "TTree.h"
#include "tinyxml2.h"

#include "G4ReweightTreeParser.hh"

class BinonFitter : public G4ReweightFitter { 
  public:
  
    BinonFitter( TFile * output );
};

#endif
