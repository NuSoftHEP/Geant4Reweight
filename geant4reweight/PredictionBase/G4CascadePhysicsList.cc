#include "geant4reweight/PredictionBase/G4CascadePhysicsList.hh"

#include "Geant4/G4DecayPhysics.hh"
#include "Geant4/G4RadioactiveDecayPhysics.hh"
#include "Geant4/G4EmStandardPhysics.hh"
#include "Geant4/G4HadronPhysicsFTFP_BERT.hh"
#include "Geant4/G4HadronPhysicsQGSP_BERT.hh"
#include "Geant4/G4HadronPhysicsQGSP_BERT_HP.hh"
#include "Geant4/G4HadronElasticPhysics.hh"
#include "Geant4/G4HadronElasticPhysicsHP.hh"

#include "Geant4/G4PhysicsConstructorRegistry.hh"

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

G4CascadePhysicsList::G4CascadePhysicsList(fhicl::ParameterSet pars)
    : G4VModularPhysicsList() {
  SetVerboseLevel(0);

  RegisterPhysics(new G4DecayPhysics);
  RegisterPhysics(new G4RadioactiveDecayPhysics);

  auto em_phys = pars.get<std::string>(
      "EMPhysics", "G4EmStandardPhysics");
  RegisterPhysics(
      G4PhysicsConstructorRegistry::Instance()->GetPhysicsConstructor(
          em_phys));

  auto inelastic_phys = pars.get<std::string>(
      "InelasticPhysics", "G4HadronPhysicsQGSP_BERT");
  RegisterPhysics(
      G4PhysicsConstructorRegistry::Instance()->GetPhysicsConstructor(
          inelastic_phys));

  auto elastic_phys = pars.get<std::string>(
      "ElasticPhysics", "G4HadronElasticPhysics");
  RegisterPhysics(
      G4PhysicsConstructorRegistry::Instance()->GetPhysicsConstructor(
          elastic_phys));

}

G4CascadePhysicsList::~G4CascadePhysicsList(){}

void G4CascadePhysicsList::SetCuts(){
  std::cout << "Set Cuts" << std::endl; 
  G4VUserPhysicsList::SetCuts();
}



