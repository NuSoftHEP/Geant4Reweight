#include "DUETFitter.hh"

#include "TDirectory.h"
#include "TGraphErrors.h"
#include "TMatrixD.h"

DUETFitter::DUETFitter(TFile * output_file,
                       fhicl::ParameterSet exp,
                       std::string frac_file_name,
                       G4ReweightParameterMaker & parMaker,
                       const fhicl::ParameterSet & material,
                       G4ReweightManager * rw_manager) 
  : G4ReweightFitter(output_file, exp, frac_file_name, parMaker, material, 
                     rw_manager) {
  nDOF = 10; // override the nDOF
} 

void DUETFitter::SaveData(TDirectory * data_dir){
  data_dir->cd();
  
  TDirectory * experiment_dir;
  //Check if the directory already exists. If so, delete it and remake
  if( data_dir->Get( fExperimentName.c_str() ) ){
     data_dir->cd();
     data_dir->rmdir( fExperimentName.c_str() );
  }
  experiment_dir = data_dir->mkdir( fExperimentName.c_str() );
  experiment_dir->cd();

  Data_xsec_graphs["abs"]->Write("abs");
  Data_xsec_graphs["cex"]->Write("cex");
  DUET_cov_matrix->Write("cov");
  DUET_cov_inv->Write("cov_inv");

}

void DUETFitter::LoadData(){
  fDataFile = new TFile( fDataFileName.c_str(), "READ"); 

  Data_xsec_graphs["abs"]   = (TGraphErrors*)fDataFile->Get("xsec_abs");    
  Data_xsec_graphs["cex"]   = (TGraphErrors*)fDataFile->Get("xsec_cex");    

  
  DUET_cov_matrix = (TMatrixD*)fDataFile->Get("cov");
  DUET_cov_inv    = &DUET_cov_matrix->Invert();

  fOutputFile->cd();

}

double DUETFitter::DoFit(bool fSave){
  double Chi2 = 0.;

  double Data_val_i, Data_val_j, MC_val_i, MC_val_j, cov_val;
  double x;
  
  int NPoints = Data_xsec_graphs["cex"]->GetN() + Data_xsec_graphs["abs"]->GetN();
  for( int i = 0; i < NPoints; ++i ){
    if( i < 5 ){
      Data_xsec_graphs["cex"]->GetPoint(i, x, Data_val_i);
      MC_val_i = MC_xsec_graphs["cex"]->Eval( x );
    }
    else{
      Data_xsec_graphs["abs"]->GetPoint(i - 5, x, Data_val_i);
      MC_val_i = MC_xsec_graphs["abs"]->Eval(x);
    }

    for( int j = 0; j < NPoints; ++j ){
   
      if( j < 5 ){
        Data_xsec_graphs["cex"]->GetPoint(j, x, Data_val_j);
        MC_val_j = MC_xsec_graphs["cex"]->Eval(x);
      }
      else{
        Data_xsec_graphs["abs"]->GetPoint(j - 5, x, Data_val_j);
        MC_val_j = MC_xsec_graphs["abs"]->Eval(x);
      }

      cov_val = DUET_cov_inv[0][i][j];

      double partial_chi2 = ( MC_val_i - Data_val_i ) * cov_val * ( MC_val_j - Data_val_j );
      Chi2 += partial_chi2; 
    }
  }

  if( fSave ){
    std::string name = "abs";
    SaveExpChi2( Chi2, name );
    name = "cex";
    SaveExpChi2( Chi2, name );
  }
  return Chi2;
}





//C Thorpe: New method compatible with new chi2 calculation
void DUETFitter::DoFitModified(bool fSave){

  fitDataStore.clear();
  double Chi2 = 0.;
  double Data_val_i, Data_val_j, MC_val_i, MC_val_j, cov_val;
  double x;
  
  int NPoints = Data_xsec_graphs["cex"]->GetN() + Data_xsec_graphs["abs"]->GetN();


  for( int i = 0; i < NPoints; ++i ){
     
    
    if( i < 5 ){
      Data_xsec_graphs["cex"]->GetPoint(i, x, Data_val_i);
      MC_val_i = MC_xsec_graphs["cex"]->Eval( x );
    }
    else{
      Data_xsec_graphs["abs"]->GetPoint(i - 5, x, Data_val_i);
      MC_val_i = MC_xsec_graphs["abs"]->Eval(x);
    }


    for( int j = 0; j < NPoints; ++j ){
   
      if( j < 5 ){
        Data_xsec_graphs["cex"]->GetPoint(j, x, Data_val_j);
        MC_val_j = MC_xsec_graphs["cex"]->Eval(x);
      }
      else{
        Data_xsec_graphs["abs"]->GetPoint(j - 5, x, Data_val_j);
        MC_val_j = MC_xsec_graphs["abs"]->Eval(x);
      }

      cov_val = DUET_cov_inv[0][i][j];

      double partial_chi2 = ( MC_val_i - Data_val_i ) * cov_val * ( MC_val_j - Data_val_j );
      Chi2 += partial_chi2; 
    }
  }

  if( fSave ){
    std::string name = "abs";
    SaveExpChi2( Chi2, name );
    name = "cex";
    SaveExpChi2( Chi2, name );
  }


  //slightly questionable but probably ok!
  Chi2Store thisStoreAbs("abs",5,Chi2/2);
  Chi2Store thisStoreCex("cex",5,Chi2/2);
  
  fitDataStore.push_back(thisStoreAbs);
  fitDataStore.push_back(thisStoreCex);
  //return Chi2;
}
