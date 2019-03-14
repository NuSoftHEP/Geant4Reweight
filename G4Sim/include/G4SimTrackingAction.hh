#ifndef G4SimTrackingAction_h
#define G4SimTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "StepTreeBuffer.hh"
#include "TrackTreeBuffer.hh"
#include "TTree.h"

#include <iostream>
#include <fstream>
#include <vector>

class G4SimTrackingAction : public G4UserTrackingAction{

  public:
    G4SimTrackingAction( StepTreeBuffer *, TrackTreeBuffer *, TTree*);
    virtual ~G4SimTrackingAction();

    virtual void PreUserTrackingAction(const G4Track *);  
    virtual void PostUserTrackingAction(const G4Track *);  
  private:
    StepTreeBuffer * MyStepTreeBuffer;
    TrackTreeBuffer * MyTrackTreeBuffer;

    TTree * track_tree_copy;

};

#endif
