#ifndef G4SimPrimaryGeneratorAction_h
#define G4SimPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"


class G4ParticleGun;
class G4GeneralParticleSource;
class G4Event;

class G4SimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction{
  
  public:
    G4SimPrimaryGeneratorAction(
      const G4String & particleName = "pi+",
      G4double energy = 100.*MeV,
      G4ThreeVector momentumDirection = G4ThreeVector(0,0,1));

   ~G4SimPrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event *);

    G4ThreeVector position;
  private:
    G4ParticleGun * fParticleGun;
    G4GeneralParticleSource * fGeneralParticleSource;
};

#endif
