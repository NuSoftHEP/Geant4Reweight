#ifndef DUETFitter_h
#define DUETFitter_h

#include "G4ReweightFitter.hh"

#include <iostream>

#include "TH1D.h"
#include "TH2D.h"
#include "TMatrix.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TFile.h"

class DUETFitter : public G4ReweightFitter { 
  public:
    DUETFitter(){};
   ~DUETFitter(){};
    void   LoadData();
    double DoFit();
    void   LoadMC();
  private:
    
    TFile * fDUETFile;
//    TFile * fMCFile;

    TGraphErrors * DUET_xsec_abs;
    TGraphErrors * DUET_xsec_cex;
    TH2D * DUET_cov;
    TMatrix * DUET_cov_matrix;

    TGraph * MC_xsec_abs;
    TGraph * MC_xsec_cex;
};

#endif
