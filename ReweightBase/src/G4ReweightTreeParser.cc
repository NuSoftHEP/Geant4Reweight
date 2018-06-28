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

void G4ReweightTreeParser::SetSteps(G4ReweightTraj * G4RTraj){

  for(int is = G4RTraj->stepRange.first; is < G4RTraj->stepRange.second; ++is){
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

//  std::cout << "Got " << G4RTraj->GetNSteps() << " steps for track " << G4RTraj->trackID << std::endl;

}

void G4ReweightTreeParser::FillCollection(){

  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  for(int ie = 0; ie < track->GetEntries(); ++ie){
    track->GetEntry(ie);

    if(skipEM){
      if( abs(tPID) == 11){continue;}
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);
    
    std::cout << tTrackID << " " << tPID << " " << tParID <<" " << tEventNum << " " <<tSteps->first << " " << tSteps->second << std::endl;

    SetSteps(G4RTraj);
//    std::cout << G4RTraj->GetNSteps() << std::endl;

    std::cout << G4RTraj->GetFinalProc() << std::endl;
    trajCollection->push_back(G4RTraj);
  }

  std::cout << "Got " << GetNTrajs() << " trajectories" << std::endl;
}

size_t G4ReweightTreeParser::GetNTrajs(){ 
  return trajCollection->size();
}

G4ReweightTraj* G4ReweightTreeParser::GetTraj(size_t index){
  if(!GetNTrajs()){
    std::cout << "Traj collection is empty" << std::endl;
    return NULL;
  }
  else if(index > (GetNTrajs() - 1)){
    std::cout << "Traj index out of range. Expecting index between 0 and " <<
    (GetNTrajs() - 1) << std::endl;
    return NULL;
  }
  else{
    return trajCollection->at(index);
  }
}

void G4ReweightTreeParser::SortCollection(){
  std::cout << "Attempting to sort " << GetNTrajs() << 
  " trajectories and set child-parent relationships" <<std::endl;

  //This will go through potential child
  //particles. The second loop will scan through and find the parents. 
  //Iterate backward first as the trees should have saved children after parents 
  for (size_t it = GetNTrajs() - 1 ; it > 0; --it){
    //Check if primary
    if (GetTraj(it)->parID == 0){
      continue;
    }

    //Then iterate forward.
    for (size_t it2 = 0; it2 < GetNTrajs(); ++it2){
      if(it == it2){
        continue;
      }

      if( GetTraj(it)->SetParent(GetTraj(it2)) ){
        std::cout << "Set parent for " << it << " and " << it2 << std::endl;
        break;
      }
    }
  }
}
