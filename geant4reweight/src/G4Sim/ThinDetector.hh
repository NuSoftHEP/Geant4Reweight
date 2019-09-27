#ifndef ThinDetector_h
#define ThinDetector_h 1

#include "G4VUserDetectorConstruction.hh"
#include "fhiclcpp/ParameterSet.h"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ThinDetector : public G4VUserDetectorConstruction{

  public:
    ThinDetector();
    ThinDetector(const fhicl::ParameterSet &);
    virtual ~ThinDetector();

    virtual G4VPhysicalVolume * Construct();


  protected:
  private:
    std::string MaterialName;
    int MaterialZ;
    double MaterialMass;
    double MaterialDensity;
};

#endif
