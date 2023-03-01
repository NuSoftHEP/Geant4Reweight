#include "geant4reweight/PredictionBase/G4CascadeDetectorConstruction.hh"

#include "Geant4/G4RunManager.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4Cons.hh"
#include "Geant4/G4Orb.hh"
#include "Geant4/G4Sphere.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4Trd.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4SystemOfUnits.hh"

G4CascadeDetectorConstruction::G4CascadeDetectorConstruction() : G4VUserDetectorConstruction(){ }

G4CascadeDetectorConstruction::G4CascadeDetectorConstruction(
    G4VPhysicalVolume * phys_vol)
  : G4VUserDetectorConstruction(),
    fPhysicalVol(phys_vol) {}

G4CascadeDetectorConstruction::~G4CascadeDetectorConstruction() { }

G4VPhysicalVolume * G4CascadeDetectorConstruction::Construct(){
  if (!fPhysicalVol) {
    std::cout << "Constructing" << std::endl;  
    G4Material * LAr = new G4Material("liquidArgon", 18., 39.95*g/mole, 1.390*g/cm3);
    G4bool checkOverlaps = true;

    //World
    //
    G4Box * solidWorld = new G4Box("World", 40.*cm, 47.*cm, 90.*cm);
    G4LogicalVolume * logicWorld = new G4LogicalVolume(solidWorld, LAr, "World");
    G4VPhysicalVolume * physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, checkOverlaps);
    return physWorld;
  }
  else {
    return fPhysicalVol;
  }

}
