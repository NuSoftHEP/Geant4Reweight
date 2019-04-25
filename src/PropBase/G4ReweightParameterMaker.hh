#ifndef G4ReweightParameterMaker_h
#define G4ReweightParameterMaker_h

#include "TH1D.h"

#include <map>
#include <vector>

#include "fhiclcpp/ParameterSet.h"

#include "FitParameter.hh"


class G4ReweightParameterMaker{

  public:
    G4ReweightParameterMaker(){};
    G4ReweightParameterMaker( const std::map< std::string, std::vector< FitParameter > > & pars );
    G4ReweightParameterMaker( const std::vector< fhicl::ParameterSet > & FitParSets );
    void SetNewVals( const std::vector< std::pair< std::string, double > > & input );
    void SetNewVals( const std::map< std::string, double > & input );
    void SetParamVals( const std::map< std::string, double > & input );
    void BuildHistsFromPars();
    const std::map< std::string, TH1D* > & GetFSHists() const { return FSHists; };

    const std::map< std::string, std::vector< FitParameter > > & GetParameterSet() const { return FullParameterSet; };

    //Return as a vector for setting values within FitManager
    std::vector< std::pair< std::string, double > > GetParametersAsPairs(){
      std::vector< std::pair< std::string, double > > results;

      for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){
        for( size_t i = 0; i < itPar->second.size(); ++i ){
          if( !itPar->second.at(i).Dummy ){
            results.push_back( std::make_pair( itPar->second.at(i).Name, itPar->second.at(i).Value ) ); 
          }
        }
      }

      return results;
    };

    std::map< std::string, double > GetActiveParameterValues(){

     std::map< std::string, double > results;

      for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){
        for( size_t i = 0; i < itPar->second.size(); ++i ){
          if( !itPar->second.at(i).Dummy ){
            results.insert( std::make_pair( itPar->second.at(i).Name, itPar->second.at(i).Value ) ); 
          }
        }       
      }

      return results;
    };

    std::map< std::string, FitParameter > GetActiveParameters(){

     std::map< std::string, FitParameter > results;

      for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){
        for( size_t i = 0; i < itPar->second.size(); ++i ){
          if( !itPar->second.at(i).Dummy ){
            results.insert( std::make_pair( itPar->second.at(i).Name, itPar->second.at(i) ) ); 
          }
        }       
      }

      return results;
    };

    std::vector< std::pair< std::string, FitParameter > > GetActiveParametersAsPairs(){

      std::vector< std::pair< std::string, FitParameter > > results;

      for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){
        for( size_t i = 0; i < itPar->second.size(); ++i ){
          if( !itPar->second.at(i).Dummy ){
            results.push_back( std::make_pair( itPar->second.at(i).Name, itPar->second.at(i) ) ); 
          }
        }       
      }

      return results;
    };
    int GetNParameters(){ return nParameters; };

  protected:

    std::map< std::string, TH1D* > FSHists;
    TH1D * dummyHist;
    std::map< std::string, std::vector< FitParameter > > FullParameterSet;

    int nParameters;
};

#endif
