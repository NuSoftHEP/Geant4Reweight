#ifndef G4ReweightStep_h
#define G4ReweightStep_h

#include <vector>
#include <string>
#include <utility>

#include <cmath>

class G4ReweightTraj;

class G4ReweightStep{
  public:
    
    G4ReweightStep();
    G4ReweightStep(int tid, int pid, int parid, int eventnum, double preStepP[3], double postStepP[3], double len, std::string chosenProc);
    ~G4ReweightStep();

    double GetFullPreStepP()  { return sqrt( preStepPx*preStepPx + preStepPy*preStepPy + preStepPz*preStepPz ); };
    double GetFullPostStepP() { return sqrt( postStepPx*postStepPx + postStepPy*postStepPy + postStepPz*postStepPz ); };

    int GetTrackID(){ return trackID; };
    int GetEventNum(){ return eventNum; };
    int GetPDG(){ return PDG; };
    int GetParID(){ return parID; };

    double GetPreStepPx(){ return preStepPx; };
    double GetPreStepPy(){ return preStepPy; };
    double GetPreStepPz(){ return preStepPz; };
    double GetPostStepPx(){ return postStepPx; };
    double GetPostStepPy(){ return postStepPy; };
    double GetPostStepPz(){ return postStepPz; };

    double GetDeltaX(){ return deltaX; };
    double GetDeltaY(){ return deltaY; };
    double GetDeltaZ(){ return deltaZ; };

    double SetDeltaX( double dX ){ deltaX = dX; };
    double SetDeltaY( double dY ){ deltaX = dY; };
    double SetDeltaZ( double dZ ){ deltaX = dZ; };

    double GetStepLength(){ return stepLength; };

    std::string GetStepChosenProc(){ return stepChosenProc; };


  private:
    int trackID; 
    int eventNum;
    int PDG;
    int parID;

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

    std::string stepChosenProc;


};

#endif
