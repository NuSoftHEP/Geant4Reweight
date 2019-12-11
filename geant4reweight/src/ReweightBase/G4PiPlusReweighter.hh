#ifndef G4PiPlusReweighter_h
#define G4PiPlusReweighter_h

#include "G4Reweighter.hh"

class G4PiPlusReweighter : public G4Reweighter {
  public:

    G4PiPlusReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales);
    G4PiPlusReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist=0x0);
    virtual ~G4PiPlusReweighter();
    std::string GetInteractionSubtype( G4ReweightTraj & ) override;
};

#endif
