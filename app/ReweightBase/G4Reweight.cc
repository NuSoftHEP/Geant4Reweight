#include <iostream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"

#include "G4ReweightTreeParser.hh"
#include "G4Reweighter.hh"

#include "G4ReweightParameterMaker.hh"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

std::string fcl_file;

bool enablePiMinus = false;

bool parseArgs(int argc, char ** argv);

int main(int argc, char ** argv){

  if(!parseArgs(argc, argv)) 
    return 0;

  fhicl::ParameterSet ps = fhicl::make_ParameterSet( fcl_file );

  std::string outFileName = ps.get< std::string >( "OutputFile" );
  std::string inFileName  = ps.get< std::string >( "InputFile" ); 

  std::string FracsFileName = ps.get< std::string >( "Fracs" );
  TFile FracsFile( FracsFileName.c_str(), "OPEN" );

  G4ReweightTreeParser * tp = new G4ReweightTreeParser(inFileName.c_str(), outFileName.c_str());

  std::vector< fhicl::ParameterSet > FitParSets = ps.get< std::vector< fhicl::ParameterSet > >("ParameterSet");
  G4ReweightParameterMaker ParMaker( FitParSets );

  G4Reweighter * theReweighter = new G4Reweighter( &FracsFile, ParMaker.GetFSHists() ); 
  if( enablePiMinus ) theReweighter->SetPiMinus();

  std::string XSecFileName = ps.get< std::string >( "XSec" );
  TFile XSecFile( XSecFileName.c_str(), "OPEN" );

  theReweighter->SetTotalGraph(&XSecFile);
  tp->FillAndAnalyzeFS(theReweighter);
  tp->CloseAndSaveOutput();
  tp->CloseInput();

  std::cout << "done" << std::endl;

  return 0;
}

bool parseArgs(int argc, char ** argv){
  
  for(int i = 1; i < argc; ++i){
    if( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 ){
      std::cout << "Usage: ./G4Reweight -c config.fcl" << std::endl;
      return false;
    }
    else if( strcmp(argv[i], "-c") == 0 ){
      fcl_file = argv[i+1];
    }
    else if( strcmp(argv[i], "--PiM") == 0 ){
      enablePiMinus = atoi( argv[i+1] );
    }

  }
  return true;
}


