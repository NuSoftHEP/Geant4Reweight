#include "G4PiMinusReweighter.hh"

G4PiMinusReweighter::G4PiMinusReweighter(
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
  
  fInelastic = "pi-Inelastic";
  theInts = {"inel", "cex", "abs", "dcex", "prod"};
  SetupProcesses();
}

std::string G4PiMinusReweighter::GetInteractionSubtype(
    const G4ReweightTraj & theTraj) {
  int nPi0     = theTraj.HasChild(111).size();
  int nPiPlus  = theTraj.HasChild(211).size();
  int nPiMinus = theTraj.HasChild(-211).size();

  if( (nPi0 + nPiPlus + nPiMinus) == 0){
    return "abs";
  }
  else if( (nPiPlus + nPiMinus) == 0 && nPi0 == 1 ){
    return "cex";
  }
  else if( (nPiPlus + nPiMinus + nPi0) > 1 ){
    return "prod";
  }
  else if( (nPi0 + nPiMinus) == 0 && nPiPlus == 1 ){
    return "dcex";
  }
  else if( (nPi0 + nPiPlus) == 0 && nPiMinus == 1 ){
    return "inel";
  }

  return "";
}

void G4PiMinusReweighter::DefineParticle() {
  std::cout << "Chose PiMinus" << std::endl;
  part_def = piminus->Definition();
}

G4PiMinusReweighter::~G4PiMinusReweighter(){}
