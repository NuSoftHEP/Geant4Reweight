#ifndef DUETFitter_h
#define DUETFitter_h

#include "geant4reweight/FitterBase/G4ReweightFitter.hh"

#include "TFile.h"
#include "TMatrixDfwd.h"

class DUETFitter : public G4ReweightFitter { 
  public:
  
    DUETFitter(TFile * output_file, fhicl::ParameterSet exp,
               std::string frac_file_name,
               G4ReweightParameterMaker & parMaker,
               const fhicl::ParameterSet & material,
               G4ReweightManager * rw_manager);
    virtual void   LoadData() override;
    void SaveData( TDirectory * ) override;
    double DoFit(bool fSave=true) override;
    virtual void DoFitModified(bool fSave=true) override;

  private:
    TMatrixD * DUET_cov_matrix;
    TMatrixD * DUET_cov_inv;
    
};

#endif
