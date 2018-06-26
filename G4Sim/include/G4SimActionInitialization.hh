#ifndef G4SimActionInitialization_h
#define G4SimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "TreeBuffer.hh"
#include "StepTreeBuffer.hh"
#include "TrackTreeBuffer.hh"

#include "TTree.h"
#include "TFile.h"



class G4SimActionInitialization : public G4VUserActionInitialization{

  public:
    G4SimActionInitialization();
    virtual ~G4SimActionInitialization();
    
    void Build() const;

    TFile * fout;
    TTree * tree;
    TTree * step;
    TTree * track;

    TreeBuffer * MyTreeBuffer;
    StepTreeBuffer * MyStepTreeBuffer;
    TrackTreeBuffer * MyTrackTreeBuffer;
};
#endif
