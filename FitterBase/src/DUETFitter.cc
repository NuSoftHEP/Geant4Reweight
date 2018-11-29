#include "DUETFitter.hh"

#include "TMatrixDUtilsfwd.h"
#include "TArrayD.h"

#include "G4ReweightInter.hh"
#include <vector>

DUETFitter::DUETFitter(std::string mc_name) : fMCFileName(mc_name) { 
  fOutFile = new TFile("DUET_fit.root", "RECREATE"); 
}

DUETFitter::DUETFitter(std::string mc_name, std::string raw_mc_name) : fMCFileName(mc_name), fRawMCFileName(raw_mc_name) { 
  fOutFile = new TFile("DUET_fit.root", "RECREATE"); 
}

void DUETFitter::LoadData(){

  fDUETFile = new TFile("../data/DUET.root", "READ");

  DUET_xsec_abs   = (TGraphErrors*)fDUETFile->Get("xsec_abs");    
  DUET_xsec_cex   = (TGraphErrors*)fDUETFile->Get("xsec_cex");    
  DUET_cov_matrix = (TMatrixD*)fDUETFile->Get("cov");
  DUET_cov_inv    = &DUET_cov_matrix->Invert();

}

void DUETFitter::LoadRawMC(){
    
}

void DUETFitter::DoReweight( ){
  
  Reweighter = new G4ReweightTreeParser( fRawMCFileName, "DUET_fit_reweight.root" );

  G4ReweightInter * dummy = new G4ReweightInter(std::vector< std::pair<double, double> >() ); 

  Reweighter->SetBranches();
  Reweighter->FillAndAnalyzeFunc(dummy, dummy);
 // Reweighter->CloseAndSaveOutput();
 // Reweighter->CloseInput();

}

void DUETFitter::LoadMC(){
  //fMCFile = new TFile(fMCFileName.c_str(), "READ");
  //fMCTree = (TTree*)fMCFile->Get("tree"); 
  fOutFile->cd();
  fMCTree = Reweighter->GetTree();
  std::cout << "Got Tree " << fMCTree << std::endl;

  std::cout << fMCTree->GetEntries();

  std::string abs_cut =  "(int == \"pi+Inelastic\" && (nPi0 + nPiPlus + nPiMinus) == 0)";
  std::string cex_cut =  "(int == \"pi+Inelastic\" && (nPiPlus + nPiMinus) == 0 && (nPi0 == 1))";

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>total(50,0,500)","","goff");
  TH1D * total = (TH1D*)gDirectory->Get("total");

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>abs(50,0,500)",abs_cut.c_str(),"goff"); 
  TH1D * abs_hist = (TH1D*)gDirectory->Get("abs");

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>cex(50,0,500)",cex_cut.c_str(),"goff"); 
  TH1D * cex_hist = (TH1D*)gDirectory->Get("cex");

  double scale = 1.E27 / (.5 * 2.266 * 6.022E23 / 12.01 );

  abs_hist->Divide( total );
  abs_hist->Scale( scale );

  cex_hist->Divide( total );
  cex_hist->Scale( scale );
  

  //For now, putting in dummy predictions
  double x[5] = {201.6, 216.6, 237.2, 265.6, 295.1};
//  double abs_y[5] = {140., 175., 169., 170., 165.1};
//  double cex_y[5] = {40., 25., 48., 55., 47.};

  double abs_y[5];
  double cex_y[5];

  for( int i = 0; i < 5; ++i ){
    abs_y[i] = abs_hist->GetBinContent( abs_hist->FindBin( x[i] ) );
    cex_y[i] = cex_hist->GetBinContent( cex_hist->FindBin( x[i] ) );
  }

  MC_xsec_abs = new TGraph(5, x, abs_y);
  MC_xsec_cex = new TGraph(5, x, cex_y);

}


double DUETFitter::DoFit(){

  double Chi2 = 0.;

  double DUET_val_i, DUET_val_j, MC_val_i, MC_val_j, cov_val;
  double x;
  
  int NPoints = DUET_xsec_cex->GetN() + DUET_xsec_abs->GetN();


  for( int i = 0; i < NPoints; ++i ){
    
    std::cout << i << std::endl;
    
    if( i < 5 ){
      std::cout << "Getting cex value" << std::endl;
      MC_xsec_cex->GetPoint(i, x, MC_val_i);
      DUET_xsec_cex->GetPoint(i, x, DUET_val_i);
    }
    else{
      std::cout << "Getting abs value" << std::endl;
      MC_xsec_abs->GetPoint(i - 5, x, MC_val_i);
      DUET_xsec_abs->GetPoint(i - 5, x, DUET_val_i);
    }

    std::cout << "\tMC: " << x << " " << MC_val_i << std::endl;
    std::cout << "\tDUET: " << x << " " << DUET_val_i << std::endl;


    for( int j = 0; j < NPoints; ++j ){
   
      std::cout << j << std::endl;

      if( j < 5 ){
        std::cout << "Getting cex value" << std::endl;
        MC_xsec_cex->GetPoint(j, x, MC_val_j);
        DUET_xsec_cex->GetPoint(j, x, DUET_val_j);
      }
      else{
        std::cout << "Getting abs value" << std::endl;
        MC_xsec_abs->GetPoint(j - 5, x, MC_val_j);
        DUET_xsec_abs->GetPoint(j - 5, x, DUET_val_j);
      }

      cov_val = DUET_cov_inv[0][i][j];
      std::cout << "cov: " << i << " " << j << " " << cov_val << std::endl;

      Chi2 += ( MC_val_i - DUET_val_i ) * cov_val * ( MC_val_j - DUET_val_j );
    }
  }

  SaveInfo(Chi2);
  return Chi2;

}

void DUETFitter::SaveInfo( double theChi2 ){
  fOutFile->cd();
  MC_xsec_cex->Write("MC_xsec_cex");
  MC_xsec_abs->Write("MC_xsec_abs");
  DUET_xsec_cex->Write();
  DUET_xsec_abs->Write();

  DUET_cov_matrix->Write("cov");
  DUET_cov_inv->Write("cov_inv");

  TArrayD * Chi2 = new TArrayD(1);
  Chi2->SetAt(theChi2,0);
  fOutFile->WriteObject(Chi2, "Chi2");

  fOutFile->Close();
}
