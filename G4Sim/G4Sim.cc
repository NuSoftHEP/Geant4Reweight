#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4SimDetectorConstruction.hh"
#include "G4SimPhysicsList.hh"
#include "G4SimActionInitialization.hh"

char * n;

int main(int argc, char ** argv){

  G4RunManager * runManager = new G4RunManager;
  
  //Define and create detector volume  
  runManager->SetUserInitialization(new G4SimDetectorConstruction);

  //Define the list of particles to be simulated
  //and on which models their behaviors are based
  runManager->SetUserInitialization(new G4SimPhysicsList);

  //Define the actions taken during various stages of the run
  //i.e. generating particles
  runManager->SetUserInitialization(new G4SimActionInitialization(argv[2]));

  G4cout << "Init"<<G4endl;
  runManager->Initialize();
  G4cout << "Done"<< G4endl;

 /* G4UImanager * UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/verbose 1");
  UI->ApplyCommand("/event/verbose 1");*/

  //read a macro file of commands
  G4UImanager * UI = G4UImanager::GetUIpointer();
  G4String command = "/control/execute ";
  G4String fileName = argv[1];
  G4cout <<"Applying Command" <<G4endl;
  UI->ApplyCommand(command+fileName);
  G4cout <<"Done"<<G4endl;

//  int nEvents = 5;
//  runManager->BeamOn(nEvents);  

  delete runManager;
  return 0;
}


