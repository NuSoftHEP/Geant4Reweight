#include "geant4reweight/ReweightBase/G4ReweightTraj.hh"
#include "geant4reweight/ReweightBase/G4ReweightStep.hh"

#include <iostream>
#include <math.h>
#include <algorithm>

G4ReweightTraj::G4ReweightTraj(int tid, int pdg, int parid)
  : fTrackID(tid), fPDG(pdg), fParentID(parid) {
  if( fPDG == 211 ){ 
    fInelastic = "pi+Inelastic";
  }
  else if( fPDG == -211 ){
    fInelastic = "pi-Inelastic";  
  }
  else if( fPDG == 2212 ){
    fInelastic = "protonInelastic";
  }
  else if(fPDG == 2112){
    fInelastic = "neutronInelastic";
  }

}

//G4ReweightTraj::~G4ReweightTraj(){
//}

void G4ReweightTraj::AddStep(const G4ReweightStep & inputStep){
  fSteps.push_back(inputStep);
}

size_t G4ReweightTraj::GetNSteps() const {
  return fSteps.size();
}

const G4ReweightStep & G4ReweightTraj::GetStep(size_t is) const {
  return fSteps.at(is); 
}


void G4ReweightTraj::AddChild(G4ReweightTraj * childTraj) {
  fChildren.push_back(childTraj);
}

size_t G4ReweightTraj::GetNChilds() const {
  return fChildren.size();
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
    return fChildren.at(ic);
  }

}

std::string G4ReweightTraj::GetFinalProc() const {
  return fSteps.back().GetStepChosenProc();
}

double G4ReweightTraj::GetTotalLength() {
  double total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){
    total += fSteps.at(is).GetStepLength();  
  }

  return total;
}

size_t G4ReweightTraj::GetNElastic(){
  size_t total = 0;

  for(size_t is = 0; is < GetNSteps(); ++is){
    const auto & theStep = GetStep(is);

    if(theStep.GetStepChosenProc() == "hadElastic")
      total++;
  }

  return total;
}

std::vector<double> G4ReweightTraj::GetElastDists(){
  std::vector<double>  dists; 
  double total = 0.;
  for(size_t is = 0; is < GetNSteps(); ++is){
    const auto & theStep = GetStep(is);
    total += theStep.GetStepLength();
    if(theStep.GetStepChosenProc() == "hadElastic"){
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

