#include "G4NeutronReweighter.hh"

G4NeutronReweighter::G4NeutronReweighter(
    TFile * FSInput,
    const std::map<std::string, TH1D*> &FSScales,
    const fhicl::ParameterSet & material_pars,
    TH1D * inputElasticBiasHist, bool fix) {

  fix_total = fix;
  MaterialParameters = material_pars;
  elasticBias = inputElasticBiasHist;

  for (auto it = theInts.begin(); it != theInts.end(); ++it) {
    std::string name = *it;
    exclusiveFracs[name] = (TGraph*)FSInput->Get(name.c_str());
    inelScales[name] = FSScales.at(name);
  }
  
  fInelastic = "pi+Inelastic";
  theInts = {"inel", "cex", "abs", "dcex", "prod"};
  fInelastic = "neutronInelastic";
  fCapture = "nCapture";
  theInts = {"total"};
  SetupProcesses();
  //SetCaptureGraph( totalInput );
}

std::string G4NeutronReweighter::GetInteractionSubtype(
    const G4ReweightTraj & theTraj) {
  return "total";
}

/*void G4NeutronReweighter::SetCaptureGraph(TFile * input){
  nCaptureGraph = (TGraph*)input->Get( "n_capture_momentum");
}*/

void G4NeutronReweighter::DefineParticle() {
  part_def = neutron->Definition();
}

G4NeutronReweighter::~G4NeutronReweighter(){}
