#include "newDUETFitter.hh"

newDUETFitter::newDUETFitter(TFile * output_file, std::string data_file) /*: 
  fOutputFile( output_file ),
  fDataFileName( data_file )*/ { 

  fOutputFile = output_file;
/*  output_dir->cd(); */ 

  points["abs"] = {201.6, 216.6, 237.2, 265.6, 295.1}; 
  points["cex"] = {201.6, 216.6, 237.2, 265.6, 295.1}; 

  binning = "(10,200,300)";

  cuts = {
    {"abs", "(int == \"pi+Inelastic\" && ( (nPi0 + nPiPlus + nPiMinus)  == 0) )"      },
    {"cex", "(int == \"pi+Inelastic\" && ( (nPiPlus + nPiMinus) == 0 ) && (nPi0 == 1))"}
  };

  scale = 1.E27 / (.5 * 2.266 * 6.022E23 / 12.01 );

  fExperimentName = "DUET";
  //fDataFileName = "/dune/app/users/calcuttj/geant/GeantReweight/data/DUET.root"; 
  fDataFileName = data_file; 

}; 

void newDUETFitter::SaveData(TDirectory * data_dir){
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


  double dummyX = 0.;
  double dummyY = 1.;

  dummyGraph = new TGraph(1, &dummyX, &dummyY );

}

void newDUETFitter::LoadData(){
  fDataFile = new TFile( fDataFileName.c_str(), "READ"); 

  Data_xsec_graphs["abs"]   = (TGraphErrors*)fDataFile->Get("xsec_abs");    
  Data_xsec_graphs["cex"]   = (TGraphErrors*)fDataFile->Get("xsec_cex");    

  
  DUET_cov_matrix = (TMatrixD*)fDataFile->Get("cov");
  DUET_cov_inv    = &DUET_cov_matrix->Invert();

  std::cout << "Loaded data" << std::endl;

  fOutputFile->cd();

}

double newDUETFitter::DoFit(){
  //BinnedChi2 = new TH2D( ("BinnedChi2_abs_" + set_prec(norm_abs_param) + "_cex_" + set_prec(norm_cex_param)).c_str(), "", 10, 0, 10, 10, 0, 10 );

  double Chi2 = 0.;

  double Data_val_i, Data_val_j, MC_val_i, MC_val_j, cov_val;
  double x;
  
  int NPoints = Data_xsec_graphs["cex"]->GetN() + Data_xsec_graphs["abs"]->GetN();


  for( int i = 0; i < NPoints; ++i ){
     
    
    if( i < 5 ){
      Data_xsec_graphs["cex"]->GetPoint(i, x, Data_val_i);
      MC_xsec_graphs["cex"]->GetPoint(i, x, MC_val_i);
    }
    else{
      MC_xsec_graphs["abs"]->GetPoint(i - 5, x, MC_val_i);
      Data_xsec_graphs["abs"]->GetPoint(i - 5, x, Data_val_i);
    }


    for( int j = 0; j < NPoints; ++j ){
   
      if( j < 5 ){
        MC_xsec_graphs["cex"]->GetPoint(j, x, MC_val_j);
        Data_xsec_graphs["cex"]->GetPoint(j, x, Data_val_j);
      }
      else{
        MC_xsec_graphs["abs"]->GetPoint(j - 5, x, MC_val_j);
        Data_xsec_graphs["abs"]->GetPoint(j - 5, x, Data_val_j);
      }

      cov_val = DUET_cov_inv[0][i][j];

      //BinnedChi2->SetBinContent(i+1, j+1, ( MC_val_i - Data_val_i ) * cov_val * ( MC_val_j - Data_val_j ) );
      double partial_chi2 = ( MC_val_i - Data_val_i ) * cov_val * ( MC_val_j - Data_val_j );
      Chi2 += partial_chi2; 
    }
  }

  //the_Chi2 = Chi2;
  //SaveInfo();
  //BinnedChi2->Write();
  std::string name = "abs";
  SaveExpChi2( Chi2, name );
  name = "cex";
  SaveExpChi2( Chi2, name );
  return Chi2;
}
