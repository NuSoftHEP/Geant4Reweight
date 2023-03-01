#include "geant4reweight/ReweightBase/G4ReweightTraj.hh"
#include "geant4reweight/ReweightBase/G4ReweightStep.hh"

#include <iostream>
#include <math.h>
#include <algorithm>

G4ReweightTraj::G4ReweightTraj(int tid, int pdg, int parid, int eventnum, std::pair<int,int> range) :
trackID(tid), eventNum(eventnum), PDG(pdg), parID(parid), stepRange(range){
  if( PDG == 211 ){ 
    fInelastic = "pi+Inelastic";
  }
  else if( PDG == -211 ){
    fInelastic = "pi-Inelastic";  
  }
  else if( PDG == 2212 ){
    fInelastic = "protonInelastic";
  }
  else if(PDG == 2112){
    fInelastic = "neutronInelastic";
  }

}

G4ReweightTraj::~G4ReweightTraj(){
  children.clear();

//  std::for_each(steps.begin(), steps.end(), DeleteVector<G4ReweightStep*>());  
  for( size_t i = 0; i < steps.size(); ++i ){
    delete steps.at(i);
  }
  steps.clear();
}

void G4ReweightTraj::AddStep(G4ReweightStep * inputStep){
  steps.push_back(inputStep);
}

size_t G4ReweightTraj::GetNSteps() const {
  return steps.size();
}

G4ReweightStep * G4ReweightTraj::GetStep(size_t is) const {
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

size_t G4ReweightTraj::GetNChilds() const {
  return children.size();
}

G4ReweightTraj * G4ReweightTraj::GetChild(size_t ic) const {
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

std::string G4ReweightTraj::GetFinalProc() const {
  return steps.back()->GetStepChosenProc();
}

double G4ReweightTraj::GetTotalLength() {
  double total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){
    total += steps.at(is)->GetStepLength();  
  }

  return total;
}

size_t G4ReweightTraj::GetNElastic(){
  size_t total = 0;

  for(size_t is = 0; is < GetNSteps(); ++is){
    auto theStep = GetStep(is);

    if(theStep->GetStepChosenProc() == "hadElastic")
      total++;
  }

  return total;
}

std::vector<double> G4ReweightTraj::GetElastDists(){
  std::vector<double>  dists; 
  double total = 0.;
  for(size_t is = 0; is < GetNSteps(); ++is){
    auto theStep = GetStep(is);
    total += theStep->GetStepLength();
    if(theStep->GetStepChosenProc() == "hadElastic"){
      dists.push_back(total);
      total = 0.;
    }     
  }
  if(dists.size() != GetNElastic())
    std::cout << "ERROR WRONG NUMBER OF ELASTS" << std::endl;
  return dists;
}


std::vector<G4ReweightTraj*> G4ReweightTraj::HasChild(int childPDG) const {
  std::vector<G4ReweightTraj*> childTrajs; 
  for(size_t ic = 0; ic < GetNChilds(); ++ic){
    if( GetChild(ic)->GetPDG() == childPDG){
      childTrajs.push_back(GetChild(ic));
    }
  }
  return childTrajs;
}

