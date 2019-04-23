#include "G4ReweightTreeParser.hh"
#include <iostream>
#include <algorithm>
#include "TH1D.h"

G4ReweightTreeParser::G4ReweightTreeParser(std::string fInputFileName, std::string fOutputFileName){
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

void G4ReweightTreeParser::CloseInput(){

  delete track;  
  delete step;

  fin->Delete("track");
  fin->Delete("step");

  fin->Close();  
  delete fin;
}

G4ReweightTreeParser::~G4ReweightTreeParser(){
  stepActivePostProcNames->clear();
  stepActivePostProcMFPs->clear();

  std::map< std::pair< size_t, size_t >, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
  for( ; itTraj != trajCollection->end(); ++itTraj ){
    delete itTraj->second;
  }
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
  step->SetBranchAddress("stepActivePostProcMFPs", &stepActivePostProcMFPs);
  step->SetBranchAddress("stepLen", &stepLength);
  step->SetBranchAddress("deltaX", &deltaX);
  step->SetBranchAddress("deltaY", &deltaY);
  step->SetBranchAddress("deltaZ", &deltaZ);
}

void G4ReweightTreeParser::SetSteps(G4ReweightTraj * G4RTraj){
  for(int is = G4RTraj->stepRange.first; is < G4RTraj->stepRange.second; ++is){
    step->GetEntry(is);

    double preStepP[3] = {preStepPx,preStepPy,preStepPz};
    double postStepP[3] = {postStepPx,postStepPy,postStepPz};

    G4ReweightStep * G4RStep = new G4ReweightStep(sTrackID, sPID, sParID, sEventNum,
                                                  preStepP, postStepP, stepLength, *stepChosenProc);
    G4RStep->deltaX = deltaX;
    G4RStep->deltaY = deltaY;
    G4RStep->deltaZ = deltaZ;

    G4RTraj->AddStep(G4RStep);
  }

}

size_t G4ReweightTreeParser::GetNTrajs(){ 
  return trajCollection->size();
}

G4ReweightTraj* G4ReweightTreeParser::GetTraj(size_t eventIndex, size_t trackIndex){
  if(!GetNTrajs()){
    std::cout << "Traj collection is empty" << std::endl;
    return NULL;
  }
  else if( !( trajCollection->count( std::make_pair(trackIndex, eventIndex) ) ) ){
    std::cout << "No traj matching trackID, eventNum: " << trackIndex << " " << eventIndex << std::endl;
    return NULL;
  }
  else{
    return (*trajCollection)[std::make_pair(trackIndex, eventIndex)];
  }
}


void G4ReweightTreeParser::GetInteractionType(int thePID){
 
  if( abs(thePID) != 211 ) intType = kNoInt; 
  else{

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

//Make this use the new style of weighting from 'G4ReweightFinalState'
void G4ReweightTreeParser::FillAndAnalyzeFS(G4ReweightFinalState * theFS){
  
  std::cout << "Saving Variations from FinalState reweighter" << std::endl;
  fout->cd();
  
  theFS->GetTotalVariationGraph()->Write( "TotalVar" );
  std::vector< std::string > cuts = {"abs", "inel", "cex", "dcex", "prod"};
  for( size_t iC = 0; iC < cuts.size(); ++iC ){
    theFS->GetExclusiveVariationGraph( cuts.at(iC) )->Write( (cuts.at(iC) + "Var").c_str() );
    theFS->GetOldGraph( cuts.at(iC) )->Write( (cuts.at(iC) + "Old").c_str() );
    theFS->GetNewGraph( cuts.at(iC) )->Write( (cuts.at(iC) + "New").c_str() );
  }

  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  int prevEvent = -1;
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    //if(!(ie%1000)){std::cout << ie << std::endl;}
    //New event. Do the analysis and Delete the current collection
    if( (prevEvent > -1) && (prevEvent != tEventNum) ){

      AnalyzeFS(theFS);

      //Make into its own method
      for( auto itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj ){
        delete itTraj->second;
      }
      trajCollection->clear();
      
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    G4RTraj->Energy = tEnergy;

    SetSteps(G4RTraj);
   
    std::pair<size_t,size_t> thisPair = std::make_pair(tTrackID,tEventNum);
    std::pair<size_t,size_t> parentPair = std::make_pair(tParID,tEventNum);

    //Add the traj to the map
    (*trajCollection)[thisPair] = G4RTraj;

    //The particle's parent is in the map
    if( trajCollection->count( parentPair ) )      
       (*trajCollection)[ parentPair ]->AddChild( G4RTraj ); 

    prevEvent = tEventNum;
  }

  
  std::cout << "Got " << GetNTrajs() << " trajectories" << std::endl;
  filled = true;

  std::cout << "Event: " << prevEvent << std::endl;
  
  AnalyzeFS(theFS);

  for( auto itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    delete itTraj->second;
  }
  trajCollection->clear();

}

//Make this use the new style of weighting from 'G4ReweightFinalState'
void G4ReweightTreeParser::AnalyzeFS(G4ReweightFinalState * theFS){

  std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
  for( ; itTraj != trajCollection->end(); ++itTraj){
    auto theTraj = itTraj->second; 
     
    if(theTraj->parID != 0)
      continue;
    if( theTraj->PID == 211 ) 
      Inel = "pi+Inelastic";
    else if( theTraj->PID == -211 ) 
      Inel = "pi-Inelastic";
    else continue;
    
    double totalDeltaZ = 0.;
    for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
      auto theStep = theTraj->GetStep(is);
      totalDeltaZ += theStep->deltaZ;
    }
    if(totalDeltaZ < 0.) continue;

    theLen         = theTraj->GetTotalLength();
    theWeight = theFS->GetWeight( theTraj );

    theElastWeight = 1.;
    theInt         = theTraj->GetFinalProc();
    nElast         = theTraj->GetNElastic();

    double px = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPx;
    double py = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPy;
    double pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPz;
    preFinalP = sqrt( px*px + py*py + pz*pz); 

    if(theInt == Inel ){
       auto products = theTraj->HasChild(theTraj->PID);
       if( products.size() == 1){
         auto theChild = products[0];
         double childPx = theChild->GetStep(0)->preStepPx;
         double childPy = theChild->GetStep(0)->preStepPy;
         double childPz = theChild->GetStep(0)->preStepPz;
         cosTheta = (px*childPx + py*childPy + pz*childPz);
         cosTheta = cosTheta/preFinalP;
         cosTheta = cosTheta/sqrt(childPx*childPx + childPy*childPy + childPz*childPz);
       }
    }
    else{
      cosTheta = 0.;
    }

    px = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPx;
    py = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPy;
    pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPz;
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
    std::map<int, int*>::iterator itN = mapPIDtoN.begin();
    for(; itN != mapPIDtoN.end(); ++itN){
      *(itN->second) = (theTraj->HasChild(itN->first)).size();
    }
    GetInteractionType(theTraj->PID);         
    Energy = theTraj->Energy;

    tree->Fill();
  }
}

void G4ReweightTreeParser::FillAndAnalyzeFSThrows( TFile * FracsFile, TFile * XSecFile, G4ReweightParameterMaker & ParMaker, G4ReweightThrowManager & ThrowMan, size_t nThrows ){

  std::map< std::string, std::vector< double > > ThrowVals;
  //Doing throws
  for( int i = 0; i < nThrows; ++i ){

    std::map< std::string, double > vals = ThrowMan.DoThrow();
    for( auto itVal = vals.begin(); itVal != vals.end(); ++itVal ){
      ThrowVals[ itVal->first ].push_back( itVal->second );
    }
  }

  G4ReweightFinalState theFS = G4ReweightFinalState(FracsFile, ParMaker.GetFSHists() );
  theFS.SetTotalGraph(XSecFile);
  
  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  int prevEvent = -1;
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    if(!(ie%1000)){std::cout << ie << std::endl;}
    //New event. Do the analysis and Delete the current collection
    if( (prevEvent > -1) && (prevEvent != tEventNum) ){

//      std::cout << "Event: " << prevEvent << std::endl;
     
      AnalyzeFSThrows( &theFS, ParMaker, ThrowVals, nThrows );
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
  
  AnalyzeFSThrows( &theFS, ParMaker, ThrowVals, nThrows );
  std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
  for( itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    //Delete the pointer
    delete itTraj->second;
  }
  //empty the container
  trajCollection->clear();

}

void G4ReweightTreeParser::AnalyzeFSThrows( G4ReweightFinalState *theFS, G4ReweightParameterMaker & ParMaker, std::map< std::string, std::vector<double> > & ThrowVals, size_t nThrows){

  for( size_t i = 0; i < nThrows; ++i ){
    std::map< std::string, double > temp_throw;
    for( auto itPar = ThrowVals.begin(); itPar != ThrowVals.end(); ++itPar ){
      temp_throw[ itPar->first ] = itPar->second.at(i);
    }

    ParMaker.SetNewVals( temp_throw );       
    theFS->SetNewHists( ParMaker.GetFSHists() );

    std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
    for( ; itTraj != trajCollection->end(); ++itTraj){
      auto theTraj = itTraj->second; 
       
      if(theTraj->parID != 0)continue;
      if( theTraj->PID == 211 ) Inel = "pi+Inelastic";
      else if( theTraj->PID == -211 ) Inel = "pi-Inelastic";
      else continue;

      theTraj->AddWeight( theFS->GetWeight( theTraj ) );
    }

  }

  std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
  for( ; itTraj != trajCollection->end(); ++itTraj){
    auto theTraj = itTraj->second; 
     
    if(theTraj->parID != 0)continue;
    if( theTraj->PID == 211 ) Inel = "pi+Inelastic";
    else if( theTraj->PID == -211 ) Inel = "pi-Inelastic";
    else continue;
    
    //       if (theTraj->parID == 0 && theTraj->PID == 211){
      //Skip any that exit out the back
      double totalDeltaZ = 0.;
      for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
        auto theStep = theTraj->GetStep(is);
        totalDeltaZ += theStep->deltaZ;
      }
      if(totalDeltaZ < 0.) continue;

      theLen         = theTraj->GetTotalLength();

      theElastWeight = 1.;
      theInt         = theTraj->GetFinalProc();
      nElast         = theTraj->GetNElastic();

      double px = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPx;
      double py = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPy;
      double pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->preStepPz;
      preFinalP = sqrt( px*px + py*py + pz*pz); 

      if(theInt == Inel ){
         auto products = theTraj->HasChild(theTraj->PID);
         if( products.size() == 1){
           auto theChild = products[0];
           double childPx = theChild->GetStep(0)->preStepPx;
           double childPy = theChild->GetStep(0)->preStepPy;
           double childPz = theChild->GetStep(0)->preStepPz;
           cosTheta = (px*childPx + py*childPy + pz*childPz);
           cosTheta = cosTheta/preFinalP;
           cosTheta = cosTheta/sqrt(childPx*childPx + childPy*childPy + childPz*childPz);
         }
      }
      else{
        cosTheta = 0.;
      }

      px = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPx;
      py = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPy;
      pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->postStepPz;
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
      std::map<int, int*>::iterator itN = mapPIDtoN.begin();
      for(; itN != mapPIDtoN.end(); ++itN){
        *(itN->second) = (theTraj->HasChild(itN->first)).size();
      }
      GetInteractionType(theTraj->PID);         
      Energy = theTraj->Energy;

      bool is_piminus = false;
      if( theTraj->PID == -211 )
        is_piminus = true;

      //theWeight      = theTraj->GetWeight( (TH1F*)theFS->GetTotalVariation() );
      //GetWeightFS( theFS, preFinalP );


      ThrowWeights = theTraj->GetWeights();
/*    
      ThrowWeights.clear();
      AltThrowWeights.clear();


      for( size_t i = 0; i < nThrows; ++i ){
        std::map< std::string, double > temp_throw;
        for( auto itPar = ThrowVals.begin(); itPar != ThrowVals.end(); ++itPar ){
          temp_throw[ itPar->first ] = itPar->second.at(i);
        }

        ParMaker.SetNewVals( temp_throw );       
        theFS->SetNewHists( ParMaker.GetFSHists() );

        double temp_weight = theTraj->GetWeight( theFS->GetTotalVariationGraph() ); 
        temp_weight *= ReturnWeightFS( theFS, preFinalP, is_piminus );
        ThrowWeights.push_back( temp_weight );

        double temp_alt_weight = theFS->GetWeight( theTraj );
        AltThrowWeights.push_back( temp_alt_weight );

      }
*/      
      

      tree->Fill();
       
    //}
  }
}

void G4ReweightTreeParser::MakeOutputBranches(){
  theLen=0.;
  theWeight=1.;
  theElastWeight = 1.;
  theFSWeight=1.;
  altFSWeight=1.;
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
  tree->Branch("altFSWeight", &altFSWeight);
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

  tree->Branch("ThrowWeights", &ThrowWeights);
  tree->Branch("AltThrowWeights", &AltThrowWeights);
}
