#include <iostream>
#include <utility>
#include <string>
#include "TTree.h"
#include "TFile.h"

#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"
#include "G4ReweightTreeParser.hh"

std::string fileName = "try.root"; 
std::string outFileName = "outtry.root";
double weight = 1.;
double elastWeight = 1.;

bool parseArgs(int argc, char ** argv);

int main(int argc, char ** argv){

  if(!parseArgs(argc, argv)) {return 0;}
  std::cout << fileName << " " << outFileName << " " << weight << " " << elastWeight << std::endl;
  
  G4ReweightTreeParser * tp = new G4ReweightTreeParser(fileName.c_str(), outFileName.c_str());
  tp->SetBranches();
  tp->FillAndAnalyze(weight,elastWeight);
  tp->CloseInput();

  std::cout << "done" << std::endl;

  return 0;
}

bool parseArgs(int argc, char ** argv){
  
  for(int i = 1; i < argc; ++i){
    if( strcmp(argv[i], "--help") == 0){
      std::cout << "Usage: ./G4Reweight -f inputFile -o outFile -i inelasticWeight -e elasticWeight" << std::endl;
      return false;
    }
    if( strcmp(argv[i], "-f") == 0){
      fileName = argv[i + 1];
    }
    else if( strcmp(argv[i], "-o") == 0){
      outFileName = argv[i + 1];       
    }
    else if( strcmp(argv[i], "-i") == 0){
      weight = atof(argv[i + 1]);
    }
    else if( strcmp(argv[i], "-e") == 0){
      elastWeight = atof(argv[i + 1]); 
    }
  }
  return true;
}
