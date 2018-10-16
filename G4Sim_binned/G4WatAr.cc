#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "WatArDetector.hh"
#include "G4SimPhysicsList.hh"
#include "G4SimActionInitialization.hh"

char * n;

std::string macFileName = "../G4Sim/run1.mac";
std::string outFileName = "try.root";
double elasticBias = 1.;
double inelasticBias = 1.;


bool ParseArgs(int argc, char* argv[]);

int main(int argc, char * argv[]){
 
  std::cout << "parsing" << std::endl;
  if(!ParseArgs(argc, argv)) {return 0;}
  std::cout << macFileName << " " << outFileName << " " << elasticBias << " " << inelasticBias << std::endl;

  G4RunManager * runManager = new G4RunManager;
  
  //Define and create detector volume  
  runManager->SetUserInitialization(new WatArDetector);

  //Define the list of particles to be simulated
  //and on which models their behaviors are based
  runManager->SetUserInitialization(new G4SimPhysicsList(inelasticBias, elasticBias));

  //Define the actions taken during various stages of the run
  //i.e. generating particles
  runManager->SetUserInitialization(new G4SimActionInitialization(outFileName));

  G4cout << "Init"<<G4endl;
  runManager->Initialize();
  G4cout << "Done"<< G4endl;

 /* G4UImanager * UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/verbose 1");
  UI->ApplyCommand("/event/verbose 1");*/

  //read a macro file of commands
  G4UImanager * UI = G4UImanager::GetUIpointer();
  G4String command = "/control/execute ";
  G4String fileName = macFileName;
  G4cout <<"Applying Command" <<G4endl;
  UI->ApplyCommand(command+fileName);
  G4cout <<"Done"<<G4endl;

//  int nEvents = 5;
//  runManager->BeamOn(nEvents);  

  delete runManager;
  return 0;
}

bool ParseArgs(int argc, char* argv[]){

  for(int i = 1; i < argc; ++i){
    if( strcmp(argv[i], "--help") == 0){
      std::cout << "Usage: -c macro file -o outfile -e elasticBias -i inelasticBias" << std::endl;
      return false;
    }
    if( strcmp(argv[i], "-c") == 0){
      macFileName = argv[i+1];      
    }
    else if( strcmp(argv[i], "-o") == 0){
      outFileName = argv[i+1];
    }
    else if( strcmp(argv[i], "-e") == 0){
      elasticBias = atof(argv[i+1]);
    }
    else if( strcmp(argv[i], "-i") == 0){
      inelasticBias = atof(argv[i+1]);
    }
  }
  return true;
}


