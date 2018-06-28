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

  trajCollection->clear();
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
  

  int prevEvent = -1;
  std::map<int, G4ReweightTraj*> * trajMap = new std::map<int, G4ReweightTraj*>();
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
    
    if(prevEvent == -1){
      //First entry. Just use the previously created pointer
      (*trajMap)[tTrackID] = G4RTraj;
    }
    else if(tEventNum == prevEvent){
      //Still on the same event. Just add the pointer to the traj 
      //to the map
      (*trajMap)[tTrackID] = G4RTraj;
    }
    else if(tEventNum != prevEvent){
      //New Event. Save the map in the vector and make new one.  
      trajCollection->push_back(trajMap);
      trajMap = new std::map<int, G4ReweightTraj*>();
      (*trajMap)[tTrackID] = G4RTraj;
    }

    prevEvent = tEventNum;


//    trajCollection->push_back(G4RTraj);
  }

  std::cout << "Got " << GetNTrajs() << " trajectories" << std::endl;
}

size_t G4ReweightTreeParser::GetNTrajs(){ 
  size_t nTraj = 0;
  
  std::cout << trajCollection->size() << " Events" << std::endl;
  for(size_t i = 0; i < trajCollection->size(); ++i){
    //Get map at i. Add its size to the count
    nTraj += (trajCollection->at(i))->size();
  }

//  return trajCollection->size();
  return nTraj;
}

size_t G4ReweightTreeParser::GetNEvents(){
  return trajCollection->size();
}

G4ReweightTraj* G4ReweightTreeParser::GetTraj(size_t eventIndex, size_t trackIndex){
  if(!GetNTrajs()){
    std::cout << "Traj collection is empty" << std::endl;
    return NULL;
  }
  else if(eventIndex > (GetNEvents() - 1) ){
    std::cout << "Event index out of range. Expecting index between 0 and " <<
    (GetNEvents() - 1) << std::endl;
    return NULL;
  }
  else if( (trajCollection->at(eventIndex))->count(trackIndex) == 0) {
    std::cout << "No trackID matching " << trackIndex << " within Event " << eventIndex << std::endl;
    return NULL;
  }
  else{
    return (*(trajCollection->at(eventIndex)))[trackIndex];
  }
}

void G4ReweightTreeParser::SortCollection(){
  std::cout << "Attempting to sort " << GetNTrajs() << 
  " trajectories and set child-parent relationships" <<std::endl;

  //This will go through potential child
  //particles. The second loop will scan through and find the parents. 
  //Iterate backward first as the trees should have saved children after parents 
/*  for (size_t it = GetNTrajs() - 1 ; it > 0; --it){
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
  }*/

  //Go through each event
  for( size_t ie = 0; ie < GetNEvents(); ++ie){
    auto trajMap = trajCollection->at(ie);

    for(auto itTraj = trajMap->begin(); itTraj != trajMap->end(); ++itTraj){
      size_t checkID = (itTraj->second)->parID;
      if(checkID == 0){
        continue;
      }
      else if( trajMap->count(checkID) == 0 ){
        std::cout << "Parent ID " << checkID << " Not in map" << std::endl;
        continue;
      }

      //Should be the parent
      auto tryTraj = (*trajMap)[checkID];
      if( (itTraj->second)->SetParent(tryTraj) ){
        std::cout << "SetParent for " << itTraj->first << " and " << checkID << std::endl;
        break;
      }
    }
    
    
  }
}
