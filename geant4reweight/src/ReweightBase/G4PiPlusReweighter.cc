#include "G4PiPlusReweighter.hh"

G4PiPlusReweighter::G4PiPlusReweighter(
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
  
  fInelastic = "pi+Inelastic";
  theInts = {"inel", "cex", "abs", "dcex", "prod"};
  SetupProcesses();
}

std::string G4PiPlusReweighter::GetInteractionSubtype(
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
    return "inel";
  }
  else if( (nPi0 + nPiPlus) == 0 && nPiMinus == 1 ){
    return "dcex";
  }

  return "";
}

void G4PiPlusReweighter::DefineParticle() {
  std::cout << "Chose PiPlus" << std::endl;
  part_def = piplus->Definition();
  inel_name = "pi+Inelastic";
}

G4PiPlusReweighter::~G4PiPlusReweighter(){}
