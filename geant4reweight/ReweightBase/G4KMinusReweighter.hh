#ifndef G4KMinusReweighter_h
#define G4KMinusReweighter_h

#include "geant4reweight/ReweightBase/G4Reweighter.hh"

class G4KaonMinus;

class G4KMinusReweighter : public G4Reweighter {
  public:

    G4KMinusReweighter(TFile *, const std::map<std::string, TH1D*> &,
                     const fhicl::ParameterSet &,
                     G4ReweightManager * rw_manager,
                     TH1D * inputElasticBiasHist = nullptr, bool fix = false);
    virtual ~G4KMinusReweighter();
    std::string GetInteractionSubtype(const G4ReweightTraj &) override;

  protected:
    G4KaonMinus * kaon;
};

#endif
