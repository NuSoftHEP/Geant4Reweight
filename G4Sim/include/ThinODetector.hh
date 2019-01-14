#ifndef ThinODetector_h
#define ThinODetector_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ThinODetector : public G4VUserDetectorConstruction{

  public:
    ThinODetector();
    virtual ~ThinODetector();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
