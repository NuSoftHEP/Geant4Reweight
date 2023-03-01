#include "geant4reweight/src/PredictionBase/G4CascadeDetectorConstruction.hh"
#include "geant4reweight/src/PredictionBase/G4CascadePhysicsList.hh"
#include "geant4reweight/src/PredictionBase/G4DecayHook.hh"

#include "Geant4/G4Box.hh"
#include "Geant4/G4CoulombScattering.hh"
#include "Geant4/G4CrossSectionDataStore.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4Element.hh"
#include "Geant4/G4HadronCaptureProcess.hh"
#include "Geant4/G4HadronElasticProcess.hh"
#include "Geant4/G4HadronFissionProcess.hh"
#include "Geant4/G4HadronInelasticProcess.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4MuonMinus.hh"
#include "Geant4/G4MuonPlus.hh"
#include "Geant4/G4Neutron.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4PionMinus.hh"
#include "Geant4/G4PionPlus.hh"
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4ProcessVector.hh"
#include "Geant4/G4Proton.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4StepPoint.hh"
#include "Geant4/G4String.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VProcess.hh"
#include "Geant4/G4hBremsstrahlung.hh"
#include "Geant4/G4hIonisation.hh"
#include "Geant4/G4hPairProduction.hh"

#include "fhiclcpp/ParameterSet.h"

#include "cetlib/filepath_maker.h"

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TVectorD.h"

#include <iostream>
#include <string>
#include <utility> // std::pair
#include <vector>

std::string fcl_file;

bool range_override = false;
double range_low_override = 0.;
double range_high_override = 0.;

std::string output_file_override = "empty"; 

int ndiv_override = 0;
int type_override = -999;
int list_override = -999;

int verbose_override = -1;
double inel_bias = 1.;
double el_bias = 1.;

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

  //fhicl::make_ParameterSet(fcl_file, lookupPolicy, pset);
  pset = fhicl::ParameterSet::make(fcl_file, lookupPolicy);




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

  int list = pset.get<int>("List", 0);
  if( list_override != -999 )
    list = list_override;
  bool print_procs = pset.get<bool>("PrintProcs", false);

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

  //World
  //
  fhicl::ParameterSet MaterialParameters = pset.get< fhicl::ParameterSet >("Material");
  std::string MaterialName = MaterialParameters.get< std::string >( "Name" );
  //int MaterialZ = MaterialParameters.get< int >( "Z" );
  //double MaterialMass = MaterialParameters.get< double >( "Mass" );
  double MaterialDensity = MaterialParameters.get< double >( "Density" );
  //G4Material * theMaterial = new G4Material(MaterialName, MaterialZ, MaterialMass*g/mole, MaterialDensity*g/cm3);
  G4Material * theMaterial = nullptr;
  
  std::vector<fhicl::ParameterSet> MaterialComponents
      = MaterialParameters.get<std::vector<fhicl::ParameterSet>>("Components");
  if (MaterialComponents.size() == 1) {
    int MaterialZ = MaterialComponents[0].get<int>("Z");
    double MaterialMass = MaterialComponents[0].get<double>("Mass");
    theMaterial = new G4Material(MaterialName, MaterialZ, MaterialMass*g/mole,
                                 MaterialDensity*g/cm3);
  }
  else {

    double sum = 0.0;
    for (auto s : MaterialComponents) {
      double frac = s.get<double>("Fraction");
      sum += frac;
    }
    if(sum < 1.0){
      std::cout << "Sum of all element fractions equals " << sum << "\n";
      std::cout << "Fractions will be divided by this factor to normalize \n";
    }
    else if(sum > 1.0){
      std::cout << "Sum of all element fractions equals " << sum << "\n";
      std::cout << "This is greater than 1.0 - something is wrong here \n";
      abort();
    }

    theMaterial = new G4Material(MaterialName, MaterialDensity*g/cm3, MaterialComponents.size());
    for (auto s : MaterialComponents) {
      int MaterialZ = s.get<int>("Z");
      double MaterialMass = s.get<double>("Mass");
      std::string name = s.get<std::string>("Name");
      double frac = s.get<double>("Fraction");
      G4Element * element = new G4Element(name, " ", MaterialZ, MaterialMass*g/mole);
      theMaterial->AddElement(element, frac/sum);
    }
  }

  //G4Material * LAr = new G4Material("liquidArgon", 18., 39.95*g/mole, 1.390*g/cm3);
  G4Box * solidWorld = new G4Box("World", 40.*cm, 47.*cm, 90.*cm);
  G4LogicalVolume * logicWorld = new G4LogicalVolume(solidWorld, theMaterial, "World");
  G4VPhysicalVolume * physWorld = new G4PVPlacement(
      0, G4ThreeVector(), logicWorld, "World", 0, false, 0, true);

  rm->SetUserInitialization(new G4CascadeDetectorConstruction(physWorld));
  rm->SetUserInitialization(new G4CascadePhysicsList(list));
  rm->Initialize();
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();
  std::cout << "LogVol " << physWorld->GetLogicalVolume() << std::endl;
  std::cout << "MCC: " << physWorld->GetLogicalVolume()->GetMaterialCutsCouple() << std::endl;
  /////

  G4PionPlus  * piplus = nullptr;
  G4PionMinus * piminus = nullptr;
  G4MuonPlus  * muonplus = nullptr;
  G4MuonMinus * muonminus = nullptr;
  G4Proton  * proton = nullptr;
  G4Neutron * neutron = nullptr;
  G4ParticleDefinition * part_def = nullptr;
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
  else if (type == -13) {
    std::cout << "Chose MuonPlus" << std::endl;
    part_def = muonplus->Definition();
  }
  else if (type == 13) {
    std::cout << "Chose MuonMinus" << std::endl;
    part_def = muonminus->Definition();
  }
  else{
    std::cout << "Please specify either 211, -211, 2212 or 2112" << std::endl;
    return 0;
  }
  G4DynamicParticle * dynamic_part = new G4DynamicParticle(part_def, G4ThreeVector(0.,0.,1.), 0. );
  std::cout << "PDG: " << dynamic_part->GetPDGcode() << std::endl;
  std::cout << "testing" << std::endl;
  /*
  G4Track * tempTrack = new G4Track( dynamic_part, 0., G4ThreeVector(0.,0.,0.) );
  G4Step * tempStep = new G4Step();
  G4StepPoint * tempPoint = new G4StepPoint();
  tempPoint->SetMaterial(physWorld->GetLogicalVolume()->GetMaterial());
  tempPoint->SetMaterialCutsCouple(physWorld->GetLogicalVolume()->GetMaterialCutsCouple());
  tempStep->SetPreStepPoint( tempPoint );
  tempTrack->SetStep( tempStep );
  std::cout << "ind: " << std::endl;
  std::cout << tempTrack->GetMaterialCutsCouple() << std::endl;
  std::cout << "Done" << std::endl;
  */



  //Material
  /*
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
  */
  auto theElement = (*theMaterial->GetElementVector())[0];
  std::cout << theElement->GetName() << " " << theElement->GetSymbol() << " " << theElement->GetZ() << " " << theElement->GetN() << std::endl;
  ///////////

  G4Track * theTrack = new G4Track( dynamic_part, 0., G4ThreeVector(0.,0.,0.) );
  G4Step * theStep = new G4Step();
  G4StepPoint * thePoint = new G4StepPoint();
  //thePoint->SetMaterial( theMaterial );
  thePoint->SetMaterial(physWorld->GetLogicalVolume()->GetMaterial());
  thePoint->SetMaterialCutsCouple(physWorld->GetLogicalVolume()->GetMaterialCutsCouple());
  theStep->SetPreStepPoint( thePoint );
  theTrack->SetStep( theStep );
  std::cout << "ind: " << std::endl;
  std::cout << theTrack->GetMaterialCutsCouple() << std::endl;
  std::cout << "Done" << std::endl;

  G4DecayHook decay_hook;
  
  std::vector<double> total_xsecs, elastic_xsecs, inelastic_xsecs, momenta,
                      kinetic_energies, decay_mfps, ioni_mfps, brems_mfps,
                      pairprod_mfps, coul_mfps, cap_mfps, fis_mfps;
  //Getting the cross sections from the processes
  G4ProcessManager * pm = part_def->GetProcessManager();
  G4ProcessVector  * pv = pm->GetProcessList();
  
  G4HadronElasticProcess   * elastic_proc = nullptr;
  G4HadronInelasticProcess * inelastic_proc = nullptr;

  //for( int i = 0; i < pv->size(); ++i ){
  for( size_t i = 0; i < (size_t)pv->size(); ++i ){
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
  if (print_procs) return 0;

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

    inelastic_xsec *= inel_bias;
    elastic_xsec *= el_bias;
    
    for( size_t i = 0; i < (size_t)pv->size(); ++i ){
      G4VProcess * proc = (*pv)(i);
      std::string theName = proc->GetProcessName();
      //std::cout <<  theName << std::endl;
      if (theName == "hIoni") { 
        G4hIonisation * ioni = (G4hIonisation*)proc;
        ioni_mfps.push_back(ioni->MeanFreePath(*theTrack));
        //if (ioni->MeanFreePath(*theTrack) != DBL_MAX) {
        //  std::cout << theName << std::endl;
        //  std::cout << ioni->MeanFreePath(*theTrack) << std::endl;
        //}
        //std::cout << (ioni->MeanFreePath(*theTrack) == DBL_MAX) << std::endl;
      }
      else if (theName == "hBrems") {
        G4hBremsstrahlung * ioni = (G4hBremsstrahlung*)proc;
        brems_mfps.push_back(ioni->MeanFreePath(*theTrack));
        //if (ioni->MeanFreePath(*theTrack) != DBL_MAX) {
        //  std::cout << theName << std::endl;
        //  std::cout << ioni->MeanFreePath(*theTrack) << std::endl;
        //}
        //std::cout << (ioni->MeanFreePath(*theTrack) == DBL_MAX) << std::endl;
      }
      else if (theName == "hPairProd") {
        G4hPairProduction * ioni = (G4hPairProduction*)proc;
        pairprod_mfps.push_back(ioni->MeanFreePath(*theTrack));
        //if (ioni->MeanFreePath(*theTrack) != DBL_MAX) {
        //  std::cout << theName << std::endl;
        //  std::cout << ioni->MeanFreePath(*theTrack) << std::endl;
        //}
        //std::cout << (ioni->MeanFreePath(*theTrack) == DBL_MAX) << std::endl;
      }
      else if (theName == "CoulombScat") {
        //std::cout << theName << std::endl;
        G4CoulombScattering * coul = (G4CoulombScattering*)proc;
        if (coul->MeanFreePath(*theTrack) != DBL_MAX) {
        //  std::cout << theName << std::endl;
        //  std::cout << "MFP: " << coul->MeanFreePath(*theTrack) << std::endl;
          coul_mfps.push_back(coul->MeanFreePath(*theTrack));
        }
        else {
          coul_mfps.push_back(0.);
        }
        //std::cout << coul->GetCurrentModel() << std::endl;
        //std::cout << coul->LambdaTable() << std::endl;
        //std::cout << coul->LambdaTablePrim() << std::endl;
      }
      else if (theName == "nCapture") {
        G4HadronCaptureProcess * cap = (G4HadronCaptureProcess*)proc;
        if (cap->GetMeanFreePath(*theTrack, 0., nullptr) != DBL_MAX) {
          cap_mfps.push_back(cap->GetMeanFreePath(*theTrack, 0., nullptr));
        }
        else {
          cap_mfps.push_back(0.);
        }
      }
      else if (theName == "nFission") {
        G4HadronFissionProcess * fis = (G4HadronFissionProcess*)proc;
        if (fis->GetMeanFreePath(*theTrack, 0., nullptr) != DBL_MAX) {
          fis_mfps.push_back(fis->GetMeanFreePath(*theTrack, 0., nullptr));
        }
        else {
          fis_mfps.push_back(0.);
        }
      }
    }

    tree->Fill();
    if( verbose && !( n % 100) ){
      std::cout << "Inelastic XSec at " << KE << " MeV " <<  inelastic_xsec << std::endl;
      std::cout << "MFP: " << inelastic_proc->GetMeanFreePath(*theTrack, 0., nullptr) << std::endl;
      std::cout << "Elastic XSec at " << KE << " MeV " <<  elastic_xsec << std::endl;
      std::cout << std::endl;
    }

    inelastic_xsecs.push_back( inelastic_xsec );
    elastic_xsecs.push_back( elastic_xsec );
    //total cross section now includes neutron capture component (zero for anything that isn't a neutron!)
    total_xsecs.push_back( elastic_xsec + inelastic_xsec);
    momenta.push_back( momentum );
    kinetic_energies.push_back( kinetic_energy );
    decay_mfps.push_back(decay_hook.GetMFP(*theTrack));

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
  TGraph decay_mfp_momentum(momenta.size(), &momenta[0], &decay_mfps[0]);
  TGraph decay_mfp_KE(kinetic_energies.size(), &kinetic_energies[0], &decay_mfps[0]);
  TGraph ioni_mfp_momentum(momenta.size(), &momenta[0], &ioni_mfps[0]);
  TGraph ioni_mfp_KE(kinetic_energies.size(), &kinetic_energies[0], &ioni_mfps[0]);
  TGraph brems_mfp_momentum(momenta.size(), &momenta[0], &brems_mfps[0]);
  TGraph brems_mfp_KE(kinetic_energies.size(), &kinetic_energies[0], &brems_mfps[0]);
  TGraph pairprod_mfp_momentum(momenta.size(), &momenta[0], &pairprod_mfps[0]);
  TGraph pairprod_mfp_KE(kinetic_energies.size(), &kinetic_energies[0], &pairprod_mfps[0]);
  TGraph coul_mfp_momentum(momenta.size(), &momenta[0], &coul_mfps[0]);
  TGraph coul_mfp_KE(kinetic_energies.size(), &kinetic_energies[0], &coul_mfps[0]);
  TGraph cap_mfp_momentum(momenta.size(), &momenta[0], &cap_mfps[0]);
  TGraph fis_mfp_momentum(momenta.size(), &momenta[0], &fis_mfps[0]);

  fout->cd();
  inel_momentum.Write( "inel_momentum" );
  inel_KE.Write( "inel_KE" );
  el_momentum.Write( "el_momentum" );
  el_KE.Write( "el_KE" );

  total_momentum.Write( "total_momentum" );
  total_KE.Write( "total_KE" );
  decay_mfp_momentum.Write("decay_mfp_momentum");
  decay_mfp_KE.Write("decay_mfp_KE");
  ioni_mfp_momentum.Write("ioni_mfp_momentum");
  ioni_mfp_KE.Write("ioni_mfp_KE");
  brems_mfp_momentum.Write("brems_mfp_momentum");
  brems_mfp_KE.Write("brems_mfp_KE");
  pairprod_mfp_momentum.Write("pairprod_mfp_momentum");
  pairprod_mfp_KE.Write("pairprod_mfp_KE");
  coul_mfp_momentum.Write("coul_mfp_momentum");
  coul_mfp_KE.Write("coul_mfp_KE");

  cap_mfp_momentum.Write("cap_mfp_momentum");
  fis_mfp_momentum.Write("fis_mfp_momentum");
  TVectorD m_vec(1);
  //m_vec[0] = MaterialMass;
  //m_vec.Write("Mass");

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

    else if( strcmp( argv[i], "-l" ) == 0 ){
      list_override = atoi( argv[i+1] );
    }

    else if( strcmp( argv[i], "-v" ) == 0 ){
      verbose_override = atoi( argv[i+1] );
    }

    else if (strcmp(argv[i], "--bi") == 0) {
      inel_bias = atof(argv[i+1]);
      std::cout << "Inel bias: " << inel_bias << std::endl;
    }
    else if (strcmp(argv[i], "--ei") == 0) {
      el_bias = atof(argv[i+1]);
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
