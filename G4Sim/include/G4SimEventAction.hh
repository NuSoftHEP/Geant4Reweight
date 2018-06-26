#ifndef G4SimEventAction_h
#define G4SimEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "TreeBuffer.hh"
#include "StepTreeBuffer.hh"
#include "TrackTreeBuffer.hh"

#include "TTree.h"
#include <vector>

class G4SimEventAction : public G4UserEventAction{

  public:
    G4SimEventAction(TTree *, TreeBuffer *, StepTreeBuffer *, TrackTreeBuffer *);
    virtual ~G4SimEventAction();

    virtual void BeginOfEventAction(const G4Event *);
    virtual void EndOfEventAction(const G4Event *);
  protected:
    G4EventManager * fpEventManager;

  private: 
    TTree * tree_copy;
    TreeBuffer * MyTreeBuffer;
    StepTreeBuffer * MyStepTreeBuffer;
    TrackTreeBuffer * MyTrackTreeBuffer;


};

#endif
