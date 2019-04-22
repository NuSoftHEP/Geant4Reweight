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

class G4Engine{
  private:
    G4RunManager * rm;
    G4SimDetectorConstruction * detcon;

    G4ParticleTable * pt;
    G4ParticleDefinition * currentPart;
    G4DynamicParticle * currentDynamicPart;
    G4Step * step;
    G4Track * track;
    G4StepPoint * point;
    G4Material * currentMaterial;

    G4ProcessManager * pm;

  public:
//    G4ProcessManager * GetProcessManager();
    bool SetCurrentParticle(std::string, G4ThreeVector, double);
    bool SetCurrentMaterial(G4Material *);
    double GetKE(double);
    std::vector< std::pair<std::string, double> > GetListOfProcs();
    G4Engine();
   ~G4Engine();

};

G4Engine::G4Engine(/*Det Construction, PhysicsList */){
  rm = new G4RunManager();
  rm->SetUserInitialization(detcon = new G4SimDetectorConstruction);
  rm->SetUserInitialization(new G4SimPhysicsList);
  rm->Initialize();
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();

  G4cout << "Initialized" << G4endl;
  pt = G4ParticleTable::GetParticleTable();


}

G4Engine::~G4Engine(){
  rm->RunTermination();
  delete rm;
}

double G4Engine::GetKE(double momentum){
  return sqrt(momentum*momentum + currentPart->GetPDGMass()*currentPart->GetPDGMass()) - currentPart->GetPDGMass();
}

bool G4Engine::SetCurrentMaterial(G4Material * material){
  currentMaterial = material;
  point->SetMaterial(currentMaterial);

  return (currentMaterial != NULL);
}



bool G4Engine::SetCurrentParticle(std::string partName, G4ThreeVector dir, double momentum ){
  currentPart = pt->FindParticle(partName);
  double KE = GetKE(momentum); 
  currentDynamicPart = new G4DynamicParticle(currentPart, dir, KE);
  
  step = new G4Step();
  //Don't care about position right now
  track = new G4Track(currentDynamicPart, 0., G4ThreeVector());
  point = new G4StepPoint();

  //Associating the step/steppoint/track
  step->SetPreStepPoint(point);
  track->SetStep(step);
  step->SetTrack(track);
  point->SetMomentumDirection(dir);
  point->SetKineticEnergy(KE);
  track->SetKineticEnergy(KE);

  pm = currentPart->GetProcessManager();
  return true;  
}

std::vector< std::pair<std::string, double> > G4Engine::GetListOfProcs(){
  auto pspv = pm->GetPostStepProcessVector(typeGPIL);
  std::vector< std::pair<std::string, double> > list;
  G4ForceCondition * fc = new G4ForceCondition();      
  for(int i = 0; i < pspv->entries(); ++i){
    auto theProc = (*pspv)(i);
    list.push_back( std::make_pair( theProc->GetProcessName(), theProc->GetMFP(*track, 0., fc) ) );        
    G4cout << theProc->GetProcessName() << G4endl;
    G4cout << theProc->GetMFP(*track, 0., fc) << G4endl;     
  }
  return list;
}

int main(int argc, char ** argv){
  
  G4Engine * engine = new G4Engine();



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

    G4cout << "StepLen: " << stepLen << G4endl;
    G4cout << "ekin: " << ekin << G4endl;
    G4cout << "px,py,pz: " << px << py << pz << G4endl;
    G4cout << "names: " << procNames->size() << G4endl;
    G4cout << "mfps: " << procMFPs->size() << G4endl;

    double p = sqrt(px*px + py*py + pz*pz);

    G4ThreeVector dir = G4ThreeVector(px/p, py/p, pz/p);
    G4NistManager * nist = G4NistManager::Instance();
    G4Material * water = nist->FindOrBuildMaterial("G4_WATER");
    engine->SetCurrentParticle("pi+",dir,p);
    engine->SetCurrentMaterial(water);
  
    std::vector< std::pair< std::string, double > > list = engine->GetListOfProcs();

    for(int i = 0; i < list.size(); ++i){
      G4cout << list[i].first << " " << list[i].second << G4endl;
    }

/*    for(int i = 0; i < pspv->entries(); ++i){
      auto theProc = (*pspv)(i);;

      G4cout << theProc->GetProcessName() << G4endl;
      G4ForceCondition * fc = new G4ForceCondition();    
      G4cout << theProc->GetMFP(*track, 0., fc) << G4endl;     
    }*/
    G4cout << "Saved processes: " << G4endl;
    for(int i =0; i < procNames->size(); ++i){
      G4cout << (*procNames)[i] << G4endl;
      G4cout << (*procMFPs)[i] << G4endl;
    }
  }
  
  delete engine;
  return 0.;
}
