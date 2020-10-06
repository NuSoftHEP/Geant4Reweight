#include "G4NeutronReweighter.hh"

G4NeutronReweighter::G4NeutronReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales){
  fInelastic = "neutronInelastic";
  fCapture = "nCapture";
  theInts = {"total"};
  Initialize(totalInput, FSInput, FSScales);
  //set neutron capture cross section graph as well
  SetCaptureGraph( totalInput );
}

G4NeutronReweighter::G4NeutronReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist){
  fInelastic = "neutronInelastic";
  fCapture = "nCapture";
  theInts = {"total"};
  std::cout << "Setting up neutron reweighter" << std::endl;
  Initialize(totalInput,FSInput,FSScales,inputElasticBiasHist);
  //set neutron capture cross section graph as well
  SetCaptureGraph( totalInput );
  std::cout << "Finished setting up neutron reweighter" << std::endl;
}

std::string G4NeutronReweighter::GetInteractionSubtype(
    const G4ReweightTraj & theTraj) {
  return "total";
}

G4NeutronReweighter::~G4NeutronReweighter(){}
