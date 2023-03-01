#ifndef G4ProtonReweighter_h
#define G4ProtonReweighter_h

#include "geant4reweight/ReweightBase/G4Reweighter.hh"

class G4ReweightManager;
class G4ReweightTraj;

namespace fhicl {
  class ParameterSet;
}

class G4Proton;

class TFile;
class TH1D;

#include <map>
#include <string>

class G4ProtonReweighter : public G4Reweighter {
  public:

    G4ProtonReweighter(TFile *, const std::map<std::string, TH1D*> &,
                       const fhicl::ParameterSet &,
                       G4ReweightManager * rw_manager,
                       TH1D * inputElasticBiasHist = nullptr, bool fix = false);
    virtual ~G4ProtonReweighter();
    std::string GetInteractionSubtype( const G4ReweightTraj & ) override;

  protected:
    G4Proton * proton;
};

#endif
