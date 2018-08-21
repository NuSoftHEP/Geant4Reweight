#ifndef G4SimSteppingAction_h
#define G4SimSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "TreeBuffer.hh"
#include "StepTreeBuffer.hh"
#include "TTree.h"

#include <iostream>
#include <fstream>
#include <vector>

class G4SimSteppingAction : public G4UserSteppingAction{

  public:
    G4SimSteppingAction(/*TreeBuffer *,*/ StepTreeBuffer *, TTree *);
    virtual ~G4SimSteppingAction();

    virtual void UserSteppingAction(const G4Step*);  

  private:
//    TreeBuffer * MyTreeBuffer;
    StepTreeBuffer * MyStepTreeBuffer;
    TTree * step_tree_copy;


};

#endif
