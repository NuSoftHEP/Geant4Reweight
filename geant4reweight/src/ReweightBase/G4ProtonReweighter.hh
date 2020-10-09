#ifndef G4ProtonReweighter_h
#define G4ProtonReweighter_h

#include "G4Reweighter.hh"

class G4ProtonReweighter : public G4Reweighter {
  public:

    G4ProtonReweighter(TFile *, const std::map<std::string, TH1D*> &,
                       const fhicl::ParameterSet &,
                       G4ReweightManager * rw_manager,
                       TH1D * inputElasticBiasHist = 0x0, bool fix = false);
    virtual ~G4ProtonReweighter();
    std::string GetInteractionSubtype( const G4ReweightTraj & ) override;

  protected:
    G4Proton * proton;
};

#endif
