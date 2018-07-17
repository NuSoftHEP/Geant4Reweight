#ifndef G4SimPhysicsList_h
#define G4SimPhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"

class G4SimPhysicsList : public G4VModularPhysicsList
{

  public:
    G4SimPhysicsList();
    virtual ~G4SimPhysicsList();

  protected:
    virtual void SetCuts(); 
 
};
#endif
