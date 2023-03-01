#ifndef G4PiMinusReweighter_h
#define G4PiMinusReweighter_h

#include "geant4reweight/ReweightBase/G4Reweighter.hh"

class G4PionMinus;

class G4PiMinusReweighter : public G4Reweighter {
  public:

    G4PiMinusReweighter(TFile *, const std::map<std::string, TH1D*> &,
                        const fhicl::ParameterSet &,
                        G4ReweightManager * rw_manager,
                        TH1D * inputElasticBiasHist = nullptr, bool fix = false);
    virtual ~G4PiMinusReweighter();
    std::string GetInteractionSubtype(const G4ReweightTraj &) override;

  protected:
    G4PionMinus * piminus;
};

#endif
