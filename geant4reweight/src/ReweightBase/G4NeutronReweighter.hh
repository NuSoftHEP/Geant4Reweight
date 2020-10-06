#ifndef G4NeutronReweighter_h
#define G4NeutronReweighter_h

#include "G4Reweighter.hh"

class G4NeutronReweighter : public G4Reweighter {
  public:

    G4NeutronReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales);
    G4NeutronReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist=0x0);
    virtual ~G4NeutronReweighter();
    std::string GetInteractionSubtype (const G4ReweightTraj &) override;
    void SetCaptureGraph( TFile *); //set neutron capture graph for neutrons
  protected:
    TGraph * nCaptureGraph = 0x0; //stores neutron capture cross section
    std::string fCapture="FakeProcess"; // Name of neutron capture process - 
    //FakeProcess -  placeholder to prevent any particles that aren't neutrons from trying to look up capture cross section
};

#endif
