#ifndef G4ReweightStep_h
#define G4ReweightStep_h

#include <cmath>
#include <string>

class G4ReweightStep{
 public:
    
   //G4ReweightStep();
   G4ReweightStep(int tid, int pid, int parid, int eventnum, double preStepP[3], double postStepP[3], double len, std::string chosenProc);
   //~G4ReweightStep();

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

   void SetDeltaX( double dX ){ deltaX = dX; };
   void SetDeltaY( double dY ){ deltaY = dY; };
   void SetDeltaZ( double dZ ){ deltaZ = dZ; };

   double GetStepLength(){ return stepLength; };

   std::string GetStepChosenProc(){ return stepChosenProc; };

 private:
   int trackID; 
   int PDG;
   int parID;
   int eventNum;
   double stepLength;

   double preStepPx;
   double preStepPy;
   double preStepPz;
   double postStepPx;
   double postStepPy;
   double postStepPz;
   std::string stepChosenProc;

   double deltaX; //Need?
   double deltaY;
   double deltaZ;




};

#endif
