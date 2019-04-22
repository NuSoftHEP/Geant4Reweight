#ifndef G4SimPhysicsListFunc_h
#define G4SimPhysicsListFunc_h 1

#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"

#include "G4ReweightInter.hh"

class G4SimPhysicsListFunc : public G4VModularPhysicsList
{

  public:
    G4SimPhysicsListFunc(G4ReweightInter * inelasticBias, G4ReweightInter * elasticBias);
    virtual ~G4SimPhysicsListFunc();

  protected:
    virtual void SetCuts(); 
 
};
#endif
