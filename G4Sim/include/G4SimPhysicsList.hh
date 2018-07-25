#ifndef G4SimPhysicsList_h
#define G4SimPhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"

class G4SimPhysicsList : public G4VModularPhysicsList
{

  public:
    G4SimPhysicsList(G4double inelasticBias=1., G4double elasticBias=1.);
    virtual ~G4SimPhysicsList();

  protected:
    virtual void SetCuts(); 
 
};
#endif
