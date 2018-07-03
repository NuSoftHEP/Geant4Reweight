#include "G4SimSteppingAction.hh"
#include "G4UImanager.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

#include "G4String.hh"
#include "G4VProcess.hh"
#include <math.h>

G4SimSteppingAction::G4SimSteppingAction(TreeBuffer * inputTreeBuffer, StepTreeBuffer * inputStepTreeBuffer, TTree * step) : G4UserSteppingAction(){

  MyTreeBuffer = inputTreeBuffer;
  MyStepTreeBuffer = inputStepTreeBuffer;

  G4cout << "Initializing stepping action" << G4endl;
  
  step_tree_copy = step;
}

G4SimSteppingAction::~G4SimSteppingAction(){
}

void G4SimSteppingAction::UserSteppingAction(const G4Step * step){

  G4Track * track = step->GetTrack();
  if(abs(track->GetDefinition()->GetPDGEncoding()) == 11){return;}

  auto prestep = step->GetPreStepPoint(); 
  auto poststep = step->GetPostStepPoint(); 

  MyTreeBuffer->tid->push_back(track->GetTrackID());
  MyTreeBuffer->pid->push_back(track->GetDefinition()->GetPDGEncoding());
  MyTreeBuffer->parid->push_back(track->GetParentID());
  MyTreeBuffer->ekin->push_back(prestep->GetKineticEnergy() / GeV);
  MyTreeBuffer->edep->push_back(step->GetTotalEnergyDeposit() / GeV); 

  
  MyTreeBuffer->xs->push_back(prestep->GetPosition().getX() / cm);
  MyTreeBuffer->ys->push_back(prestep->GetPosition().getY() / cm);
  MyTreeBuffer->zs->push_back(prestep->GetPosition().getZ() / cm);

  //Prestep Process
  auto prePro = prestep->GetProcessDefinedStep();
  if(prePro){
    auto preProName = prePro->GetProcessName(); 
    MyTreeBuffer->preStepProcess->push_back(preProName); 
  }
  else{
    MyTreeBuffer->preStepProcess->push_back("NULL"); 
  }



  //BEGIN STEP TREE FILLING


  //PostStep Process
  auto postPro = poststep->GetProcessDefinedStep(); 
  auto postProName = postPro->GetProcessName();
  MyTreeBuffer->postStepProcess->push_back(postProName); 
  MyStepTreeBuffer->stepChosenProc = new std::string(postProName);
  ///

  int nPostProcs = step->postStepProcNames->size();
//  G4cout << "PostStep Procs" << G4endl;
//  G4cout << std::setw(15) << "Name" <<std::setw(15) << "MFP"  <<std::setw(15) << "IntLen" <<G4endl;

  MyStepTreeBuffer->stepActivePostProcNames->clear();
  MyStepTreeBuffer->stepActivePostProcMFPs->clear();
  MyStepTreeBuffer->stepActivePostProcLens->clear();
  for(int ip = 0; ip < nPostProcs; ++ip){
/*    G4cout << std::setw(15) << step->postStepProcNames->at(ip)  
              << std::setw(15) << step->postStepProcMFPs->at(ip) 
              << std::setw(15) << step->postStepProcIntLens->at(ip) << G4endl;
*/    
    MyStepTreeBuffer->stepActivePostProcNames->push_back(step->postStepProcNames->at(ip));
    MyStepTreeBuffer->stepActivePostProcMFPs->push_back(step->postStepProcMFPs->at(ip));
    MyStepTreeBuffer->stepActivePostProcLens->push_back(step->postStepProcIntLens->at(ip));
  }

  //MyTreeBuffer->postProcNameToMFP->push_back(tempProcNameToMFP);



  int nAlongProcs = step->alongStepProcNames->size();
//  G4cout << "AlongStep Procs" << G4endl;

  MyStepTreeBuffer->stepActiveAlongProcNames->clear();
  MyStepTreeBuffer->stepActiveAlongProcMFPs->clear();
  MyStepTreeBuffer->stepActiveAlongProcLens->clear();
  for(int ip = 0; ip < nAlongProcs; ++ip){
/*    G4cout << std::setw(15) << step->alongStepProcNames->at(ip)  
              << std::setw(15) << step->alongStepProcMFPs->at(ip) 
              << std::setw(15) << step->alongStepProcIntLens->at(ip) << G4endl;
*/
    MyStepTreeBuffer->stepActiveAlongProcNames->push_back(step->alongStepProcNames->at(ip));
    MyStepTreeBuffer->stepActiveAlongProcMFPs->push_back(step->alongStepProcMFPs->at(ip));
    MyStepTreeBuffer->stepActiveAlongProcLens->push_back(step->alongStepProcIntLens->at(ip));
  }


  MyStepTreeBuffer->preStepPx = prestep->GetMomentum()[0];
  MyStepTreeBuffer->preStepPy = prestep->GetMomentum()[1];
  MyStepTreeBuffer->preStepPz = prestep->GetMomentum()[2];
  MyStepTreeBuffer->postStepPx = poststep->GetMomentum()[0];
  MyStepTreeBuffer->postStepPy = poststep->GetMomentum()[1];
  MyStepTreeBuffer->postStepPz = poststep->GetMomentum()[2];

  double xi,yi,zi;
  double xf,yf,zf;
  xi = (prestep->GetPosition().getX() / cm);
  xf = (poststep->GetPosition().getX() / cm);
  yi = (prestep->GetPosition().getY() / cm);
  yf = (poststep->GetPosition().getY() / cm);
  zi = (prestep->GetPosition().getZ() / cm);
  zf = (poststep->GetPosition().getZ() / cm);

  MyStepTreeBuffer->stepLen = sqrt( pow((xi - xf),2) + pow((yi - yf),2) + pow((zi - zf),2) );

  step_tree_copy->Fill();
  MyStepTreeBuffer->nsteps++;
}
