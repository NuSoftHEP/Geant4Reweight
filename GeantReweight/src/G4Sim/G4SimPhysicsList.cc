#include "G4SimPhysicsList.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmStandardPhysics.hh"


G4SimPhysicsList::G4SimPhysicsList() : G4VModularPhysicsList(){
  SetVerboseLevel(5);

  RegisterPhysics( new G4DecayPhysics );
  RegisterPhysics( new G4RadioactiveDecayPhysics );
  RegisterPhysics( new G4EmStandardPhysics );

  RegisterPhysics( new G4HadronPhysicsFTFP_BERT );
  RegisterPhysics( new G4HadronElasticPhysics );
  G4cout << "Finished Physics" << G4endl;
}

G4SimPhysicsList::~G4SimPhysicsList(){}

void G4SimPhysicsList::SetCuts(){
  G4cout << "Set Cuts" << G4endl; 
//  G4VUserPhysicsList::SetCuts();
}



