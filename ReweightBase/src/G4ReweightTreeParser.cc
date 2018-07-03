#include "G4ReweightTreeParser.hh"
#include <iostream>
#include "TH1D.h"

G4ReweightTreeParser::G4ReweightTreeParser(std::string fInputFileName){
  fin = new TFile(fInputFileName.c_str()); 
  track = (TTree*)fin->Get("track");
  step = (TTree*)fin->Get("step");
}

void G4ReweightTreeParser::CloseInput(){
  fin->Close();  
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
  step->SetBranchAddress("stepLen", &stepLength);
}

void G4ReweightTreeParser::SetSteps(G4ReweightTraj * G4RTraj){

  for(int is = G4RTraj->stepRange.first; is < G4RTraj->stepRange.second; ++is){
    step->GetEntry(is);

    double preStepP[3] = {preStepPx,preStepPy,preStepPz};
    double postStepP[3] = {postStepPx,postStepPy,postStepPz};

    G4ReweightStep * G4RStep = new G4ReweightStep(sTrackID, sPID, sParID, sEventNum,
                                                  preStepP, postStepP, stepLength, *stepChosenProc);

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
  std::vector<int> * skipped = new std::vector<int>();
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    if(!(ie%1000)){std::cout << ie << std::endl;}

 
    //First event: prev = -1 -> just use already created pointers
    //Same event: prev = eventNum -> use same pointers
    //else: we're on a new event and need to make new pointers
    //      and store the previous map
    if( (prevEvent != -1) && (tEventNum != prevEvent) ){
      //store the previous map and make a new one
      trajCollection->push_back(trajMap);

      std::map<int,G4ReweightTraj*>::iterator itMap;
/*      for(itMap = trajMap->begin(); itMap != trajMap->end(); ++itMap){
        std::cout << itMap->first << " " << itMap->second->PID << std::endl;
      }
*/
      trajMap = new std::map<int, G4ReweightTraj*>();

      //clear skipped 
      skipped->clear();
//      std::cout << "Event: " << tEventNum << std::endl;
    }
/*    else if(prevEvent == -1){
      std::cout << "Event: " << tEventNum << std::endl;
    }
*/ 
    if(skipEM){
      if( abs(tPID) == 11){
        skipped->push_back(tTrackID);
        continue;
      }     
    }
    
    std::vector<int>::iterator checkSkipped = skipped->begin();
    for(checkSkipped; checkSkipped != skipped->end(); ++checkSkipped){
      if (*checkSkipped == tParID){
        break;
      }
    }
    if(checkSkipped != skipped->end()){
      skipped->push_back(tTrackID);
      continue;
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    //std::cout << tTrackID << " " << tPID << " " << tParID <<" " << tEventNum << " " <<tSteps->first << " " << tSteps->second << std::endl;
    SetSteps(G4RTraj);
    //std::cout << G4RTraj->GetFinalProc() << std::endl;
   
    (*trajMap)[tTrackID] = G4RTraj;
    prevEvent = tEventNum;
  }

//  std::cout<< "Event: " << tEventNum << std::endl;
  trajCollection->push_back(trajMap);
  std::map<int,G4ReweightTraj*>::iterator itMap;
/*  for(itMap = trajMap->begin(); itMap != trajMap->end(); ++itMap){
    std::cout << itMap->first << " " << itMap->second << std::endl;
  }
*/
  skipped->clear();
  
  std::cout << "Got " << GetNTrajs() << " trajectories" << std::endl;
  filled = true;
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

  //Go through each event
  for( size_t ie = 0; ie < GetNEvents(); ++ie){
    auto trajMap = trajCollection->at(ie);

//    std::cout << "Event " << ie << std::endl;

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
  //      break;
      }
      else{
        std::cout << "meh" << itTraj->first << " and " << checkID << std::endl;
      }
    }
    
    
  }

  sorted = true;
}

void G4ReweightTreeParser::Analyze(){
  if( !(filled && sorted) ){
    std::cout << "Please Fill and Sort before analyzing" << std::endl;   
    return;
  }
   
  TFile * fout = new TFile("outtry.root","RECREATE");
  TH1D * lenHist = new TH1D("lenHist", "", 50, 0., 5.);

  //Iterate through the collection events
  for(size_t ie = 0; ie < trajCollection->size(); ++ie){
    auto trajMap = trajCollection->at(ie); 
  
    std::cout << "Event " << ie << std::endl;

    //Get the primary trajs
    std::map<int,G4ReweightTraj*>::iterator itTraj;
    for(itTraj = trajMap->begin(); itTraj != trajMap->end(); ++itTraj){
      auto theTraj = itTraj->second;

      if (theTraj->parID == 0){
        std::cout << "Found primary " << theTraj->PID << std::endl;
        std::cout << "Has NChildren: " << theTraj->GetNChilds() << std::endl;
        std::cout << "Has Final Proc: " << theTraj->GetFinalProc() << std::endl;
        for(int ic = 0; ic < theTraj->GetNChilds(); ++ic){
          std::cout <<"\t"<<theTraj->GetChild(ic)->PID << std::endl;
        }

        if(theTraj->GetFinalProc() == "pi+Inelastic"){
          double len = theTraj->GetTotalLength();
          std::cout << "Total Length" << len << std::endl;
          lenHist->Fill(len);
        }
      }
    }
  }

  lenHist->Write();
  fout->Close();
}


