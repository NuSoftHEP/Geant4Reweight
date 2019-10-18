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

#ifdef FNAL_FHICL
#include "cetlib/filepath_maker.h"
#endif

//Defines parseArgs and the command line options
#include "parse_reweight_args.hh"

int main(int argc, char ** argv){

  if(!parseArgs(argc, argv)) 
    return 0;

  fhicl::ParameterSet pset;
  #ifdef FNAL_FHICL
    // Configuration file lookup policy.
    char const* fhicl_env = getenv("FHICL_FILE_PATH");
    std::string search_path;

    if (fhicl_env == nullptr) {
      std::cerr << "Expected environment variable FHICL_FILE_PATH is missing or empty: using \".\"\n";
      search_path = ".";
    }
    else {
      search_path = std::string{fhicl_env};
    }

    cet::filepath_first_absolute_or_lookup_with_dot lookupPolicy{search_path};

    fhicl::make_ParameterSet(fcl_file, lookupPolicy, pset);

  #else
    pset = fhicl::make_ParameterSet(fcl_file);
  #endif

  std::string outFileName = pset.get< std::string >( "OutputFile" );
  if( output_file_override  != "empty" ){
    outFileName = output_file_override;
  }

  std::string inFileName  = pset.get< std::string >( "InputFile" ); 
  if( input_file_override  != "empty" ){
    inFileName = input_file_override;
  }

  std::string FracsFileName = pset.get< std::string >( "Fracs" );
  TFile FracsFile( FracsFileName.c_str(), "OPEN" );

  G4ReweightTreeParser * tp = new G4ReweightTreeParser(inFileName.c_str(), outFileName.c_str());

  std::vector< fhicl::ParameterSet > FitParSets = pset.get< std::vector< fhicl::ParameterSet > >("ParameterSet");

  try{
    G4ReweightParameterMaker ParMaker( FitParSets );

    std::string XSecFileName = pset.get< std::string >( "XSec" );
    TFile XSecFile( XSecFileName.c_str(), "OPEN" );

    G4Reweighter * theReweighter = new G4Reweighter( &XSecFile, &FracsFile, ParMaker.GetFSHists() ); 

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




