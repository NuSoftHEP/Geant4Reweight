#include "geant4reweight/ReweightBase/G4Reweighter.hh"

#include "geant4reweight/PredictionBase/G4CascadeDetectorConstruction.hh"
#include "geant4reweight/PredictionBase/G4CascadePhysicsList.hh"

#include "geant4reweight/ReweightBase/G4ReweightManager.hh"
#include "geant4reweight/ReweightBase/G4ReweightStep.hh"
#include "geant4reweight/ReweightBase/G4ReweightTraj.hh"

#include "cetlib_except/exception.h"

#include "Geant4/G4Box.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4Element.hh"
#include "Geant4/G4HadronElasticProcess.hh"
#include "Geant4/G4HadronInelasticProcess.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4ProcessVector.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4StepPoint.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4VProcess.hh"

#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"

#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <utility>

G4Reweighter::G4Reweighter(TFile * FSInput,
                           const std::map<std::string, TH1D*> &FSScales,
                           const fhicl::ParameterSet & material_pars,
                           G4ReweightManager * rw_manager,
                           std::vector<std::string> the_ints,
                           TH1D * inputElasticBiasHist, bool fix)
  : fix_total(fix),
    MaterialParameters(material_pars),
    RWManager(rw_manager),
    elasticBias(inputElasticBiasHist) {

  for (auto it = the_ints.begin(); it != the_ints.end(); ++it) {
    std::string name = *it;
    exclusiveFracs[name] = (TGraph*)FSInput->Get(name.c_str());
    inelScales[name] = FSScales.at(name);
  }
}

void G4Reweighter::SetNewHists(const std::map<std::string, TH1D*> & FSScales) {
  for (auto it = inelScales.begin(); it != inelScales.end(); ++it) {
    std::string name = it->first;
    it->second = FSScales.at(name);
  }
}

void G4Reweighter::SetNewElasticHists(TH1D * inputElasticBiasHist) {
  elasticBias = inputElasticBiasHist;
}

void G4Reweighter::SetupParticle() {

  std::string material_name = MaterialParameters.get<std::string>("Name");

  dynamic_part = new G4DynamicParticle(part_def, G4ThreeVector(0.,0.,1.), 0.);
  double center = RWManager->GetCenter(material_name);
  G4ThreeVector point(0., 0., center);
  testTrack = new G4Track(dynamic_part, 0., point);
  testStep = new G4Step();
  testPoint = new G4StepPoint();
  testMaterial =
      RWManager->GetVolume(material_name)->GetLogicalVolume()->GetMaterial();

  testPoint->SetMaterial(testMaterial);
  testPoint->SetMaterialCutsCouple(
      RWManager->GetVolume(
          material_name)->GetLogicalVolume()->GetMaterialCutsCouple());
  testStep->SetPreStepPoint(testPoint);
  testTrack->SetStep(testStep);
}

void G4Reweighter::SetupProcesses() {
  SetupParticle();
  G4ProcessManager * pm = part_def->GetProcessManager();
  G4ProcessVector  * pv = pm->GetProcessList();

  for( size_t i = 0; i < (size_t)pv->size(); ++i ){
    G4VProcess * proc = (*pv)(i);
    std::string theName = proc->GetProcessName();
    if( theName == "hadElastic" ){
      elastic_proc = (G4HadronElasticProcess*)proc;
    }
    else if( theName == fInelastic ){
      inelastic_proc = (G4HadronInelasticProcess*)proc;
    }

    else {
      std::cout << theName << std::endl;
    }
  }

  if (!elastic_proc || !inelastic_proc) {
    throw cet::exception("G4Reweighter") << "Fatal Error: Could not find procs";
  }

}


void G4Reweighter::SetMomentum(double p) {
  double KE = sqrt(p*p + std::pow(part_def->GetPDGMass(), 2))
              - part_def->GetPDGMass();
  dynamic_part->SetKineticEnergy(KE);
}

double G4Reweighter::GetInelasticXSec(double p, bool mb_units) {
  SetMomentum(p);

  double scale = (mb_units ?
                  (1.e24/(*testMaterial->GetAtomicNumDensityVector())) :
                  1.);

  return scale*(inelastic_proc->GetCrossSectionDataStore()->GetCrossSection(
              dynamic_part, testMaterial)*cm*fInelasticPreBias);
}

double G4Reweighter::GetExclusiveXSec(double p, std::string cut, bool mb_units) {
  return (exclusiveFracs[cut]->Eval(p)*GetInelasticXSec(p, mb_units));
}

double G4Reweighter::GetElasticXSec(double p, bool mb_units) {
  SetMomentum(p);

  double scale = (mb_units ?
                  (1.e24/(*testMaterial->GetAtomicNumDensityVector())) :
                  1.);

  return scale*(elastic_proc->GetCrossSectionDataStore()->GetCrossSection(
              dynamic_part, testMaterial)*cm*fElasticPreBias);
}

double G4Reweighter::GetNominalMFP(double p) {
  double xsec = GetInelasticXSec(p);
  return 1.0 / xsec;
}

double G4Reweighter::GetBiasedMFP(double p){
  double b = GetInelasticBias(p);
  return  GetNominalMFP( p ) / b;
}

double G4Reweighter::GetBiasedMFP(double p, const G4ReweightVarMap & var_map) {
  double b = GetInelasticBias(p, var_map);
  return  GetNominalMFP( p ) / b;
}


double G4Reweighter::GetNominalElasticMFP(double p){
  double xsec = GetElasticXSec(p);
  return 1.0 / xsec;
}

double G4Reweighter::GetElasticBias(double p) {
  return elasticBias->GetBinContent(elasticBias->FindBin(p));
}

double G4Reweighter::GetElasticBias(double p, const G4ReweightVarMap & var_map) {
  return var_map.GetElasticVar(p);
}

double G4Reweighter::GetInelasticBias(double p) {
  double bias = 0.;
  for (auto it = inelScales.begin(); it != inelScales.end(); ++it) {
    std::string name = it->first;
    TH1D * scale = it->second;
    bias += (exclusiveFracs[name]->Eval(p) *
             scale->GetBinContent(scale->FindBin(p)));
  }
  return bias;
}

double G4Reweighter::GetInelasticBias(double p,
                                      const G4ReweightVarMap & var_map) {
  //Inelastic bias = total_inel_var*(sum over cuts {cut_fraction * cut_variation})
  // = var_total_xsec / nominal_total_xsec

  double bias = 0.;
  for (const auto & [name, frac] : exclusiveFracs) {
    bias += (frac->Eval(p)*
             var_map.GetExclusiveVar(name, p));
  }

  return bias*var_map.GetTotalInelVar(p);
}

double G4Reweighter::GetExclusiveFactor(double p, std::string cut) {
  return inelScales[cut]->GetBinContent(inelScales[cut]->FindBin(p));
}

double G4Reweighter::GetExclusiveFactor(double p, std::string cut,
                                        const G4ReweightVarMap & var_map) {
  return var_map.GetExclusiveVar(cut, p);
}

double G4Reweighter::GetBiasedElasticMFP( double p ){
  double b = GetElasticBias(p);
  return GetNominalElasticMFP( p ) / b;
}

double G4Reweighter::GetBiasedElasticMFP(double p,
                                         const G4ReweightVarMap & var_map) {
  double b = GetElasticBias(p, var_map);
  return GetNominalElasticMFP(p)/b;
}

std::string G4Reweighter::GetInteractionSubtype( const G4ReweightTraj & theTraj ){
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

double G4Reweighter::GetWeight( const G4ReweightTraj * theTraj ){


  if (theTraj->GetPDG() != dynamic_part->GetPDGcode()) return 1.;
  double total = 0.;
  double bias_total = 0.;
  double weight = 1.;
  size_t nsteps = theTraj->GetNSteps();
  double min = 1.e-14;

  for (size_t i = 0; i < nsteps; ++i) {
    auto theStep = theTraj->GetStep(i);
    double p = theStep.GetFullPreStepP();

    total += theStep.GetStepLength()*(
        (GetNominalMFP(p) > min ? 1. / GetNominalMFP(p) : min) +
        (GetNominalElasticMFP(p) > min ? 1. / GetNominalElasticMFP(p) : min));

    bias_total += theStep.GetStepLength() *(
        (GetNominalMFP(p) > min ? 1. / GetBiasedMFP(p) : min) +
        (GetNominalElasticMFP(p) > min ? 1. / GetBiasedElasticMFP(p) : min));

    if (theStep.GetStepChosenProc() == "hadElastic") {
      weight *= GetElasticBias(p);
    }
    else if (theStep.GetStepChosenProc() == fInelastic) {
      std::string cut = GetInteractionSubtype(*theTraj);
      if (cut == "") {
        return 1.;
      }
      weight *= GetExclusiveFactor(p, cut);
    }
  }

  weight *= exp(total - bias_total);
  return weight;
}

double G4Reweighter::GetWeight(const G4ReweightTraj & traj,
                               const G4ReweightVarMap & var_map) {


  if (traj.GetPDG() != dynamic_part->GetPDGcode()) return 1.;
  double total = 0.;
  double bias_total = 0.;
  double weight = 1.;
  size_t nsteps = traj.GetNSteps();
  double min = 1.e-14;

  for (size_t i = 0; i < nsteps; ++i) {
    auto theStep = traj.GetStep(i);
    double p = theStep.GetFullPreStepP();

    total += theStep.GetStepLength()*(
        (GetNominalMFP(p) > min ? 1. / GetNominalMFP(p) : min) +
        (GetNominalElasticMFP(p) > min ? 1. / GetNominalElasticMFP(p) : min));

    bias_total += theStep.GetStepLength() *(
        (GetNominalMFP(p) > min ? 1. / GetBiasedMFP(p, var_map) : min) +
        (GetNominalElasticMFP(p) > min ? 1. / GetBiasedElasticMFP(p, var_map) : min));

    if (theStep.GetStepChosenProc() == "hadElastic") {
      weight *= var_map.GetElasticVar(p);
    }
    else if (theStep.GetStepChosenProc() == fInelastic) {
      std::string cut = GetInteractionSubtype(traj);
      if (cut == "") {
        return 1.;
      }
      //Decoupled these two variations from each other
      weight *= (var_map.GetExclusiveVar(cut, p)*var_map.GetTotalInelVar(p));
    }
  }

  weight *= exp(total - bias_total);
  return weight;
}

G4Reweighter::~G4Reweighter(){}
