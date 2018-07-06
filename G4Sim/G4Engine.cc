#include "G4RunManager.hh"
#include "G4SimDetectorConstruction.hh"
#include "G4SimPhysicsList.hh"
#include "G4SimActionInitialization.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"

int main(int argc, char ** argv){
  G4RunManager * rm = new G4RunManager;
  rm->SetUserInitialization(new G4SimDetectorConstruction);
  rm->SetUserInitialization(new G4SimPhysicsList);
//  rm->SetUserInitialization(new G4SimActionInitialization);
  rm->InitializeGeometry();
  rm->InitializePhysics(); 
  std::cout << "Initialized" << std::endl;

  G4ParticleTable * pt = G4ParticleTable::GetParticleTable();
  std::cout << "Got Particle Table" << std::endl;

  G4ParticleDefinition * piplus = pt->FindParticle("pi+");
  std::cout << piplus->GetPDGMass() << std::endl;

  G4ProcessManager * pm = piplus->GetProcessManager();
  auto pv = pm->GetProcessList();

  G4Step * step = new G4Step();
  G4Track * track = new G4Track();
  G4StepPoint * point = new G4StepPoint();

  step->SetPreStepPoint(point);
  track->SetStep(step);
  step->SetTrack(track);

//  G4SimDetectorConstruction * det = new G4SimDetectorConstruction();
//  auto con = det->Construct();
  G4Material * water = nist->FindOrBuildMaterial("G4_WATER");
  point->SetMaterial(water);

  std::cout << pv->size() << std::endl;

  for(int i = 0; i < pv->entries(); ++i){
    std::cout << (*pv)(i)->GetProcessName() << std::endl;
  }


  return 0.;
}
