#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"

#include <utility>
#include <iostream>

G4ReweightStep::G4ReweightStep(){}

G4ReweightStep::G4ReweightStep(int tid, int pid, int parid, int eventnum,
                               double preStepP[3], double postStepP[3], double len, std::string chosenProc){
  trackID = tid;
  PID = pid;
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

G4ReweightStep::~G4ReweightStep(){
  stepActivePostProcs.clear();
  stepActiveAlongProcs.clear();
}

void G4ReweightStep::AddActivePostProc(std::pair< std::string, double > proc){  
  Proc temp;
  temp.Name = proc.first;
  temp.MFP = proc.second;
  stepActivePostProcs.push_back(temp);
}

void G4ReweightStep::AddActiveAlongProc(std::pair< std::string, double > proc){
  Proc temp;
  temp.Name = proc.first;
  temp.MFP = proc.second;
  stepActivePostProcs.push_back(temp);
}

void G4ReweightStep::AddActivePostProc(Proc theProc){  
  stepActivePostProcs.push_back(theProc);
}

void G4ReweightStep::AddActiveAlongProc(Proc theProc){
  stepActiveAlongProcs.push_back(theProc);
}

size_t G4ReweightStep::GetNActivePostProcs(){
  return stepActivePostProcs.size(); 
}

size_t G4ReweightStep::GetNActiveAlongProcs(){
  return stepActiveAlongProcs.size(); 
}

Proc G4ReweightStep::GetActivePostProc(size_t ip){
  size_t NProcs = GetNActivePostProcs();

  if( NProcs == 0 ){
    std::cout << "No associated post-step processes" << std::endl;
    Proc temp; 
    temp.Name = "";
    temp.MFP = 0.;
    return temp;
  }
  else if (ip > NProcs - 1){
    std::cout << "Requested Process Out of Range" << std::endl <<
    "Please Provide index in Range [0, " << NProcs - 1 << "]" << std::endl;
    Proc temp; 
    temp.Name = "";
    temp.MFP = 0.;
    return temp;
  }
  else{
    return stepActivePostProcs.at(ip); 
  }
}

Proc G4ReweightStep::GetActiveAlongProc(size_t ip){
  size_t NProcs = GetNActiveAlongProcs();

  if( NProcs == 0 ){
    std::cout << "No associated along-step processes" << std::endl;
    Proc temp; 
    temp.Name = "";
    temp.MFP = 0.;
    return temp;
  }
  else if (ip > NProcs - 1){
    std::cout << "Requested Process Out of Range" << std::endl <<
    "Please Provide index in Range [0, " << NProcs - 1 << "]" << std::endl;
    Proc temp; 
    temp.Name = "";
    temp.MFP = 0.;
    return temp;
  }
  else{
    return stepActiveAlongProcs.at(ip); 
  }
}
