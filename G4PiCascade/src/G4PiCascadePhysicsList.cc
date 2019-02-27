#include "G4PiCascadePhysicsList.hh"

#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
//#include "G4HadronPhysicsFTFP_BERT_binned.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronElasticPhysics.hh"
//#include "G4HadronElasticPhysics_binned.hh"


G4PiCascadePhysicsList::G4PiCascadePhysicsList() : G4VModularPhysicsList(){
  SetVerboseLevel(5);

  RegisterPhysics( new G4DecayPhysics );
  RegisterPhysics( new G4RadioactiveDecayPhysics );
  RegisterPhysics( new G4EmStandardPhysics );

  RegisterPhysics( new G4HadronPhysicsFTFP_BERT() );
  RegisterPhysics( new G4HadronElasticPhysics() );
  G4cout << "Finished Physics" << G4endl;
}

G4PiCascadePhysicsList::~G4PiCascadePhysicsList(){}

void G4PiCascadePhysicsList::SetCuts(){
  G4cout << "Set Cuts" << G4endl; 
//  G4VUserPhysicsList::SetCuts();
}



