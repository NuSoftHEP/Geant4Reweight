#include "G4Reweighter.hh"
#include "G4ReweightStep.hh"

#include <utility>
#include <algorithm>
#include <iostream>

#include "TROOT.h"
#include "TVectorD.h"



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


void G4Reweighter::SetupWorld() {

  testMaterial = 0x0;

  std::string MaterialName = MaterialParameters.get<std::string>("Name");

  std::vector<fhicl::ParameterSet> MaterialComponents
  = MaterialParameters.get<std::vector<fhicl::ParameterSet>>("Components");

  Density = MaterialParameters.get<double>("Density");


  if(MaterialComponents.size() == 1){
    int MaterialZ = MaterialComponents[0].get<int>("Z");

    testMaterial = new G4Material(MaterialName,
                                  MaterialZ,
                                  Mass*g/mole,
                                  Density*g/cm3);
  }
  else {                                                                                       
    double sum = 0.0;                                                                  
    for (auto s : MaterialComponents) {                                      
      double frac = s.get<double>("Fraction");                                         
      sum += frac;                                                                     
    }                                                                                  
    if(sum < 1.0){                                                                     
      std::cout << "Sum of all element fractions equals " << sum << "\n";              
      std::cout << "Fractions will be divided by this factor to normalize \n";         
    }                                                                                  
    else if(sum > 1.0){                                                                
      std::cout << "Sum of all element fractions equals " << sum << "\n";              
      std::cout << "This is greater than 1.0 - something is wrong here \n";            
      abort();                                                                         
    }                                                                                  
                                                                                       
    testMaterial = new G4Material(MaterialName, 
                                 Density*g/cm3, 
                                 MaterialComponents.size());    
    for (auto s : MaterialComponents) {                                      
      int MaterialZ = s.get<int>("Z");                                                 
      double Mass = s.get<double>("Mass");                                     
      std::string name = s.get<std::string>("Name");                                   
      double frac = s.get<double>("Fraction");                                         
      G4Element * element = new G4Element(name, " ", MaterialZ, Mass*g/mole);  
      testMaterial->AddElement(element, frac/sum);                                      
    }                                                                                  
  }// end else()  (complex material)                       
   




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

void G4Reweighter::SetupParticle() {
  Density = MaterialParameters.get<double>("Density");

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
  decay_hook = new G4DecayHook();
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
    else if (theName == "CoulombScat") {
      coul_proc = (G4CoulombScattering*)proc;
    }
  }

  if (!elastic_proc || !inelastic_proc /*|| !coul_proc*/) {
    throw cet::exception("G4Reweighter") << "Fatal Error: Could not find procs";
  }

}


void G4Reweighter::SetMomentum(double p) {
  double KE = sqrt(p*p + std::pow(part_def->GetPDGMass(), 2))
              - part_def->GetPDGMass(); 
  dynamic_part->SetKineticEnergy(KE);
}

double G4Reweighter::GetDecayMFP(double p) {
  SetMomentum(p);
  return decay_hook->GetMFP(*testTrack);
}

double G4Reweighter::GetCoulMFP(double p) {
  SetMomentum(p);
  double mfp = coul_proc->MeanFreePath(*testTrack);
  if (mfp == DBL_MAX) {
    return 0.;
  }
  return mfp;
}

double G4Reweighter::GetInelasticXSec(double p) {
  SetMomentum(p);

  return ( inelastic_proc->GetCrossSectionDataStore()->GetCrossSection(
             dynamic_part, testMaterial) *cm );
}

double G4Reweighter::GetExclusiveXSec(double p, std::string cut) {
  return (exclusiveFracs[cut]->Eval(p)*GetInelasticXSec(p));
}

double G4Reweighter::GetElasticXSec(double p) {
  SetMomentum(p);

  return ( elastic_proc->GetCrossSectionDataStore()->GetCrossSection(
            dynamic_part, testMaterial) *cm );
}

double G4Reweighter::GetNominalMFP(double p) {
  double xsec = GetInelasticXSec(p);
  return 1.0 / xsec;
}

double G4Reweighter::GetBiasedMFP(double p){
  double b = GetInelasticBias(p);
  return  GetNominalMFP( p ) / b;
}

double G4Reweighter::GetNominalElasticMFP(double p){
  double xsec = GetElasticXSec(p);/*(elastic_proc->GetCrossSectionDataStore()->GetCrossSection(
      dynamic_part, (*testMaterial->GetElementVector())[0],
      testMaterial) / millibarn);*/
  return 1.0 / xsec;
}

double G4Reweighter::GetElasticBias(double p) {
  return elasticBias->GetBinContent(elasticBias->FindBin(p));
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

double G4Reweighter::GetExclusiveFactor(double p, std::string cut) {
  return inelScales[cut]->GetBinContent(inelScales[cut]->FindBin(p));
}

double G4Reweighter::GetBiasedElasticMFP( double p ){
  double b = GetElasticBias(p);
  return GetNominalElasticMFP( p ) / b;
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
                  //Handled by exclusive factor below
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


G4Reweighter::~G4Reweighter(){
}

