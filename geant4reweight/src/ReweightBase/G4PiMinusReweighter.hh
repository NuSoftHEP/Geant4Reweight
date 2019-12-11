#ifndef G4PiMinusReweighter_h
#define G4PiMinusReweighter_h

#include "G4Reweighter.hh"

class G4PiMinusReweighter : public G4Reweighter {
  public:

    G4PiMinusReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales);
    G4PiMinusReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist=0x0);
    virtual ~G4PiMinusReweighter();
    std::string GetInteractionSubtype( G4ReweightTraj & ) override;
};

#endif
