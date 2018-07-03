#ifndef G4ReweightStep_h
#define G4ReweightStep_h

#include <vector>
#include <string>
#include <utility>

struct Proc{
  std::string Name;
  double MFP;
};

//Forward Declaration to let step know of traj
class G4ReweightTraj;

class G4ReweightStep{
  public:
    
    G4ReweightStep();
    G4ReweightStep(int tid, int pid, int parid, int eventnum, double preStepP[3], double postStepP[3], double len, std::string chosenProc);
    ~G4ReweightStep();

    int trackID; 
    int eventNum;
    int PID;
    int parID;

    double preStepPx;
    double preStepPy;
    double preStepPz;
    double postStepPx;
    double postStepPy;
    double postStepPz;

    double stepLength;

    std::string stepChosenProc;
    void AddActivePostProc(std::pair< std::string, double >);
    void AddActiveAlongProc(std::pair< std::string, double >);
    void AddActivePostProc(Proc);
    void AddActiveAlongProc(Proc);

    size_t GetNActivePostProcs();
    size_t GetNActiveAlongProcs();

    Proc GetActivePostProc(size_t ip);
    Proc GetActiveAlongProc(size_t ip);
   
    //double GetWeight(double bias);

    //Function:
    //Return traj: Get pointer to the subsuming trajectory.

  private:

    std::vector<Proc> stepActivePostProcs;
    std::vector<Proc> stepActiveAlongProcs;

};

#endif
