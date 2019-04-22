#ifndef ThinCDetector_h
#define ThinCDetector_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ThinCDetector : public G4VUserDetectorConstruction{

  public:
    ThinCDetector();
    virtual ~ThinCDetector();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
