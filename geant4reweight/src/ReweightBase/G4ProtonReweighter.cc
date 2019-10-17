#include "G4ProtonReweighter.hh"

std::string G4ProtonReweighter::GetInteractionSubtype( G4ReweightTraj & theTraj ){
    return "total";
}

G4ProtonReweighter::~G4ProtonReweighter(){}
