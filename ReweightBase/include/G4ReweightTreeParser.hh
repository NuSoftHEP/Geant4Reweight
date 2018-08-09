#ifndef G4ReweightTreeParser_hh 
#define G4ReweightTreeParser_hh 1

#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"

#include "TFile.h"
#include "TTree.h"

enum interactionType {
      kNone,
      kInel,
      kCEX,
      kABS,
      kOther 
};

class G4ReweightTreeParser{

  public:
    //Functions
    G4ReweightTreeParser(std::string, std::string);
   ~G4ReweightTreeParser();
    void SetBranches();
    void SetSteps(G4ReweightTraj *);
    void FillCollection();
    void Analyze(double,double);
    void FillAndAnalyze(double,double);
    void CloseInput();
    void GetInteractionType(int);

    size_t GetNTrajs();
  //  size_t GetNEvents();
    G4ReweightTraj * GetTraj(size_t, size_t);
//    void SortCollection();

    bool skipEM = true;

  private:

    //Data Members
    TFile * fin;
    TFile * fout;
    TTree * track;
    TTree * step;
    TTree * tree;

    double theLen;
    double theWeight;
    double theElastWeight;
    double N;
    std::string theInt; 
    double postFinalP;
    double preFinalP;
    int nElast;
    double cosTheta;
    std::vector<double> * elastDists;
    std::vector<double> * sliceEnergy;
    std::vector<int> * sliceInts;

    double Energy;
    
    std::map< std::pair<size_t,size_t>, G4ReweightTraj*  > * trajCollection = new std::map< std::pair<size_t,size_t>, G4ReweightTraj* >();
    
    //For input tree branches 
    int tPID;
    int tTrackID;
    int tEventNum;
    int tParID;
    double tEnergy;
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

    double deltaX;
    double deltaY;
    double deltaZ;

    double stepLength;
     
    int nPiPlus;
    int nPiMinus;
    int nPi0;
    int nProton;
    int nNeutron;

    std::map<int, int*> mapPIDtoN;
    int intType; 


    std::vector<std::string> * stepActivePostProcNames = 0;
    std::vector<std::string> * stepActiveAlongProcNames = 0;
    std::vector<double> *      stepActivePostProcMFPs = 0;
    std::vector<double> *      stepActiveAlongProcMFPs = 0;
    std::string *              stepChosenProc = 0;

    //To check if sorted and filled
    bool filled = false;
    bool sorted = false;
};


#endif
