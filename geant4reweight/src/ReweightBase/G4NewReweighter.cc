#include "G4NewReweighter.hh"
#include "G4ReweightStep.hh"

#include <utility>
#include <algorithm>
#include <iostream>

#include "TROOT.h"
#include "TVectorD.h"



G4NewReweighter::G4NewReweighter(TFile * FSInput,
                                 const std::map<std::string, TH1D*> &FSScales,
                                 const fhicl::ParameterSet & material_pars,
                                 TH1D * inputElasticBiasHist, bool fix)
  : fix_total(fix),
    MaterialParameters(material_pars),
    elasticBias(inputElasticBiasHist) {

  for (auto it = theInts.begin(); it != theInts.end(); ++it) {
    std::string name = *it;
    exclusiveFracs[name] = (TGraph*)FSInput->Get(name.c_str());
    inelScales[name] = FSScales.at(name);
  }
  
  SetupProcesses();
}

void G4NewReweighter::SetNewHists(const std::map<std::string, TH1D*> & FSScales) {
   for (auto it = theInts.begin(); it != theInts.end(); ++it) {
    std::string name = *it;
    inelScales[name] = FSScales.at(name);
  } 
}

void G4NewReweighter::SetNewElasticHists(TH1D * inputElasticBiasHist)
{
  elasticBias = inputElasticBiasHist;
}


void G4NewReweighter::SetupWorld() {
  std::string MaterialName = MaterialParameters.get<std::string>("Name");

  int MaterialZ = MaterialParameters.get<int>("Z");
  Mass = MaterialParameters.get<double>("Mass");
  Density = MaterialParameters.get<double>("Density");
  testMaterial = new G4Material(MaterialName,
                                MaterialZ,
                                Mass*g/mole,
                                Density*g/cm3);
  //World
  solidWorld = new G4Box("World", 40.*cm, 47.*cm, 90.*cm);
  logicWorld = new G4LogicalVolume(solidWorld,
                                   testMaterial,
                                   "World");
  physWorld = new G4PVPlacement(
      0, G4ThreeVector(), logicWorld, "World", 0, false, 0, true);

  detector = new G4CascadeDetectorConstruction(physWorld);
  physList = new G4CascadePhysicsList();
 
}

void G4NewReweighter::SetupParticle() {
  //G4PionPlus  * piplus = 0x0;
  //G4PionMinus * piminus = 0x0;
  //G4Proton  * proton = 0x0;
  //G4Neutron * neutron = 0x0;
  //G4ParticleDefinition * part_def = 0x0;
  //if( type == 211 ){
    std::cout << "Chose PiPlus" << std::endl;
    part_def = piplus->Definition();
    inel_name = "pi+Inelastic";
  /*}
  else if( type == -211 ){
    std::cout << "Chose PiMinus" << std::endl;
    part_def = piminus->Definition();
    inel_name = "pi-Inelastic";
  }
  else if( type == 2212 ){
    std::cout << "Chose Proton" << std::endl;
    part_def = proton->Definition();
    inel_name = "protonInelastic";
  }
  else if( type == 2112 ){
    std::cout << "Chose Neutron" << std::endl;
    part_def = neutron->Definition();
    inel_name = "neutronInelastic";
  }
  else{
    std::cout << "Please specify either 211, -211, or 2212" << std::endl;
    //throw
  }*/

  dynamic_part = new G4DynamicParticle(part_def, G4ThreeVector(0.,0.,1.), 0.);
  testTrack = new G4Track( dynamic_part, 0., G4ThreeVector(0.,0.,0.) );
  testStep = new G4Step();
  testPoint = new G4StepPoint();
  testPoint->SetMaterial(physWorld->GetLogicalVolume()->GetMaterial());
  testPoint->SetMaterialCutsCouple(
      physWorld->GetLogicalVolume()->GetMaterialCutsCouple());
  testStep->SetPreStepPoint(testPoint);
  testTrack->SetStep(testStep);



}

void G4NewReweighter::SetupProcesses() {
  //Initializing
  rm = new G4RunManager();

  SetupWorld();

  rm->SetUserInitialization(detector);
  rm->SetUserInitialization(physList);
  rm->Initialize();
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();

  //auto theElement = (*testMaterial->GetElementVector())[0];

  SetupParticle();
  decay_hook = new G4DecayHook();
  G4ProcessManager * pm = part_def->GetProcessManager();
  G4ProcessVector  * pv = pm->GetProcessList();
  
  for( size_t i = 0; i < (size_t)pv->size(); ++i ){
    G4VProcess * proc = (*pv)(i);
    std::string theName = proc->GetProcessName();
    if( theName == "hadElastic" ){          
      std::cout << "Found elastic" << std::endl;
      elastic_proc = (G4HadronElasticProcess*)proc;
    }
    else if( theName == inel_name ){
      std::cout << "Found inelastic" << std::endl;
      inelastic_proc = (G4HadronInelasticProcess*)proc;
    }
    else if (theName == "CoulombScat") {
      coul_proc = (G4CoulombScattering*)proc;
    }
  }

  if (!elastic_proc || !inelastic_proc || !coul_proc) {
    std::cout << "Fatal Error: could not get the processes" << std::endl;
    //throw;
  }

  //G4CrossSectionDataStore *theElastStore   = elastic_proc->GetCrossSectionDataStore();
  //G4CrossSectionDataStore *theInelastStore = inelastic_proc->GetCrossSectionDataStore();
    //inelastic_xsec = theInelastStore->GetCrossSection( dynamic_part, theElement, testMaterial ) / millibarn;
    //elastic_xsec = theElastStore->GetCrossSection( dynamic_part, theElement, testMaterial ) / millibarn;

}


void G4NewReweighter::SetMomentum(double p) {
  double KE = sqrt(p*p + std::pow(part_def->GetPDGMass(), 2))
              - part_def->GetPDGMass(); 
  dynamic_part->SetKineticEnergy(KE);
}

double G4NewReweighter::GetDecayMFP(double p) {
  SetMomentum(p);
  return decay_hook->GetMFP(*testTrack);
}

double G4NewReweighter::GetCoulMFP(double p) {
  SetMomentum(p);
  double mfp = coul_proc->MeanFreePath(*testTrack);
  if (mfp == DBL_MAX) {
    return 0.;
  }
  return mfp;
}

double G4NewReweighter::GetInelasticXSec(double p) {
  SetMomentum(p);

  return (inelastic_proc->GetCrossSectionDataStore()->GetCrossSection(
      dynamic_part, (*testMaterial->GetElementVector())[0],
      testMaterial) / millibarn);
}

double G4NewReweighter::GetExclusiveXSec(double p, std::string cut) {
  return (exclusiveFracs[cut]->Eval(p)*GetInelasticXSec(p));
}

double G4NewReweighter::GetElasticXSec(double p) {
  SetMomentum(p);

  return (elastic_proc->GetCrossSectionDataStore()->GetCrossSection(
      dynamic_part, (*testMaterial->GetElementVector())[0],
      testMaterial) / millibarn);
}

double G4NewReweighter::GetNominalMFP(double p) {
  double xsec = GetInelasticXSec(p);
  return 1.e27 * Mass / ( Density * 6.022e23 * xsec );
}

double G4NewReweighter::GetBiasedMFP(double p){
  double b = GetInelasticBias(p);
  return  GetNominalMFP( p ) / b;
}

double G4NewReweighter::GetNominalElasticMFP(double p){
  double xsec = GetElasticXSec(p);/*(elastic_proc->GetCrossSectionDataStore()->GetCrossSection(
      dynamic_part, (*testMaterial->GetElementVector())[0],
      testMaterial) / millibarn);*/
  return 1.e27 * Mass / ( Density * 6.022e23 * xsec );
}

double G4NewReweighter::GetElasticBias(double p) {
  return elasticBias->GetBinContent(elasticBias->FindBin(p));
}

double G4NewReweighter::GetInelasticBias(double p) {
  double bias = 0.;
  for (auto it = theInts.begin(); it != theInts.end(); ++it) {
    std::string name = *it;
    TH1D * scale = inelScales[name];
    bias += (exclusiveFracs[name]->Eval(p) *
             scale->GetBinContent(scale->FindBin(p)));
  }
  return bias;
}

double G4NewReweighter::GetExclusiveFactor(double p, std::string cut) {
  return inelScales[cut]->GetBinContent(inelScales[cut]->FindBin(p));
}

double G4NewReweighter::GetBiasedElasticMFP( double p ){
  double b = GetElasticBias(p);
  return GetNominalElasticMFP( p ) / b;
}

std::string G4NewReweighter::GetInteractionSubtype( const G4ReweightTraj & theTraj ){
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

double G4NewReweighter::GetWeight( const G4ReweightTraj * theTraj ){

  double total = 0.;
  double bias_total = 0.;

  double weight = 1.;

  size_t nsteps = theTraj->GetNSteps();
  
  double min = 1.e-14;

  for (size_t i = 0; i < nsteps; ++i) {
    auto theStep = theTraj->GetStep(i);
    double p = theStep->GetFullPreStepP();

    double bias_val =
        theStep->GetStepLength() *
        ((GetNominalMFP(p) > min ? 1. / GetBiasedMFP(p) : min) +
         (GetDecayMFP(p) > min ? 1. / GetDecayMFP(p) : min) +
         (GetNominalElasticMFP(p) > min ? 1. / GetBiasedElasticMFP(p) : min)/* +
         (GetCoulMFP(p) > min ? 1. / GetCoulMFP(p) : min)*/);
    double val = theStep->GetStepLength() *
                 ((GetNominalMFP(p) > min ? 1. / GetNominalMFP(p) : min) +
                  (GetDecayMFP(p) > min ? 1. / GetDecayMFP(p) : min) +
                  (GetNominalElasticMFP(p) > min ? 1. / GetNominalElasticMFP(p) 
                                              : min)/* +
                  (GetCoulMFP(p) > min ? 1. / GetCoulMFP(p) : min)*/);

    //std::cout << i << "vals: " << bias_val << " " << val << std::endl;
    /**/if (theStep->GetStepChosenProc() == "hadElastic") {

      if (bias_val > min && val > min) {
        weight *= (1. - exp(-1.*bias_val));
        weight /= (1. - exp(-1.*val));
        weight *= GetElasticBias(p) / bias_val;
        weight /= 1. / val;
      }
    }
    else /**/if (theStep->GetStepChosenProc() == fInelastic) {

      if (bias_val > min && val > min) {
        weight *= (1. - exp(-1.*bias_val));
        weight /= (1. - exp(-1.*val));
        weight *= /*GetInelasticBias(p)*/1. / bias_val;
        weight /= 1. / val;
      }

      std::string cut = GetInteractionSubtype(*theTraj);
      if( cut == "" ){
        return 1.;
      }

/*
      TGraph * theGraph = GetExclusiveVariationGraph( cut );
      if( theGraph ){
        double exclusive_factor = 1;
        if (p > theGraph->GetX()[0] &&
            p < theGraph->GetX()[theGraph->GetN() - 1]) {
          exclusive_factor = theGraph->Eval( p );
        }

        weight *= exclusive_factor;
      }
      */
      
      weight *= GetExclusiveFactor(p, cut);

    }
    else if (theStep->GetStepChosenProc() == "Decay"/* ||
             theStep->GetStepChosenProc() == "CoulombScat"*/) {
      if (bias_val > min && val > min) {
        weight *= (1. - exp(-1.*bias_val));
        weight /= (1. - exp(-1.*val));
        weight *= 1. / bias_val;
        weight /= 1. / val;
      }
    }
    else {
      //std::cout << "Survive " << p << " " << GetNominalMFP(p) <<
      //             " " << GetBiasedMFP(p) << " " <<
      //             totalGraph->Eval( p ) << std::endl;
      total += theStep->GetStepLength()*(
          (GetNominalMFP(p) > min ? 1. / GetNominalMFP(p) : min) +
          (GetNominalElasticMFP(p) > min ? 1. / GetNominalElasticMFP(p) : min));

      bias_total += theStep->GetStepLength() *(
          (GetNominalMFP(p) > min ? 1. / GetBiasedMFP(p) : min) +
          (GetNominalElasticMFP(p) > min ? 1. / GetBiasedElasticMFP(p) : min));
      //std::cout << "totals: " << total << " " << bias_total << std::endl;
    }
    //std::cout << i << " " << theStep->GetStepChosenProc() << " " << weight << std::endl;
  }

  //std::cout << "weight: " << weight << std::endl;
  weight *= exp(total - bias_total);
  //std::cout << "weight after totals: " << weight << std::endl;
  return weight;
}

G4NewReweighter::~G4NewReweighter(){
}

