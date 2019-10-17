#ifndef G4ProtonReweighter_h
#define G4ProtonReweighter_h

#include "G4Reweighter.hh"

class G4ProtonReweighter : public G4Reweighter {
  public: 
    
    using G4Reweighter::G4Reweighter;
    virtual ~G4ProtonReweighter();
    std::string GetInteractionSubtype( G4ReweightTraj & ) override;


  protected: 
    std::string fInelastic = "protonInelastic";
};

#endif

