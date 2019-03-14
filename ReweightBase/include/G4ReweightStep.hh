#ifndef G4ReweightStep_h
#define G4ReweightStep_h

#include <vector>
#include <string>
#include <utility>

#include <cmath>

struct Proc{
  std::string Name;
  double MFP;
};

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

    double GetFullPreStepP()  { return sqrt( preStepPx*preStepPx + preStepPy*preStepPy + preStepPz*preStepPz ); };
    double GetFullPostStepP() { return sqrt( postStepPx*postStepPx + postStepPy*postStepPy + postStepPz*postStepPz ); };

    double deltaX;
    double deltaY;
    double deltaZ;

    double stepLength;

    std::string stepChosenProc;

    //Remove post proc after trimming down
    void AddActivePostProc(std::pair< std::string, double >);
    void AddActivePostProc(Proc);

    size_t GetNActivePostProcs();

    Proc GetActivePostProc(size_t ip);
   

  private:

    //int preStepMaterialZ
    //double preStepMaterialMass
    //double preStepMaterialDensity
    //std::string preStepMaterialName
    //
    //Or pointer to G4Material

    //Remove
    std::vector<Proc> stepActivePostProcs;

};

#endif
