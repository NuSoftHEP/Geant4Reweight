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

//Defines parseArgs and the command line options
#include "parse_reweight_args.hh"

int main(int argc, char ** argv){

  if(!parseArgs(argc, argv)) 
    return 0;

  fhicl::ParameterSet ps = fhicl::make_ParameterSet( fcl_file );

  std::string outFileName = ps.get< std::string >( "OutputFile" );
  if( output_file_override  != "empty" ){
    outFileName = output_file_override;
  }

  std::string inFileName  = ps.get< std::string >( "InputFile" ); 
  if( input_file_override  != "empty" ){
    inFileName = input_file_override;
  }

  std::string FracsFileName = ps.get< std::string >( "Fracs" );
  TFile FracsFile( FracsFileName.c_str(), "OPEN" );

  G4ReweightTreeParser * tp = new G4ReweightTreeParser(inFileName.c_str(), outFileName.c_str());

  std::vector< fhicl::ParameterSet > FitParSets = ps.get< std::vector< fhicl::ParameterSet > >("ParameterSet");

  try{
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
  }
  catch( const std::exception &e ){
    std::cout << "Caught exception" << std::endl;
  }

  return 0;
}




