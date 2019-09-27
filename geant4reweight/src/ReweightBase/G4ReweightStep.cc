#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"

#include <utility>
#include <iostream>

G4ReweightStep::G4ReweightStep(){}

G4ReweightStep::G4ReweightStep(int tid, int pdg, int parid, int eventnum,
                               double preStepP[3], double postStepP[3], double len, std::string chosenProc){
  trackID = tid;
  PDG = pdg;
  parID = parid; 
  eventNum = eventnum; 
  stepLength = len;

  preStepPx = preStepP[0];
  preStepPy = preStepP[1];
  preStepPz = preStepP[2];
  postStepPx = postStepP[0];
  postStepPy = postStepP[1];
  postStepPz = postStepP[2];

  stepChosenProc = chosenProc;
}

G4ReweightStep::~G4ReweightStep(){}


