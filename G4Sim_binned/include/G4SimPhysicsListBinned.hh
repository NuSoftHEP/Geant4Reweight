#ifndef G4SimPhysicsListBinned_h
#define G4SimPhysicsListBinned_h 1

#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"

#include "G4ReweightHist.hh"

class G4SimPhysicsListBinned : public G4VModularPhysicsList
{

  public:
    G4SimPhysicsListBinned(G4ReweightHist * inelasticBias, G4ReweightHist * elasticBias);
    virtual ~G4SimPhysicsListBinned();

  protected:
    virtual void SetCuts(); 
 
};
#endif
