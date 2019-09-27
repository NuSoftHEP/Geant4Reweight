#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

#ifdef FNAL_FHICL
#include "cetlib/filepath_maker.h"
#endif

#include "G4ReweightParameterMaker.hh"
#include "G4ReweightThrowManager.hh"

#include "G4ReweightTreeParser.hh"

#include <string>

#include "TFile.h" 

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

  int nThrows = pset.get< int >( "nThrows" );

  G4ReweightTreeParser * tp = new G4ReweightTreeParser(inFileName.c_str(), outFileName.c_str());


  std::string FracsFileName = pset.get< std::string >( "Fracs" );
  TFile FracsFile( FracsFileName.c_str(), "OPEN" );

  std::string XSecFileName = pset.get< std::string >( "XSec" );
  TFile XSecFile( XSecFileName.c_str(), "OPEN" );

  std::string FitResultsFileName = pset.get< std::string >( "FitResults" );
  TFile FitResultsFile( FitResultsFileName.c_str(), "OPEN" );
  G4ReweightThrowManager ThrowMan( FitResultsFile );

  if( !ThrowMan.Decomp() )
    return 0;


  //Input the ParameterSets 
  //Then create the paramater maker
  std::vector< fhicl::ParameterSet > FitParSets = pset.get< std::vector< fhicl::ParameterSet > >("ParameterSet");
  try{
    G4ReweightParameterMaker ParMaker( FitParSets );


    //Implement this in the tree parser   
    //Will have to do the throws. Save them (maybe in a vector of vectors/maps?, map of vectors?).
    //Then for each set of throws, create teh parameters (hists) and do the reweighting.
    //Weights from these will need to be saved in a tree branch of a vector<double> 
    tp->FillAndAnalyzeFSThrows( &FracsFile, &XSecFile, ParMaker, ThrowMan, nThrows );
    tp->CloseAndSaveOutput();
  }
  catch( const std::exception &e ){
    std::cout << "Caught exception" << std::endl;
  }

  return 0;
}
