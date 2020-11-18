#ifndef G4KPlusReweighter_h
#define G4KPlusReweighter_h

#include "G4Reweighter.hh"

class G4KPlusReweighter : public G4Reweighter {
  public:

    G4KPlusReweighter(TFile *, const std::map<std::string, TH1D*> &,
                     const fhicl::ParameterSet &,
                     G4ReweightManager * rw_manager,
                     TH1D * inputElasticBiasHist = 0x0, bool fix = false);
    virtual ~G4KPlusReweighter();
    std::string GetInteractionSubtype(const G4ReweightTraj &) override;

  protected:
    G4KaonPlus * kaon;
};

#endif
