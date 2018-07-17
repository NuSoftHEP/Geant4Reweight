#include "G4SimDetectorConstruction.hh"

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

G4SimDetectorConstruction::G4SimDetectorConstruction() : G4VUserDetectorConstruction(){ }

G4SimDetectorConstruction::~G4SimDetectorConstruction() { }

G4VPhysicalVolume * G4SimDetectorConstruction::Construct(){
  G4cout << "Constructing" << G4endl;  
  G4NistManager * nist = G4NistManager::Instance();

  G4double radius = 1.5*m, height = 50.*m;
  G4Material * air = nist->FindOrBuildMaterial("G4_AIR");
  G4Material * water = nist->FindOrBuildMaterial("G4_WATER");
  G4Material * LAr = new G4Material("liquidArgon", 18., 39.95*g/mole, 1.390*g/cm3);
  G4bool checkOverlaps = true;

  //World
  //
  G4Tubs * solidWorld = new G4Tubs("World", 0., radius, height, 0.*deg, 360.*deg);
  G4LogicalVolume * logicWorld = new G4LogicalVolume(solidWorld, water, "World");
  G4VPhysicalVolume * physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, checkOverlaps);

  //Disk
  //
  //G4Tubs * solidDisk = new G4Tubs("Disk", 0., radius, height, 0.*deg, 360.*deg);
  //G4LogicalVolume * logicDisk = new G4LogicalVolume(solidDisk, water, "Disk");
  //G4VPhysicalVolume * physDisk = new G4PVPlacement(0, G4ThreeVector(), logicDisk, "Disk", logicWorld, false, 0, checkOverlaps);

/*  G4Box * solidCube = new G4Box("Box", 10*height, 10*height, 10*height);
  G4LogicalVolume * logicCube = new G4LogicalVolume(solidCube, water, "Box");
  G4VPhysicalVolume * physCube = new G4PVPlacement(0, G4ThreeVector(), logicCube, "Box", logicWorld, false, 0, checkOverlaps);*/

  return physWorld;

}
