#include "geant4reweight/ReweightBase/G4NeutronReweighter.hh"

#include "Geant4/G4Neutron.hh"

G4NeutronReweighter::G4NeutronReweighter(
    TFile * FSInput,
    const std::map<std::string, TH1D*> &FSScales,
    const fhicl::ParameterSet & material_pars,
    G4ReweightManager * rw_manager,
    TH1D * inputElasticBiasHist, bool fix)
  : G4Reweighter(FSInput, FSScales, material_pars, rw_manager,
                 {"total"},
                 inputElasticBiasHist, fix) {


  part_def = neutron->Definition();
  fInelastic = "neutronInelastic";
  SetupProcesses();
}

std::string G4NeutronReweighter::GetInteractionSubtype(
    const G4ReweightTraj & theTraj) {
  return "total";
}

G4NeutronReweighter::~G4NeutronReweighter(){}
