#include <iostream>
#include <utility>
#include "TTree.h"
#include "TFile.h"

#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"

int main(){

  TFile * fin = new TFile("try.root","READ");   

  TTree * track = (TTree*)fin->Get("track");
  TTree * step = (TTree*)fin->Get("step");
 
  int tPID;
  int tTrackID;
  int tEventNum;
  int tParID;
  std::pair<int,int> * tSteps = new std::pair<int,int>(0,0); 

  track->SetBranchAddress("PID", &tPID);
  track->SetBranchAddress("trackID", &tTrackID);
  std::cout << "tid"<< std::endl;
  track->SetBranchAddress("steps", &tSteps);
  std::cout << "steps" << std::endl;

  int sPID;
  int sTrackID; 
  int sEventNum;
  int sParID;

  double preStepPx;
  double preStepPy;
  double preStepPz;
  double postStepPx;
  double postStepPy;
  double postStepPz;  
  

  std::vector<std::string> * stepActivePostProcNames = 0;
  std::vector<std::string> * stepActiveAlongProcNames = 0;
  std::vector<std::string> * stepActivePostProcMFPs = 0;
  std::vector<std::string> * stepActiveAlongProcMFPs = 0;

  std::string * stepChosenProc = 0;

  step->SetBranchAddress("PID", &sPID);
  step->SetBranchAddress("trackID", &sTrackID);
  step->SetBranchAddress("eventNum", &sEventNum);
  step->SetBranchAddress("parID", &sParID);
  step->SetBranchAddress("preStepPx", &preStepPx);
  step->SetBranchAddress("preStepPy", &preStepPy);
  step->SetBranchAddress("preStepPz", &preStepPz);
  step->SetBranchAddress("postStepPx", &postStepPx);
  step->SetBranchAddress("postStepPy", &postStepPy);
  step->SetBranchAddress("postStepPz", &postStepPz);
  step->SetBranchAddress("stepChosenProc", &stepChosenProc);
  step->SetBranchAddress("stepActivePostProcNames", &stepActivePostProcNames);
  step->SetBranchAddress("stepActiveAlongProcNames", &stepActiveAlongProcNames);
  step->SetBranchAddress("stepActivePostProcMFPs", &stepActivePostProcMFPs);
  step->SetBranchAddress("stepActiveAlongProcMFPs", &stepActiveAlongProcMFPs);

  std::cout << "getting entry" << std::endl;
  track->GetEntry(0);  
  std::cout << "got it" << std::endl;
  for(int is = tSteps->first; is <= tSteps->second; ++is){
    std::cout << "Step " << is << std::endl;
    step->GetEntry(is);

    double preStepP[3] = {preStepPx,preStepPy,preStepPz};
    double postStepP[3] = {postStepPx,postStepPy,postStepPz};
    std::cout << *stepChosenProc << std::endl;
    for(int is = 0; is < stepActivePostProcNames->size(); is++){
      std::cout << stepActivePostProcNames->at(is) << std::endl;
    }
    G4ReweightStep * G4RStep = new G4ReweightStep(sTrackID, sPID, sParID, sEventNum,
                                                  preStepP, postStepP, *stepChosenProc);

    delete G4RStep;
  }

  return 0;
}
