#ifndef G4PiCascadeDetectorConstruction_h
#define G4PiCascadeDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class G4PiCascadeDetectorConstruction : public G4VUserDetectorConstruction{

  public:
    G4PiCascadeDetectorConstruction();
    virtual ~G4PiCascadeDetectorConstruction();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
