#include "geant4reweight/src/PropBase/G4ReweightParameterMaker.hh"
#include "geant4reweight/src/PropBase/G4ReweightThrowManager.hh"
#include "geant4reweight/src/FitterBase/FitParameter.hh"

#include "TFile.h"

#include <string>
#include <map>

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

//#ifdef FNAL_FHICL
#include "cetlib/filepath_maker.h"
//#endif

int main( int argc, char ** argv ){

  fhicl::ParameterSet pset; 

  //#ifdef FNAL_FHICL
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

    fhicl::make_ParameterSet(argv[1], lookupPolicy, pset);

//  #else
//    pset = fhicl::make_ParameterSet(argv[1]);
//  #endif

  std::vector< fhicl::ParameterSet > FitParSets = pset.get< std::vector< fhicl::ParameterSet > >("ParameterSet");

  G4ReweightParameterMaker parMaker(  FitParSets );
  const std::map< std::string, TH1D* > & hists = parMaker.GetFSHists();

  TFile fout( "par_try.root", "RECREATE" );
  fout.cd();
  for( auto itHist = hists.begin(); itHist != hists.end(); ++itHist ){
    itHist->second->Write( ("nominal_" + itHist->first).c_str() );
  }
  
  TFile FitResults( argv[2], "OPEN" );
  G4ReweightThrowManager throwMan( FitResults );
  if( throwMan.Decomp() ){
    
    for( int i = 0; i < 1000; ++i ){

      std::map< std::string, double > vals = throwMan.DoThrow();
      parMaker.SetNewVals( vals );

      fout.cd();
      for( auto itHist = hists.begin(); itHist != hists.end(); ++itHist ){
        itHist->second->Write( ("new_" + std::to_string(i) + "_" + itHist->first).c_str() );
      }

    }
  }

  fout.cd();
  /*for( auto h = par_val_hists.begin(); h != par_val_hists.end(); ++h ){
    h->second->Write();
  } */

  fout.Close();

  return 0;
}
