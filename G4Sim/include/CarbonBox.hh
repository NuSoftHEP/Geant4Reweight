#ifndef CarbonBox_h
#define CarbonBox_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class CarbonBox : public G4VUserDetectorConstruction{

  public:
    CarbonBox();
    virtual ~CarbonBox();

    virtual G4VPhysicalVolume * Construct();


  protected:
};

#endif
