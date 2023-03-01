#ifndef G4PiPlusReweighter_h
#define G4PiPlusReweighter_h

#include "geant4reweight/ReweightBase/G4Reweighter.hh"

class G4PionPlus;

class G4PiPlusReweighter : public G4Reweighter {
  public:

    G4PiPlusReweighter(TFile *, const std::map<std::string, TH1D*> &,
                       const fhicl::ParameterSet &,
                       G4ReweightManager * rw_manager,
                       TH1D * inputElasticBiasHist = nullptr, bool fix = false);
    virtual ~G4PiPlusReweighter();
    std::string GetInteractionSubtype(const G4ReweightTraj &) override;

  protected:
    G4PionPlus * piplus;
};

#endif
