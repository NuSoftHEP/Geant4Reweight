#include "G4SimPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"




G4SimPrimaryGeneratorAction::G4SimPrimaryGeneratorAction(
 const G4String & particleName, G4double energy,
  G4ThreeVector momentumDirection ) : G4VUserPrimaryGeneratorAction(){
  
  G4int nParticles = 1;
  fGeneralParticleSource = new G4GeneralParticleSource();

}

G4SimPrimaryGeneratorAction::~G4SimPrimaryGeneratorAction(){
  delete fGeneralParticleSource;
}

void G4SimPrimaryGeneratorAction::GeneratePrimaries(G4Event * thisEvent){
  fGeneralParticleSource->GeneratePrimaryVertex(thisEvent);
}
