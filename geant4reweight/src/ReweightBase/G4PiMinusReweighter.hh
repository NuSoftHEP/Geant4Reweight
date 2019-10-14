#ifndef G4PiMinusReweighter_h
#define G4PiMinusReweighter_h

#include "G4Reweighter.hh"

class G4PiMinusReweighter : public G4Reweighter {
  public: 
    
    using G4Reweighter::G4Reweighter;
    virtual ~G4PiMinusReweighter();
    std::string GetInteractionSubtype( G4ReweightTraj & ) override;


  protected: 
    std::string fInelastic = "pi-Inelastic";
};

#endif

