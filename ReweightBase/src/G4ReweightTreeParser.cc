#include "G4ReweightTreeParser.hh"
#include <iostream>
#include <algorithm>
#include "TH1D.h"

G4ReweightTreeParser::G4ReweightTreeParser(std::string fInputFileName, std::string fOutputFileName){
/*  fin = new TFile(fInputFileName.c_str(), "READ"); 
  track = (TTree*)fin->Get("track");
  step = (TTree*)fin->Get("step");

  SetBranches();
*/

  OpenNewInput( fInputFileName );

  fout = new TFile(fOutputFileName.c_str(), "RECREATE"); 
  tree = new TTree("tree","");

  mapPIDtoN = { {211, &nPiPlus},
                {-211, &nPiMinus},
                {111, &nPi0},
                {2212, &nProton},
                {2112, &nNeutron} };

  MakeOutputBranches();

}

void G4ReweightTreeParser::OpenNewInput(std::string fInputFileName){
  
  fin = new TFile(fInputFileName.c_str(), "READ"); 
  track = (TTree*)fin->Get("track");
  step = (TTree*)fin->Get("step");

  SetBranches();

}

/*G4ReweightTreeParser::G4ReweightTreeParser(std::string fInputFileName, TFile * fOutputFile, TTree * fOutputTree){
  fin = new TFile(fInputFileName.c_str(), "READ"); 
  track = (TTree*)fin->Get("track");
  step = (TTree*)fin->Get("step");

  SetBranches();

  fout = fOutputFile; 
  tree = new TTree("tree","");

  mapPIDtoN = { {211, &nPiPlus},
                {-211, &nPiMinus},
                {111, &nPi0},
                {2212, &nProton},
                {2112, &nNeutron} };

  MakeOutputBranches();

}
*/

void G4ReweightTreeParser::CloseInput(){
  fin->Close();  
  delete fin;
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
  track->SetBranchAddress("Energy", &tEnergy);

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
  step->SetBranchAddress("deltaX", &deltaX);
  step->SetBranchAddress("deltaY", &deltaY);
  step->SetBranchAddress("deltaZ", &deltaZ);
}

void G4ReweightTreeParser::SetSteps(G4ReweightTraj * G4RTraj){
//  std::cout << G4RTraj->eventNum << " " << G4RTraj->trackID << " " << G4RTraj->PID << " " << G4RTraj->parID << " " <<G4RTraj->stepRange.first << " " << G4RTraj->stepRange.second << std::endl;
  for(int is = G4RTraj->stepRange.first; is < G4RTraj->stepRange.second; ++is){
//    std::cout << is << std::endl;
    step->GetEntry(is);

   // std::cout << preStepPx << " " << preStepPy << " " << preStepPz << std::endl;
   // std::cout << postStepPx << " " << postStepPy << " " << postStepPz << std::endl;
    double preStepP[3] = {preStepPx,preStepPy,preStepPz};
    double postStepP[3] = {postStepPx,postStepPy,postStepPz};

    G4ReweightStep * G4RStep = new G4ReweightStep(sTrackID, sPID, sParID, sEventNum,
                                                  preStepP, postStepP, stepLength, *stepChosenProc);
    // std::cout << is << " " << *stepChosenProc << std::endl;                                                  

    G4RStep->deltaX = deltaX;
    G4RStep->deltaY = deltaY;
    G4RStep->deltaZ = deltaZ;

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

//    if(!(ie%10000)){std::cout << ie << std::endl;}

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
        (*trajCollection)[thisPair] = G4RTraj;
        std::cout << "Added child " << tTrackID << " " << tPID << " " << tEventNum << std::endl;
        std::cout << "\tTo " << tParID << std::endl;
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

void G4ReweightTreeParser::Analyze(double bias, double elastBias){
      std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
      for( ; itTraj != trajCollection->end(); ++itTraj){
        auto theTraj = itTraj->second; 

        /* std::string Inel;
           if(theTraj->parID != 0)continue;
           if( theTraj->PID == 211 ) Inel = "pi+Inelastic";
           else if( theTraj->PID == -211 ) Inel = "pi-Inelastic";
           else continue;
        */
        if (theTraj->parID == 0 && theTraj->PID == 211){ //Delete this line
          //Skip any that exit out the back
          double totalDeltaZ = 0.;
          for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
            auto theStep = theTraj->GetStep(is);
            totalDeltaZ += theStep->deltaZ;
          }
          if(totalDeltaZ < 0.) continue;

//          std::cout << "Found primary " << theTraj->PID << std::endl;
//          std::cout << "Has NChildren: " << theTraj->GetNChilds() << std::endl;
//          std::cout << "Has Final Proc: " << theTraj->GetFinalProc() << std::endl;
//         for(int ic = 0; ic < theTraj->GetNChilds(); ++ic){
//            std::cout <<"\t"<<theTraj->GetChild(ic)->PID << std::endl;
//         }

          double w = theTraj->GetWeight(bias);

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

          if(theInt == "pi+Inelastic" || theInt == "pi-Inelastic"){
             //Now check the scattering angle 
             auto products = theTraj->HasChild(theTraj->PID);
             if( products.size() == 1){
//               std::cout << "Found inelastic scatter" << std::endl;
               auto theChild = products[0];
               double childPx = theChild->GetStep(0)->preStepPx;
               double childPy = theChild->GetStep(0)->preStepPy;
               double childPz = theChild->GetStep(0)->preStepPz;
               cosTheta = (px*childPx + py*childPy + pz*childPz);
               cosTheta = cosTheta/preFinalP;
               cosTheta = cosTheta/sqrt(childPx*childPx + childPy*childPy + childPz*childPz);
//               std::cout << cosTheta << std::endl;
               
             }
          }
          else{
            cosTheta = 0.;
          }

          px = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPx;
          py = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPy;
          pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPz;
//          std::cout << theInt << " " << px << " " << py << " " << pz << std::endl;
          postFinalP = sqrt( px*px + py*py + pz*pz); 
          if(elastDists) elastDists->clear();
          std::vector<double> dists = theTraj->GetElastDists();
          for(size_t id = 0; id < nElast; ++id){
            elastDists->push_back(dists[id]);
          }

          if(sliceEnergy){
            sliceEnergy->clear();
            sliceInts->clear();
          }           
           
//          std::vector< std::pair<double, int> > slices = theTraj->ThinSliceMethod(.5); 
          std::vector< std::pair<double, int> > slices = theTraj->ThinSliceBetheBloch(.5); 
          for(size_t it = 0; it < slices.size(); ++it){
            sliceEnergy->push_back(slices[it].first); 
            sliceInts->push_back(slices[it].second); 
          }

          if(sliceEnergyInelastic){
            sliceEnergyInelastic->clear();
            sliceIntsInelastic->clear();
          }

          std::vector< std::pair<double, int> > slicesInelastic = theTraj->ThinSliceBetheBlochInelastic(.5);          
          for(size_t it = 0; it < slicesInelastic.size(); ++it){
            sliceEnergyInelastic->push_back(slicesInelastic[it].first); 
            sliceIntsInelastic->push_back(slicesInelastic[it].second); 
          }
//          std::cout << "New Track" << std::endl;
          std::map<int, int*>::iterator itN = mapPIDtoN.begin();
          for(; itN != mapPIDtoN.end(); ++itN){
            *(itN->second) = (theTraj->HasChild(itN->first)).size();
//            std::cout << "This track has " << *(itN->second) << " " << itN->first << std::endl;
          }
          GetInteractionType(theTraj->PID);
         
          Energy = theTraj->Energy;

          tree->Fill();
           
        } //Delete this line
      }
}

void G4ReweightTreeParser::Analyze(TH1F * inelBiasHist, TH1F * elastBiasHist){

      std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
      for( ; itTraj != trajCollection->end(); ++itTraj){
        auto theTraj = itTraj->second; 
        if (theTraj->parID == 0 && theTraj->PID == 211){
          //Skip any that exit out the back
          double totalDeltaZ = 0.;
          for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
            auto theStep = theTraj->GetStep(is);
            totalDeltaZ += theStep->deltaZ;
          }
          if(totalDeltaZ < 0.) continue;

//          std::cout << "Found primary " << theTraj->PID << std::endl;
//          std::cout << "Has NChildren: " << theTraj->GetNChilds() << std::endl;
//          std::cout << "Has Final Proc: " << theTraj->GetFinalProc() << std::endl;
//         for(int ic = 0; ic < theTraj->GetNChilds(); ++ic){
//            std::cout <<"\t"<<theTraj->GetChild(ic)->PID << std::endl;
//         }

          theLen         = theTraj->GetTotalLength();
          theWeight      = theTraj->GetWeight(inelBiasHist);
          theElastWeight = theTraj->GetWeight_Elast(elastBiasHist);
          theInt         = theTraj->GetFinalProc();
          nElast         = theTraj->GetNElastic();

          //std::cout << "Final " << theInt << std::endl;
          double px = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPx;
          double py = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPy;
          double pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPz;
          //std::cout << theInt << " " << px << " " << py << " " << pz << std::endl;
          preFinalP = sqrt( px*px + py*py + pz*pz); 

          if(theInt == "pi+Inelastic" || theInt == "pi-Inelastic"){
             //Now check the scattering angle 
             auto products = theTraj->HasChild(theTraj->PID);
             if( products.size() == 1){
//               std::cout << "Found inelastic scatter" << std::endl;
               auto theChild = products[0];
               double childPx = theChild->GetStep(0)->preStepPx;
               double childPy = theChild->GetStep(0)->preStepPy;
               double childPz = theChild->GetStep(0)->preStepPz;
               cosTheta = (px*childPx + py*childPy + pz*childPz);
               cosTheta = cosTheta/preFinalP;
               cosTheta = cosTheta/sqrt(childPx*childPx + childPy*childPy + childPz*childPz);
//               std::cout << cosTheta << std::endl;
               
             }
          }
          else{
            cosTheta = 0.;
          }

          px = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPx;
          py = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPy;
          pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPz;
//          std::cout << theInt << " " << px << " " << py << " " << pz << std::endl;
          postFinalP = sqrt( px*px + py*py + pz*pz); 
          if(elastDists) elastDists->clear();
          std::vector<double> dists = theTraj->GetElastDists();
          for(size_t id = 0; id < nElast; ++id){
            elastDists->push_back(dists[id]);
          }

          if(sliceEnergy){
            sliceEnergy->clear();
            sliceInts->clear();
          }           
           
//          std::vector< std::pair<double, int> > slices = theTraj->ThinSliceMethod(.5); 
          std::vector< std::pair<double, int> > slices = theTraj->ThinSliceBetheBloch(.5); 
          for(size_t it = 0; it < slices.size(); ++it){
            sliceEnergy->push_back(slices[it].first); 
            sliceInts->push_back(slices[it].second); 
          }

          if(sliceEnergyInelastic){
            sliceEnergyInelastic->clear();
            sliceIntsInelastic->clear();
          }

          std::vector< std::pair<double, int> > slicesInelastic = theTraj->ThinSliceBetheBlochInelastic(.5);          
          for(size_t it = 0; it < slicesInelastic.size(); ++it){
            sliceEnergyInelastic->push_back(slicesInelastic[it].first); 
            sliceIntsInelastic->push_back(slicesInelastic[it].second); 
          }
//          std::cout << "New Track" << std::endl;
          std::map<int, int*>::iterator itN = mapPIDtoN.begin();
          for(; itN != mapPIDtoN.end(); ++itN){
            *(itN->second) = (theTraj->HasChild(itN->first)).size();
//            std::cout << "This track has " << *(itN->second) << " " << itN->first << std::endl;
          }
          GetInteractionType(theTraj->PID);
         
          Energy = theTraj->Energy;

          tree->Fill();
           
        }
      }
}

void G4ReweightTreeParser::AnalyzeFunc(G4ReweightInter * inelBias, G4ReweightInter * elastBias){

      std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
      for( ; itTraj != trajCollection->end(); ++itTraj){
        auto theTraj = itTraj->second; 
        if (theTraj->parID == 0 && theTraj->PID == 211){
          //Skip any that exit out the back
          double totalDeltaZ = 0.;
          for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
            auto theStep = theTraj->GetStep(is);
            totalDeltaZ += theStep->deltaZ;
          }
          if(totalDeltaZ < 0.) continue;

//          std::cout << "Found primary " << theTraj->PID << std::endl;
//          std::cout << "Has NChildren: " << theTraj->GetNChilds() << std::endl;
//          std::cout << "Has Final Proc: " << theTraj->GetFinalProc() << std::endl;
//         for(int ic = 0; ic < theTraj->GetNChilds(); ++ic){
//            std::cout <<"\t"<<theTraj->GetChild(ic)->PID << std::endl;
//         }

          theLen         = theTraj->GetTotalLength();
          theWeight      = theTraj->GetWeightFunc(inelBias);
          theElastWeight = theTraj->GetWeightFunc_Elast(elastBias);
          theInt         = theTraj->GetFinalProc();
          nElast         = theTraj->GetNElastic();

          //std::cout << "Final " << theInt << std::endl;
          double px = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPx;
          double py = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPy;
          double pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPz;
          //std::cout << theInt << " " << px << " " << py << " " << pz << std::endl;
          preFinalP = sqrt( px*px + py*py + pz*pz); 

          if(theInt == "pi+Inelastic" || theInt == "pi-Inelastic"){
             //Now check the scattering angle 
             auto products = theTraj->HasChild(theTraj->PID);
             if( products.size() == 1){
//               std::cout << "Found inelastic scatter" << std::endl;
               auto theChild = products[0];
               double childPx = theChild->GetStep(0)->preStepPx;
               double childPy = theChild->GetStep(0)->preStepPy;
               double childPz = theChild->GetStep(0)->preStepPz;
               cosTheta = (px*childPx + py*childPy + pz*childPz);
               cosTheta = cosTheta/preFinalP;
               cosTheta = cosTheta/sqrt(childPx*childPx + childPy*childPy + childPz*childPz);
//               std::cout << cosTheta << std::endl;
               
             }
          }
          else{
            cosTheta = 0.;
          }

          px = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPx;
          py = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPy;
          pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPz;
//          std::cout << theInt << " " << px << " " << py << " " << pz << std::endl;
          postFinalP = sqrt( px*px + py*py + pz*pz); 
          if(elastDists) elastDists->clear();
          std::vector<double> dists = theTraj->GetElastDists();
          for(size_t id = 0; id < nElast; ++id){
            elastDists->push_back(dists[id]);
          }

          if(sliceEnergy){
            sliceEnergy->clear();
            sliceInts->clear();
          }           
           
//          std::vector< std::pair<double, int> > slices = theTraj->ThinSliceMethod(.5); 
          std::vector< std::pair<double, int> > slices = theTraj->ThinSliceBetheBloch(.5); 
          for(size_t it = 0; it < slices.size(); ++it){
            sliceEnergy->push_back(slices[it].first); 
            sliceInts->push_back(slices[it].second); 
          }

          if(sliceEnergyInelastic){
            sliceEnergyInelastic->clear();
            sliceIntsInelastic->clear();
          }

          std::vector< std::pair<double, int> > slicesInelastic = theTraj->ThinSliceBetheBlochInelastic(.5);          
          for(size_t it = 0; it < slicesInelastic.size(); ++it){
            sliceEnergyInelastic->push_back(slicesInelastic[it].first); 
            sliceIntsInelastic->push_back(slicesInelastic[it].second); 
          }
//          std::cout << "New Track" << std::endl;
          std::map<int, int*>::iterator itN = mapPIDtoN.begin();
          for(; itN != mapPIDtoN.end(); ++itN){
            *(itN->second) = (theTraj->HasChild(itN->first)).size();
//            std::cout << "This track has " << *(itN->second) << " " << itN->first << std::endl;
          }
          GetInteractionType(theTraj->PID);
         
          Energy = theTraj->Energy;

          tree->Fill();
           
        }
      }
}


void G4ReweightTreeParser::GetInteractionType(int thePID){
 
//  std::cout << thePID << std::endl;

  if( abs(thePID) != 211 ) intType = kNoInt; 
  else{
//    std::cout << thePID << " " << (*mapPIDtoN[thePID]) << std::endl;
//    std::cout << -1*thePID << " " << (*mapPIDtoN[-1*thePID]) << std::endl;
//    std::cout << 111 << " " << (*mapPIDtoN[111]) << std::endl;

    if( !(theInt.find("Inelastic")) ) intType = kNoInt;
    else{      

      if ( ( (*mapPIDtoN[thePID])    == 1 ) &&
           ( (*mapPIDtoN[-1*thePID]) == 0 ) && 
           ( (*mapPIDtoN[111])     == 0 ) ){
        intType = kInel;
      }
      else if( ( (*mapPIDtoN[thePID])    == 0 ) &&
               ( (*mapPIDtoN[-1*thePID]) == 0 ) && 
               ( (*mapPIDtoN[111])     == 0 ) ){
        intType = kABS;
      }    
      else if( ( (*mapPIDtoN[thePID])      == 0 ) &&
                 ( (*mapPIDtoN[-1*thePID]) == 0 ) && 
                 ( (*mapPIDtoN[111])     == 1 ) ){
        intType = kCEX;
      }
      else intType = kOther;

    }

  }
}

void G4ReweightTreeParser::GetWeightFS( G4ReweightFinalState * theFS, double theMomentum ){

  //std::cout << "Getting FS weight" << std::endl;

  //NOTE: MAKE THIS WORK FOR PI- AS WELL
  if( theInt != "pi+Inelastic" ){
//    std::cout << "Not inelastic" << std::endl;
    theFSWeight = 1.;
    return;
  }
  
//  std::cout << "%%%%%%%%%%%%%%%%%%%%" << std::endl;
//  std::cout << "nPiPlus: " << nPiPlus << std::endl;
//  std::cout << "nPiMinus: " << nPiMinus << std::endl;
//  std::cout << "nPi0: " << nPi0 << std::endl;
  
  std::string interaction;
  if( (nPiPlus + nPiMinus + nPi0) == 0 ) interaction = "abs";
  else if( (nPiPlus + nPiMinus) == 0 && ( nPi0 == 1 ) ) interaction = "cex";
  else if( (nPiPlus + nPi0) == 0 && ( nPiMinus == 1 ) ) interaction = "dcex";
  else if( (nPiMinus + nPi0) == 0 && ( nPiPlus == 1 ) ) interaction = "inel";
  else interaction = "prod";

  //std::cout << "Interaction: " << interaction << std::endl;
  //std::cout << "momentum: " << theMomentum << std::endl;
  theFSWeight = theFS->GetWeight( interaction, theMomentum );
//  std::cout << "Weight: " << theFSWeight << std::endl;
//  std::cout << "Interaction: " << interaction << std::endl << std::endl;

  
  //std::cout << "Weight: " << theFSWeight << std::endl;

}

void G4ReweightTreeParser::FillAndAnalyze(double bias, double elastBias){

  //TFile * fout = new TFile("outtry.root","RECREATE");
//  tree = new TTree("tree","");
  
/*  theLen=0.;
  theWeight=0.;
  theElastWeight = 0.;
  N=0.;
  theInt = ""; 
  postFinalP=0.;
  preFinalP=0.;
  nElast = 0;
  elastDists = 0;
  sliceEnergy = 0;
  sliceInts = 0;
  sliceEnergyInelastic = 0;
  sliceIntsInelastic = 0;
  cosTheta = 0.;

  nPiPlus = 0;
  nPiMinus = 0;
  nPi0 = 0;
  nProton = 0;
  nNeutron = 0;

  Energy = 0.;

  tree->Branch("len", &theLen);  
  tree->Branch("weight", &theWeight);  
  tree->Branch("elastWeight", &theElastWeight);  
  tree->Branch("N", &N);
  tree->Branch("nElast", &nElast);
  tree->Branch("elastDists", &elastDists);
  tree->Branch("sliceEnergy", &sliceEnergy);
  tree->Branch("sliceEnergyInelastic", &sliceEnergyInelastic);
  tree->Branch("Energy", &Energy);
  tree->Branch("sliceInts", &sliceInts);
  tree->Branch("sliceIntsInelastic", &sliceIntsInelastic);
  tree->Branch("int", &theInt);
  tree->Branch("postFinalP", &postFinalP);
  tree->Branch("preFinalP", &preFinalP);
  tree->Branch("cosTheta", &cosTheta);

  tree->Branch("nPiPlus", &nPiPlus);
  tree->Branch("nPiMinus", &nPiMinus);
  tree->Branch("nPi0", &nPi0);
  tree->Branch("nProton", &nProton);
  tree->Branch("nNeutron", &nNeutron);

  tree->Branch("intType", &intType);
  */
  //MakeOutputBranches();

  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  int prevEvent = -1;
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    //if(!(ie%1000)){std::cout << ie << std::endl;}
    //New event. Do the analysis and Delete the current collection
    if( (prevEvent > -1) && (prevEvent != tEventNum) ){

//      std::cout << "Event: " << prevEvent << std::endl;
     
      Analyze(bias,elastBias);
      std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
      for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
        //Delete the pointer
        delete itTraj->second;
      }
      //empty the container
      trajCollection->clear();
      
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    G4RTraj->Energy = tEnergy;

    SetSteps(G4RTraj);
    //std::cout << tTrackID << " " << tPID << " " << tParID << " " << tSteps->first << " " << tSteps->second << std::endl;
   
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
  

  Analyze(bias,elastBias);
  std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
  for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    //Delete the pointer
    delete itTraj->second;
  }
  //empty the container
  trajCollection->clear();

}

void G4ReweightTreeParser::FillAndAnalyze(TH1F * inelBiasHist, TH1F * elastBiasHist){
  
  /*theLen=0.;
  theWeight=0.;
  theElastWeight = 0.;
  N=0.;
  theInt = ""; 
  postFinalP=0.;
  preFinalP=0.;
  nElast = 0;
  elastDists = 0;
  sliceEnergy = 0;
  sliceInts = 0;
  sliceEnergyInelastic = 0;
  sliceIntsInelastic = 0;
  cosTheta = 0.;

  nPiPlus = 0;
  nPiMinus = 0;
  nPi0 = 0;
  nProton = 0;
  nNeutron = 0;

  Energy = 0.;

  tree->Branch("len", &theLen);  
  tree->Branch("weight", &theWeight);  
  tree->Branch("elastWeight", &theElastWeight);  
  tree->Branch("N", &N);
  tree->Branch("nElast", &nElast);
  tree->Branch("elastDists", &elastDists);
  tree->Branch("sliceEnergy", &sliceEnergy);
  tree->Branch("sliceEnergyInelastic", &sliceEnergyInelastic);
  tree->Branch("Energy", &Energy);
  tree->Branch("sliceInts", &sliceInts);
  tree->Branch("sliceIntsInelastic", &sliceIntsInelastic);
  tree->Branch("int", &theInt);
  tree->Branch("postFinalP", &postFinalP);
  tree->Branch("preFinalP", &preFinalP);
  tree->Branch("cosTheta", &cosTheta);

  tree->Branch("nPiPlus", &nPiPlus);
  tree->Branch("nPiMinus", &nPiMinus);
  tree->Branch("nPi0", &nPi0);
  tree->Branch("nProton", &nProton);
  tree->Branch("nNeutron", &nNeutron);

  tree->Branch("intType", &intType);
*/
  //MakeOutputBranches();

  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  int prevEvent = -1;
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    //if(!(ie%1000)){std::cout << ie << std::endl;}
    //New event. Do the analysis and Delete the current collection
    if( (prevEvent > -1) && (prevEvent != tEventNum) ){

//      std::cout << "Event: " << prevEvent << std::endl;
     
      Analyze(inelBiasHist, elastBiasHist);
      std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
      for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
        //Delete the pointer
        delete itTraj->second;
      }
      //empty the container
      trajCollection->clear();
      
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    G4RTraj->Energy = tEnergy;

    SetSteps(G4RTraj);
    //std::cout << tTrackID << " " << tPID << " " << tParID << " " << tSteps->first << " " << tSteps->second << std::endl;
   
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
  

  Analyze(inelBiasHist, elastBiasHist);
  std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
  for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    //Delete the pointer
    delete itTraj->second;
  }
  //empty the container
  trajCollection->clear();

}

void G4ReweightTreeParser::FillAndAnalyzeFunc(G4ReweightInter * inelBias, G4ReweightInter * elastBias){
  
  /*theLen=0.;
  theWeight=0.;
  theElastWeight = 0.;
  N=0.;
  theInt = ""; 
  postFinalP=0.;
  preFinalP=0.;
  nElast = 0;
  elastDists = 0;
  sliceEnergy = 0;
  sliceInts = 0;
  sliceEnergyInelastic = 0;
  sliceIntsInelastic = 0;
  cosTheta = 0.;

  nPiPlus = 0;
  nPiMinus = 0;
  nPi0 = 0;
  nProton = 0;
  nNeutron = 0;

  Energy = 0.;

  tree->Branch("len", &theLen);  
  tree->Branch("weight", &theWeight);  
  tree->Branch("elastWeight", &theElastWeight);  
  tree->Branch("N", &N);
  tree->Branch("nElast", &nElast);
  tree->Branch("elastDists", &elastDists);
  tree->Branch("sliceEnergy", &sliceEnergy);
  tree->Branch("sliceEnergyInelastic", &sliceEnergyInelastic);
  tree->Branch("Energy", &Energy);
  tree->Branch("sliceInts", &sliceInts);
  tree->Branch("sliceIntsInelastic", &sliceIntsInelastic);
  tree->Branch("int", &theInt);
  tree->Branch("postFinalP", &postFinalP);
  tree->Branch("preFinalP", &preFinalP);
  tree->Branch("cosTheta", &cosTheta);

  tree->Branch("nPiPlus", &nPiPlus);
  tree->Branch("nPiMinus", &nPiMinus);
  tree->Branch("nPi0", &nPi0);
  tree->Branch("nProton", &nProton);
  tree->Branch("nNeutron", &nNeutron);

  tree->Branch("intType", &intType);
*/
  //MakeOutputBranches();

  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  int prevEvent = -1;
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    //if(!(ie%1000)){std::cout << ie << std::endl;}
    //New event. Do the analysis and Delete the current collection
    if( (prevEvent > -1) && (prevEvent != tEventNum) ){

//      std::cout << "Event: " << prevEvent << std::endl;
     
      AnalyzeFunc(inelBias, elastBias);
      std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
      for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
        //Delete the pointer
        delete itTraj->second;
      }
      //empty the container
      trajCollection->clear();
      
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    G4RTraj->Energy = tEnergy;

    SetSteps(G4RTraj);
    //std::cout << tTrackID << " " << tPID << " " << tParID << " " << tSteps->first << " " << tSteps->second << std::endl;
   
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
  

  AnalyzeFunc(inelBias, elastBias);
  std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
  for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    //Delete the pointer
    delete itTraj->second;
  }
  //empty the container
  trajCollection->clear();

}

//At some point add in elastic? 
void G4ReweightTreeParser::FillAndAnalyzeFS(G4ReweightFinalState * theFS){
  
/*  theLen=0.;
  theWeight=0.;
  theElastWeight = 0.;
  theFSWeight=0.;
  N=0.;
  theInt = ""; 
  postFinalP=0.;
  preFinalP=0.;
  nElast = 0;
  elastDists = 0;
  sliceEnergy = 0;
  sliceInts = 0;
  sliceEnergyInelastic = 0;
  sliceIntsInelastic = 0;
  cosTheta = 0.;

  nPiPlus = 0;
  nPiMinus = 0;
  nPi0 = 0;
  nProton = 0;
  nNeutron = 0;

  Energy = 0.;

  tree->Branch("len", &theLen);  
  tree->Branch("weight", &theWeight);  
  tree->Branch("elastWeight", &theElastWeight);  
  tree->Branch("finalStateWeight", &theFSWeight);
  tree->Branch("N", &N);
  tree->Branch("nElast", &nElast);
  tree->Branch("elastDists", &elastDists);
  tree->Branch("sliceEnergy", &sliceEnergy);
  tree->Branch("sliceEnergyInelastic", &sliceEnergyInelastic);
  tree->Branch("Energy", &Energy);
  tree->Branch("sliceInts", &sliceInts);
  tree->Branch("sliceIntsInelastic", &sliceIntsInelastic);
  tree->Branch("int", &theInt);
  tree->Branch("postFinalP", &postFinalP);
  tree->Branch("preFinalP", &preFinalP);
  tree->Branch("cosTheta", &cosTheta);

  tree->Branch("nPiPlus", &nPiPlus);
  tree->Branch("nPiMinus", &nPiMinus);
  tree->Branch("nPi0", &nPi0);
  tree->Branch("nProton", &nProton);
  tree->Branch("nNeutron", &nNeutron);

  tree->Branch("intType", &intType);
  */

  //MakeOutputBranches();

  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  int prevEvent = -1;
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    //if(!(ie%1000)){std::cout << ie << std::endl;}
    //New event. Do the analysis and Delete the current collection
    if( (prevEvent > -1) && (prevEvent != tEventNum) ){

//      std::cout << "Event: " << prevEvent << std::endl;
     
      AnalyzeFS(theFS);
      std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
      for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
        //Delete the pointer
        delete itTraj->second;
      }
      //empty the container
      trajCollection->clear();
      
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    G4RTraj->Energy = tEnergy;

    SetSteps(G4RTraj);
    //std::cout << tTrackID << " " << tPID << " " << tParID << " " << tSteps->first << " " << tSteps->second << std::endl;
   
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
  
  AnalyzeFS(theFS);
  std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
  for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    //Delete the pointer
    delete itTraj->second;
  }
  //empty the container
  trajCollection->clear();

}

//Add in elastic reweighting?
void G4ReweightTreeParser::AnalyzeFS(G4ReweightFinalState * theFS){

      std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
      for( ; itTraj != trajCollection->end(); ++itTraj){
        auto theTraj = itTraj->second; 
        if (theTraj->parID == 0 && theTraj->PID == 211){
          //Skip any that exit out the back
          double totalDeltaZ = 0.;
          for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
            auto theStep = theTraj->GetStep(is);
            totalDeltaZ += theStep->deltaZ;
          }
          if(totalDeltaZ < 0.) continue;

//          std::cout << "Found primary " << theTraj->PID << std::endl;
//          std::cout << "Has NChildren: " << theTraj->GetNChilds() << std::endl;
//          std::cout << "Has Final Proc: " << theTraj->GetFinalProc() << std::endl;
//         for(int ic = 0; ic < theTraj->GetNChilds(); ++ic){
//            std::cout <<"\t"<<theTraj->GetChild(ic)->PID << std::endl;
//         }

          theLen         = theTraj->GetTotalLength();
          theWeight      = theTraj->GetWeight( (TH1F*)theFS->GetTotalVariation() );
//          theFSWeight    = theTraj->GetWeightFS(theFS);
          theElastWeight = 1.;
          theInt         = theTraj->GetFinalProc();
          nElast         = theTraj->GetNElastic();

          //std::cout << "Final " << theInt << std::endl;
          double px = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPx;
          double py = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPy;
          double pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPz;
          //std::cout << theInt << " " << px << " " << py << " " << pz << std::endl;
          preFinalP = sqrt( px*px + py*py + pz*pz); 

          if(theInt == "pi+Inelastic" || theInt == "pi-Inelastic"){
             //Now check the scattering angle 
             auto products = theTraj->HasChild(theTraj->PID);
             if( products.size() == 1){
//               std::cout << "Found inelastic scatter" << std::endl;
               auto theChild = products[0];
               double childPx = theChild->GetStep(0)->preStepPx;
               double childPy = theChild->GetStep(0)->preStepPy;
               double childPz = theChild->GetStep(0)->preStepPz;
               cosTheta = (px*childPx + py*childPy + pz*childPz);
               cosTheta = cosTheta/preFinalP;
               cosTheta = cosTheta/sqrt(childPx*childPx + childPy*childPy + childPz*childPz);
//               std::cout << cosTheta << std::endl;
               
             }
          }
          else{
            cosTheta = 0.;
          }

          px = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPx;
          py = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPy;
          pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPz;
//          std::cout << theInt << " " << px << " " << py << " " << pz << std::endl;
          postFinalP = sqrt( px*px + py*py + pz*pz); 
          if(elastDists) elastDists->clear();
          std::vector<double> dists = theTraj->GetElastDists();
          for(size_t id = 0; id < nElast; ++id){
            elastDists->push_back(dists[id]);
          }

          if(sliceEnergy){
            sliceEnergy->clear();
            sliceInts->clear();
          }           
           
//          std::vector< std::pair<double, int> > slices = theTraj->ThinSliceMethod(.5); 
          std::vector< std::pair<double, int> > slices = theTraj->ThinSliceBetheBloch(.5); 
          for(size_t it = 0; it < slices.size(); ++it){
            sliceEnergy->push_back(slices[it].first); 
            sliceInts->push_back(slices[it].second); 
          }

          if(sliceEnergyInelastic){
            sliceEnergyInelastic->clear();
            sliceIntsInelastic->clear();
          }

          std::vector< std::pair<double, int> > slicesInelastic = theTraj->ThinSliceBetheBlochInelastic(.5);          
          for(size_t it = 0; it < slicesInelastic.size(); ++it){
            sliceEnergyInelastic->push_back(slicesInelastic[it].first); 
            sliceIntsInelastic->push_back(slicesInelastic[it].second); 
          }
//          std::cout << "New Track" << std::endl;
          std::map<int, int*>::iterator itN = mapPIDtoN.begin();
          for(; itN != mapPIDtoN.end(); ++itN){
            *(itN->second) = (theTraj->HasChild(itN->first)).size();
//            std::cout << "This track has " << *(itN->second) << " " << itN->first << std::endl;
          }
          GetInteractionType(theTraj->PID);         
          Energy = theTraj->Energy;

          GetWeightFS( theFS, preFinalP );

          tree->Fill();
           
        }
      }
}

void G4ReweightTreeParser::MakeOutputBranches(){
  theLen=0.;
  theWeight=1.;
  theElastWeight = 1.;
  theFSWeight=1.;
  N=0.;
  theInt = ""; 
  postFinalP=0.;
  preFinalP=0.;
  nElast = 0;
  elastDists = 0;
  sliceEnergy = 0;
  sliceInts = 0;
  sliceEnergyInelastic = 0;
  sliceIntsInelastic = 0;
  cosTheta = 0.;

  nPiPlus = 0;
  nPiMinus = 0;
  nPi0 = 0;
  nProton = 0;
  nNeutron = 0;

  Energy = 0.;

  tree->Branch("len", &theLen);  
  tree->Branch("weight", &theWeight);  
  tree->Branch("elastWeight", &theElastWeight);  
  tree->Branch("finalStateWeight", &theFSWeight);
  tree->Branch("N", &N);
  tree->Branch("nElast", &nElast);
  tree->Branch("elastDists", &elastDists);
  tree->Branch("sliceEnergy", &sliceEnergy);
  tree->Branch("sliceEnergyInelastic", &sliceEnergyInelastic);
  tree->Branch("Energy", &Energy);
  tree->Branch("sliceInts", &sliceInts);
  tree->Branch("sliceIntsInelastic", &sliceIntsInelastic);
  tree->Branch("int", &theInt);
  tree->Branch("postFinalP", &postFinalP);
  tree->Branch("preFinalP", &preFinalP);
  tree->Branch("cosTheta", &cosTheta);

  tree->Branch("nPiPlus", &nPiPlus);
  tree->Branch("nPiMinus", &nPiMinus);
  tree->Branch("nPi0", &nPi0);
  tree->Branch("nProton", &nProton);
  tree->Branch("nNeutron", &nNeutron);

  tree->Branch("intType", &intType);
}

/*void G4ReweightTreeParser::SetOutputBranches(){
  theLen=0.;
  theWeight=0.;
  theElastWeight = 0.;
  theFSWeight=0.;
  N=0.;
  theInt = ""; 
  postFinalP=0.;
  preFinalP=0.;
  nElast = 0;
  elastDists = 0;
  sliceEnergy = 0;
  sliceInts = 0;
  sliceEnergyInelastic = 0;
  sliceIntsInelastic = 0;
  cosTheta = 0.;

  nPiPlus = 0;
  nPiMinus = 0;
  nPi0 = 0;
  nProton = 0;
  nNeutron = 0;

  Energy = 0.;

  tree->SetBranchAddress("len", &theLen);  
  tree->SetBranchAddress("weight", &theWeight);  
  tree->SetBranchAddress("elastWeight", &theElastWeight);  
  tree->SetBranchAddress("finalStateWeight", &theFSWeight);
  tree->SetBranchAddress("N", &N);
  tree->SetBranchAddress("nElast", &nElast);
  tree->SetBranchAddress("elastDists", &elastDists);
  tree->SetBranchAddress("sliceEnergy", &sliceEnergy);
  tree->SetBranchAddress("sliceEnergyInelastic", &sliceEnergyInelastic);
  tree->SetBranchAddress("Energy", &Energy);
  tree->SetBranchAddress("sliceInts", &sliceInts);
  tree->SetBranchAddress("sliceIntsInelastic", &sliceIntsInelastic);
  tree->SetBranchAddress("int", &theInt);
  tree->SetBranchAddress("postFinalP", &postFinalP);
  tree->SetBranchAddress("preFinalP", &preFinalP);
  tree->SetBranchAddress("cosTheta", &cosTheta);

  tree->SetBranchAddress("nPiPlus", &nPiPlus);
  tree->SetBranchAddress("nPiMinus", &nPiMinus);
  tree->SetBranchAddress("nPi0", &nPi0);
  tree->SetBranchAddress("nProton", &nProton);
  tree->SetBranchAddress("nNeutron", &nNeutron);

  tree->SetBranchAddress("intType", &intType);
}
*/
