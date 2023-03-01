#ifndef G4ReweighterFactory_h
#define G4ReweighterFactory_h

class G4Reweighter;
class G4ReweightManager;

namespace fhicl {
  class ParameterSet;
}

class TFile;
class TH1D;

#include <map>
#include <string>

class G4ReweighterFactory{

  public:
   
    G4Reweighter * BuildReweighter (
        int PDG, TFile * FSInput,
        const std::map<std::string, TH1D*> & FSScales,
        const fhicl::ParameterSet & material_pars,
        G4ReweightManager * rw_manager,
        TH1D * inputElasticBiasHist=nullptr, bool fix_total=false);
    
};

#endif
