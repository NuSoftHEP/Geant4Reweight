#include "geant4reweight/FitterBase/G4ReweightFitManager.hh"
#include "geant4reweight/FitterBase/G4ReweightPionFitManager.hh"
#include "geant4reweight/ReweightBase/G4ReweightManager.hh"
#include "geant4reweight/FitterBase/FitParameter.hh"
#include <vector>
#include <string>
#include "TVectorD.h"

#include "TFile.h"
#include "TH2D.h"
#include "TGraph2D.h"

//#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

//#ifdef FNAL_FHICL
#include "cetlib/filepath_maker.h"
//#endif

std::string set_prec(double);

std::string fcl_file;
std::string output_file_override = "empty"; 

int scan_override = -1;
int save_override = -1;

bool parseArgs(int argc, char ** argv);

int main(int argc, char ** argv){

  if( !parseArgs(argc, argv) ) return 0;

  fhicl::ParameterSet pset;

  //#ifdef FNAL_FHICL
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

  //#else
  //  pset = fhicl::make_ParameterSet(fcl_file);
  //#endif

  bool fSave = pset.get< bool >( "Save", false );
  if( save_override != -1 )
    fSave = save_override;


  std::string outFileName = pset.get< std::string >( "OutputFile" );
  if( output_file_override  != "empty" ){
    outFileName = output_file_override;
  }

  //Get the materials
  std::vector<fhicl::ParameterSet> FCLSets =
      pset.get<std::vector<fhicl::ParameterSet>>("Sets");
  std::vector<fhicl::ParameterSet> all_materials;
  for (size_t i = 0; i < FCLSets.size(); ++i) {
    auto set = FCLSets[i];
    fhicl::ParameterSet material = set.get<fhicl::ParameterSet>("Material");
    bool found_material = false;
    for (size_t j = 0; j < all_materials.size(); ++j) {
      if (all_materials[j].get<std::string>("Name") ==
          material.get<std::string>("Name")) {
        found_material = true;
        break;
      }
    }
    if (!found_material) {
      all_materials.push_back(material);
      std::cout << "Adding " << material.get<std::string>("Name") << std::endl;
    }
  }
  G4ReweightManager rw_manager(all_materials);

  //scales contribution to chi2 from total cross section data
  double total_mix = pset.get<double>("TotalMix",1.0);
 
  G4ReweightPionFitManager FitMan( outFileName, fSave, &rw_manager, total_mix );
  //setup exclusive channel code 
  FitMan.SetExclusiveChannels();         
 
  
  std::vector< fhicl::ParameterSet > FitParSets = pset.get< std::vector< fhicl::ParameterSet > >("ParameterSet");

  try{ 
    FitMan.MakeFitParameters( FitParSets );

    ///Defining MC Sets
    FitMan.DefineMCSets( FCLSets );
    ///////////////////////////////////////////

    ///Defining experiments
    FitMan.DefineExperiments( pset );
    std::cout << "Experiments defined" << std::endl;
    ///////////////////////////////////////////

    FitMan.MakeMinimizer( pset );
    FitMan.GetAllData();

    bool fFitScan = pset.get< bool >( "FitScan", false );
    if( scan_override != -1 )
      fFitScan = scan_override;

    std::cout << "Preparing to run fit" << std::endl;
    FitMan.RunFitAndSave(fFitScan);

    std::cout << "Fit is run" << std::endl;
  }
  catch( const std::exception &e ){
    std::cout << "Caught exception " << std::endl;  
  }
  
  return 0;
}

bool parseArgs(int argc, char ** argv){

  bool found_fcl_file = false;

  for( int i = 1; i < argc; ++i ){
    if( ( strcmp( argv[i], "--help" )  == 0 ) || ( strcmp( argv[i], "-h" ) == 0 ) ){
      std::cout << "Usage: ./Fitter -c <fitter_config>.fcl [options]" << std::endl;
      std::cout << std::endl;
      std::cout << "Options: " << std::endl;
      std::cout << "\t-o <output_file_override>.root" << std::endl;
      std::cout << "\t--scan <override scan setting: 0 = no, 1 = yes>" << std::endl;
      std::cout << "\t--save <override save setting: 0 = no, 1 = yes>" << std::endl;

      return false;
    }
    else if( strcmp( argv[i], "-c" ) == 0 ){
      fcl_file = argv[i+1];
      found_fcl_file = true;
    }
    else if( strcmp( argv[i], "-o" ) == 0 ){
      output_file_override = argv[i+1];
    }
    else if( strcmp( argv[i], "--scan" ) == 0 ){
      scan_override = atoi( argv[i+1] );
      if( scan_override > 1 || scan_override < 0 ){
        std::cout << "Scan override value must be 0 or 1" << std::endl;
        return false;
      }
    }
    else if( strcmp( argv[i], "--save" ) == 0 ){
      save_override = atoi( argv[i+1] );
      if( save_override > 1 || save_override < 0 ){
        std::cout << "Save override value must be 0 or 1" << std::endl;
        return false;
      }
    }
  }

  if( !found_fcl_file ){
    std::cout << "Error: Must provide fcl file with option '-c'" << std::endl;
    return false;
  }

  return true;
}


