#include "geant4reweight/PredictionBase/G4DecayHook.hh"

double G4DecayHook::GetMFP(const G4Track & aTrack) {
  return this->GetMeanFreePath(aTrack, 0., nullptr);
}
