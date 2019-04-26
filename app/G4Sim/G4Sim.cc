#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4SimDetectorConstruction.hh"
#include "G4SimPhysicsList.hh"
#include "G4SimActionInitialization.hh"

#include <utility>
#include <iostream>
#include <fstream>

#include "TH1F.h"
#include "TFile.h"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

char * n;

std::string macFileName = "../G4Sim/run1.mac";
std::string outFileName = "try.root";
std::string material_fcl_file = "empty";

bool ParseArgs(int argc, char* argv[]);

int main(int argc, char * argv[]){

  std::cout << "parsing" << std::endl;
  if(!ParseArgs(argc, argv)) {return 0;}
  std::cout << macFileName << " " << outFileName << " " << std::endl;

  G4RunManager * runManager = new G4RunManager;
  
  if( material_fcl_file != "empty" ){
    fhicl::ParameterSet pset = fhicl::make_ParameterSet( material_fcl_file ); 
    fhicl::ParameterSet theMaterial = pset.get< fhicl::ParameterSet >("Material"); 
    runManager->SetUserInitialization(new G4SimDetectorConstruction(theMaterial) );
  }
  else{
    runManager->SetUserInitialization(new G4SimDetectorConstruction);
  }

  //Define and create detector volume  
  runManager->SetUserInitialization(new G4SimPhysicsList);
  

  //Define the actions taken during various stages of the run
  //i.e. generating particles
  runManager->SetUserInitialization(new G4SimActionInitialization(outFileName));

  G4cout << "Init"<<G4endl;
  runManager->Initialize();
  G4cout << "Done"<< G4endl;


  //read a macro file of commands
  G4UImanager * UI = G4UImanager::GetUIpointer();
  G4String command = "/control/execute ";
  G4String fileName = macFileName;
  G4cout <<"Applying Command" <<G4endl;
  UI->ApplyCommand(command+fileName);
  G4cout <<"Done"<<G4endl;

  delete runManager;
  return 0;
}

bool ParseArgs(int argc, char* argv[]){

  for(int i = 1; i < argc; ++i){
    if( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 ){
      std::cout << "Usage: -c macro file -o outfile" << std::endl;
      return false;
    }
    if( strcmp(argv[i], "-c") == 0){
      macFileName = argv[i+1];      
    }
    else if( strcmp(argv[i], "-o") == 0){
      outFileName = argv[i+1];
    }
    else if( strcmp(argv[i], "-m") == 0){
      material_fcl_file = argv[i+1];
    }

  }
  return true;
}

