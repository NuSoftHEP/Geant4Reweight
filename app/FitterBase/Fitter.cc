#include "G4ReweightFitManager.hh"
#include "FitParameter.hh"
#include <vector>
#include <string>
#include "TVectorD.h"

#include "TFile.h"
#include "TH2D.h"
#include "TGraph2D.h"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

std::string set_prec(double);

std::string fcl_file;
std::string output_file_override = "empty"; 

int scan_override = -1;
int save_override = -1;

bool parseArgs(int argc, char ** argv);

int main(int argc, char ** argv){

  if( !parseArgs(argc, argv) ) return 0;

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(fcl_file);
  bool fSave    = ps.get< bool >( "Save", false );
  if( save_override != -1 )
    fSave = save_override;


  std::string outFileName = ps.get< std::string >( "OutputFile" );
  if( output_file_override  != "empty" ){
    outFileName = output_file_override;
  }

  G4ReweightFitManager FitMan( outFileName, fSave);

  std::vector< fhicl::ParameterSet > FitParSets = ps.get< std::vector< fhicl::ParameterSet > >("ParameterSet");

  try{ 
    FitMan.MakeFitParameters( FitParSets );

    ///Defining MC Sets
    std::vector< fhicl::ParameterSet > FCLSets = ps.get< std::vector< fhicl::ParameterSet > >("Sets");
    FitMan.DefineMCSets( FCLSets );
    ///////////////////////////////////////////

    ///Defining experiments
    FitMan.DefineExperiments( ps );
    ///////////////////////////////////////////


    FitMan.GetAllData();

    FitMan.MakeMinimizer( ps );

    bool fFitScan = ps.get< bool >( "FitScan", false );
    if( scan_override != -1 )
      fFitScan = scan_override;

    FitMan.RunFitAndSave(fFitScan);
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


