#ifndef TreeBuffer_h
#define TreeBuffer_h 1

#include <vector>
#include <map>
#include <string>

enum theModes{
  kNone,
  kABS,
  kCEX,
  kDCEX,
  kINEL,
  kDecay,

  kCEXPiC,
  kCEXN,

  kPiMPi0,
  kPiMPiP,
  kDCEXN,

  kINELPi0,
  kINELPiP,
  kINELN
};

struct TreeBuffer{
  std::vector<int> * tid;
  std::vector<int> * pid;
  std::vector<int> * track_tid;
  std::vector<int> * track_pid;
  std::vector<std::string> * track_creator;  
  std::vector<int> * primaryPDGs;
  std::vector<double> * primaryEnergy;
  std::vector<int> * parid;

  std::vector<std::string> * postStepProcess;
  std::vector<std::string> * preStepProcess;
  std::vector<std::string> * preStepMat;
  std::vector<std::string> * postStepMat;
  std::vector<double> * ekin;
  std::vector<double> * edep;
  std::vector<double> * xs, * ys, * zs;

  int fEvent;
  
  double xi, yi, zi;

  std::vector<int> * secondaryProductIDs;
  std::vector<int> * secondaryProductPIDs;
  std::string * secondaryProcess;
  
  std::string * interactionModeName;
  int interactionMode;
  int nPi0, nPiPlus, nPiMinus, nProton, nNeutron, nNuclear, nGamma;


  std::vector< std::string > * stepChosenProc;

};

#endif
