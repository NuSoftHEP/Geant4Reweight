#include "G4PiCascadeDetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

G4PiCascadeDetectorConstruction::G4PiCascadeDetectorConstruction() : G4VUserDetectorConstruction(){ }

G4PiCascadeDetectorConstruction::~G4PiCascadeDetectorConstruction() { }

G4VPhysicalVolume * G4PiCascadeDetectorConstruction::Construct(){
  G4cout << "Constructing" << G4endl;  
  G4Material * LAr = new G4Material("liquidArgon", 18., 39.95*g/mole, 1.390*g/cm3);
  G4bool checkOverlaps = true;

  //World
  //
  G4Box * solidWorld = new G4Box("World", 40.*cm, 47.*cm, 90.*cm);
  G4LogicalVolume * logicWorld = new G4LogicalVolume(solidWorld, LAr, "World");
  G4VPhysicalVolume * physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, checkOverlaps);
  return physWorld;

}
