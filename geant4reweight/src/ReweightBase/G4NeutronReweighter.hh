#ifndef G4NeutronReweighter_h
#define G4NeutronReweighter_h

#include "G4Reweighter.hh"

class G4NeutronReweighter : public G4Reweighter {
  public:

    G4NeutronReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales);
    G4NeutronReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist=0x0);
    virtual ~G4NeutronReweighter();
    std::string GetInteractionSubtype( G4ReweightTraj & ) override;
};

#endif
