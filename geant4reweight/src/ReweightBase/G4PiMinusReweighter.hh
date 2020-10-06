#ifndef G4PiMinusReweighter_h
#define G4PiMinusReweighter_h

#include "G4Reweighter.hh"

class G4PiMinusReweighter : public G4Reweighter {
  public:

    G4PiMinusReweighter(TFile *, const std::map<std::string, TH1D*> &,
                        const fhicl::ParameterSet &,
                        TH1D * inputElasticBiasHist = 0x0, bool fix = false);
    virtual ~G4PiMinusReweighter();
    std::string GetInteractionSubtype(const G4ReweightTraj &) override;
    void DefineParticle() override;

  protected:
    G4PionMinus * piminus;
};

#endif
