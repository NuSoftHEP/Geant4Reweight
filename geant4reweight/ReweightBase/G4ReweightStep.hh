#ifndef G4ReweightStep_h
#define G4ReweightStep_h

#include <cmath>
#include <string>

class G4ReweightStep{
 public:
   G4ReweightStep(int tid, int pid, int parid, int eventnum, double preStepP[3], double postStepP[3], double len, std::string chosenProc);

   double GetFullPreStepP() const {
     return sqrt(preStepPx*preStepPx + preStepPy*preStepPy + preStepPz*preStepPz);
   };
   double GetFullPostStepP() const {
     return sqrt(postStepPx*postStepPx + postStepPy*postStepPy + postStepPz*postStepPz);
   };

   int GetTrackID() const {
     return trackID;
   };
   int GetEventNum() const {
     return eventNum;
   };
   int GetPDG() const {
     return PDG;
   };
   int GetParID() const {
     return parID;
   };

   double GetPreStepPx() const {
     return preStepPx;
   };
   double GetPreStepPy() const {
     return preStepPy;
   };
   double GetPreStepPz() const {
     return preStepPz;
   };
   double GetPostStepPx() const {
     return postStepPx;
   };
   double GetPostStepPy() const {
     return postStepPy;
   };
   double GetPostStepPz() const {
     return postStepPz;
   };

   double GetDeltaX() const {
     return deltaX;
   };
   double GetDeltaY() const {
     return deltaY;
   };
   double GetDeltaZ() const {
     return deltaZ;
   };

   void SetDeltaX(double dX) {
     deltaX = dX;
   };
   void SetDeltaY(double dY) {
     deltaY = dY;
   };
   void SetDeltaZ(double dZ) {
     deltaZ = dZ;
   };

   double GetStepLength() const {
     return stepLength;
   };

   std::string GetStepChosenProc() const {
     return stepChosenProc;
   };

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
