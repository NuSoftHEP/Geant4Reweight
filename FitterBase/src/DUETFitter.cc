#include "DUETFitter.hh"

#include "TMatrixDUtilsfwd.h"
#include "TArrayD.h"
#include "TGraph2D.h"
#include "TCanvas.h"

#include "G4ReweightInter.hh"

#include <vector>
#include <iomanip>
#include <sstream>


std::string set_prec(double input){
  std::stringstream stream_in; 
  stream_in << std::fixed << std::setprecision(2) << input;
  return stream_in.str();
};

/*DUETFitter::DUETFitter(){
  fOutFile = new TFile( "DUET_fit.root", "RECREATE"); 
}*/

DUETFitter::DUETFitter(std::string raw_mc_name/*, std::string output_dir*/) : fRawMCFileName(raw_mc_name)/*, fOutputDir(output_dir)*/{ 
//  fOutFile = new TFile( (fOutputDir + "/DUET_fit.root").c_str(), "RECREATE"); 
  fOutFile = new TFile( "DUET_fit.root", "RECREATE"); 

/*  fFitTree = new TTree("Fit","");
  fFitTree->Branch("norm", &norm_param);
  fFitTree->Branch("Chi2", &the_Chi2);
*/
}

DUETFitter::DUETFitter( std::vector<std::string> raw_mc_names ) : fRawMCFileNameVector( raw_mc_names ){
  fOutFile = new TFile( "DUET_fit.root", "RECREATE"); 
}

DUETFitter::~DUETFitter(){
  fOutFile->cd(); 
  
  std::cout << "norm_abs: " << norm_abs_vector.size() << std::endl;
  std::cout << "norm_cex: " << norm_cex_vector.size() << std::endl;
  std::cout << "Chi2: " << Chi2_vector.size() << std::endl;
  TGraph2D * gr = new TGraph2D( norm_abs_vector.size(), &norm_abs_vector[0], &norm_cex_vector[0], &Chi2_vector[0] );
  gr->SetTitle("");
  gr->GetXaxis()->SetTitle("Abs Norm.");
  gr->GetYaxis()->SetTitle("Cex Norm.");
  gr->Write( "Chi2_vs_norm" );

  //fFitTree->Write();
  fOutFile->Close();
  std::cout << "outfile closed" << std::endl;
  if(fFrac->IsOpen()){fFrac->Close();}
  //fFSFracs->CloseInput();
}

void DUETFitter::LoadData(){

  fDUETFile = new TFile(fDataFileName.c_str(), "READ");

  DUET_xsec_abs   = (TGraphErrors*)fDUETFile->Get("xsec_abs");    
  DUET_xsec_cex   = (TGraphErrors*)fDUETFile->Get("xsec_cex");    
  DUET_cov_matrix = (TMatrixD*)fDUETFile->Get("cov");
  DUET_cov_inv    = &DUET_cov_matrix->Invert();

  fOutFile->cd();
  DUET_xsec_abs->Write();
  DUET_xsec_cex->Write();
  DUET_cov_matrix->Write("cov");
  DUET_cov_inv->Write("cov_inv");


}

void DUETFitter::LoadFracs(std::string fFracName){
  fFrac = new TFile( fFracName.c_str(), "OPEN" );
  fFSTree = (TTree*)fFrac->Get("tree");
}

void DUETFitter::LoadRawMC(){
  //Use input file to get a tree to use for nominal fractions
  std::cout << "\nGetting Raw Data" << std::endl;
  fFSFracs = new G4ReweightTreeParser( fRawMCFileName, "DUET_MC_final_states.root" );

  fFSFracs->FillAndAnalyze(1., 1.);

  fFSTree = (TTree*)fFSFracs->GetTree();
}

void DUETFitter::LoadRawMCVector(){
  //Use input file to get a tree to use for nominal fractions
  std::cout << "\nGetting Raw Data" << std::endl;
  fFSFracs = new G4ReweightTreeParser( fRawMCFileNameVector[0], "DUET_MC_final_states.root" );


  std::cout << "Raw File: " << 0 << std::endl;
  fFSFracs->FillAndAnalyze(1., 1.);
  std::cout << "Tree Entries: " << fFSFracs->GetTree()->GetEntries() << std::endl;
  fFSFracs->CloseInput();
  
  for( size_t i = 1; i < fRawMCFileNameVector.size(); ++i ){
    std::cout << "Raw File: " << i << std::endl;
    fFSFracs->OpenNewInput( fRawMCFileNameVector[i] );
    fFSFracs->FillAndAnalyze(1., 1.);
    std::cout << "Tree Entries: " << fFSFracs->GetTree()->GetEntries() << std::endl;

    fFSFracs->CloseInput();
  } 

  fFSTree = (TTree*)fFSFracs->GetTree();

  std::cout << "FSTree Entries: " << fFSTree->GetEntries() << std::endl; 

}

void DUETFitter::DoReweightFS( double norm_abs, double norm_cex ){

  std::string norm_abs_str = set_prec(norm_abs);

  std::string norm_cex_str = set_prec(norm_cex);

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
  Reweighter = new G4ReweightTreeParser( fRawMCFileName,  RWFileName );
  Reweighter->FillAndAnalyzeFS(FSReweighter);
  ////////////////////////////////

  norm_abs_param = norm_abs;
  norm_cex_param = norm_cex;

  delete FSReweighter;
}

void DUETFitter::DoReweightFSVector( double norm_abs, double norm_cex ){

  std::string norm_abs_str = set_prec(norm_abs);
  std::string norm_cex_str = set_prec(norm_cex);
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
  Reweighter = new G4ReweightTreeParser( fRawMCFileNameVector[0],  RWFileName );
  std::cout << "MC File: " << 0 << std::endl;
  Reweighter->FillAndAnalyzeFS(FSReweighter);

  std::cout << "Tree Entries: " << Reweighter->GetTree()->GetEntries() << std::endl;
  
  for( size_t i = 1; i < fRawMCFileNameVector.size(); ++i ){

    Reweighter->CloseInput();

    std::cout << "MC File: " << i << std::endl;
    Reweighter->OpenNewInput( fRawMCFileNameVector[i] );
    Reweighter->FillAndAnalyzeFS(FSReweighter);
    std::cout << "Tree Entries: " << Reweighter->GetTree()->GetEntries() << std::endl;

  } 
  ////////////////////////////////

  norm_abs_param = norm_abs;
  norm_cex_param = norm_cex;

  delete FSReweighter;
}

TTree* DUETFitter::GetReweightFS( FitSample theSample ){
  std::cout << "Sample: " << std::endl
            << "\tabs: "  << theSample.abs  << std::endl
            << "\tcex: "  << theSample.cex  << std::endl
            << "\tdcex: " << theSample.dcex << std::endl
            << "\tinel: " << theSample.inel << std::endl
            << "\tprod: " << theSample.prod << std::endl
            << "\tFile: " << theSample.theFile << std::endl;

  norm_abs_param = theSample.abs;
  norm_cex_param = theSample.cex;

  TFile * RWFile = new TFile(theSample.theFile.c_str(), "READ");
  TTree * RWTree = (TTree*)RWFile->Get("tree");
  return RWTree;
}


void DUETFitter::DoReweight( double norm ){

  std::string normstr = set_prec(norm);

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

  if( ActiveSample->Raw ){ 
    DoReweightFS( ActiveSample->abs, ActiveSample->cex );
    fMCTree = Reweighter->GetTree();
  }
  else{
    fMCTree = GetReweightFS( *ActiveSample );
  }
  std::cout << "Got Tree " << fMCTree << std::endl;


  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>total(10,200,300)","","goff");
  TH1D * total = (TH1D*)gDirectory->Get("total");

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>abs(10,200,300)",(weight + abs_cut).c_str(),"goff"); 
  TH1D * abs_hist = (TH1D*)gDirectory->Get("abs");

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>cex(10,200,300)",(weight + cex_cut).c_str(),"goff"); 
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

void DUETFitter::LoadMCVector(){
  
  if( ActiveSample->Raw ){ 
    DoReweightFSVector( ActiveSample->abs, ActiveSample->cex );
    fMCTree = Reweighter->GetTree();
  }
  else{
    fMCTree = GetReweightFS( *ActiveSample );
  }
  std::cout << "Got Tree " << fMCTree << std::endl;
  std::cout << "Entries: " << fMCTree->GetEntries() << std::endl; 

  /*
    Ideas for making this more user-friendly:

    ActiveSample->Experiment
    ActiveSample->GetRange()
    ActiveSample->GetBins()

    ActiveSample->Cut
    ActiveSample->Scale
  */


  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>total(10,200,300)","","goff");
  TH1D * total = (TH1D*)gDirectory->Get("total");

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>abs(10,200,300)",(weight + abs_cut).c_str(),"goff"); 
  TH1D * abs_hist = (TH1D*)gDirectory->Get("abs");

  fMCTree->Draw("sqrt(Energy*Energy - 139.57*139.57)>>cex(10,200,300)",(weight + cex_cut).c_str(),"goff"); 
  TH1D * cex_hist = (TH1D*)gDirectory->Get("cex");

  double scale = 1.E27 / (.5 * 2.266 * 6.022E23 / 12.01 );

  abs_hist->Divide( total );
  abs_hist->Scale( scale );

  cex_hist->Divide( total );
  cex_hist->Scale( scale );
  
  abs_hist->SetMinimum(0.);
  cex_hist->SetMinimum(0.);

  double x[5] = {201.6, 216.6, 237.2, 265.6, 295.1};

  double abs_y[5];
  double cex_y[5];

  for( int i = 0; i < 5; ++i ){
    abs_y[i] = abs_hist->GetBinContent( abs_hist->FindBin( x[i] ) );
    cex_y[i] = cex_hist->GetBinContent( cex_hist->FindBin( x[i] ) );
    std::cout << "abs: " << abs_y[i] << " cex: " << cex_y[i] << std::endl;
  }

  MC_xsec_abs = new TGraph(5, x, abs_y);
  MC_xsec_cex = new TGraph(5, x, cex_y);

}


double DUETFitter::DoFit(){
  BinnedChi2 = new TH2D( ("BinnedChi2_abs_" + set_prec(norm_abs_param) + "_cex_" + set_prec(norm_cex_param)).c_str(), "", 10, 0, 10, 10, 0, 10 );

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

      BinnedChi2->SetBinContent(i+1, j+1, ( MC_val_i - DUET_val_i ) * cov_val * ( MC_val_j - DUET_val_j ) );
      Chi2 += ( MC_val_i - DUET_val_i ) * cov_val * ( MC_val_j - DUET_val_j );
    }
  }

  the_Chi2 = Chi2;
  SaveInfo();
  //BinnedChi2->Write();
  return Chi2;
}

void DUETFitter::SaveInfo(){
  fOutFile->cd();

  std::string norm_abs_str = set_prec(norm_abs_param);

  std::string norm_cex_str = set_prec(norm_cex_param);

  MC_xsec_cex->Write(("MC_xsec_cex_" + norm_cex_str).c_str());
  MC_xsec_abs->Write(("MC_xsec_abs_" + norm_abs_str).c_str());
  Chi2_vector.push_back(the_Chi2);
  norm_abs_vector.push_back(norm_abs_param);
  norm_cex_vector.push_back(norm_cex_param);

  BinnedChi2->Write();

  //fFitTree->Fill();

//  fOutFile->Close();
  
  if( ActiveSample->Raw ){
    Reweighter->CloseAndSaveOutput();
    Reweighter->CloseInput();

    //Risky putting this here
    delete Reweighter;
  }

  ClearMemory();
}

void DUETFitter::ClearMemory(){
  delete MC_xsec_abs;
  delete MC_xsec_cex;
}

void DUETFitter::ParseXML(std::string FileName){

  tinyxml2::XMLDocument doc;

  tinyxml2::XMLError loadResult = doc.LoadFile( FileName.c_str() );
  if( loadResult != tinyxml2::XML_SUCCESS ){
    std::cout << "Could not load file" << std::endl;
    return;
  }

  tinyxml2::XMLNode * theRoot = doc.FirstChild();
  if( !theRoot ){
    std::cout << "Could Not get first child" << std::endl;
    return;
  }

 
  tinyxml2::XMLElement * theSample = theRoot->FirstChildElement( "Sample" );
  if( !theSample ){
    std::cout << "Could Not get element " << std::endl;
    return;
  }
  
  while( theSample ){
   
    double cex, abs;
    tinyxml2::XMLError attResult = theSample->QueryDoubleAttribute("abs", &abs);
    if( attResult != tinyxml2::XML_SUCCESS ){
      std::cout << "Could not get abs" << std::endl;
    }

    attResult = theSample->QueryDoubleAttribute("cex", &cex);
    if( attResult != tinyxml2::XML_SUCCESS ){
      std::cout << "Could not get cex" << std::endl;
    }

    std::cout << "abs: " << set_prec(abs) << " cex: " << set_prec(cex) << std::endl;
    
    bool Raw;
    attResult = theSample->QueryBoolAttribute("Raw", &Raw);
    if( attResult != tinyxml2::XML_SUCCESS ){
      std::cout << "Could not get Raw" << std::endl;
    }

    std::string reweightFile;

    if( !Raw ){ 
      const char * reweightFileText = nullptr;

      reweightFileText = theSample->Attribute("File");
      if (reweightFileText != nullptr) reweightFile = reweightFileText;

      std::cout << "File: " << reweightFile << std::endl;
    }
    else std::cout << "Need to run reweighting" << std::endl;
    
    FitSample theFitSample;
    theFitSample.Raw  = Raw;
    theFitSample.abs  = abs;
    theFitSample.cex  = cex;
    theFitSample.inel = 1.0;
    theFitSample.prod = 1.0;
    theFitSample.dcex = 1.0;
    theFitSample.theFile = reweightFile;
   
    samples.push_back( theFitSample );
    
    theSample = theSample->NextSiblingElement("Sample");
  }

  tinyxml2::XMLElement * theData = theRoot->FirstChildElement("Data");
  if( !theData ){
    std::cout << "Could Not get Data " << std::endl;
    return;
  }

  std::string dataFile;
  const char * dataFileText = nullptr;
  dataFileText = theData->Attribute("File");
  if (dataFileText != nullptr) dataFile = dataFileText;

  std::cout << "Data: " << dataFile << std::endl;

  fDataFileName = dataFile;
}
