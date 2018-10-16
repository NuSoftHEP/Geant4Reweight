#ifndef ThinDetector_h
#define ThinDetector_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ThinDetector : public G4VUserDetectorConstruction{

  public:
    ThinDetector();
    virtual ~ThinDetector();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
