#include "ThinDetector.hh"

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

ThinDetector::ThinDetector() :
  G4VUserDetectorConstruction(), 
  MaterialName("liquidArgon"),
  MaterialZ(18),
  MaterialMass(39.95),
  MaterialDensity(1.390)
{}

ThinDetector::ThinDetector( const fhicl::ParameterSet & pset ):
  G4VUserDetectorConstruction(),
  MaterialName( pset.get<std::string>("Name") ),
  MaterialZ( pset.get<int>("Z") ),
  MaterialMass( pset.get<double>("Mass") ),
  MaterialDensity( pset.get<double>("Density") )
{}

ThinDetector::~ThinDetector() { }

G4VPhysicalVolume * ThinDetector::Construct(){
  G4cout << "Constructing" << G4endl;  
  G4NistManager * nist = G4NistManager::Instance();

  G4double radius = 1.5*m, height = 50.*m;
  G4Material * material = new G4Material(MaterialName, MaterialZ, MaterialMass*g/mole, MaterialDensity*g/cm3);

  //Disk
  //
  G4Tubs * solidDisk = new G4Tubs("Disk", 0., radius, .5*cm, 0.*deg, 360.*deg);
  G4LogicalVolume * logicDisk = new G4LogicalVolume(solidDisk, material, "Disk");
  G4VPhysicalVolume * physDisk = new G4PVPlacement(0, G4ThreeVector(), logicDisk, "Disk", 0, false, 0, true);
  return physDisk;


}
