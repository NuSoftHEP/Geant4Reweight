#include "G4ProtonReweighter.hh"

G4ProtonReweighter::G4ProtonReweighter(
    TFile * FSInput,
    const std::map<std::string, TH1D*> &FSScales,
    const fhicl::ParameterSet & material_pars,
    G4ReweightManager * rw_manager,
    TH1D * inputElasticBiasHist, bool fix) {

  fix_total = fix;
  RWManager = rw_manager;
  MaterialParameters = material_pars;
  elasticBias = inputElasticBiasHist;

  for (auto it = theInts.begin(); it != theInts.end(); ++it) {
    std::string name = *it;
    exclusiveFracs[name] = (TGraph*)FSInput->Get(name.c_str());
    inelScales[name] = FSScales.at(name);
  }
  
  fInelastic = "protonInelastic";
  theInts = {"total"};
  SetupProcesses();
}

std::string G4ProtonReweighter::GetInteractionSubtype(
    const G4ReweightTraj & theTraj) {
  return "total";
}

void G4ProtonReweighter::DefineParticle() {
  std::cout << "Chose Proton" << std::endl;
  part_def = proton->Definition();
}

G4ProtonReweighter::~G4ProtonReweighter(){}
