#ifndef ThinPbDetector_h
#define ThinPbDetector_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ThinPbDetector : public G4VUserDetectorConstruction{

  public:
    ThinPbDetector();
    virtual ~ThinPbDetector();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
