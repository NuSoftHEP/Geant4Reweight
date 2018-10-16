#ifndef G4SimDetectorConstruction_h
#define G4SimDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class G4SimDetectorConstruction : public G4VUserDetectorConstruction{

  public:
    G4SimDetectorConstruction();
    virtual ~G4SimDetectorConstruction();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
