#include "DUETFitter.hh"

#include "TMatrixDUtilsfwd.h"

void DUETFitter::LoadData(){

  fDUETFile = new TFile("../data/DUET.root", "READ");

  DUET_xsec_abs = (TGraphErrors*)fDUETFile->Get("xsec_abs");    
  DUET_xsec_cex = (TGraphErrors*)fDUETFile->Get("xsec_cex");    
  DUET_cov      = (TH2D*)fDUETFile->Get("cov");

  DUET_cov_matrix = new TMatrix(10,10);
  for(int i = 0; i < 10; ++i){
    for(int j = 0; j < 10; ++j){
      //Convention is reversed
      TMatrixTRow<double> row = DUET_cov_matrix[i];
      row[j] = DUET_cov->GetBinContent(j+1, i+1);
    }
  }
  //Just save the inverted covariance
  DUET_cov_matrix = (TMatrix*)DUET_cov_matrix->Invert();

}

void DUETFitter::LoadMC(){
//  fMCFile = new TFile();

  //For now, putting in dummy predictions
  double * x = {201.6, 216.6, 237.2, 265.6, 295.1};
  double * abs_y = {140., 175., 169., 170., 165.1};
  double * cex_y = {40., 25., 48., 55., 47.};

  MC_xsec_abs = new TGraph(5, x, abs_y);
  MC_xsec_cex = new TGraph(5, x, cex_y);

}


double DUETFitter::DoFit(){

  double Chi2 = 0.;

  double DUET_val[2], MC_val[2], cov_val;
  double x;
  
  int NPoints = DUET_xsec_cex->GetN() + DUET_xsec_abs->GetN();


  for( int i = 0; i < NPoints; ++i ){
    
    std::cout << i << std::endl;
    
    if( i < 5 ){
      std::cout << "Getting cex value" << std::endl;
      MC_xsec_cex->GetPoint(i, x, MC_val[0]);
      DUET_xsec_cex->GetPoint(i, x, DUET_val[0]);
    }
    else{
      std::cout << "Getting abs value" << std::endl;
      MC_xsec_abs->GetPoint(i, x, MC_val[0]);
      DUET_xsec_abs->GetPoint(i, x, DUET_val[0]);
    }

    std::cout << "\tMC: " << x << " " << MC_val[0] << std::endl;
    std::cout << "\tDUET: " << x << " " << DUET_val[0] << std::endl;


    for( int j = 0; j < NPoints; ++j ){
   
      std::cout << j << std::endl;

      if( j < 5 ){
        std::cout << "Getting cex value" << std::endl;
        MC_xsec_cex->GetPoint(j, x, MC_val[1]);
        DUET_xsec_cex->GetPoint(j, x, DUET_val[1]);
      }
      else{
        std::cout << "Getting abs value" << std::endl;
        MC_xsec_abs->GetPoint(j, x, MC_val[1]);
        DUET_xsec_abs->GetPoint(j, x, DUET_val[1]);
      }

      cov_val = DUET_cov_matrix[i][j];
      std::cout << "cov: " << i << " " << j << " " << cov_val << std::endl;

      Chi2 += (MC_val[0] - DUET_val[0]) 
    }
  }

  return Chi2;

}


