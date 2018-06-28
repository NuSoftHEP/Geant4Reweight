#include "G4ReweightTreeParser.hh"
#include <iostream>

G4ReweightTreeParser::G4ReweightTreeParser(std::string fInputFileName){
  fin = new TFile(fInputFileName.c_str()); 
  track = (TTree*)fin->Get("track");
  step = (TTree*)fin->Get("step");
}

G4ReweightTreeParser::~G4ReweightTreeParser(){
  stepActivePostProcNames->clear();
  stepActiveAlongProcNames->clear();
  stepActivePostProcMFPs->clear();
  stepActiveAlongProcMFPs->clear();
}

void G4ReweightTreeParser::SetBranches(){
  track->SetBranchAddress("PID", &tPID);
  track->SetBranchAddress("trackID", &tTrackID);
  track->SetBranchAddress("steps", &tSteps);
  track->SetBranchAddress("parID", &tParID);
  track->SetBranchAddress("eventNum", &tEventNum);

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
}

void G4ReweightTreeParser::GetSteps(G4ReweightTraj * G4RTraj){

  for(int is = G4RTraj->stepRange.first; is <= G4RTraj->stepRange.second; ++is){
    if(!(is%100)){std::cout << "Step " << is << std::endl;}
    step->GetEntry(is);

    double preStepP[3] = {preStepPx,preStepPy,preStepPz};
    double postStepP[3] = {postStepPx,postStepPy,postStepPz};

    G4ReweightStep * G4RStep = new G4ReweightStep(sTrackID, sPID, sParID, sEventNum,
                                                  preStepP, postStepP, *stepChosenProc);

    Proc theProc;
    for(size_t ip = 0; ip < stepActivePostProcMFPs->size(); ++ip){
      std::string theName = stepActivePostProcNames->at(ip);
      double theMFP = stepActivePostProcMFPs->at(ip); 

      theProc.Name = theName;
      theProc.MFP = theMFP;

      G4RStep->AddActivePostProc(theProc);
    }
    for(size_t ip = 0; ip < stepActiveAlongProcMFPs->size(); ++ip){
      std::string theName = stepActiveAlongProcNames->at(ip);
      double theMFP = stepActiveAlongProcMFPs->at(ip); 

      theProc.Name = theName;
      theProc.MFP = theMFP;

      G4RStep->AddActiveAlongProc(theProc);
    }

    G4RTraj->AddStep(G4RStep);
  }

  std::cout << "Got " << G4RTraj->GetNSteps() << " steps for track " << G4RTraj->trackID << std::endl;

}

void G4ReweightTreeParser::FillCollection(){

  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;

  for(int ie = 0; ie < track->GetEntries(); ++ie){
    track->GetEntry(ie);
    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);

    GetSteps(G4RTraj);
    std::cout << G4RTraj->GetNSteps() << std::endl;
  }
}
