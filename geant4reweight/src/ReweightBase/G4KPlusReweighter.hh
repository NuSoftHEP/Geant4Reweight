#ifndef G4KPlusReweighter_h
#define G4KPlusReweighter_h

#include "G4Reweighter.hh"

class G4KPlusReweighter : public G4Reweighter {
  public:

    G4KPlusReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales);
    G4KPlusReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist=0x0);
    virtual ~G4KPlusReweighter();
    std::string GetInteractionSubtype( G4ReweightTraj & ) override;
};

#endif
