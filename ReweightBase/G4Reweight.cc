#include <iostream>
#include <utility>
#include <string>
#include "TTree.h"
#include "TFile.h"

#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"
#include "G4ReweightTreeParser.hh"

std::string fileName = "try.root"; 

void parseArgs(int argc, char ** argv);

int main(int argc, char ** argv){

  parseArgs(argc, argv);
  
  G4ReweightTreeParser * tp = new G4ReweightTreeParser(fileName.c_str());
  tp->SetBranches();
  tp->FillCollection();
  tp->SortCollection();
  tp->Analyze();  
  

  return 0;
}

void parseArgs(int argc, char ** argv){
  std::cout << "Nargs: " << argc << std::endl;
  if(argc == 2){
    std::cout << argv[1] << std::endl;
  }

  fileName = argv[1];
}
