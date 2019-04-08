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
    void SetNewVals( const std::map< std::string, std::vector< FitParameter > > & pars );
    const std::map< std::string, TH1D* > & GetFSHists() const { return FSHists; };

  protected:

    std::map< std::string, TH1D* > FSHists;
    TH1D * dummyHist;
};

#endif
