#ifndef G4KPlusReweighter_h
#define G4KPlusReweighter_h

#include "geant4reweight/ReweightBase/G4Reweighter.hh"

class G4KaonPlus;

class G4KPlusReweighter : public G4Reweighter {
  public:

    G4KPlusReweighter(TFile *, const std::map<std::string, TH1D*> &,
                     const fhicl::ParameterSet &,
                     G4ReweightManager * rw_manager,
                     TH1D * inputElasticBiasHist = nullptr, bool fix = false);
    virtual ~G4KPlusReweighter();
    std::string GetInteractionSubtype(const G4ReweightTraj &) override;

  protected:
    G4KaonPlus * kaon;
};

#endif
