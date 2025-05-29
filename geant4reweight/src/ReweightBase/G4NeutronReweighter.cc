#include "G4NeutronReweighter.hh"

G4NeutronReweighter::G4NeutronReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales){
  fInelastic = "neutronInelastic";
  theInts = {"total"};
  Initialize(totalInput, FSInput, FSScales);
}

G4NeutronReweighter::G4NeutronReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist){
  fInelastic = "neutronInelastic";
  theInts = {"total"};
  Initialize(totalInput,FSInput,FSScales,inputElasticBiasHist);
}

std::string G4NeutronReweighter::GetInteractionSubtype( G4ReweightTraj & theTraj ){
    return "total";
}

G4NeutronReweighter::~G4NeutronReweighter(){}
