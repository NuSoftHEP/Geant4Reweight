#include "G4RunManager.hh"
#include "G4SimDetectorConstruction.hh"
#include "G4SimPhysicsList.hh"
#include "G4SimActionInitialization.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

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

  return 0.;
}
