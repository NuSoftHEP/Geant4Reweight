#include "G4ReweightParameterMaker.hh"
#include "G4ReweightThrowManager.hh"
#include "FitParameter.hh"

#include "TFile.h"

#include <string>
#include <map>

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

int main( int argc, char ** argv ){

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(argv[1]);
  std::vector< fhicl::ParameterSet > FitParSets = ps.get< std::vector< fhicl::ParameterSet > >("ParameterSet");

  std::map< std::string, std::vector< FitParameter > > FullParameterSet;

  std::map< std::string, TH1D* > par_val_hists;
 
  for( size_t i = 0; i < FitParSets.size(); ++i ){
    fhicl::ParameterSet theSet = FitParSets.at(i);
    std::string theCut = theSet.get< std::string >("Cut");

    if( FullParameterSet.find( theCut ) == FullParameterSet.end() ){  
      FullParameterSet[ theCut ] = std::vector< FitParameter  >();
    }   

    bool isDummy = theSet.get< bool >("Dummy");
    if( isDummy ){
      FitParameter dummyPar;
      dummyPar.Name = "dummy";
      dummyPar.Cut = theCut;
      dummyPar.Value = 1.; 
      dummyPar.Range = std::make_pair( 0., 0.);
      dummyPar.Dummy = true;
          
      FullParameterSet[ theCut ].push_back( dummyPar );
    }   
    else{ 
      std::cout << "Making parameters for " << theCut << std::endl;
        
      std::vector< fhicl::ParameterSet > theParameters = theSet.get< std::vector< fhicl::ParameterSet > >("Parameters");
      for( size_t j = 0; j < theParameters.size(); ++j ){
        fhicl::ParameterSet thePar = theParameters.at(j);


        std::string theName = thePar.get< std::string >("Name");
        std::cout << theName << std::endl;
            
        std::pair< double, double > theRange = thePar.get< std::pair< double, double > >("Range");
        std::cout << "Range Low: " << theRange.first << " High: " << theRange.second << std::endl;

        double nominal = thePar.get< double >("Nominal",1.);

        FitParameter par;
        par.Name = theName;
        par.Cut = theCut;
        par.Dummy = false;
        par.Value = nominal; 
        par.Range = theRange;
        FullParameterSet[ theCut ].push_back( par );

        TH1D * hist = new TH1D(theName.c_str(), "",100,0,3.);
        par_val_hists[theName] = hist;

      }   
    }   
  }

  G4ReweightParameterMaker parMaker( FullParameterSet );

  const std::map< std::string, TH1D* > & hists = parMaker.GetFSHists();

  TFile fout( "par_try.root", "RECREATE" );
  fout.cd();
  for( auto itHist = hists.begin(); itHist != hists.end(); ++itHist ){
    itHist->second->Write( ("nominal_" + itHist->first).c_str() );
  }
  
  TFile FitResults( "alt_curveFitter_try.root", "OPEN" );
  G4ReweightThrowManager throwMan( FitResults );
  if( throwMan.Decomp() ){
    
    for( int i = 0; i < 1000; ++i ){
      std::map< std::string, double > vals = throwMan.DoThrow();

      for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){
        if( !itPar->second.at(0).Dummy ){
          for( size_t j = 0; j < itPar->second.size(); ++j ){ 
            
            itPar->second.at(j).Value = vals[itPar->second.at(j).Name];
            
            par_val_hists[itPar->second.at(j).Name]->Fill( vals[itPar->second.at(j).Name] );
          }
        }
      }

      parMaker.SetNewVals( FullParameterSet );
      fout.cd();
      for( auto itHist = hists.begin(); itHist != hists.end(); ++itHist ){
        itHist->second->Write( ("new_" + std::to_string(i) + "_" + itHist->first).c_str() );
      }

    }
  }

  fout.cd();
  for( auto h = par_val_hists.begin(); h != par_val_hists.end(); ++h ){
    h->second->Write();
  } 

  fout.Close();

  return 0;
}
