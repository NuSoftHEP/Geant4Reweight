#ifndef G4CascadePhysicsList_h
#define G4CascadePhysicsList_h 1

#include "Geant4/G4VUserPhysicsList.hh"
#include "Geant4/G4VModularPhysicsList.hh"
#include "fhiclcpp/ParameterSet.h"

class G4CascadePhysicsList : public G4VModularPhysicsList
{

  public:
    G4CascadePhysicsList(int list = 0);
    G4CascadePhysicsList(fhicl::ParameterSet pars);
    virtual ~G4CascadePhysicsList();

  protected:
    virtual void SetCuts(); 
 
};
#endif
