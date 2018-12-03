#include "DUETFitter.hh"

#include "TMatrixDUtilsfwd.h"
#include "TArrayD.h"
#include "TGraph2D.h"

#include "G4ReweightInter.hh"
#include <vector>
#include <iomanip>
#include <sstream>

DUETFitter::DUETFitter(std::string mc_name) : fMCFileName(mc_name) { 
  fOutFile = new TFile("DUET_fit.root", "RECREATE"); 
}

DUETFitter::DUETFitter(std::string mc_name, std::string raw_mc_name) : fMCFileName(mc_name), fRawMCFileName(raw_mc_name) { 
  fOutFile = new TFile("DUET_fit.root", "RECREATE"); 

  fFitTree = new TTree("Fit","");
  fFitTree->Branch("norm", &norm_param);
  fFitTree->Branch("Chi2", &the_Chi2);
}

DUETFitter::~DUETFitter(){
  fOutFile->cd(); 
  
  //TGraph * gr = new TGraph( norm_vector.size(), &norm_vector[0], &Chi2_vector[0] );
  //gr->SetTitle(";Normalization; #chi^{2}");
  //gr->Write( "Chi2_vs_norm" );

  TGraph2D * gr = new TGraph2D( norm_abs_vector.size(), &norm_abs_vector[0], &norm_cex_vector[0], &Chi2_vector[0] );
  gr->SetTitle("");
  gr->GetXaxis()->SetTitle("Abs Norm.");
  gr->GetYaxis()->SetTitle("Cex Norm.");
  gr->Write( "Chi2_vs_norm" );


  fFitTree->Write();
  fOutFile->Close();
}

void DUETFitter::LoadData(){

  fDUETFile = new TFile("../data/DUET.root", "READ");

  DUET_xsec_abs   = (TGraphErrors*)fDUETFile->Get("xsec_abs");    
  DUET_xsec_cex   = (TGraphErrors*)fDUETFile->Get("xsec_cex");    
  DUET_cov_matrix = (TMatrixD*)fDUETFile->Get("cov");
  DUET_cov_inv    = &DUET_cov_matrix->Invert();

  fOutFile->cd();
  DUET_xsec_abs->Write();
  DUET_xsec_cex->Write();
  DUET_cov_matrix->Write("cov");
  DUET_cov_inv ->Write("cov_inv");


}

void DUETFitter::LoadRawMC(){
  //Use input file to get a tree to use for nominal fractions
  G4ReweightTreeParser * FSFracs = new G4ReweightTreeParser( fRawMCFileName, "DUET_MC_final_states.root" );
  
  FSFracs->SetBranches();
  FSFracs->FillAndAnalyze(1., 1.);

  fFSTree = FSFracs->GetTree();

}


void DUETFitter::DoReweightFS( double norm_abs, double norm_cex ){

  //Create output file
  std::stringstream stream_abs; 
  stream_abs << std::fixed << std::setprecision(2) << norm_abs;
  std::string norm_abs_str = stream_abs.str();

  std::stringstream stream_cex; 
  stream_cex << std::fixed << std::setprecision(2) << norm_cex;
  std::string norm_cex_str = stream_cex.str();

  std::string RWFileName = "DUET_fit_reweight_abs_" + norm_abs_str + "_cex_" + norm_cex_str + ".root";
  /////////////////////////////
  
  //Make Final State Reweighter
  G4ReweightInter * dummy = new G4ReweightInter(std::vector< std::pair<double, double> >() ); 

  std::map< std::string, G4ReweightInter* > FSInters;
  FSInters["inel"] = dummy;
  FSInters["dcex"] = dummy;
  FSInters["prod"] = dummy;

  std::vector< std::pair< double, double > > abs_vector;
  abs_vector.push_back( std::make_pair(200., norm_abs) );  
  abs_vector.push_back( std::make_pair(300., norm_abs) );  
  FSInters["abs"]  = new G4ReweightInter(abs_vector);

  std::vector< std::pair< double, double > > cex_vector;
  cex_vector.push_back( std::make_pair(200., norm_cex) );  
  cex_vector.push_back( std::make_pair(300., norm_cex) );       
  FSInters["cex"]  = new G4ReweightInter(cex_vector);

  G4ReweightFinalState * FSReweighter = new G4ReweightFinalState( fFSTree, FSInters, 300., 200. );
  //////////////////////////////

  //Make the reweighter pointer and do the reweighting
  Reweighter = new G4ReweightTreeParser( fRawMCFileName, RWFileName );
  Reweighter->SetBranches();
  Reweighter->FillAndAnalyzeFS(FSReweighter);
  ////////////////////////////////

  norm_abs_param = norm_abs;
  norm_cex_param = norm_cex;

}

void DUETFitter::DoReweight( double norm ){

  std::stringstream stream; 
  stream << std::fixed << std::setprecision(2) << norm;
  std::string normstr = stream.str();

  std::string RWFileName = "DUET_fit_reweight_norm_" + normstr + ".root";
  
  Reweighter = new G4ReweightTreeParser( fRawMCFileName, RWFileName );

  std::vector< std::pair< double, double > > reweight_vector;

  reweight_vector.push_back( std::make_pair(200., norm) );  
  reweight_vector.push_back( std::make_pair(300., norm) );  

  G4ReweightInter * dummy = new G4ReweightInter(std::vector< std::pair<double, double> >() ); 
  G4ReweightInter * InelasticReweighter = new G4ReweightInter( reweight_vector );

  Reweighter->SetBranches();
  Reweighter->FillAndAnalyzeFunc(InelasticReweighter, dummy);
 // Reweighter->CloseAndSaveOutput();
 // Reweighter->CloseInput();

 norm_param = norm;

}

void DUETFitter::LoadMC(){
  //fMCFile = new TFile(fMCFileName.c_str(), "READ");
  //fMCTree = (TTree*)fMCFile->Get("tree"); 
  fOutFile->cd();
  fMCTree = Reweighter->GetTree();
  std::cout << "Got Tree " << fMCTree << std::endl;


  std::string abs_cut =  "(int == \"pi+Inelastic\" && (nPi0 + nPiPlus + nPiMinus) == 0)";
  std::string cex_cut =  "(int == \"pi+Inelastic\" && (nPiPlus + nPiMinus) == 0 && (nPi0 == 1))";

  std::string weight = "weight*";

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>total(50,0,500)","","goff");
  TH1D * total = (TH1D*)gDirectory->Get("total");

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>abs(50,0,500)",(weight + abs_cut).c_str(),"goff"); 
  TH1D * abs_hist = (TH1D*)gDirectory->Get("abs");

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>cex(50,0,500)",(weight + cex_cut).c_str(),"goff"); 
  TH1D * cex_hist = (TH1D*)gDirectory->Get("cex");

  double scale = 1.E27 / (.5 * 2.266 * 6.022E23 / 12.01 );

  abs_hist->Divide( total );
  abs_hist->Scale( scale );

  cex_hist->Divide( total );
  cex_hist->Scale( scale );
  

  double x[5] = {201.6, 216.6, 237.2, 265.6, 295.1};

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
    
    if( i < 5 ){
      MC_xsec_cex->GetPoint(i, x, MC_val_i);
      DUET_xsec_cex->GetPoint(i, x, DUET_val_i);
    }
    else{
      MC_xsec_abs->GetPoint(i - 5, x, MC_val_i);
      DUET_xsec_abs->GetPoint(i - 5, x, DUET_val_i);
    }

    for( int j = 0; j < NPoints; ++j ){
   
      if( j < 5 ){
        MC_xsec_cex->GetPoint(j, x, MC_val_j);
        DUET_xsec_cex->GetPoint(j, x, DUET_val_j);
      }
      else{
        MC_xsec_abs->GetPoint(j - 5, x, MC_val_j);
        DUET_xsec_abs->GetPoint(j - 5, x, DUET_val_j);
      }

      cov_val = DUET_cov_inv[0][i][j];

      Chi2 += ( MC_val_i - DUET_val_i ) * cov_val * ( MC_val_j - DUET_val_j );
    }
  }

  the_Chi2 = Chi2;
  SaveInfo();
  return Chi2;
}

void DUETFitter::SaveInfo(){
  fOutFile->cd();

  std::stringstream stream_abs; 
  stream_abs << std::fixed << std::setprecision(2) << norm_abs_param;
  std::string norm_abs_str = stream_abs.str();

  std::stringstream stream_cex; 
  stream_cex << std::fixed << std::setprecision(2) << norm_cex_param;
  std::string norm_cex_str = stream_cex.str();

  MC_xsec_cex->Write(("MC_xsec_cex_" + norm_cex_str).c_str());
  MC_xsec_abs->Write(("MC_xsec_abs_" + norm_abs_str).c_str());
//  DUET_xsec_cex->Write();
//  DUET_xsec_abs->Write();

//  DUET_cov_matrix->Write("cov");
//  DUET_cov_inv->Write("cov_inv");

  Chi2_vector.push_back(the_Chi2);
  norm_abs_vector.push_back(norm_abs_param);
  norm_cex_vector.push_back(norm_cex_param);

  fFitTree->Fill();

//  fOutFile->Close();
   
  Reweighter->CloseAndSaveOutput();
  Reweighter->CloseInput();

  ClearMemory();
}

void DUETFitter::ClearMemory(){
  delete MC_xsec_abs;
  delete MC_xsec_cex;
}
