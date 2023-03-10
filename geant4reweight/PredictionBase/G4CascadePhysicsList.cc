#include "geant4reweight/PredictionBase/G4CascadePhysicsList.hh"

#include "Geant4/G4DecayPhysics.hh"
#include "Geant4/G4RadioactiveDecayPhysics.hh"
#include "Geant4/G4EmStandardPhysics.hh"
#include "Geant4/G4HadronPhysicsFTFP_BERT.hh"
#include "Geant4/G4HadronPhysicsQGSP_BERT.hh"
#include "Geant4/G4HadronPhysicsQGSP_BERT_HP.hh"
#include "Geant4/G4HadronElasticPhysics.hh"
#include "Geant4/G4HadronElasticPhysicsHP.hh"

G4CascadePhysicsList::G4CascadePhysicsList(int list) : G4VModularPhysicsList(){
  SetVerboseLevel(0);

  RegisterPhysics( new G4DecayPhysics );
  RegisterPhysics( new G4RadioactiveDecayPhysics );
  RegisterPhysics( new G4EmStandardPhysics );

  switch (list) {
    case 0: {
      RegisterPhysics(new G4HadronPhysicsQGSP_BERT());
      RegisterPhysics( new G4HadronElasticPhysics() );
      break;
    }
    case 1: {
      RegisterPhysics(new G4HadronPhysicsFTFP_BERT());
      RegisterPhysics( new G4HadronElasticPhysics() );
      break;
    }
    case 2: {
      RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());
      RegisterPhysics( new G4HadronElasticPhysicsHP() );
      break;
    }
    default: {
      RegisterPhysics(new G4HadronPhysicsQGSP_BERT());
      RegisterPhysics( new G4HadronElasticPhysics() );
      break;
    }
  }
}

G4CascadePhysicsList::~G4CascadePhysicsList(){}

void G4CascadePhysicsList::SetCuts(){
  std::cout << "Set Cuts" << std::endl; 
  G4VUserPhysicsList::SetCuts();
}



