#ifndef G4SimActionInitialization_h
#define G4SimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "StepTreeBuffer.hh"
#include "TrackTreeBuffer.hh"

#include "TTree.h"
#include "TFile.h"
#include <string>


class G4SimActionInitialization : public G4VUserActionInitialization{

  public:
    G4SimActionInitialization(std::string);
    virtual ~G4SimActionInitialization();
    
    void Build() const;

    TFile * fout;
    TTree * step;
    TTree * track;

    StepTreeBuffer * MyStepTreeBuffer;
    TrackTreeBuffer * MyTrackTreeBuffer;
};
#endif
