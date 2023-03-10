#ifndef G4ReweightTreeParser_hh 
#define G4ReweightTreeParser_hh 1

class G4ReweightParameterMaker;
class G4ReweightThrowManager;
class G4ReweightTraj;
class G4Reweighter;

class TFile;
class TTree;

#include <map>
#include <string>
#include <utility> // std::pair
#include <vector>

enum interactionType {
      kNoInt,
      kInel,
      kCEX,
      kABS,
      kOther 
};

class G4ReweightTreeParser{

  public:
    G4ReweightTreeParser(std::string, std::string);

   ~G4ReweightTreeParser();

    void SetBranches();
    void MakeOutputBranches();

    void SetSteps(G4ReweightTraj *);

    void FillAndAnalyzeFS( G4Reweighter * );
    void FillAndAnalyzeFSThrows( TFile * FracsFile, TFile * XSecFile, G4ReweightParameterMaker & ParMaker, G4ReweightThrowManager & ThrowMan, size_t nThrows = 10 );
    void AnalyzeFS( G4Reweighter * );
    void AnalyzeFSThrows( G4Reweighter *, G4ReweightParameterMaker & ParMaker, std::map< std::string, std::vector<double> > & ThrowVals, size_t nThrows );
    void ClearCollection();
    void OpenNewInput( std::string );
    void CloseInput();
    void CloseAndSaveOutput();
    void GetInteractionType(int);

    TTree * GetTree(){ return tree; };

    size_t GetNTrajs();
    G4ReweightTraj * GetTraj(size_t, size_t);

    bool skipEM = true;

    std::vector< std::pair<double,int> > ThinSliceMethod(G4ReweightTraj*,double);
    std::vector< std::pair<double,int> > ThinSliceBetheBloch(G4ReweightTraj*,double);
    std::vector< std::pair<double,int> > ThinSliceBetheBlochInelastic(G4ReweightTraj*,double);
    std::vector< std::pair<double,int> > ThinSliceMethodInelastic(G4ReweightTraj*,double);
    double BetheBloch(double);


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
