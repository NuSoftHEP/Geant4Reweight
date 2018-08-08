#include "G4SimActionInitialization.hh"
#include "G4SimPrimaryGeneratorAction.hh"
#include "G4SimTrackingAction.hh"
#include "G4SimSteppingAction.hh"
#include "G4SimEventAction.hh"
#include "TROOT.h"

G4SimActionInitialization::G4SimActionInitialization(std::string fOutFileName) : G4VUserActionInitialization(){

  MyTreeBuffer = new TreeBuffer();
  MyStepTreeBuffer = new StepTreeBuffer();
  MyTrackTreeBuffer = new TrackTreeBuffer();
 
  G4cout << "Making trees" << G4endl;
 
  fout = new TFile(fOutFileName.c_str(), "RECREATE");
  tree = new TTree("tree","");
  step = new TTree("step","");
  track = new TTree("track","");
  G4cout << "Done Making trees" << G4endl;


  //Tree Branches
  G4cout << "Making Branches" << G4endl;
  tree->Branch("pid", &MyTreeBuffer->pid);
  tree->Branch("tid", &MyTreeBuffer->tid);
  tree->Branch("track_pid", &MyTreeBuffer->track_pid);
  tree->Branch("track_tid", &MyTreeBuffer->track_tid);
  tree->Branch("track_creator", &MyTreeBuffer->track_creator);
  tree->Branch("secondaryProductIDs",  &MyTreeBuffer->secondaryProductIDs);
  tree->Branch("secondaryProductPIDs", &MyTreeBuffer->secondaryProductPIDs);
  tree->Branch("secondaryProcess",     &MyTreeBuffer->secondaryProcess);
  tree->Branch("interactionMode",      &MyTreeBuffer->interactionMode);
  tree->Branch("interactionModeName",      &MyTreeBuffer->interactionModeName);
  tree->Branch("nPi0",     &MyTreeBuffer->nPi0);    
  tree->Branch("nPiPlus",  &MyTreeBuffer->nPiPlus);
  tree->Branch("nPiMinus", &MyTreeBuffer->nPiMinus);
  tree->Branch("nNuclear", &MyTreeBuffer->nNuclear);
  tree->Branch("nProton",  &MyTreeBuffer->nProton);
  tree->Branch("nNeutron", &MyTreeBuffer->nNeutron);
  tree->Branch("nGamma",   &MyTreeBuffer->nGamma);
  tree->Branch("fEvent", &MyTreeBuffer->fEvent);
  tree->Branch("xi", &MyTreeBuffer->xi);
  tree->Branch("yi", &MyTreeBuffer->yi);
  tree->Branch("zi", &MyTreeBuffer->zi);
  tree->Branch("primaryPDGs", &MyTreeBuffer->primaryPDGs);
  tree->Branch("primaryEnergy", &MyTreeBuffer->primaryEnergy);
  tree->Branch("parid", &MyTreeBuffer->parid);
  tree->Branch("ekin", &MyTreeBuffer->ekin);
  tree->Branch("edep", &MyTreeBuffer->edep);
  tree->Branch("xs", &MyTreeBuffer->xs);
  tree->Branch("ys", &MyTreeBuffer->ys);
  tree->Branch("zs", &MyTreeBuffer->zs);
  tree->Branch("postProcess", &MyTreeBuffer->postStepProcess);
  tree->Branch("preProcess", &MyTreeBuffer->preStepProcess);
  tree->Branch("preMat", &MyTreeBuffer->preStepMat);
  tree->Branch("postMat", &MyTreeBuffer->postStepMat);
  G4cout << "Made tree Branches" << G4endl;
  /////////////////////
  
  //Step Tree Branches
  step->Branch("trackID", &MyStepTreeBuffer->trackID);
  step->Branch("eventNum", &MyStepTreeBuffer->eventNum);
  step->Branch("PID", &MyStepTreeBuffer->PID);
  step->Branch("parID", &MyStepTreeBuffer->parID);
  step->Branch("stepChosenProc", &MyStepTreeBuffer->stepChosenProc);
  step->Branch("stepActivePostProcNames", &MyStepTreeBuffer->stepActivePostProcNames);
  step->Branch("stepActiveAlongProcNames", &MyStepTreeBuffer->stepActiveAlongProcNames);
  step->Branch("stepActivePostProcMFPs", &MyStepTreeBuffer->stepActivePostProcMFPs);
  step->Branch("stepActiveAlongProcMFPs", &MyStepTreeBuffer->stepActiveAlongProcMFPs);
  step->Branch("stepActivePostProcLens", &MyStepTreeBuffer->stepActivePostProcLens);
  step->Branch("stepActiveAlongProcLens", &MyStepTreeBuffer->stepActiveAlongProcLens);
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
  G4cout << "Made track Branches" << G4endl;
  /////////////////////
}

G4SimActionInitialization::~G4SimActionInitialization(){
  tree->Write();
  step->Write();
  track->Write();
  fout->Close();
}

void G4SimActionInitialization::Build() const{
  
  SetUserAction(new G4SimPrimaryGeneratorAction());
  //Pass trees and branches to these
  G4cout << "Passing tree at " << tree << G4endl;
  SetUserAction(new G4SimEventAction(tree, MyTreeBuffer, MyStepTreeBuffer, MyTrackTreeBuffer));//Will have to fill tree in this
  SetUserAction(new G4SimSteppingAction(MyTreeBuffer, MyStepTreeBuffer, step));
  SetUserAction(new G4SimTrackingAction(MyTreeBuffer, MyStepTreeBuffer, MyTrackTreeBuffer, track));
}
