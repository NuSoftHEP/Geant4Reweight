#include "G4ReweightParameterMaker.hh"
#include "FitParameter.hh"

#include "TFile.h"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

int main( int argc, char ** argv ){

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(argv[1]);
  std::vector< fhicl::ParameterSet > FitParSets = ps.get< std::vector< fhicl::ParameterSet > >("ParameterSet");

  std::map< std::string, std::vector< FitParameter > > FullParameterSet;
 
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
  
  FullParameterSet[ "reac" ].at(0).Value = 2.;
  FullParameterSet[ "reac" ].at(1).Value = .5;
  FullParameterSet[ "abs"  ].at(0).Value = 1.5;
  FullParameterSet[ "cex"  ].at(0).Value = 2.;
  FullParameterSet[ "inel"  ].at(0).Value = .5;
  
  parMaker.SetNewVals( FullParameterSet );
  for( auto itHist = hists.begin(); itHist != hists.end(); ++itHist ){
    itHist->second->Write( ("new_" + itHist->first).c_str() );
  }
  fout.Close();

  return 0;
}
