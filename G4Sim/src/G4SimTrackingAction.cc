#include "G4SimTrackingAction.hh"
#include "G4UImanager.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4String.hh"

G4SimTrackingAction::G4SimTrackingAction(TreeBuffer * inputTreeBuffer, StepTreeBuffer * inputStepTreeBuffer, TrackTreeBuffer * inputTrackTreeBuffer, TTree * track) : G4UserTrackingAction(){

  MyTreeBuffer = inputTreeBuffer;
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


  MyTreeBuffer->track_tid->push_back(trackID);
  MyTreeBuffer->track_pid->push_back(PID);

  MyStepTreeBuffer->parID = parID; 
  MyStepTreeBuffer->trackID = trackID;
  MyStepTreeBuffer->PID = PID;
   
  MyTrackTreeBuffer->trackID = trackID;
  MyTrackTreeBuffer->parID = parID;


  //nsteps will be iterated in Stepping Action
  MyTrackTreeBuffer->steps->first = MyStepTreeBuffer->nsteps; 
  MyTrackTreeBuffer->xi = track->GetPosition()[0];
  MyTrackTreeBuffer->yi = track->GetPosition()[1];
  MyTrackTreeBuffer->zi = track->GetPosition()[2];
}

void G4SimTrackingAction::PostUserTrackingAction(const G4Track * track){  

  if( (abs(MyTrackTreeBuffer->PID) >= 1000000000) || (abs(MyTrackTreeBuffer->PID) == 11) || (abs(MyTrackTreeBuffer->PID) == 22) ){return;}

  if(track->GetCreatorProcess()){//Skips primary particles which have no creator process

    G4String processName = track->GetCreatorProcess()->GetProcessName();
    MyTreeBuffer->track_creator->push_back(processName); 
    
    if(track->GetParentID() == 1){//If the parent is primary
      if((processName != "hIoni") && (processName != "eIoni")){//Skip ionization
  	G4cout << "Non-ionization created track: " << processName << G4endl;        
        MyTreeBuffer->secondaryProductIDs->push_back(track->GetTrackID());
        MyTreeBuffer->secondaryProductPIDs->push_back(track->GetDefinition()->GetPDGEncoding());
        *(MyTreeBuffer->secondaryProcess) = processName;
      }

    }
  }
  
  MyTrackTreeBuffer->steps->second = MyStepTreeBuffer->nsteps;
  track_tree_copy->Fill();

}
