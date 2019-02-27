#include "G4CascadeInterface.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4Material.hh"
#include "G4Nucleus.hh"
#include "G4SystemOfUnits.hh"
#include "G4CascadeInterface.hh"
#include "G4HadProjectile.hh"
#include "G4HadFinalState.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4RunManager.hh"
#include "G4PiCascadeDetectorConstruction.hh"
#include "G4PiCascadePhysicsList.hh"


#include <utility>
#include <iostream>
#include <fstream>

#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"


int main(int argc, char * argv[]){

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(argv[1]);

  int nCascades = ps.get< int >("NCascades");
  int type      = ps.get< int >("Type");
  std::pair< double, double > range = ps.get< std::pair< double, double > >("Range");
  int nDivisions = ps.get< int >("NDivisions");
  if( nDivisions < 1 ){
    std::cout << "Please give NDivision >= 1" << std::endl;
    return 0;
  }
  if( range.second < range.first ){
    std::cout << "Please give range in increasing order" << std::endl;
    return 0;
  }

  std::cout << "Range: " << range.first << " " << range.second << std::endl;
  std::vector< double > momenta;
  double delta = range.second - range.first;
  double step = delta / nDivisions; 
  for( size_t i = 0; i <= nDivisions; ++i ){
    momenta.push_back( range.first + i * step );
    std::cout << momenta.back() << std::endl;
  }

  TFile * fout = new TFile( "cascade_out.root", "RECREATE");
  TTree * tree = new TTree("tree","");  
  int nPi0 = 0, nPiPlus = 0, nPiMinus = 0;
  double momentum;
  tree->Branch( "nPi0", &nPi0 );
  tree->Branch( "nPiPlus", &nPiPlus );
  tree->Branch( "nPiMinus", &nPiMinus );
  tree->Branch( "momentum", &momentum );

  //Initializing
  G4RunManager * rm = new G4RunManager();
  rm->SetUserInitialization(new G4PiCascadeDetectorConstruction);
  rm->SetUserInitialization(new G4PiCascadePhysicsList);
  rm->Initialize();
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();
  ////

  G4PionPlus  * piplus;
  G4PionMinus * piminus;
  G4ParticleDefinition * part_def;
  if( type == 211 ){
    part_def = piplus->Definition();
  }
  else if( type == -211 ){
    part_def = piminus->Definition();
  }
  else{
    std::cout << "Please specify either 211 or -211" << std::endl;
    return 0;
  }

  fhicl::ParameterSet MaterialParameters = ps.get< fhicl::ParameterSet >("Material");
  std::string MaterialName = MaterialParameters.get< std::string >( "Name" );
  int MaterialA = MaterialParameters.get< int >( "A" );
  double MaterialMass = MaterialParameters.get< double >( "Mass" );
  double MaterialDensity = MaterialParameters.get< double >( "Density" );
  G4Material * theMaterial = new G4Material(MaterialName, MaterialA, MaterialMass*g/mole, MaterialDensity*g/cm3);
  G4Nucleus * theNucleus = new G4Nucleus( theMaterial );
  G4CascadeInterface * theCascade = new G4CascadeInterface( "BertiniCascade" );
  
  G4DynamicParticle * dynamic_part = new G4DynamicParticle(part_def, G4ThreeVector(0.,0.,1.), 0. );

  for( size_t iM = 0; iM < momenta.size(); ++iM ){
    
    double theMomentum = momenta[iM]; 
    double KE = sqrt( theMomentum*theMomentum + part_def->GetPDGMass()*part_def->GetPDGMass() ) - part_def->GetPDGMass();
    dynamic_part->SetKineticEnergy( KE );
    for( size_t iC = 0; iC < nCascades; ++iC ){
      nPi0 = 0; 
      nPiPlus = 0; 
      nPiMinus = 0;
      momentum = dynamic_part->GetTotalMomentum();
      G4HadFinalState * theFS = theCascade->ApplyYourself( *dynamic_part, *theNucleus );
//      std::cout << "Secondaries: " << theFS->GetNumberOfSecondaries() << std::endl;

      size_t nSecondaries = theFS->GetNumberOfSecondaries();
      for( size_t i = 0; i < nSecondaries; ++i ){
        auto part = theFS->GetSecondary(i)->GetParticle();
      //  std::cout << i << " " << part->GetPDGcode() << " " << part->GetTotalMomentum() << std::endl;

        switch( part->GetPDGcode() ){
          case( 211 ):
            ++nPiPlus; break;
          case( -211 ):
            ++nPiMinus; break;
          case( 111 ):
            ++nPi0; break;          
          default:
            break;
        }
      }
      //std::cout << "nPiPlus: " << nPiPlus << std::endl;
      //std::cout << "nPiMinus: " << nPiMinus << std::endl;
      //std::cout << "nPi0: " << nPi0 << std::endl;
      tree->Fill();

//      std::cout << std::endl;
    }
  }
  fout->cd();
  tree->Write();
  fout->Close();
  delete rm;

  return 0;
}

