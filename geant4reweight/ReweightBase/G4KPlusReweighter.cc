#include "geant4reweight/ReweightBase/G4KPlusReweighter.hh"

#include "Geant4/G4KaonPlus.hh"

G4KPlusReweighter::G4KPlusReweighter(
    TFile * FSInput,
    const std::map<std::string, TH1D*> &FSScales,
    const fhicl::ParameterSet & material_pars,
    G4ReweightManager * rw_manager,
    TH1D * inputElasticBiasHist, bool fix)
  : G4Reweighter(FSInput, FSScales, material_pars, rw_manager,
                 {"total"},
                 inputElasticBiasHist, fix) {
  part_def = kaon->Definition();
  fInelastic = "kaon+Inelastic";
  SetupProcesses();
}

std::string G4KPlusReweighter::GetInteractionSubtype(
    const G4ReweightTraj & theTraj) {
  return "total";
}

G4KPlusReweighter::~G4KPlusReweighter(){}
