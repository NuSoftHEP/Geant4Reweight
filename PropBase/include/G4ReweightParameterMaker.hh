#ifndef G4ReweightParameterMaker_h
#define G4ReweightParameterMaker_h

#include "TH1D.h"

#include <map>
#include <vector>

#include "fhiclcpp/ParameterSet.h"

#include "FitParameter.hh"


class G4ReweightParameterMaker{

  public:
    G4ReweightParameterMaker( const std::map< std::string, std::vector< FitParameter > > & pars );
    G4ReweightParameterMaker( const std::vector< fhicl::ParameterSet > & FitParSets );
    void SetNewVals( const std::map< std::string, double > & input );
    void SetParamVals( const std::map< std::string, double > & input );
    void BuildHistsFromPars();
    const std::map< std::string, TH1D* > & GetFSHists() const { return FSHists; };

    const std::map< std::string, std::vector< FitParameter > > & GetParameterSet() const { return FullParameterSet; };

  protected:

    std::map< std::string, TH1D* > FSHists;
    TH1D * dummyHist;
    std::map< std::string, std::vector< FitParameter > > FullParameterSet;
};

#endif
