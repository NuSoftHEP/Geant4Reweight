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

G4SimDetectorConstruction::G4SimDetectorConstruction(): 
  G4VUserDetectorConstruction(), 
  MaterialName("liquidArgon"),
  MaterialZ(18),
  MaterialMass(39.95),
  MaterialDensity(1.390)
{}

G4SimDetectorConstruction::G4SimDetectorConstruction( const fhicl::ParameterSet & pset ):
  G4VUserDetectorConstruction(),
  MaterialName( pset.get<std::string>("Name") ),
  MaterialZ( pset.get<int>("Z") ),
  MaterialMass( pset.get<double>("Mass") ),
  MaterialDensity( pset.get<double>("Density") )
{}

G4SimDetectorConstruction::~G4SimDetectorConstruction() { }

G4VPhysicalVolume * G4SimDetectorConstruction::Construct(){
  G4cout << "Constructing" << G4endl;  
  G4NistManager * nist = G4NistManager::Instance();

  G4Material * material = new G4Material(MaterialName, MaterialZ, MaterialMass*g/mole, MaterialDensity*g/cm3);

  G4Box * solidWorld = new G4Box("World", 40.*cm, 47.*cm, 90.*cm);
  G4LogicalVolume * logicWorld = new G4LogicalVolume(solidWorld, material, "World");
  G4VPhysicalVolume * physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, true);

  return physWorld;

}
