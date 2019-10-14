#ifndef G4PiPlusReweighter_h
#define G4PiPlusReweighter_h

#include "G4Reweighter.hh"

class G4PiPlusReweighter : public G4Reweighter {
  public: 
    
    using G4Reweighter::G4Reweighter;
    virtual ~G4PiPlusReweighter();
    std::string GetInteractionSubtype( G4ReweightTraj & ) override;


  protected: 
    std::string fInelastic = "pi+Inelastic";
};

#endif

