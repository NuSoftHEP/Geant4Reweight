#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4SimDetectorConstruction.hh"
#include "G4SimPhysicsList.hh"
#include "G4SimActionInitialization.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4DynamicParticle.hh"

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

  double KE = 100.;  
  G4ThreeVector dir = G4ThreeVector(1./sqrt(3.), 1./sqrt(3.), 1./sqrt(3.));
  G4DynamicParticle * part = new G4DynamicParticle(piplus, dir, KE);

  G4ProcessManager * pm = piplus->GetProcessManager();
  auto pv = pm->GetProcessList();

  G4Step * step = new G4Step();
  G4Track * track = new G4Track(part, 0., G4ThreeVector());
  G4StepPoint * point = new G4StepPoint();
  
  //Associating the step/steppoint/track
  step->SetPreStepPoint(point);
  track->SetStep(step);
  step->SetTrack(track);


  point->SetMomentumDirection(dir);
  point->SetKineticEnergy(KE);

  std::cout << "KE: " << point->GetKineticEnergy() << std::endl <<
  point->GetMomentum().x() << " " << point->GetMomentum().y() << " " << point->GetMomentum().z() << std::endl;

  track->SetKineticEnergy(KE);
  std::cout << "Track KE: " << track->GetKineticEnergy() << std::endl;

  std::cout << track->GetStep()->GetPreStepPoint()->GetKineticEnergy() << std::endl;

//  G4SimDetectorConstruction * det = new G4SimDetectorConstruction();
//  auto con = det->Construct();
  G4NistManager * nist = G4NistManager::Instance();
  G4Material * water = nist->FindOrBuildMaterial("G4_WATER");
  point->SetMaterial(water);

  std::cout << pv->size() << std::endl;
  G4ForceCondition * fc = new G4ForceCondition();
  for(int i = 0; i < pv->entries(); ++i){
    auto theProc = (*pv)(i);
    if(theProc->GetProcessName() == "CoulombScat"){
      theProc->SetParticle(piplus);
    }
    std::cout << theProc->GetProcessName() << std::endl;
    std::cout << theProc->GetMFP(*track, 0., fc) << std::endl;
  }


  return 0.;
}
