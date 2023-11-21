#ifndef G4ReweightManger_h
#define G4ReweightManger_h

#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4RunManager.hh"

#include "geant4reweight/src/PredictionBase/G4CascadeDetectorConstruction.hh"
#include "geant4reweight/src/PredictionBase/G4CascadePhysicsList.hh"

#include "fhiclcpp/ParameterSet.h"
#include "cetlib/filepath_maker.h"

class G4ReweightManager {
 public:
  G4ReweightManager(std::vector<fhicl::ParameterSet> material_sets);
  G4ReweightManager(std::vector<fhicl::ParameterSet> material_sets,
                    const fhicl::ParameterSet & physics_list_set);
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
