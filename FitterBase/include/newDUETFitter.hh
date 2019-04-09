#ifndef newDUETFitter_h
#define newDUETFitter_h

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

#include "G4ReweightTreeParser.hh"

class newDUETFitter : public G4ReweightFitter { 
  public:
  
    newDUETFitter( TFile * output, std::string data_file );
    virtual void   LoadData();
    void SaveData( TDirectory * ) override;
    double DoFit(bool fSave=true) override;


  private:
    TMatrixD * DUET_cov_matrix;
    TMatrixD * DUET_cov_inv;
    
};

#endif
