#ifndef G4PiPlusReweighter_h
#define G4PiPlusReweighter_h

#include "G4Reweighter.hh"

class G4PiPlusReweighter : public G4Reweighter {
  public:

    G4PiPlusReweighter(TFile *, const std::map<std::string, TH1D*> &,
                       const fhicl::ParameterSet &,
                       G4ReweightManager * rw_manager,
                       TH1D * inputElasticBiasHist = 0x0, bool fix = false);
    virtual ~G4PiPlusReweighter();
    std::string GetInteractionSubtype(const G4ReweightTraj &) override;
    void DefineParticle() override;

  protected:
    G4PionPlus * piplus;
};

#endif
