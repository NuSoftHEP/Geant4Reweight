#ifndef WatArDetector_h
#define WatArDetector_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class WatArDetector : public G4VUserDetectorConstruction{

  public:
    WatArDetector();
    virtual ~WatArDetector();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
