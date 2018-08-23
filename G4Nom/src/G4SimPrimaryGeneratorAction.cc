#include "G4SimPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"




G4SimPrimaryGeneratorAction::G4SimPrimaryGeneratorAction(
 const G4String & particleName, G4double energy,
 /*G4ThreeVector position,*/ G4ThreeVector momentumDirection ) : G4VUserPrimaryGeneratorAction(){
  
  G4int nParticles = 1;
//fParticleGun = new G4ParticleGun(nParticles);
  fGeneralParticleSource = new G4GeneralParticleSource();

  //default particle kinematics 
/*  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle
  = particleTable->FindParticle(particleName);
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleEnergy(energy);


//  rand = new TRandom3(floor(double(std::chrono::system_clock::now())));
  double rand_radius = rand->Uniform(.005*cm);
  double rand_phi = rand->Uniform(2*M_PI);

  double pos_x = rand_radius*cos(rand_phi);
  double pos_y = rand_radius*sin(rand_phi);
//  double pos_x = 0, pos_y = 0;
  position = G4ThreeVector(pos_x,pos_y,-1*cm);
 
  fParticleGun->SetParticlePosition(position);
  fParticleGun->SetParticleMomentumDirection(momentumDirection);*/
  
}

G4SimPrimaryGeneratorAction::~G4SimPrimaryGeneratorAction(){
  //delete fParticleGun;
  delete fGeneralParticleSource;
}

void G4SimPrimaryGeneratorAction::GeneratePrimaries(G4Event * thisEvent){
//  fParticleGun->GeneratePrimaryVertex(thisEvent);
  fGeneralParticleSource->GeneratePrimaryVertex(thisEvent);
}
