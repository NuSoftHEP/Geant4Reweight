#include "geant4reweight/ReweightBase/G4ProtonReweighter.hh"

#include "Geant4/G4Proton.hh"

G4ProtonReweighter::G4ProtonReweighter(
    TFile * FSInput,
    const std::map<std::string, TH1D*> &FSScales,
    const fhicl::ParameterSet & material_pars,
    G4ReweightManager * rw_manager,
    TH1D * inputElasticBiasHist, bool fix)
  : G4Reweighter(FSInput, FSScales, material_pars, rw_manager,
                 {"total"},
                 inputElasticBiasHist, fix) {

  part_def = proton->Definition();
  fInelastic = "protonInelastic";
  SetupProcesses();
}

std::string G4ProtonReweighter::GetInteractionSubtype(
    const G4ReweightTraj & theTraj) {
  return "total";
}

G4ProtonReweighter::~G4ProtonReweighter(){}
