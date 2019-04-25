#include "G4PiNuclearCrossSection.hh"
#include "G4BGGPionElasticXS.hh"
#include "G4HadronElasticDataSet.hh"
#include "G4CrossSectionDataSetRegistry.hh"
#include "G4CrossSectionDataStore.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4Proton.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Material.hh"
#include "G4Nucleus.hh"
#include "G4SystemOfUnits.hh"
#include "G4CascadeInterface.hh"
#include "G4HadProjectile.hh"
#include "G4HadFinalState.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4VProcess.hh"
#include "G4RunManager.hh"
#include "G4PiCascadeDetectorConstruction.hh"
#include "G4PiCascadePhysicsList.hh"
#include "G4HadronicException.hh"
#include "G4HadronInelasticProcess.hh"
#include "G4HadronElasticProcess.hh"
#include "G4String.hh"
#include "G4ForceCondition.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"


#include <utility>
#include <iostream>
#include <fstream>

#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TVectorD.h"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

std::string fcl_file;

bool range_override = false;
double range_low_override = 0.;
double range_high_override = 0.;

std::string output_file_override = "empty"; 

int ndiv_override = 0;
int type_override = -999;

bool parseArgs(int argc, char* argv[]);

int main(int argc, char * argv[]){

  if( !parseArgs(argc, argv) ) 
    return 0;

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(fcl_file);

  //FHICL parameters here
  int type      = ps.get< int >("Type");
  if( type_override != -999 )
    type = type_override;

  bool verbose  = ps.get< bool >("Verbose");

  std::pair< double, double > range = ps.get< std::pair< double, double > >("Range");
  if( range_override) 
    range = std::make_pair(range_low_override, range_high_override);

  int nDivisions = ps.get< int >("NDivisions");
  if( ndiv_override > 0 ) 
    nDivisions = ndiv_override;

  if( nDivisions < 1 ){
    std::cout << "Please give NDivision >= 1" << std::endl;
    return 0;
  }
  if( range.second < range.first ){
    std::cout << "Please give range in increasing order" << std::endl;
    return 0;
  }
  double delta = ( range.second - range.first ) / nDivisions;

  //Root Output here
  std::string outFileName = ps.get< std::string >("Outfile");
   if( output_file_override  != "empty" ){
    outFileName = output_file_override;
  }

  TFile * fout = new TFile( outFileName.c_str(), "RECREATE");
  TTree * tree = new TTree("tree","");  
  double inelastic_xsec, elastic_xsec;  
  double momentum, kinetic_energy;
  tree->Branch( "momentum", &momentum );
  tree->Branch( "kinetic_energy", &kinetic_energy );
  tree->Branch( "inelastic_xsec", &inelastic_xsec );
  tree->Branch( "elastic_xsec", &elastic_xsec );

  //Initializing
  G4RunManager * rm = new G4RunManager();
  rm->SetUserInitialization(new G4PiCascadeDetectorConstruction);
  rm->SetUserInitialization(new G4PiCascadePhysicsList);
  rm->Initialize();
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();
  /////

  G4PionPlus  * piplus;
  G4PionMinus * piminus;
  G4Proton * proton;
  G4ParticleDefinition * part_def;
  G4String inel_name;
  if( type == 211 ){
    std::cout << "Chose PiPlus" << std::endl;
    part_def = piplus->Definition();
    inel_name = "pi+Inelastic";
  }
  else if( type == -211 ){
    std::cout << "Chose PiMinus" << std::endl;
    part_def = piminus->Definition();
    inel_name = "pi-Inelastic";
  }
  else if( type == 2212 ){
    std::cout << "Chose Proton" << std::endl;
    part_def = proton->Definition();
    inel_name = "protonInelastic";
  }
  else{
    std::cout << "Please specify either 211, -211, or 2212" << std::endl;
    return 0;
  }
  G4DynamicParticle * dynamic_part = new G4DynamicParticle(part_def, G4ThreeVector(0.,0.,1.), 0. );
  std::cout << "PDG: " << dynamic_part->GetPDGcode() << std::endl;




  //Material
  fhicl::ParameterSet MaterialParameters = ps.get< fhicl::ParameterSet >("Material");
  std::string MaterialName = MaterialParameters.get< std::string >( "Name" );
  int MaterialZ = MaterialParameters.get< int >( "Z" );
  double MaterialMass = MaterialParameters.get< double >( "Mass" );
  double MaterialDensity = MaterialParameters.get< double >( "Density" );
  G4Material * theMaterial = new G4Material(MaterialName, MaterialZ, MaterialMass*g/mole, MaterialDensity*g/cm3);

  std::cout << "Checking material" << std::endl;
  std::cout << "N Elements: " << theMaterial->GetNumberOfElements() << std::endl;
  if( theMaterial->GetNumberOfElements() != 1 ){
    std::cout << "Fatal: exiting the application because NElements != 1" << std::endl;
    return 0;
  }
  auto theElement = (*theMaterial->GetElementVector())[0];
  std::cout << theElement->GetName() << " " << theElement->GetSymbol() << " " << theElement->GetZ() << " " << theElement->GetN() << std::endl;
  ///////////

  
  std::vector< double > total_xsecs, elastic_xsecs, inelastic_xsecs, momenta, kinetic_energies;


  //Getting the cross sections from the processes
  G4ProcessManager * pm = part_def->GetProcessManager();
  G4ProcessVector  * pv = pm->GetProcessList();
  
  G4HadronElasticProcess   * elastic_proc;
  G4HadronInelasticProcess * inelastic_proc;

  for( int i = 0; i < pv->size(); ++i ){
    G4VProcess * proc = (*pv)(i);
    std::string theName = proc->GetProcessName();
    std::cout <<  theName << std::endl;
    if( theName == "hadElastic" ){          
      std::cout << "Found elastic" << std::endl;
      elastic_proc = (G4HadronElasticProcess*)proc;
    }
    else if( theName == inel_name ){
      std::cout << "Found inelastic" << std::endl;
      inelastic_proc = (G4HadronInelasticProcess*)proc;
    }
  }

  G4CrossSectionDataStore *theElastStore, *theInelastStore;
  if( elastic_proc ){
    theElastStore = elastic_proc->GetCrossSectionDataStore();
  }
  if( inelastic_proc ){
    theInelastStore = inelastic_proc->GetCrossSectionDataStore();
  }


  //Getting the values
  double theMomentum = range.first;
  int n = 1; 
  while( theMomentum <= range.second ){
    double KE = sqrt( theMomentum*theMomentum + part_def->GetPDGMass()*part_def->GetPDGMass() ) - part_def->GetPDGMass(); 
    dynamic_part->SetKineticEnergy( KE );

    momentum = theMomentum;
    kinetic_energy = KE;

    inelastic_xsec = theInelastStore->GetCrossSection( dynamic_part, theElement, theMaterial ) / millibarn;
    elastic_xsec = theElastStore->GetCrossSection( dynamic_part, theElement, theMaterial ) / millibarn;

    tree->Fill();
    if( verbose && !( n % 100) ){
      std::cout << "Inelastic XSec at " << KE << " MeV " <<  inelastic_xsec << std::endl;
      std::cout << "Elastic XSec at " << KE << " MeV " <<  elastic_xsec << std::endl;
      std::cout << std::endl;
    }

    inelastic_xsecs.push_back( inelastic_xsec );
    elastic_xsecs.push_back( elastic_xsec );
    total_xsecs.push_back( elastic_xsec + inelastic_xsec );
    momenta.push_back( momentum );
    kinetic_energies.push_back( kinetic_energy );

    theMomentum += delta;
    n++;
  }

  //Make Graphs
  TGraph inel_momentum( momenta.size(), &momenta[0], &inelastic_xsecs[0] );
  TGraph inel_KE( kinetic_energies.size(), &kinetic_energies[0], &inelastic_xsecs[0] );
  TGraph el_momentum( momenta.size(), &momenta[0], &elastic_xsecs[0] );
  TGraph el_KE( kinetic_energies.size(), &kinetic_energies[0], &elastic_xsecs[0] );
  TGraph total_momentum( momenta.size(), &momenta[0], &total_xsecs[0] );
  TGraph total_KE( kinetic_energies.size(), &kinetic_energies[0], &total_xsecs[0] );

  fout->cd();
  inel_momentum.Write( "inel_momentum" );
  inel_KE.Write( "inel_KE" );
  el_momentum.Write( "el_momentum" );
  el_KE.Write( "el_KE" );
  total_momentum.Write( "total_momentum" );
  total_KE.Write( "total_KE" );

  TVectorD m_vec(1);
  m_vec[0] = MaterialMass;
  m_vec.Write("Mass");

  TVectorD d_vec(1);
  d_vec[0] = MaterialDensity;
  d_vec.Write("Density");


  tree->Write();
  fout->Close();

  delete rm;
  return 0;
}

bool parseArgs(int argc, char ** argv){

  bool found_fcl_file = false;

  bool found_range_low = false;
  bool found_range_high = false;

  for( int i = 1; i < argc; ++i ){
    if( ( strcmp( argv[i], "--help" )  == 0 ) || ( strcmp( argv[i], "-h" ) == 0 ) ){
      std::cout << "Usage: ./G4CrossSection -c <xsec_config>.fcl [options]" << std::endl;
      std::cout << std::endl;
      std::cout << "Options: " << std::endl;
      std::cout << "\t-o <output_file_override>.root" << std::endl;
      std::cout << "\t--low <range_low_value> (must be provided along with -high)" << std::endl; 
      std::cout << "\t--high <range_high_value> (must be provided along with -low)" << std::endl; 
      std::cout << "\t--ND <divisor_of_range> (must be > 0 to work)" << std::endl;
      std::cout << "\t-t <probe type> (currently only works with 211, -211, and 2212)" << std::endl;

      return false;
    }

    else if( strcmp( argv[i], "-c" ) == 0 ){
      fcl_file = argv[i+1];
      found_fcl_file = true;
    }

    else if( strcmp( argv[i], "-o" ) == 0 ){
      output_file_override = argv[i+1];
    }

    else if( strcmp( argv[i], "--low" ) == 0 ){
      range_low_override = atof( argv[i+1] ); 
      found_range_low = true;
    }

    else if( strcmp( argv[i], "--high" ) == 0 ){
      range_high_override = atof( argv[i+1] ); 
      found_range_high = true;
    }

    else if( strcmp( argv[i], "--ND" ) == 0 ){
      ndiv_override = atoi( argv[i+1] ); 
    }

    else if( strcmp( argv[i], "-t" ) == 0 ){
      type_override = atoi( argv[i+1] );
    }
  }

  if( !found_fcl_file ){
    std::cout << "Error: Must provide fcl file with option '-c'" << std::endl;
    return false;
  }

  if( found_range_low != found_range_high ){
    std::cout << "Error: If overriding fcl range, you must provide both values with options '-l' and '-h'" << std::endl;
    return false;
  }
  else{
    range_override = found_range_low;
  }

  

  return true;
}
