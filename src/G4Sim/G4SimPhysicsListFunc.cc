#include "G4SimPhysicsListFunc.hh"

#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsFTFP_BERT_func.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysics_func.hh"


G4SimPhysicsListFunc::G4SimPhysicsListFunc(G4ReweightInter * inelasticBias, G4ReweightInter * elasticBias) : G4VModularPhysicsList(){
  SetVerboseLevel(5);

  RegisterPhysics( new G4DecayPhysics );
  RegisterPhysics( new G4RadioactiveDecayPhysics );
  RegisterPhysics( new G4EmStandardPhysics );
  RegisterPhysics( new G4HadronPhysicsFTFP_BERT_func(1,inelasticBias) );
  RegisterPhysics( new G4HadronElasticPhysics_func(1,elasticBias) );
  G4cout << "Finished Physics" << G4endl;
}

G4SimPhysicsListFunc::~G4SimPhysicsListFunc(){}

void G4SimPhysicsListFunc::SetCuts(){
  G4cout << "Set Cuts" << G4endl; 
//  G4VUserPhysicsList::SetCuts();
}



