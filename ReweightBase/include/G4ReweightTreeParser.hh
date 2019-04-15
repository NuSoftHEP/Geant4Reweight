#ifndef G4ReweightTreeParser_hh 
#define G4ReweightTreeParser_hh 1

#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"
#include "G4ReweightInter.hh"
#include "G4ReweightFinalState.hh"

#include "G4ReweightParameterMaker.hh"
#include "G4ReweightThrowManager.hh"

//#include "G4RunManager.hh"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

enum interactionType {
      kNoInt,
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
    void MakeOutputBranches();

    void SetSteps(G4ReweightTraj *);
    void FillCollection();

    void Analyze(double, double);
    void Analyze(TH1F *, TH1F *);
    void AnalyzeFunc(G4ReweightInter *, G4ReweightInter *);
    void FillAndAnalyze(double, double);
    void FillAndAnalyze(TH1F *, TH1F *);
    void FillAndAnalyzeFunc(G4ReweightInter *, G4ReweightInter *);

    void FillAndAnalyzeFS( G4ReweightFinalState * );
    void FillAndAnalyzeFSThrows( TFile * FracsFile, G4ReweightParameterMaker & ParMaker, G4ReweightThrowManager & ThrowMan, size_t nThrows = 10 );
    void AnalyzeFS( G4ReweightFinalState * );
    void AnalyzeFSThrows( G4ReweightFinalState *, G4ReweightParameterMaker & ParMaker, std::map< std::string, std::vector<double> > & ThrowVals, size_t nThrows );
    void GetWeightFS( G4ReweightFinalState *, double );
    double ReturnWeightFS( G4ReweightFinalState * theFS, double theMomentum, bool IsPiMinus=false );

    void OpenNewInput( std::string );
    void CloseInput();
    void CloseAndSaveOutput(){ fout->cd(); tree->Write(); delete tree; fout->Delete("tree"); fout->Close(); };
    void GetInteractionType(int);

    TTree * GetTree(){ return tree; };

    size_t GetNTrajs();
    G4ReweightTraj * GetTraj(size_t, size_t);

    bool skipEM = true;

  private:
    std::string Inel;

    //Data Members
    TFile * fin;
    TFile * fout;
    TTree * track;
    TTree * step;
    TTree * tree;

    double theLen;
    double theWeight;
    std::vector<double> ThrowWeights;
    std::vector<double> AltThrowWeights;
    double theFSWeight;
    double altFSWeight;
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
    std::vector<double> * sliceEnergyInelastic;
    std::vector<int> * sliceIntsInelastic;

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


    //Eventually: Remove active post procs
    std::vector<std::string> * stepActivePostProcNames = 0;
    std::vector<double> *      stepActivePostProcMFPs = 0;
    std::string *              stepChosenProc = 0;

    //To check if sorted and filled
    bool filled = false;
    bool sorted = false;


    //G4CrossSectionDataStore
    //G4RunManager
    //G4ProcessManager
    //G4ProcessVector
    //G4HadronInelasticProcess
    //G4HadronElasticProcess
};


#endif
