#ifndef DUETFitter_h
#define DUETFitter_h

#include "G4ReweightFitter.hh"

#include <iostream>
#include <string>

#include "TH1D.h"
#include "TH2D.h"
#include "TMatrixD.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TFile.h"
#include "TTree.h"

#include "G4ReweightTreeParser.hh"

class DUETFitter : public G4ReweightFitter { 
  public:
    DUETFitter(){fOutFile = new TFile("DUET_fit.root", "RECREATE"); };
    DUETFitter( std::string );
    DUETFitter( std::string, std::string );
   ~DUETFitter(){};
    void   LoadData();
    double DoFit();
    void   LoadMC();
    void   LoadRawMC();
    void   DoReweight();
    void   SaveInfo(double);
  private:
    
    TFile * fDUETFile;
    TFile * fOutFile;

    std::string fMCFileName;
    std::string fRawMCFileName;
    TFile * fMCFile;
    TFile * fRawMCFile;
    TTree * fMCTree;

    TGraphErrors * DUET_xsec_abs;
    TGraphErrors * DUET_xsec_cex;
    //TH2D * DUET_cov;
    TMatrixD * DUET_cov_matrix;
    TMatrixD * DUET_cov_inv;

    TGraph * MC_xsec_abs;
    TGraph * MC_xsec_cex;


    //Reweight stuff
    G4ReweightTreeParser * Reweighter;
};

#endif
