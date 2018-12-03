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
    //DUETFitter( std::string, std::string );
   ~DUETFitter();
    void   LoadData();
    double DoFit();
    void   LoadMC();
    void   LoadRawMC();
    void   DoReweight(double);
    void   DoReweightFS(double,double);
    void   SaveInfo();
    void   ClearMemory();

  private:
    
    TFile * fDUETFile;
    TFile * fOutFile;
    TTree * fFitTree;

    std::string fMCFileName;
    std::string fRawMCFileName;
    TFile * fMCFile;
    TFile * fRawMCFile;
    TTree * fMCTree;
    TTree * fFSTree;

    TGraphErrors * DUET_xsec_abs;
    TGraphErrors * DUET_xsec_cex;
    //TH2D * DUET_cov;
    TMatrixD * DUET_cov_matrix;
    TMatrixD * DUET_cov_inv;

    TGraph * MC_xsec_abs;
    TGraph * MC_xsec_cex;


    //Reweight stuff
    G4ReweightTreeParser * Reweighter;

    double norm_param;
    double norm_abs_param;
    double norm_cex_param;
    double the_Chi2;

    std::vector< double > norm_vector;
    std::vector< double > norm_abs_vector;
    std::vector< double > norm_cex_vector;
    std::vector< double > Chi2_vector;
};

#endif
