#include "WatArDetector.hh"

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

WatArDetector::WatArDetector() : G4VUserDetectorConstruction(){ }

WatArDetector::~WatArDetector() { }

G4VPhysicalVolume * WatArDetector::Construct(){
  G4cout << "Constructing" << G4endl;  
  G4NistManager * nist = G4NistManager::Instance();

  G4double radius = 1.5*m, height = 50.*m;
  G4Material * air = nist->FindOrBuildMaterial("G4_AIR");
  G4Material * water = nist->FindOrBuildMaterial("G4_WATER");
  G4Material * LAr = new G4Material("liquidArgon", 18., 39.95*g/mole, 1.390*g/cm3);
  G4bool checkOverlaps = true;

  //World
  //
  //G4Box * solidWorld = new G4Box("World", 4*height, 4*height, 4*height);
  G4Box * solidWorld = new G4Box("World", 40.*cm, 47.*cm, 90.*cm);
  G4LogicalVolume * logicWorld = new G4LogicalVolume(solidWorld, LAr, "World");
  G4VPhysicalVolume * physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, checkOverlaps);

/*  //Argon box
  G4Box * argonBox = new G4Box("ArBox", 40.*cm, 47.*cm, 5.*m);
  G4LogicalVolume * logicAr = new G4LogicalVolume(argonBox, LAr, "Ar");
  G4VPhysicalVolume * physAr = new G4PVPlacement(0, G4ThreeVector(0.,0.,2.5*m), logicAr, "Ar", 0, false, 0, checkOverlaps);
*/
  //Water box
  G4Box * waterBox = new G4Box("WaterBox", 40.*cm, 47.*cm, 7.5*cm );
  G4LogicalVolume * logicWater = new G4LogicalVolume(waterBox, water, "water");
  G4VPhysicalVolume * physWater0 = new G4PVPlacement(0, G4ThreeVector(0.,0.,7.5*cm), logicWater, "water", logicWorld, false, 0, checkOverlaps);

  G4VPhysicalVolume * physWater1 = new G4PVPlacement(0, G4ThreeVector(0.,0., (2.*15. + 7.5)*cm), logicWater, "water", logicWorld, false, 0, checkOverlaps);

  G4VPhysicalVolume * physWater2 = new G4PVPlacement(0, G4ThreeVector(0.,0., (4.*15. + 7.5)*cm), logicWater, "water", logicWorld, false, 0, checkOverlaps);

  return physWorld;

}
