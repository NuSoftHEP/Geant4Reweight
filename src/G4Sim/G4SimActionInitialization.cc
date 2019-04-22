#include "G4SimActionInitialization.hh"
#include "G4SimPrimaryGeneratorAction.hh"
#include "G4SimTrackingAction.hh"
#include "G4SimSteppingAction.hh"
#include "G4SimEventAction.hh"
#include "TROOT.h"

G4SimActionInitialization::G4SimActionInitialization(std::string fOutFileName) : G4VUserActionInitialization(){

  MyStepTreeBuffer = new StepTreeBuffer();
  MyTrackTreeBuffer = new TrackTreeBuffer();
 
  G4cout << "Making trees" << G4endl;
 
  fout = new TFile(fOutFileName.c_str(), "RECREATE");
  step = new TTree("step","");
  track = new TTree("track","");
  G4cout << "Done Making trees" << G4endl;


  //Tree Branches
  G4cout << "Making Branches" << G4endl;
  //Step Tree Branches
  step->Branch("trackID", &MyStepTreeBuffer->trackID);
  step->Branch("eventNum", &MyStepTreeBuffer->eventNum);
  step->Branch("PID", &MyStepTreeBuffer->PID);
  step->Branch("parID", &MyStepTreeBuffer->parID);
  step->Branch("stepChosenProc", &MyStepTreeBuffer->stepChosenProc);
  step->Branch("stepActivePostProcNames", &MyStepTreeBuffer->stepActivePostProcNames);
  step->Branch("stepActivePostProcMFPs", &MyStepTreeBuffer->stepActivePostProcMFPs);
  step->Branch("stepActivePostProcLens", &MyStepTreeBuffer->stepActivePostProcLens);
  step->Branch("preStepPx", &MyStepTreeBuffer->preStepPx);
  step->Branch("preStepPy", &MyStepTreeBuffer->preStepPy);
  step->Branch("preStepPz", &MyStepTreeBuffer->preStepPz);
  step->Branch("postStepPx", &MyStepTreeBuffer->postStepPx);
  step->Branch("postStepPy", &MyStepTreeBuffer->postStepPy);
  step->Branch("postStepPz", &MyStepTreeBuffer->postStepPz);
  step->Branch("stepLen", &MyStepTreeBuffer->stepLen);
  step->Branch("deltaX", &MyStepTreeBuffer->dX);
  step->Branch("deltaY", &MyStepTreeBuffer->dY);
  step->Branch("deltaZ", &MyStepTreeBuffer->dZ);
  step->Branch("deltaE", &MyStepTreeBuffer->deltaE);
  G4cout << "Made step Branches" << G4endl;
  /////////////////////

  //Track Tree Branches
  track->Branch("trackID", &MyTrackTreeBuffer->trackID);
  track->Branch("steps", &MyTrackTreeBuffer->steps);
  track->Branch("PID", &MyTrackTreeBuffer->PID);
  track->Branch("parID", &MyTrackTreeBuffer->parID);
  track->Branch("eventNum", &MyTrackTreeBuffer->eventNum);
  track->Branch("xi", &MyTrackTreeBuffer->xi);
  track->Branch("yi", &MyTrackTreeBuffer->yi);
  track->Branch("zi", &MyTrackTreeBuffer->zi);
  track->Branch("Energy", &MyTrackTreeBuffer->Energy);
  G4cout << "Made track Branches" << G4endl;
  /////////////////////
}

G4SimActionInitialization::~G4SimActionInitialization(){
  step->Write();
  track->Write();
  fout->Close();
}

void G4SimActionInitialization::Build() const{
  
  SetUserAction(new G4SimPrimaryGeneratorAction());
  SetUserAction(new G4SimEventAction( MyStepTreeBuffer, MyTrackTreeBuffer));
  SetUserAction(new G4SimSteppingAction( MyStepTreeBuffer, step));
  SetUserAction(new G4SimTrackingAction( MyStepTreeBuffer, MyTrackTreeBuffer, track));

}
