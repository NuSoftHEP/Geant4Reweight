#ifndef G4ProtonReweighter_h
#define G4ProtonReweighter_h

#include "G4Reweighter.hh"

class G4ProtonReweighter : public G4Reweighter {
  public:

    G4ProtonReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales);
    G4ProtonReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist=0x0);
    virtual ~G4ProtonReweighter();
    std::string GetInteractionSubtype( G4ReweightTraj & ) override;
};

#endif
