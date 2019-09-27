#include "G4SimSteppingAction.hh"
#include "G4UImanager.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

#include "G4String.hh"
#include "G4VProcess.hh"
#include <math.h>

G4SimSteppingAction::G4SimSteppingAction(StepTreeBuffer * inputStepTreeBuffer, TTree * step) : G4UserSteppingAction(){

  MyStepTreeBuffer = inputStepTreeBuffer;

  G4cout << "Initializing stepping action" << G4endl;
  
  step_tree_copy = step;
}

G4SimSteppingAction::~G4SimSteppingAction(){
}

void G4SimSteppingAction::UserSteppingAction(const G4Step * step){
 
  G4Track * track = step->GetTrack();
  
//  G4cout << track->GetMaterial()->GetName() << G4endl;
//  if( track->GetParentID() > 0){return;}

 // if( (abs(track->GetDefinition()->GetPDGEncoding()) >= 1000000000) || (abs(track->GetDefinition()->GetPDGEncoding()) == 11) || (abs(track->GetDefinition()->GetPDGEncoding()) == 22)){return;}

  if( !((abs(track->GetDefinition()->GetPDGEncoding()) == 211) || (track->GetDefinition()->GetPDGEncoding() == 111)) ){return;}

  auto prestep = step->GetPreStepPoint(); 
  auto poststep = step->GetPostStepPoint(); 

  auto prePro = prestep->GetProcessDefinedStep();

  //PostStep Process
  auto postPro = poststep->GetProcessDefinedStep(); 
  auto postProName = postPro->GetProcessName();
  MyStepTreeBuffer->stepChosenProc = std::string(postProName);
  ///

  MyStepTreeBuffer->preStepPx = prestep->GetMomentum()[0];
  MyStepTreeBuffer->preStepPy = prestep->GetMomentum()[1];
  MyStepTreeBuffer->preStepPz = prestep->GetMomentum()[2];
  MyStepTreeBuffer->postStepPx = poststep->GetMomentum()[0];
  MyStepTreeBuffer->postStepPy = poststep->GetMomentum()[1];
  MyStepTreeBuffer->postStepPz = poststep->GetMomentum()[2];

  MyStepTreeBuffer->stepLen = step->GetStepLength() / cm; 
  MyStepTreeBuffer->dX = step->GetDeltaPosition()[0] / cm;
  MyStepTreeBuffer->dY = step->GetDeltaPosition()[1] / cm;
  MyStepTreeBuffer->dZ = step->GetDeltaPosition()[2] / cm;
  MyStepTreeBuffer->deltaE = ( step->GetTotalEnergyDeposit() - step->GetNonIonizingEnergyDeposit() ) / MeV;
  step_tree_copy->Fill();
  MyStepTreeBuffer->nsteps++;
}
