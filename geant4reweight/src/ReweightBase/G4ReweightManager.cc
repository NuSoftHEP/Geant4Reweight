#include "G4ReweightManager.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4SystemOfUnits.hh"

#include "Geant4/G4Box.hh"
#include "Geant4/G4LogicalVolume.hh"


//#include "fhiclcpp/make_ParameterSet.h"



G4ReweightManager::G4ReweightManager(
    std::vector<fhicl::ParameterSet> material_sets) {
  fMaterialSets = material_sets;

  rm = new G4RunManager();
  SetupWorld();
  physList = new G4CascadePhysicsList();
  rm->SetUserInitialization(detector);
  rm->SetUserInitialization(physList);
  rm->Initialize();
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();
}

G4ReweightManager::G4ReweightManager(
    std::vector<fhicl::ParameterSet> material_sets,
    const fhicl::ParameterSet & physics_list_set) {
  fMaterialSets = material_sets;

  rm = new G4RunManager();
  SetupWorld();
  physList = new G4CascadePhysicsList(physics_list_set);
  rm->SetUserInitialization(detector);
  rm->SetUserInitialization(physList);
  rm->Initialize();
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();
}

void G4ReweightManager::SetupWorld() {
  //Set up some boxes side-by-side
  //5cm x 5cm x 5cm each

  double total_length = fMaterialSets.size() * 5. *cm;
  G4Box * solidWorld = new G4Box("World", 5.*cm, 5.*cm, total_length);
  G4Box * subBox = new G4Box("SubBox", 5.*cm, 5.*cm, 5.*cm);
  G4LogicalVolume * logicWorld = 0x0;
  for (size_t i = 0; i < fMaterialSets.size(); ++i) {
    const fhicl::ParameterSet & material_set = fMaterialSets[i];

    std::vector<fhicl::ParameterSet> MaterialComponents
    = material_set.get<std::vector<fhicl::ParameterSet>>("Components");

    double Density = material_set.get<double>("Density");
    std::string MaterialName = material_set.get<std::string>("Name");


    G4Material* testMaterial = 0x0;

    if(MaterialComponents.size() == 1){
      int MaterialZ = MaterialComponents[0].get<int>("Z");
      double Mass = MaterialComponents[0].get<double>("Mass");

      testMaterial = new G4Material(MaterialName,
                                    MaterialZ,
                                    Mass*g/mole,
                                    Density*g/cm3);
    }
    else {
      double sum = 0.0;
      for (auto s : MaterialComponents) {
        double frac = s.get<double>("Fraction");
        sum += frac;
      }
      if(sum < 1.0){
        std::cout << "Sum of all element fractions equals " << sum << "\n";
        std::cout << "Fractions will be divided by this factor to normalize \n";
      }
      else if(sum > 1.0){
        std::cout << "Sum of all element fractions equals " << sum << "\n";
        std::cout << "This is greater than 1.0 - something is wrong here \n";
        abort();
      }

      testMaterial = new G4Material(MaterialName,
                                   Density*g/cm3,
                                   MaterialComponents.size());
      for (auto s : MaterialComponents) {
        int MaterialZ = s.get<int>("Z");
        double Mass = s.get<double>("Mass");
        std::string name = s.get<std::string>("Name");
        double frac = s.get<double>("Fraction");
        G4Element * element = new G4Element(name, " ", MaterialZ, Mass*g/mole);
        testMaterial->AddElement(element, frac/sum);
      }
    }// end else()  (complex material)


    if (i == 0) {
      //For the first material, set it as the 'world'
      //centered at 0
      logicWorld = new G4LogicalVolume(solidWorld, testMaterial, "World");
      physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0,
                                    false, 0, true);
      double center = -.5*total_length + 2.5*cm + i*5.*cm;
      fCenters[MaterialName] = center;
      fVolumes[MaterialName] = physWorld;
    }
    else {
      G4LogicalVolume * logicSubBox = new G4LogicalVolume(subBox, testMaterial,
                                                          MaterialName);

      double center = -.5*total_length + 2.5*cm + i*5.*cm;
      fCenters[MaterialName] = center;
      physSubBoxes.push_back(
          new G4PVPlacement(0, G4ThreeVector(0., 0., center), logicSubBox,
                            MaterialName, logicWorld, false, 0, true));
      fVolumes[MaterialName] = physSubBoxes.back();
    }

  }

  detector = new G4CascadeDetectorConstruction(physWorld);
}
