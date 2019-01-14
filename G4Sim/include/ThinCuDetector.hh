#ifndef ThinCuDetector_h
#define ThinCuDetector_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ThinCuDetector : public G4VUserDetectorConstruction{

  public:
    ThinCuDetector();
    virtual ~ThinCuDetector();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
