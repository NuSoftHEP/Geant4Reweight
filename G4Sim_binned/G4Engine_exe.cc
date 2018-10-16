#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4SimDetectorConstruction.hh"
#include "G4SimPhysicsList.hh"
#include "G4SimActionInitialization.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4DynamicParticle.hh"
#include "G4ProductionCutsTable.hh"
#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <string>

int main(int argc, char ** argv){
  G4RunManager * rm = new G4RunManager;
  G4SimDetectorConstruction * detcon;
  rm->SetUserInitialization(detcon = new G4SimDetectorConstruction);
  rm->SetUserInitialization(new G4SimPhysicsList);
  //rm->SetUserInitialization(new G4SimActionInitialization);
  rm->Initialize();
  
  //Recreates actions done when you run rm->BeamOn()
  //Except actually running the events
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();

  G4cout << "Initialized" << G4endl;

  G4ParticleTable * pt = G4ParticleTable::GetParticleTable();
  G4cout << "Got Particle Table" << G4endl;
  G4ParticleDefinition * piplus = pt->FindParticle("pi+");
  G4cout << piplus->GetPDGMass() << G4endl;


///Initializing Input
 double px, py, pz, ekin;   
  double stepLen;
  std::vector<std::string> * procNames = 0;
  std::vector<double> * procMFPs = 0 ;

  TFile * fin = new TFile(argv[1],"READ"); 
  TTree * tree = (TTree*)fin->Get("step");
  tree->SetBranchAddress("preStepPx",&px);
  tree->SetBranchAddress("preStepPy",&py);
  tree->SetBranchAddress("preStepPz",&pz);
  tree->SetBranchAddress("stepLen", &stepLen);
  tree->SetBranchAddress("stepActivePostProcNames", &procNames);
  tree->SetBranchAddress("stepActivePostProcMFPs", &procMFPs);
  tree->SetBranchAddress("ekin",&ekin);


  G4cout <<"entries: " << tree->GetEntries() << G4endl;
  for(int is = 0; is < tree->GetEntries(); ++is){
    G4cout << "Getting Entry" << G4endl;
    tree->GetEntry(is);
    G4cout << "Got Entry" << G4endl;
//  double KE = 100.;  

    G4cout << "StepLen: " << stepLen << G4endl;
    G4cout << "ekin: " << ekin << G4endl;
    G4cout << "px,py,pz: " << px << py << pz << G4endl;
    G4cout << "names: " << procNames->size() << G4endl;
    G4cout << "mfps: " << procMFPs->size() << G4endl;

    double p = sqrt(px*px + py*py + pz*pz);
    double KE = sqrt(p*p + piplus->GetPDGMass()*piplus->GetPDGMass()) - piplus->GetPDGMass() ;
    G4ThreeVector dir = G4ThreeVector(px/p, py/p, pz/p);
    G4DynamicParticle * part = new G4DynamicParticle(piplus, dir, KE);
  
    G4ProcessManager * pm = piplus->GetProcessManager();
    auto pv = pm->GetProcessList();
  
    G4Step * step = new G4Step();
    G4Track * track = new G4Track(part, 0., G4ThreeVector());
    G4StepPoint * point = new G4StepPoint();
    
    //Associating the step/steppoint/track
    step->SetPreStepPoint(point);
    track->SetStep(step);
    step->SetTrack(track);
    point->SetMomentumDirection(dir);
    point->SetKineticEnergy(KE);
    track->SetKineticEnergy(KE);
    G4cout << "Track KE: " << track->GetKineticEnergy() << G4endl;
  
    G4NistManager * nist = G4NistManager::Instance();
    G4Material * water = nist->FindOrBuildMaterial("G4_WATER");
    point->SetMaterial(water);
  
    G4cout << "PM processes: " << G4endl;
  /*  for(int i = 0; i < pv->entries(); ++i){
      auto theProc = (*pv)(i);
  //    if(theProc->GetProcessName() == "CoulombScat") continue;
      
      G4cout << theProc->GetProcessName() << G4endl;
      G4ForceCondition * fc = new G4ForceCondition();    
      G4cout << theProc->GetMFP(*track, 0., fc) << G4endl;
    }*/

    auto pspv = pm->GetPostStepProcessVector(typeGPIL);
    for(int i = 0; i < pspv->entries(); ++i){
      auto theProc = (*pspv)(i);;

      G4cout << theProc->GetProcessName() << G4endl;
      G4ForceCondition * fc = new G4ForceCondition();    
      G4cout << theProc->GetMFP(*track, 0., fc) << G4endl;     
    }
    G4cout << "Saved processes: " << G4endl;
    for(int i =0; i < procNames->size(); ++i){
      G4cout << (*procNames)[i] << G4endl;
      G4cout << (*procMFPs)[i] << G4endl;
    }
  }

  rm->RunTermination();
  delete rm;
  return 0.;
}
