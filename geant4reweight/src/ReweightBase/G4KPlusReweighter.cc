#include "G4KPlusReweighter.hh"

G4KPlusReweighter::G4KPlusReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales){
  fInelastic = "kaon+Inelastic";
  theInts = {"total"};
  Initialize(totalInput, FSInput, FSScales);
}

G4KPlusReweighter::G4KPlusReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist){
  fInelastic = "kaon+Inelastic";
  theInts = {"total"};
  Initialize(totalInput,FSInput,FSScales,inputElasticBiasHist);
}

std::string G4KPlusReweighter::GetInteractionSubtype( G4ReweightTraj & theTraj ){
    return "total";
}

G4KPlusReweighter::~G4KPlusReweighter(){}
