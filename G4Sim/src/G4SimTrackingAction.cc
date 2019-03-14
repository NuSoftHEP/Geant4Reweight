#include "G4SimTrackingAction.hh"
#include "G4UImanager.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "G4SystemOfUnits.hh"

G4SimTrackingAction::G4SimTrackingAction(StepTreeBuffer * inputStepTreeBuffer, TrackTreeBuffer * inputTrackTreeBuffer, TTree * track) : G4UserTrackingAction(){

  MyStepTreeBuffer = inputStepTreeBuffer;
  MyTrackTreeBuffer = inputTrackTreeBuffer; 

  G4cout << "Initializing tracking action" << G4endl;
  track_tree_copy = track;
}

G4SimTrackingAction::~G4SimTrackingAction(){
}

void G4SimTrackingAction::PreUserTrackingAction(const G4Track * track){
  int parID = track->GetParentID();
  int trackID = track->GetTrackID();
  int PID = track->GetDefinition()->GetPDGEncoding();
  MyTrackTreeBuffer->PID = PID;

  if((abs(PID) >= 1000000000) || (abs(PID) == 11) || (abs(PID) == 22) ){return;}


  MyStepTreeBuffer->parID = parID; 
  MyStepTreeBuffer->trackID = trackID;
  MyStepTreeBuffer->PID = PID;
   
  MyTrackTreeBuffer->trackID = trackID;
  MyTrackTreeBuffer->parID = parID;

  //if(parID == 0){
  //Now saving steps for any pions
  if( (abs(PID) == 211) || (PID == 111) ){
    //nsteps will be iterated in Stepping Action
    MyTrackTreeBuffer->steps->first = MyStepTreeBuffer->nsteps; 
  }
  else{
    MyTrackTreeBuffer->steps->first = -1;
  }

  MyTrackTreeBuffer->xi = track->GetPosition()[0] / cm;
  MyTrackTreeBuffer->yi = track->GetPosition()[1] / cm;
  MyTrackTreeBuffer->zi = track->GetPosition()[2] / cm;
  MyTrackTreeBuffer->Energy = track->GetTotalEnergy() / MeV;
}

void G4SimTrackingAction::PostUserTrackingAction(const G4Track * track){  

  if( (abs(MyTrackTreeBuffer->PID) >= 1000000000) || (abs(MyTrackTreeBuffer->PID) == 11) || (abs(MyTrackTreeBuffer->PID) == 22) ){return;}

  if( (abs(MyTrackTreeBuffer->PID) == 211) || (MyTrackTreeBuffer->PID == 111) ){
    MyTrackTreeBuffer->steps->second = MyStepTreeBuffer->nsteps;
  }
  else{
    MyTrackTreeBuffer->steps->second = -1;
  }
  track_tree_copy->Fill();

}
