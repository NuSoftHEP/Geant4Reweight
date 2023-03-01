#include "geant4reweight/PropBase/G4ReweightThrowManager.hh"

#include "TFile.h"
#include "TH1D.h"
#include "TMatrixD.h"
#include "TVectorD.h"

#include <iostream>

G4ReweightThrowManager::G4ReweightThrowManager( TFile & FitResults )
: rng(0)
{
  
  TMatrixD * cov = (TMatrixD*)FitResults.Get("FitCovariance");
  Cholesky = TDecompChol( *cov );

  TH1D * parsHist = (TH1D*)FitResults.Get("parsHist");
  std::cout << "Best Fit: ";
  for( int i = 1; i <= parsHist->GetNbinsX(); ++i ){
    BestFitVals.push_back( parsHist->GetBinContent(i) );
    ParNames.push_back( parsHist->GetXaxis()->GetBinLabel(i) );
    std::cout << BestFitVals.back() << " ";
  }
  std::cout << std::endl;
}

bool G4ReweightThrowManager::Decomp(){
  DecompSuccess = Cholesky.Decompose();

  if( !DecompSuccess )
    std::cout << "Could not decompose" << std::endl;
//  else{
//    CholMatrix = (TMatrixD)Cholesky.GetU();
//    for( int i = 0; i < CholMatrix.GetNrows(); ++i ){
//      for( int j = 0; j < CholMatrix.GetNcols(); ++j ){
//        std::cout << CholMatrix(i,j) << " " << std::endl;
//      }
//      std::cout << std::endl;
//    }
//  }
  return DecompSuccess;
}

std::map< std::string, double > G4ReweightThrowManager::DoThrow(){

  std::map< std::string, double > ThrownVals;

  TVectorD rand( BestFitVals.size() );
  for( size_t i = 0; i < BestFitVals.size(); ++i ){
    rand[i] = rng.Gaus();
  }

  TVectorD rand_times_chol = ( Cholesky.GetU() )* rand;

  for( size_t i = 0; i < BestFitVals.size(); ++i ){
    ThrownVals[ ParNames[i] ] = ( BestFitVals[i] + rand_times_chol[i] );
  }

  return ThrownVals;
}
