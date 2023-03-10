#ifndef G4NeutronReweighter_h
#define G4NeutronReweighter_h

#include "geant4reweight/ReweightBase/G4Reweighter.hh"

class G4Neutron;

class G4NeutronReweighter : public G4Reweighter {
  public:

    G4NeutronReweighter(TFile *, const std::map<std::string, TH1D*> &,
                        const fhicl::ParameterSet &,
                        G4ReweightManager * rw_manager,
                        TH1D * inputElasticBiasHist = nullptr, bool fix = false);
    virtual ~G4NeutronReweighter();
    std::string GetInteractionSubtype (const G4ReweightTraj &) override;
    //void SetCaptureGraph( TFile *); //set neutron capture graph for neutrons
  protected:
    G4Neutron * neutron;
    TGraph * nCaptureGraph = nullptr; //stores neutron capture cross section
    std::string fCapture="FakeProcess"; // Name of neutron capture process - 
    //FakeProcess -  placeholder to prevent any particles that aren't neutrons from trying to look up capture cross section
};

#endif
