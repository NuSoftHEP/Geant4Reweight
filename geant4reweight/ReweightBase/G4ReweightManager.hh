#ifndef G4ReweightManger_h
#define G4ReweightManger_h

#include "fhiclcpp/ParameterSet.h"

class G4CascadeDetectorConstruction;
class G4CascadePhysicsList;

class G4RunManager;
class G4VPhysicalVolume;

#include <map>
#include <string>
#include <vector>

class G4ReweightManager {
 public:
  G4ReweightManager(std::vector<fhicl::ParameterSet> fMaterialSets);
  double GetCenter(std::string name) {return fCenters[name];};
  G4VPhysicalVolume * GetVolume(std::string name) {return fVolumes[name];};
 protected:
  void SetupWorld();

  std::map<std::string, double> fCenters;
  std::map<std::string, G4VPhysicalVolume *> fVolumes;
  std::vector<fhicl::ParameterSet> fMaterialSets;
  G4VPhysicalVolume * physWorld;
  std::vector<G4VPhysicalVolume *> physSubBoxes;
  G4RunManager * rm;
  G4CascadeDetectorConstruction * detector;
  G4CascadePhysicsList * physList;
};
#endif
