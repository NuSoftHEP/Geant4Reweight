#ifndef G4ReweightTreeParser_hh 
#define G4ReweightTreeParser_hh 1

#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"

#include "TFile.h"
#include "TTree.h"

class G4ReweightTreeParser{

  public:
    G4ReweightTreeParser(std::string inputFileName);
   ~G4ReweightTreeParser();
    void SetBranches();
    void GetSteps(G4ReweightTraj *);
    void FillCollection();

  private:
    TFile * fin;
    TTree * track;
    TTree * step;

    std::vector<G4ReweightTraj *> trajCollection;

    int tPID;
    int tTrackID;
    int tEventNum;
    int tParID;
    std::pair<int,int> * tSteps = new std::pair<int,int>(0,0); 
  
    int sPID;
    int sTrackID; 
    int sEventNum;
    int sParID;

    double preStepPx;
    double preStepPy;
    double preStepPz;
    double postStepPx;
    double postStepPy;
    double postStepPz;  

    std::vector<std::string> * stepActivePostProcNames = 0;
    std::vector<std::string> * stepActiveAlongProcNames = 0;
    std::vector<double> * stepActivePostProcMFPs = 0;
    std::vector<double> * stepActiveAlongProcMFPs = 0;

    std::string * stepChosenProc = 0;
};

#endif
