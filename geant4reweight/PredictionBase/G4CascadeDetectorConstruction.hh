#ifndef G4CascadeDetectorConstruction_h
#define G4CascadeDetectorConstruction_h 1

#include "Geant4/G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class G4CascadeDetectorConstruction : public G4VUserDetectorConstruction{

  public:
    G4CascadeDetectorConstruction();
    G4CascadeDetectorConstruction(G4VPhysicalVolume * phys_vol);
    virtual ~G4CascadeDetectorConstruction();

    virtual G4VPhysicalVolume * Construct();


  protected:
    G4VPhysicalVolume * fPhysicalVol = nullptr;
};

#endif
