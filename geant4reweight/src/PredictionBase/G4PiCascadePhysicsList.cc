#include "G4PiCascadePhysicsList.hh"

#include "Geant4/G4DecayPhysics.hh"
#include "Geant4/G4RadioactiveDecayPhysics.hh"
#include "Geant4/G4EmStandardPhysics.hh"
#include "Geant4/G4HadronPhysicsFTFP_BERT.hh"
#include "Geant4/G4HadronElasticPhysics.hh"


G4PiCascadePhysicsList::G4PiCascadePhysicsList() : G4VModularPhysicsList(){
  SetVerboseLevel(5);

  RegisterPhysics( new G4DecayPhysics );
  RegisterPhysics( new G4RadioactiveDecayPhysics );
  RegisterPhysics( new G4EmStandardPhysics );

  RegisterPhysics( new G4HadronPhysicsFTFP_BERT() );
  RegisterPhysics( new G4HadronElasticPhysics() );
  std::cout << "Finished Physics" << std::endl;
}

G4PiCascadePhysicsList::~G4PiCascadePhysicsList(){}

void G4PiCascadePhysicsList::SetCuts(){
  std::cout << "Set Cuts" << std::endl; 
  G4VUserPhysicsList::SetCuts();
}



