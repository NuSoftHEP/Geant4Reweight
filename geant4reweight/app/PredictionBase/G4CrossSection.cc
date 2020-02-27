#include "Geant4/G4CrossSectionDataStore.hh"
#include "Geant4/G4PionPlus.hh"
#include "Geant4/G4PionMinus.hh"
#include "Geant4/G4Proton.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4VProcess.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4HadronInelasticProcess.hh"
#include "Geant4/G4HadronElasticProcess.hh"
#include "Geant4/G4String.hh"

#include "geant4reweight/src/PredictionBase/G4CascadeDetectorConstruction.hh"
#include "geant4reweight/src/PredictionBase/G4CascadePhysicsList.hh"

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

//#ifdef FNAL_FHICL
#include "cetlib/filepath_maker.h"
//#endif

std::string fcl_file;

bool range_override = false;
double range_low_override = 0.;
double range_high_override = 0.;

std::string output_file_override = "empty"; 

int ndiv_override = 0;
int type_override = -999;

int verbose_override = -1;

bool parseArgs(int argc, char* argv[]);

int main(int argc, char * argv[]){

  if( !parseArgs(argc, argv) ) 
    return 0;

  fhicl::ParameterSet pset;

  // Configuration file lookup policy.
  char const* fhicl_env = getenv("FHICL_FILE_PATH");
  std::string search_path;

  if (fhicl_env == nullptr) {
    std::cerr << "Expected environment variable FHICL_FILE_PATH is missing or empty: using \".\"\n";
    search_path = ".";
  }
  else {
    search_path = std::string{fhicl_env};
  }

  cet::filepath_first_absolute_or_lookup_with_dot lookupPolicy{search_path};

  fhicl::make_ParameterSet(fcl_file, lookupPolicy, pset);




  //FHICL parameters here
  int type      = pset.get< int >("Type");
  if( type_override != -999 )
    type = type_override;

  bool verbose  = pset.get< bool >("Verbose");
  if( verbose_override != -1 ){
  
    if( verbose_override == 0 || verbose_override == 1 ){
      verbose = verbose_override;
    }
    else{ 
      std::cout << "Warning: Verbose override is " << verbose_override << std::endl;
      std::cout << "Using fcl value " << verbose << std::endl;
    }

  }

  std::pair< double, double > range = pset.get< std::pair< double, double > >("Range");
  if( range_override) 
    range = std::make_pair(range_low_override, range_high_override);

  int nDivisions = pset.get< int >("NDivisions");
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
  std::string outFileName = pset.get< std::string >("Outfile");
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
  rm->SetUserInitialization(new G4CascadeDetectorConstruction);
  rm->SetUserInitialization(new G4CascadePhysicsList);
  rm->Initialize();
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();
  /////

  G4PionPlus  * piplus = 0x0;
  G4PionMinus * piminus = 0x0;
  G4Proton  * proton = 0x0;
  G4Neutron * neutron = 0x0;
  G4ParticleDefinition * part_def = 0x0;
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
  else if( type == 2112 ){
    std::cout << "Chose Neutron" << std::endl;
    part_def = neutron->Definition();
    inel_name = "neutronInelastic";
  }
  else{
    std::cout << "Please specify either 211, -211, or 2212" << std::endl;
    return 0;
  }
  G4DynamicParticle * dynamic_part = new G4DynamicParticle(part_def, G4ThreeVector(0.,0.,1.), 0. );
  std::cout << "PDG: " << dynamic_part->GetPDGcode() << std::endl;




  //Material
  fhicl::ParameterSet MaterialParameters = pset.get< fhicl::ParameterSet >("Material");
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
  
  G4HadronElasticProcess   * elastic_proc = 0x0;
  G4HadronInelasticProcess * inelastic_proc = 0x0;

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

  if ( !elastic_proc || !inelastic_proc ){
    std::cout << "Fatal Error: could not get the processes" << std::endl;
    return 0;
  }

  G4CrossSectionDataStore *theElastStore   = elastic_proc->GetCrossSectionDataStore();
  G4CrossSectionDataStore *theInelastStore = inelastic_proc->GetCrossSectionDataStore();

  std::cout << std::endl << "Got the Cross Section Tables" << std::endl;
  std::cout << "Generating Cross Sections" << std::endl;

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

    else if( strcmp( argv[i], "-v" ) == 0 ){
      verbose_override = atoi( argv[i+1] );
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
