#include "geant4reweight/ReweightBase/G4PiPlusReweighter.hh"
#include "geant4reweight/ReweightBase/G4ReweightTraj.hh"

#include "Geant4/G4PionPlus.hh"

G4PiPlusReweighter::G4PiPlusReweighter(
    TFile * FSInput,
    const std::map<std::string, TH1D*> &FSScales,
    const fhicl::ParameterSet & material_pars,
    G4ReweightManager * rw_manager,
    TH1D * inputElasticBiasHist, bool fix)
  : G4Reweighter(FSInput, FSScales, material_pars, rw_manager,
                 {"inel", "cex", "abs", "dcex", "prod"},
                 inputElasticBiasHist, fix) {
  part_def = piplus->Definition();
  fInelastic = "pi+Inelastic";
  std::cout << "Part def: " << part_def << std::endl;
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

G4PiPlusReweighter::~G4PiPlusReweighter(){}
