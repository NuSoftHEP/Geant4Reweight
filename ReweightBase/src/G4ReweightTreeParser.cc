#include "G4ReweightTreeParser.hh"
#include <iostream>
#include <algorithm>
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
  //std::cout << G4RTraj->stepRange.first << " " << G4RTraj->stepRange.second << std::endl;
  for(int is = G4RTraj->stepRange.first; is < G4RTraj->stepRange.second; ++is){
    //std::cout << is << std::endl;
    step->GetEntry(is);

   // std::cout << preStepPx << " " << preStepPy << " " << preStepPz << std::endl;
   // std::cout << postStepPx << " " << postStepPy << " " << postStepPz << std::endl;
    double preStepP[3] = {preStepPx,preStepPy,preStepPz};
    double postStepP[3] = {postStepPx,postStepPy,postStepPz};

    G4ReweightStep * G4RStep = new G4ReweightStep(sTrackID, sPID, sParID, sEventNum,
                                                  preStepP, postStepP, stepLength, *stepChosenProc);
    // std::cout << is << " " << *stepChosenProc << std::endl;                                                  

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

}

void G4ReweightTreeParser::FillCollection(){

  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  

  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    if(!(ie%1000)){std::cout << ie << std::endl;}

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    SetSteps(G4RTraj);
   
    std::pair<size_t,size_t> thisPair = std::make_pair(tTrackID,tEventNum);
    std::pair<size_t,size_t> parentPair = std::make_pair(tParID,tEventNum);

    //Primary particle
    if(tParID == 0){
      (*trajCollection)[thisPair] = G4RTraj;
//      std::cout << "Added primary " << tTrackID << " " << tPID << " " << tEventNum << std::endl;
    }
    //The particle's parent is in the map
    else if( trajCollection->count( parentPair ) ){      
      if( (*trajCollection)[ parentPair ]->AddChild( G4RTraj ) ){
//        std::cout << "Added child " << tTrackID << " " << tPID << " " << tEventNum << std::endl;
//        std::cout << "\tTo " << tParID << std::endl;
      }
/*      else{
        std::cout << "Failed to add child " <<tTrackID << " " << tPID << " " << tEventNum << std::endl;
        std::cout << "\tTo " << tParID << std::endl;
      }     
*/
    }
    else{
//      std::cout << "Did not find parent " << tParID << " in map" << std::endl;
//      std::cout << "Deleting traj " << tTrackID << " " << tPID << " " << tEventNum << std::endl;
      delete G4RTraj;
    }
    
  }

  
  std::cout << "Got " << GetNTrajs() << " trajectories" << std::endl;
  filled = true;
}

size_t G4ReweightTreeParser::GetNTrajs(){ 
//  size_t nTraj = 0;
  
//  std::cout << trajCollection->size() << " Events" << std::endl;
/*  for(size_t i = 0; i < trajCollection->size(); ++i){
    //Get map at i. Add its size to the count
    nTraj += (trajCollection->at(i))->size();
  }
*/
  return trajCollection->size();
//  return nTraj;
}

/*size_t G4ReweightTreeParser::GetNEvents(){
  return trajCollection->size();
}*/

G4ReweightTraj* G4ReweightTreeParser::GetTraj(size_t eventIndex, size_t trackIndex){
  if(!GetNTrajs()){
    std::cout << "Traj collection is empty" << std::endl;
    return NULL;
  }
/*  else if(eventIndex > (GetNEvents() - 1) ){
    std::cout << "Event index out of range. Expecting index between 0 and " <<
    (GetNEvents() - 1) << std::endl;
    return NULL;
  }
  else if( (trajCollection->at(eventIndex))->count(trackIndex) == 0) {
    std::cout << "No trackID matching " << trackIndex << " within Event " << eventIndex << std::endl;
    return NULL;
  }*/
  else if( !( trajCollection->count( std::make_pair(trackIndex, eventIndex) ) ) ){
    std::cout << "No traj matching trackID, eventNum: " << trackIndex << " " << eventIndex << std::endl;
    return NULL;
  }
  else{
    return (*trajCollection)[std::make_pair(trackIndex, eventIndex)];
  }
}

/*void G4ReweightTreeParser::SortCollection(){
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
}*/

void G4ReweightTreeParser::Analyze(){
  if( !filled ){
    std::cout << "Please Fill before analyzing" << std::endl;   
    return;
  }
   
  TFile * fout = new TFile("outtry.root","RECREATE");
  TH1D * lenHist = new TH1D("lenHist", "", 250, 0., 50.);
  TH1D * weightHist = new TH1D("weightHist", "", 500, 0, 5.);

  //Iterate through the collection 
  std::map< std::pair<size_t,size_t>, G4ReweightTraj*>::iterator itTraj = trajCollection->begin();
  for(itTraj; itTraj != trajCollection->end(); ++itTraj){
  
    auto theTraj = itTraj->second;
    if (theTraj->parID > 0){ std::cout << "Error! Found non primary traj in collection" << std::endl; break;}
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
    double w = theTraj->GetWeight(1.5);
    std::cout << "Weight: " << w << std::endl;
  }
  

  lenHist->Write();
  fout->Close();
}


void G4ReweightTreeParser::FillAndAnalyze(double bias, double elastBias){

  TFile * fout = new TFile("outtry.root","RECREATE");
  TH1D * lenHist = new TH1D("lenHist", "", 100, 0, 40.);
  TH1D * weightHist = new TH1D("weights", "", 200,0,2.);  
  TH1D * weightedLen = new TH1D("weightedLen", "", 100, 0, 40.);
  TTree * tree = new TTree("tree","");
  
  double theLen=0.;
  double theWeight=0.;
  double theElastWeight = 0.;
  double N=0.;
  std::string theInt = ""; 
  double postFinalP=0.;
  double preFinalP=0.;
  int nElast;

  tree->Branch("len", &theLen);  
  tree->Branch("weight", &theWeight);  
  tree->Branch("elastWeight", &theElastWeight);  
  tree->Branch("N", &N);
  tree->Branch("nElast", &nElast);
  tree->Branch("int", &theInt);
  tree->Branch("postFinalP", &postFinalP);
  tree->Branch("preFinalP", &preFinalP);


  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  int prevEvent = -1;
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    if(!(ie%1000)){std::cout << ie << std::endl;}

    //New event. Do the analysis and Delete the current collection
    if( (prevEvent > -1) && (prevEvent != tEventNum) ){

//      std::cout << "Event: " << prevEvent << std::endl;
     
      //Do analysis here
      std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
      for( ; itTraj != trajCollection->end(); ++itTraj){
        auto theTraj = itTraj->second; 
        if (theTraj->parID == 0){
//          std::cout << "Found primary " << theTraj->PID << std::endl;
//          std::cout << "Has NChildren: " << theTraj->GetNChilds() << std::endl;
//          std::cout << "Has Final Proc: " << theTraj->GetFinalProc() << std::endl;
          for(int ic = 0; ic < theTraj->GetNChilds(); ++ic){
//            std::cout <<"\t"<<theTraj->GetChild(ic)->PID << std::endl;
          }

          double w = theTraj->GetWeight(bias);
          weightHist->Fill(w);

          if(theTraj->GetFinalProc() == "pi+Inelastic"){
            double len = theTraj->GetTotalLength();
//            std::cout << "Total Length" << len << std::endl;
            lenHist->Fill(len);
            weightedLen->Fill(len,w);
          }
          theLen = theTraj->GetTotalLength();
          theWeight = w;
          theElastWeight = theTraj->GetWeight_Elast(elastBias);
          theInt = theTraj->GetFinalProc();
          nElast = theTraj->GetNElastic();
          //std::cout << "Final " << theInt << std::endl;
          double px = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPx;
          double py = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPy;
          double pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPz;
          //std::cout << theInt << " " << px << " " << py << " " << pz << std::endl;
          preFinalP = sqrt( px*px + py*py + pz*pz); 

          px = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPx;
          py = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPy;
          pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPz;
          //std::cout << theInt << " " << px << " " << py << " " << pz << std::endl;
          postFinalP = sqrt( px*px + py*py + pz*pz); 
          tree->Fill();

           
//          std::cout << "Weight: " << w << std::endl;
        }
      }

      for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
        //Delete the pointer
        delete itTraj->second;
      }
      //empty the container
      trajCollection->clear();
      
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    SetSteps(G4RTraj);
   
    std::pair<size_t,size_t> thisPair = std::make_pair(tTrackID,tEventNum);
    std::pair<size_t,size_t> parentPair = std::make_pair(tParID,tEventNum);

    //Add the traj to the map
    (*trajCollection)[thisPair] = G4RTraj;

    //The particle's parent is in the map
    if( trajCollection->count( parentPair ) ){      
       (*trajCollection)[ parentPair ]->AddChild( G4RTraj ); 
    }
    else{
//      std::cout << "Could not find parent" << std::endl;
//      std::cout << thisPair.first << " " << parentPair.first << " " << thisPair.second << std::endl;
    }
        
    prevEvent = tEventNum;
  }

  
  std::cout << "Got " << GetNTrajs() << " trajectories" << std::endl;
  filled = true;

  std::cout << "Event: " << prevEvent << std::endl;
  
  //Do analysis here
  std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
  for( ; itTraj != trajCollection->end(); ++itTraj){
    auto theTraj = itTraj->second; 
    if (theTraj->parID == 0){
//      std::cout << "Found primary " << theTraj->PID << std::endl;
//      std::cout << "Has NChildren: " << theTraj->GetNChilds() << std::endl;
//      std::cout << "Has Final Proc: " << theTraj->GetFinalProc() << std::endl;
      for(int ic = 0; ic < theTraj->GetNChilds(); ++ic){
//        std::cout <<"\t"<<theTraj->GetChild(ic)->PID << std::endl;
      }

      double w = theTraj->GetWeight(bias);
//      double w = theTraj->GetWeight_Elast(1.5,2.);
      weightHist->Fill(w);

      if(theTraj->GetFinalProc() == "pi+Inelastic"){
        double len = theTraj->GetTotalLength();
//        std::cout << "Total Length" << len << std::endl;
        lenHist->Fill(len);
        weightedLen->Fill(len, w);
      }
      theLen = theTraj->GetTotalLength();
      theWeight = w;
      theElastWeight = theTraj->GetWeight_Elast(elastBias);
      theInt = theTraj->GetFinalProc();
      nElast = theTraj->GetNElastic();

      
      double px = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPx;
      double py = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPy;
      double pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPz;
      //std::cout << px << " " << py << " " << pz << std::endl;
      preFinalP = sqrt( px*px + py*py + pz*pz); 

      px = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPx;
      py = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPy;
      pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPz;
      //std::cout << px << " " << py << " " << pz << std::endl;
      postFinalP = sqrt( px*px + py*py + pz*pz); 
      tree->Fill();
//      std::cout << "Weight: " << w << std::endl;
    }
  }

  for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    //Delete the pointer
    delete itTraj->second;
  }
  //empty the container
  trajCollection->clear();

  lenHist->Write();
  weightedLen->Write();;
  weightHist->Write();
  tree->Write();
  fout->Close();
}
