#ifndef G4PiCascadePhysicsList_h
#define G4PiCascadePhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"

class G4PiCascadePhysicsList : public G4VModularPhysicsList
{

  public:
    G4PiCascadePhysicsList();
    virtual ~G4PiCascadePhysicsList();

  protected:
    virtual void SetCuts(); 
 
};
#endif
