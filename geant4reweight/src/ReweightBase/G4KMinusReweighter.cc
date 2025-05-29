#include "G4KMinusReweighter.hh"

G4KMinusReweighter::G4KMinusReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales){
  fInelastic = "kaon-Inelastic";
  theInts = {"total"};
  Initialize(totalInput, FSInput, FSScales);
}

G4KMinusReweighter::G4KMinusReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist){
  fInelastic = "kaon-Inelastic";
  theInts = {"total"};
  Initialize(totalInput,FSInput,FSScales,inputElasticBiasHist);
}

std::string G4KMinusReweighter::GetInteractionSubtype( G4ReweightTraj & theTraj ){
    return "total";
}

G4KMinusReweighter::~G4KMinusReweighter(){}
