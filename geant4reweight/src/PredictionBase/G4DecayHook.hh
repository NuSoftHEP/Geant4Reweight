#ifndef G4DecayHook_h
#define G4DecayHook_h 1
#include "Geant4/G4Decay.hh"

class G4Track;

class G4DecayHook : public G4Decay {
 public:
   double GetMFP(const G4Track& aTrack) /*{
     return this->GetMFP(aTrack, 0., nullptr);
   }*/;
};
#endif
