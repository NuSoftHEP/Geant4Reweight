#include <iostream>
#include <utility>
#include <string>
#include "TTree.h"
#include "TFile.h"

#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"
#include "G4ReweightTreeParser.hh"

std::string fileName = "try.root"; 
double weight = 1.;
double elastWeight = 1.;

void parseArgs(int argc, char ** argv);

int main(int argc, char ** argv){

  parseArgs(argc, argv);
  
  G4ReweightTreeParser * tp = new G4ReweightTreeParser(fileName.c_str());
  tp->SetBranches();
  tp->FillAndAnalyze(weight,elastWeight);
  //tp->FillCollection();
  //tp->SortCollection();
  //tp->Analyze();  
  tp->CloseInput();

  std::cout << "done" << std::endl;

  return 0;
}

void parseArgs(int argc, char ** argv){
  std::cout << "Nargs: " << argc << std::endl;
  
  std::cout << argv[1] << std::endl;
  fileName = argv[1];
  
  if(argc == 4){
    weight = atof(argv[2]);
    elastWeight = atof(argv[3]);
  }
  std::cout << "Weight: " << weight << std::endl;
  std::cout << "Elast Weight: " << elastWeight << std::endl;

}
