#include "G4ReweightTraj.hh"
#include "G4ReweightStep.hh"

#include <iostream>
#include <math.h>

G4ReweightTraj::G4ReweightTraj(int tid, int pid, int parid, int eventnum, std::pair<int,int> range){
  trackID = tid;
  PID = pid;
  parID = parid;
  eventNum = eventnum;
  stepRange = range;
}

G4ReweightTraj::~G4ReweightTraj(){
  children.clear();
  steps.clear();
}

void G4ReweightTraj::AddStep(G4ReweightStep * inputStep){
  steps.push_back(inputStep);
}

size_t G4ReweightTraj::GetNSteps(){
  return steps.size();
}

G4ReweightStep * G4ReweightTraj::GetStep(size_t is){
  size_t NSteps = GetNSteps();
  
  if ( NSteps == 0 ){
    std::cout << "No associated steps" << std::endl;
    return NULL;
  }
  else if (is > NSteps - 1) {
    std::cout << "Requested Step Out of Range" << std::endl <<
    "Please Provide index in Range [0, " << NSteps - 1 << "]" << std::endl;

    return NULL;
  }
  else{
    return steps.at(is); 
  }
}


bool G4ReweightTraj::SetParent(G4ReweightTraj * parTraj){
  
  //Check if the event nums match, parID matches trackID
  bool check = (  (eventNum == parTraj->eventNum) 
               && (parID == parTraj->trackID) );
  
//  std::cout << "Parent check: " << check << std::endl;
  bool check_2 = parTraj->AddChild(this);
  if(check && check_2){

//    std::cout << "Setting parent" << std::endl;
    //Set the pointer  
    parent = parTraj; 
  }

  return (check && check_2);
}

bool G4ReweightTraj::AddChild(G4ReweightTraj * childTraj){

  //Check if  the event nums match, and trackID matches child's parent
  bool check = (  (eventNum == childTraj->eventNum)
               && (trackID == childTraj->parID) );

//  std::cout << "Child check: " << check << std::endl;
  if(check){
    //Set the pointer
    children.push_back(childTraj);
//    std::cout << "Added child" << std::endl;
  }

  return check;

}

size_t G4ReweightTraj::GetNChilds(){
  return children.size();
}

G4ReweightTraj * G4ReweightTraj::GetChild(size_t ic){
  size_t NChilds = GetNChilds();
  if(NChilds == 0){
    std::cout << "Has no childs" << std::endl;
    return NULL;
  }
  else if(ic > NChilds - 1){
    std::cout << "Index out of range." << std::endl <<
    "Please Provide index in Range [0, " << NChilds - 1 << "]" << std::endl;
    return NULL;
  }
  else{
    return children.at(ic);
  }

}

std::string G4ReweightTraj::GetFinalProc(){
  return steps.back()->stepChosenProc;
}

double G4ReweightTraj::GetTotalLength(){
  double total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){
    total += steps.at(is)->stepLength;  
  }

  return total;
}

double G4ReweightTraj::GetWeight(double bias){
  double total = 0.;
  double bias_total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){
    auto theStep = GetStep(is);

    for(size_t ip = 0; ip < theStep->GetNActivePostProcs(); ++ip){
      auto theProc = theStep->GetActivePostProc(ip);

      if(theProc.Name == "pi+Inelastic"){
        total += (theStep->stepLength/theProc.MFP);
        bias_total += ( (theStep->stepLength*bias) / theProc.MFP);
      }

    }

  }
  
  std::cout << "Total: " << total << std::endl <<
  "Biased: " << bias_total << std::endl <<
  "Weight: " << exp(total - bias_total) << std::endl;
  
  return exp(total - bias_total);
}
