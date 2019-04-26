#ifndef G4SimDetectorConstruction_h
#define G4SimDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "fhiclcpp/ParameterSet.h"

class G4VPhysicalVolume;
class G4LogicalVolume;

class G4SimDetectorConstruction : public G4VUserDetectorConstruction{

  public:
    G4SimDetectorConstruction();
    G4SimDetectorConstruction(const fhicl::ParameterSet &);
    virtual ~G4SimDetectorConstruction();

    virtual G4VPhysicalVolume * Construct();


  protected:
  private:
    std::string MaterialName;
    int MaterialZ;
    double MaterialMass;
    double MaterialDensity;
};

#endif
