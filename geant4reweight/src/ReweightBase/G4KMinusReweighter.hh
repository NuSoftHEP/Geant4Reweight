#ifndef G4KMinusReweighter_h
#define G4KMinusReweighter_h

#include "G4Reweighter.hh"
#include "Geant4/G4KaonMinus.hh"

class G4KMinusReweighter : public G4Reweighter {
  public:

    G4KMinusReweighter(TFile *, const std::map<std::string, TH1D*> &,
                     const fhicl::ParameterSet &,
                     G4ReweightManager * rw_manager,
                     TH1D * inputElasticBiasHist = 0x0, bool fix = false);
    virtual ~G4KMinusReweighter();
    std::string GetInteractionSubtype(const G4ReweightTraj &) override;

  protected:
    G4KaonMinus * kaon;
};

#endif
