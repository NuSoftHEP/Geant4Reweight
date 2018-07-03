#include "G4SimEventAction.hh"

G4SimEventAction::G4SimEventAction(TTree * tree, TreeBuffer * inputTreeBuffer, StepTreeBuffer * inputStepTreeBuffer, TrackTreeBuffer * inputTrackTreeBuffer) : G4UserEventAction(){

  //Pass the pointers to this class;
  tree_copy = tree;  
  G4cout << "Got tree at " << tree << G4endl;

  MyTreeBuffer = inputTreeBuffer;
  MyStepTreeBuffer = inputStepTreeBuffer;
  MyTrackTreeBuffer = inputTrackTreeBuffer;
  
}

G4SimEventAction::~G4SimEventAction(){ 
}

void G4SimEventAction::BeginOfEventAction(const G4Event * event){
  MyTreeBuffer->tid->clear();
  MyTreeBuffer->pid->clear();
  MyTreeBuffer->track_tid->clear();
  MyTreeBuffer->track_pid->clear();
  MyTreeBuffer->track_creator->clear();

  MyTreeBuffer->secondaryProductIDs->clear();
  MyTreeBuffer->secondaryProductPIDs->clear();
  MyTreeBuffer->secondaryProcess->clear();

  MyTreeBuffer->interactionModeName->clear();
  MyTreeBuffer->interactionMode = 0;

  (MyTreeBuffer->nPi0) = 0;   
  (MyTreeBuffer->nPiPlus) = 0;
  (MyTreeBuffer->nPiMinus) = 0;
  (MyTreeBuffer->nNuclear) = 0;
  (MyTreeBuffer->nProton) = 0;
  (MyTreeBuffer->nNeutron) = 0;
  (MyTreeBuffer->nGamma) = 0;

  MyTreeBuffer->primaryPDGs->clear();
  MyTreeBuffer->primaryEnergy->clear();
  MyTreeBuffer->parid->clear();
  MyTreeBuffer->ekin->clear();
  MyTreeBuffer->edep->clear();
  MyTreeBuffer->xs->clear();
  MyTreeBuffer->ys->clear();
  MyTreeBuffer->zs->clear();
  MyTreeBuffer->preStepMat->clear();
  MyTreeBuffer->postStepMat->clear();

  MyTreeBuffer->postStepProcess->clear();
  MyTreeBuffer->preStepProcess->clear();

  MyStepTreeBuffer->eventNum  = event->GetEventID();
  MyTrackTreeBuffer->eventNum = event->GetEventID();
}

void G4SimEventAction::EndOfEventAction(const G4Event * event){

  G4PrimaryVertex* vtx = event->GetPrimaryVertex();

  //Positions
  MyTreeBuffer->xi = vtx->GetX0();
  MyTreeBuffer->yi = vtx->GetY0();
  MyTreeBuffer->zi = vtx->GetZ0();

  //Primary particles
  int nPrimary = vtx->GetNumberOfParticle();
  G4cout << "Got primaries: " << nPrimary << G4endl;
  for(int ip = 0; ip < nPrimary; ++ip){
    G4PrimaryParticle * part = vtx->GetPrimary(ip);    
    G4cout<< "\t" << ip << " " << part->GetPDGcode() << G4endl;
    MyTreeBuffer->primaryPDGs->push_back(part->GetPDGcode());
    MyTreeBuffer->primaryEnergy->push_back(part->GetTotalEnergy());
  }

  G4cout << "track vec size" << MyTreeBuffer->track_tid->size() << G4endl; 

  MyTreeBuffer->fEvent = event->GetEventID();

  G4cout << "Event: " << event->GetEventID() << G4endl;
  G4cout << "Event: " << MyTreeBuffer->fEvent << G4endl;
  G4cout << "primarypdgs " << MyTreeBuffer->primaryPDGs << G4endl;

  //Secondary Interaction:
  int nSecondary = MyTreeBuffer->secondaryProductPIDs->size();
  int nPi0 = 0;
  int nPiPlus = 0;
  int nPiMinus = 0;
  int nProton = 0;
  int nNeutron = 0;
  int nNuclear = 0;
  int nGamma = 0;
  for(int is = 0; is < nSecondary; ++is){

    int thisPID = MyTreeBuffer->secondaryProductPIDs->at(is);
    if(thisPID > 1000000000){
      nNuclear++;
    }
    else{
      switch (MyTreeBuffer->secondaryProductPIDs->at(is)){
        case 111:
          nPi0++; break;
        case 211:
          nPiPlus++; break;
        case -211:
          nPiMinus++; break;
        case 2212:
          nProton++; break;
        case 2112:
          nNeutron++; break;          
        case 22:
          nGamma++; break;
      }
    }
  }

  if(*(MyTreeBuffer->secondaryProcess) == "Decay"){
    *(MyTreeBuffer->interactionModeName) = "Decay";
    MyTreeBuffer->interactionMode = kDecay;
  }
  else if(*(MyTreeBuffer->secondaryProcess) == "pi+Inelastic"){

    //Do some checks on the interaction mode here
    if(nPi0 > 0){
      if(nPiPlus > 0 || nPiMinus > 0){
        G4cout << "Pi0 and Pi+/-!!!!!!!!" << G4endl;
        *(MyTreeBuffer->interactionModeName) = "CEX+pic";
        MyTreeBuffer->interactionMode = kCEXPiC;
      }
      else if(nPi0 == 1){
        *(MyTreeBuffer->interactionModeName) = "CEX";
        MyTreeBuffer->interactionMode = kCEX;
      }
      else if(nPi0 > 1){
        *(MyTreeBuffer->interactionModeName) = "CEXN";
        MyTreeBuffer->interactionMode = kCEXN;
      }      
    }
    else if(nPiPlus > 0){
      if(nPi0 > 0){
        G4cout <<"PiPlus and Pi0!!!!!!!"  << G4endl;
        *(MyTreeBuffer->interactionModeName) = "INEL+Pi0";
        MyTreeBuffer->interactionMode = kINELPi0;
      }
      else if(nPiMinus > 0){
        G4cout << "PiPlus and PiMinus!!!!!" << G4endl;
        *(MyTreeBuffer->interactionModeName) = "INEL+Pi-";
        MyTreeBuffer->interactionMode = kINELPiP;
      }
      else if(nPiPlus > 1){
        *(MyTreeBuffer->interactionModeName) = "INEL N";
        MyTreeBuffer->interactionMode = kINELN;
      }
      else{ 
        *(MyTreeBuffer->interactionModeName) = "INEL";
        MyTreeBuffer->interactionMode = kINEL;
      }
    }
    else if(nPiMinus > 0){
      if(nPi0 > 0){
        *(MyTreeBuffer->interactionModeName) = "Pi-+Pi0";
        MyTreeBuffer->interactionMode = kPiMPi0;
      }
      else if(nPiPlus > 0){
        *(MyTreeBuffer->interactionModeName) = "Pi-+Pi+";
        MyTreeBuffer->interactionMode = kPiMPiP;
      }
      else if(nPiMinus > 1){
        *(MyTreeBuffer->interactionModeName) = "DCEX N";
        MyTreeBuffer->interactionMode = kDCEXN;
      }
      else{
        *(MyTreeBuffer->interactionModeName) = "DCEX";
        MyTreeBuffer->interactionMode = kDCEX;
      }
    }
    else if(nPi0 == 0 && nPiPlus == 0 && nPiMinus == 0){
      *(MyTreeBuffer->interactionModeName) = "ABS";
      MyTreeBuffer->interactionMode = kABS;
    }    

  }

  (MyTreeBuffer->nPi0)     = nPi0; 
  (MyTreeBuffer->nPiPlus)  = nPiPlus; 
  (MyTreeBuffer->nPiMinus) = nPiMinus;
  (MyTreeBuffer->nProton)  = nProton; 
  (MyTreeBuffer->nNeutron) = nNeutron;
  (MyTreeBuffer->nNuclear) = nNuclear;
  (MyTreeBuffer->nGamma)   = nGamma;  
  tree_copy->Fill();
}
