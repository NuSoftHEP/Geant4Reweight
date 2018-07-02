#include <iostream>
#include <utility>
#include "TTree.h"
#include "TFile.h"

#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"
#include "G4ReweightTreeParser.hh"

int main(){

  G4ReweightTreeParser * tp = new G4ReweightTreeParser("try.root");
  tp->SetBranches();
  tp->FillCollection();
  tp->SortCollection();
  tp->Analyze();  
  

  return 0;
}
