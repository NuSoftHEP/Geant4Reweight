#include "G4ReweightParameterMaker.hh"
#include "G4ReweightThrowManager.hh"
#include "FitParameter.hh"

#include "TFile.h"

#include <string>
#include <map>

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

int main( int argc, char ** argv ){

  fhicl::ParameterSet ps; 
  fhicl::make_ParameterSet(argv[1], ps);
  std::vector< fhicl::ParameterSet > FitParSets = ps.get< std::vector< fhicl::ParameterSet > >("ParameterSet");

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
