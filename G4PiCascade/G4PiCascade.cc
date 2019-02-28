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
#include "TGraph.h"

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
  }

  std::string outFileName = ps.get< std::string >("Outfile");
  TFile * fout = new TFile( outFileName.c_str(), "RECREATE");
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
    std::cout << "Chose PiPlus" << std::endl;
    part_def = piplus->Definition();
  }
  else if( type == -211 ){
    std::cout << "Chose PiMinus" << std::endl;
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
  std::cout << "PDG: " << dynamic_part->GetPDGcode() << std::endl;

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
 
  std::map< std::string, std::string > cuts;
  //Define cuts and make graphs out of the results
  cuts["abs"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 0";
  cuts["prod"] = " (nPi0 + nPiPlus + nPiMinus) > 1";
  cuts["cex"] = "nPi0 == 1 && nPiPlus == 0 && nPiMinus == 0";
  if( type == 211 ){
    cuts["inel"] = "nPi0 == 0 && nPiPlus == 1 && nPiMinus == 0";
    cuts["dcex"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 1";
  }
  if( type == -211 ){
    cuts["inel"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 1";
    cuts["dcex"] = "nPi0 == 0 && nPiPlus == 1 && nPiMinus == 0";
  }

  int nbins = int(range.second) + 1;
  std::string binning = "(" + std::to_string(nbins) + ",0," + std::to_string(range.second + 1.) + ")";

  std::string draw = "momentum>>total" + binning;
  tree->Draw( draw.c_str(), "", "goff" );
  TH1F * total = (TH1F*)gDirectory->Get("total");
  std::vector< int > bins;
  for( int i = 1; i <= total->GetNbinsX(); ++i ){
    if( total->GetBinContent( i ) > 0. ){
      bins.push_back( i );
      std::cout << i << std::endl;
    }
  }

  std::map< std::string, std::string >::iterator itCut = cuts.begin();
  for( itCut; itCut != cuts.end(); ++itCut ){
    std::cout << itCut->first << std::endl;
    std::string name = "h" + itCut->first;
    std::string draw = "momentum>>" + name + binning;
    std::cout << "Draw: " << draw << std::endl;

    tree->Draw( draw.c_str(), itCut->second.c_str(), "goff" ); 
    TH1F * hist = (TH1F*)gDirectory->Get( name.c_str() );
    hist->Divide( total );

    std::vector< double > xs,ys;
    
    for( int i = 0; i < bins.size(); ++i ){
      xs.push_back( bins.at(i) );
      ys.push_back( hist->GetBinContent(bins.at(i)) );
    }
    TGraph gr(xs.size(), &xs[0], &ys[0]);
    gr.Write( itCut->first.c_str() );

  }


  fout->Close();
  delete rm;

  return 0;
}

