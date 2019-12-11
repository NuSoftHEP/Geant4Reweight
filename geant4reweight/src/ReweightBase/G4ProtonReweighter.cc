#include "G4ProtonReweighter.hh"

G4ProtonReweighter::G4ProtonReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales){
  fInelastic = "protonInelastic";
  theInts = {"total"};
  Initialize(totalInput, FSInput, FSScales);
}

G4ProtonReweighter::G4ProtonReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist){
  fInelastic = "protonInelastic";
  theInts = {"total"};
  Initialize(totalInput,FSInput,FSScales,inputElasticBiasHist);
}

std::string G4ProtonReweighter::GetInteractionSubtype( G4ReweightTraj & theTraj ){
    return "total";
}

G4ProtonReweighter::~G4ProtonReweighter(){}
