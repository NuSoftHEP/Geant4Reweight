#ifndef G4KMinusReweighter_h
#define G4KMinusReweighter_h

#include "G4Reweighter.hh"

class G4KMinusReweighter : public G4Reweighter {
  public:

    G4KMinusReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales);
    G4KMinusReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist=0x0);
    virtual ~G4KMinusReweighter();
    std::string GetInteractionSubtype( G4ReweightTraj & ) override;
};

#endif
