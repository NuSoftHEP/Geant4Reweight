#ifndef ThinFeDetector_h
#define ThinFeDetector_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ThinFeDetector : public G4VUserDetectorConstruction{

  public:
    ThinFeDetector();
    virtual ~ThinFeDetector();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
