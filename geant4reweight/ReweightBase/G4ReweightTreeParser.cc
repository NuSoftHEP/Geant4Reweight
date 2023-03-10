#include "geant4reweight/ReweightBase/G4ReweightTreeParser.hh"

#include "geant4reweight/ReweightBase/G4ReweightStep.hh"
#include "geant4reweight/ReweightBase/G4ReweightTraj.hh"

#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

#include <iostream>
#include <map>
#include <utility> // std::pair
#include <vector>

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
  for(int is = G4RTraj->GetStepRange().first; is < G4RTraj->GetStepRange().second; ++is){
    step->GetEntry(is);

    double preStepP[3] = {preStepPx,preStepPy,preStepPz};
    double postStepP[3] = {postStepPx,postStepPy,postStepPz};

    G4ReweightStep * G4RStep = new G4ReweightStep(sTrackID, sPID, sParID, sEventNum,
                                                  preStepP, postStepP, stepLength, *stepChosenProc);
    G4RStep->SetDeltaX( deltaX );
    G4RStep->SetDeltaY( deltaY );
    G4RStep->SetDeltaZ( deltaZ );

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

void G4ReweightTreeParser::FillAndAnalyzeFS(G4Reweighter * theFS){
  
  fout->cd();
  
  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  int prevEvent = -1;
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    //if(!(ie%1000)){std::cout << ie << std::endl;}
    //New event. Do the analysis and Delete the current collection
    if( (prevEvent > -1) && (prevEvent != tEventNum) ){
      AnalyzeFS(theFS);
      ClearCollection();
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    G4RTraj->SetEnergy( tEnergy );

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

  
  filled = true;

  AnalyzeFS(theFS);
  ClearCollection();
}

void G4ReweightTreeParser::ClearCollection(){
   for( auto itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    delete itTraj->second;
  }
  trajCollection->clear(); 
}

void G4ReweightTreeParser::AnalyzeFS(G4Reweighter * theFS){

  for( auto itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    auto theTraj = itTraj->second; 
     
    if(theTraj->GetParID() != 0)
      continue;
    if( theTraj->GetPDG() == 211 ) 
      Inel = "pi+Inelastic";
    else if( theTraj->GetPDG() == -211 ) 
      Inel = "pi-Inelastic";
    else continue;
    
    double totalDeltaZ = 0.;
    for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
      auto theStep = theTraj->GetStep(is);
      totalDeltaZ += theStep->GetDeltaZ();
    }
    if(totalDeltaZ < 0.) continue;

    theLen         = theTraj->GetTotalLength();
    theWeight = theFS->GetWeight( theTraj );

    theElastWeight = 1.;
    theInt         = theTraj->GetFinalProc();
    nElast         = theTraj->GetNElastic();

    double px = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPreStepPx();
    double py = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPreStepPy();
    double pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPreStepPz();
    preFinalP = sqrt( px*px + py*py + pz*pz); 

    if(theInt == Inel ){
       auto products = theTraj->HasChild(theTraj->GetPDG());
       if( products.size() == 1){
         auto theChild = products[0];
         double childPx = theChild->GetStep(0)->GetPreStepPx();
         double childPy = theChild->GetStep(0)->GetPreStepPy();
         double childPz = theChild->GetStep(0)->GetPreStepPz();
         cosTheta = (px*childPx + py*childPy + pz*childPz);
         cosTheta = cosTheta/preFinalP;
         cosTheta = cosTheta/sqrt(childPx*childPx + childPy*childPy + childPz*childPz);
       }
    }
    else{
      cosTheta = 0.;
    }

    px = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPostStepPx();
    py = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPostStepPy();
    pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPostStepPz();
    postFinalP = sqrt( px*px + py*py + pz*pz); 
    if(elastDists) elastDists->clear();
    std::vector<double> dists = theTraj->GetElastDists();
    for(size_t id = 0; id < nElast; ++id){
      elastDists->push_back(dists[id]);
    }

/*
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
    */
    std::map<int, int*>::iterator itN = mapPIDtoN.begin();
    for(; itN != mapPIDtoN.end(); ++itN){
      *(itN->second) = (theTraj->HasChild(itN->first)).size();
    }
    GetInteractionType(theTraj->GetPDG());         
    Energy = theTraj->GetEnergy();

    tree->Fill();
  }
}

void G4ReweightTreeParser::FillAndAnalyzeFSThrows( TFile * FracsFile, TFile * XSecFile, G4ReweightParameterMaker & ParMaker, G4ReweightThrowManager & ThrowMan, size_t nThrows ){

  std::map< std::string, std::vector< double > > ThrowVals;

  //Get the best fit values first
  std::map< std::string, double > best_fit = ThrowMan.GetBestFit();
  for( auto itVal = best_fit.begin(); itVal != best_fit.end(); ++itVal ){
    ThrowVals[ itVal->first ].push_back( itVal->second );
  }

  //Doing throws
  for( int i = 0; i < nThrows; ++i ){
    std::map< std::string, double > vals = ThrowMan.DoThrow();
    for( auto itVal = vals.begin(); itVal != vals.end(); ++itVal ){
      ThrowVals[ itVal->first ].push_back( itVal->second );
    }
  }

  G4Reweighter theFS = G4Reweighter(XSecFile, FracsFile, ParMaker.GetFSHists() );
  
  std::cout << "Filling Collection of " << track->GetEntries() << " tracks" << std::endl;
  if(skipEM){ std::cout << "NOTE: Skipping EM activity" << std::endl;}
  
  int prevEvent = -1;
  for(int ie = 0; ie < track->GetEntries(); ++ie){    
    track->GetEntry(ie);

    if(!(ie%1000)){std::cout << ie << std::endl;}
    //New event. Do the analysis and Delete the current collection
    if( (prevEvent > -1) && (prevEvent != tEventNum) ){

      AnalyzeFSThrows( &theFS, ParMaker, ThrowVals, nThrows );
      ClearCollection(); 
    }

    G4ReweightTraj * G4RTraj = new G4ReweightTraj(tTrackID, tPID, tParID, tEventNum, *tSteps);   
    G4RTraj->SetEnergy( tEnergy );

    SetSteps(G4RTraj);
   
    std::pair<size_t,size_t> thisPair = std::make_pair(tTrackID,tEventNum);
    std::pair<size_t,size_t> parentPair = std::make_pair(tParID,tEventNum);

    //Add the traj to the map
    (*trajCollection)[thisPair] = G4RTraj;

    //The particle's parent is in the map
    if( trajCollection->count( parentPair ) ){      
       (*trajCollection)[ parentPair ]->AddChild( G4RTraj ); 
    }
        
    prevEvent = tEventNum;
  }

  filled = true;
  
  AnalyzeFSThrows( &theFS, ParMaker, ThrowVals, nThrows );
  ClearCollection();
}

void G4ReweightTreeParser::AnalyzeFSThrows( G4Reweighter *theFS, G4ReweightParameterMaker & ParMaker, std::map< std::string, std::vector<double> > & ThrowVals, size_t nThrows){

  //+1 to include best fit values
  for( size_t i = 0; i < (nThrows+1); ++i ){
    std::map< std::string, double > temp_throw;
    for( auto itPar = ThrowVals.begin(); itPar != ThrowVals.end(); ++itPar ){
      temp_throw[ itPar->first ] = itPar->second.at(i);
    }

    ParMaker.SetNewVals( temp_throw );       
    theFS->SetNewHists( ParMaker.GetFSHists() );

    std::map< std::pair<size_t,size_t>, G4ReweightTraj* >::iterator itTraj = trajCollection->begin();
    for( ; itTraj != trajCollection->end(); ++itTraj){
      auto theTraj = itTraj->second; 
       
      if(theTraj->GetParID() != 0)continue;
      if( theTraj->GetPDG() == 211 ) Inel = "pi+Inelastic";
      else if( theTraj->GetPDG() == -211 ) Inel = "pi-Inelastic";
      else continue;

      theTraj->AddWeight( theFS->GetWeight( theTraj ) );
    }

  }
  
  for( auto itTraj = trajCollection->begin(); itTraj != trajCollection->end(); ++itTraj){
    auto theTraj = itTraj->second; 
     
    if(theTraj->GetParID() != 0)continue;
    if( theTraj->GetPDG() == 211 ) Inel = "pi+Inelastic";
    else if( theTraj->GetPDG() == -211 ) Inel = "pi-Inelastic";
    else continue;
    
    //       if (theTraj->parID == 0 && theTraj->PID == 211){
      //Skip any that exit out the back
      double totalDeltaZ = 0.;
      for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
        auto theStep = theTraj->GetStep(is);
        totalDeltaZ += theStep->GetDeltaZ();
      }
      if(totalDeltaZ < 0.) continue;

      theLen         = theTraj->GetTotalLength();

      theElastWeight = 1.;
      theInt         = theTraj->GetFinalProc();
      nElast         = theTraj->GetNElastic();

      double px = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPreStepPx();
      double py = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPreStepPy();
      double pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPreStepPz();
      preFinalP = sqrt( px*px + py*py + pz*pz); 

      if(theInt == Inel ){
         auto products = theTraj->HasChild(theTraj->GetPDG());
         if( products.size() == 1){
           auto theChild = products[0];
           double childPx = theChild->GetStep(0)->GetPreStepPx();
           double childPy = theChild->GetStep(0)->GetPreStepPy();
           double childPz = theChild->GetStep(0)->GetPreStepPz();
           cosTheta = (px*childPx + py*childPy + pz*childPz);
           cosTheta = cosTheta/preFinalP;
           cosTheta = cosTheta/sqrt(childPx*childPx + childPy*childPy + childPz*childPz);
         }
      }
      else{
        cosTheta = 0.;
      }

      px = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPostStepPx();
      py = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPostStepPy();
      pz = theTraj->GetStep( theTraj->GetNSteps() - 1)->GetPostStepPz();
      postFinalP = sqrt( px*px + py*py + pz*pz); 
      if(elastDists) elastDists->clear();
      std::vector<double> dists = theTraj->GetElastDists();
      for(size_t id = 0; id < nElast; ++id){
        elastDists->push_back(dists[id]);
      }

/*
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
      */
      std::map<int, int*>::iterator itN = mapPIDtoN.begin();
      for(; itN != mapPIDtoN.end(); ++itN){
        *(itN->second) = (theTraj->HasChild(itN->first)).size();
      }
      GetInteractionType(theTraj->GetPDG());         
      Energy = theTraj->GetEnergy();

      bool is_piminus = false;
      if( theTraj->GetPDG() == -211 )
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

std::vector< std::pair<double, int> > G4ReweightTreeParser::ThinSliceMethod(G4ReweightTraj* theTraj, double res){

  std::vector< std::pair<double, int> > result;

  double sliceEnergy = 0.;  
  
  //First slice position
  double sliceEdge = res;
  double lastPos = 0.;
  double nextPos = 0.;
  double px,py,pz;
  
  int interactInSlice = 0;

  for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
    
    auto theStep = theTraj->GetStep(is);
    nextPos = lastPos + theStep->GetDeltaZ(); 
    px = theStep->GetPreStepPx(); 
    py = theStep->GetPreStepPy(); 
    pz = theStep->GetPreStepPz(); 
    sliceEnergy = sqrt( (px*px + py*py + pz*pz) + 139.57*139.57);

    std::string theProc = theStep->GetStepChosenProc(); 
//    std::cout << "StepLen: " << theStep->stepLength << " Proc: " << theProc << std::endl;
    if( (theProc == "hadElastic" || theProc == Inel) ) interactInSlice++;
    //std::cout << nextPos << " " << sliceEdge << " " << theProc << std::endl;
     
    //Passed the slice edge or it's the last step, save Energy
    if( nextPos > sliceEdge || is == (theTraj->GetNSteps() - 1) ){
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) ); 
      interactInSlice = 0;
      sliceEdge = res*ceil(nextPos/res);
    }


    lastPos = nextPos;
  }


  
  return result;
}

std::vector< std::pair<double, int> > G4ReweightTreeParser::ThinSliceBetheBloch(G4ReweightTraj* theTraj, double res){

  std::vector< std::pair<double, int> > result;
  
  //First slice position
  double sliceEdge = res;
  double lastPos = 0.;
  double nextPos = 0.;
  double px,py,pz; 
  int interactInSlice = 0;

  //Get total distance traveled in z
  double totalDeltaZ = 0.;
  double disp = 0.;
  double oldDisp = 0.;
  int crossedSlices = 0; 

  int currentSlice = 0;
  int oldSlice = 0;

  double sliceEnergy = Energy;
  for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
    auto theStep = theTraj->GetStep(is);

    disp += theStep->GetDeltaZ();
    currentSlice = floor(disp/res);
    
    std::string theProc = theStep->GetStepChosenProc(); 
    
    //Check to see if in a new slice and it's not the end
    if( oldSlice != currentSlice && is < theTraj->GetNSteps() - 1){ 


      //Save Interaction info of the prev slice
      //and reset
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
      interactInSlice = 0;

      //Update the energy
      sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
      if( sliceEnergy - 139.57 < 0.){
        //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
        //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
        sliceEnergy = 0.0001;
      }    
      //If it's more than 1 slice, add in non-interacting slices
      for(int ic = 1; ic < abs( oldSlice - currentSlice ); ++ic){

        result.push_back( std::make_pair(sliceEnergy, 0.) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }      
      
      if( (theProc == "hadElastic" || theProc == Inel) ) interactInSlice++;      
    }
    //It's crossed a slice and it's the last step. Save both info
    else if( oldSlice != currentSlice && is == theTraj->GetNSteps() - 1 ){
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
      interactInSlice = 0;
      
      //Update the energy
      sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
      if( sliceEnergy - 139.57 < 0.){
        //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
        //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
        sliceEnergy = 0.0001;
      }
      //If it's more than 1 slice, add in non-interacting slices
      for(int ic = 1; ic < abs( oldSlice - currentSlice ); ++ic){

        result.push_back( std::make_pair(sliceEnergy, 0.) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }
      
      //Save the last slice
      if( (theProc == "hadElastic" || theProc == Inel) ) interactInSlice++;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //It's the end, so just save this last info
    else if( oldSlice == currentSlice && is == theTraj->GetNSteps() - 1 ){
      if( (theProc == "hadElastic" || theProc == Inel) ) interactInSlice++;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //Same slice, not the end. Check for interactions
    else{
      if( (theProc == "hadElastic" || theProc == Inel) ) interactInSlice++;
    }

    //Update oldslice
    oldSlice = currentSlice;
  }

  return result;
}

double G4ReweightTreeParser::BetheBloch(double energy){
  
  //Need to make this configurable? Or delete...
  double K = .307075;   
  double rho = 1.390; 
  double Z = 18;
  double A = 40;
  double I = 188E-6;
  double mass = 139.57;
  double me = .511;
  //Need to make sure this is total energy, not KE
  double gamma = energy/mass;
  double beta = sqrt( 1. - (1. / (gamma*gamma)) );  double Tmax = 2 * me * beta*beta * gamma*gamma;

  double first = K * (Z/A) * rho / (beta*beta);
  double second = .5 * log(Tmax*Tmax/(I*I)) - beta*beta;

  double dEdX = first*second;
  return dEdX;  
}

std::vector< std::pair<double, int> > G4ReweightTreeParser::ThinSliceMethodInelastic(G4ReweightTraj* theTraj, double res){

  std::vector< std::pair<double, int> > result;
  double sliceEnergy = 0.;  
  
  //First slice position
  double sliceEdge = res;
  double lastPos = 0.;
  double nextPos = 0.;
  double px,py,pz;
  
  int interactInSlice = 0;

  for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
    
    auto theStep = theTraj->GetStep(is);
    nextPos = lastPos + theStep->GetDeltaZ(); 
    px = theStep->GetPreStepPx(); 
    py = theStep->GetPreStepPy(); 
    pz = theStep->GetPreStepPz(); 
    sliceEnergy = sqrt( (px*px + py*py + pz*pz) + 139.57*139.57);

    std::string theProc = theStep->GetStepChosenProc(); 
    if( (theProc == Inel) ) interactInSlice++;
     
    //Passed the slice edge or it's the last step, save Energy
    if( nextPos > sliceEdge || is == (theTraj->GetNSteps() - 1) ){
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) ); 
      interactInSlice = 0;
      sliceEdge = res*ceil(nextPos/res);
    }

    lastPos = nextPos;
  }
  return result;
}

std::vector< std::pair<double, int> > G4ReweightTreeParser::ThinSliceBetheBlochInelastic(G4ReweightTraj * theTraj, double res){

  std::vector< std::pair<double, int> > result;
  
  //First slice position
  double sliceEdge = res;
  double lastPos = 0.;
  double nextPos = 0.;
  double px,py,pz; 
  int interactInSlice = 0;

  //Get total distance traveled in z
  double totalDeltaZ = 0.;
  double disp = 0.;
  double oldDisp = 0.;
  int crossedSlices = 0; 

  int currentSlice = 0;
  int oldSlice = 0;

  double sliceEnergy = Energy;
  for(size_t is = 0; is < theTraj->GetNSteps(); ++is){
    auto theStep = theTraj->GetStep(is);

    disp += theStep->GetDeltaZ();
    currentSlice = floor(disp/res);
    
    std::string theProc = theStep->GetStepChosenProc(); 
    
    //Check to see if in a new slice and it's not the end
    if( oldSlice != currentSlice && is < theTraj->GetNSteps() - 1){ 


      //Save Interaction info of the prev slice
      //and reset
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
      interactInSlice = 0;

      //Update the energy
      sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
      if( sliceEnergy - 139.57 < 0.){
        //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
        //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
        sliceEnergy = 0.0001;
      }    
      //If it's more than 1 slice, add in non-interacting slices
      for(int ic = 1; ic < abs( oldSlice - currentSlice ); ++ic){

        result.push_back( std::make_pair(sliceEnergy, 0.) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }      
      
      if( theProc == Inel ) interactInSlice++;      
    }
    //It's crossed a slice and it's the last step. Save both info
    else if( oldSlice != currentSlice && is == theTraj->GetNSteps() - 1 ){
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
      interactInSlice = 0;
      
      //Update the energy
      sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
      if( sliceEnergy - 139.57 < 0.){
        //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
        //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
        sliceEnergy = 0.0001;
      }
      //If it's more than 1 slice, add in non-interacting slices
      for(int ic = 1; ic < abs( oldSlice - currentSlice ); ++ic){

        result.push_back( std::make_pair(sliceEnergy, 0.) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }
      
      //Save the last slice
      if( theProc == Inel ) interactInSlice++;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //It's the end, so just save this last info
    else if( oldSlice == currentSlice && is == theTraj->GetNSteps() - 1 ){
      if( theProc == Inel ) interactInSlice++;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //Same slice, not the end. Check for interactions
    else{
      if( theProc == Inel ) interactInSlice++;
    }

    //Update oldslice
    oldSlice = currentSlice;
  }

  return result;
}

void G4ReweightTreeParser::CloseAndSaveOutput()
{
  fout->cd();
  tree->Write();
  delete tree;
  fout->Delete("tree");
  fout->Close();
}
