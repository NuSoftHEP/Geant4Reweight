#include "G4SimEventAction.hh"

G4SimEventAction::G4SimEventAction(StepTreeBuffer * inputStepTreeBuffer, TrackTreeBuffer * inputTrackTreeBuffer) : G4UserEventAction(){

  //Pass the pointers to this class;
  MyStepTreeBuffer = inputStepTreeBuffer;
  MyTrackTreeBuffer = inputTrackTreeBuffer;
  
}

G4SimEventAction::~G4SimEventAction(){ 
}

void G4SimEventAction::BeginOfEventAction(const G4Event * event){
  MyStepTreeBuffer->eventNum  = event->GetEventID();
  MyTrackTreeBuffer->eventNum = event->GetEventID();
}

void G4SimEventAction::EndOfEventAction(const G4Event * event){

  G4cout << "Event: " << event->GetEventID() << G4endl;
}
