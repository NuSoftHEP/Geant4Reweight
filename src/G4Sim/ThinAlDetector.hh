#ifndef ThinAlDetector_h
#define ThinAlDetector_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ThinAlDetector : public G4VUserDetectorConstruction{

  public:
    ThinAlDetector();
    virtual ~ThinAlDetector();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
