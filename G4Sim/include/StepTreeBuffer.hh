#ifndef StepTreeBuffer_h
#define StepTreeBuffer_h 1

#include <vector>
#include <map>
#include <string>

struct StepTreeBuffer{
  
  int eventNum;
  int trackID;
  int PID;
  int parID;

  std::string stepChosenProc;
  std::vector<std::string> * stepActivePostProcNames;
  std::vector<std::string> * stepActiveAlongProcNames;
  std::vector<double> * stepActivePostProcMFPs;
  std::vector<double> * stepActiveAlongProcMFPs;

  double preStepPx, preStepPy, preStepPz; 
  double postStepPx, postStepPy, postStepPz; 

  double ekin;

  int nsteps;
 
};

#endif
