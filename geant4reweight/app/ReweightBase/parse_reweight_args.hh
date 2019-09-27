std::string fcl_file;
std::string output_file_override = "empty"; 
std::string input_file_override = "empty"; 

bool enablePiMinus = false;

bool parseArgs(int argc, char ** argv);

bool parseArgs(int argc, char ** argv){
  
  bool found_fcl_file = false;

  for(int i = 1; i < argc; ++i){
    if( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 ){
      std::cout << "Usage: ./G4Reweight -c config.fcl [options]" << std::endl;
      std::cout << std::endl;
      std::cout << "Options: " << std::endl;
      std::cout << "\t-o <output_file_override>.root" << std::endl;
      std::cout << "\t-i <input_file_override>.root" << std::endl;
      std::cout << "\t--PiM <enable_pi_minus>" << std::endl;

      return false;
    }
    else if( strcmp(argv[i], "-c") == 0 ){
      fcl_file = argv[i+1];
      found_fcl_file = true;
    }
    else if( strcmp(argv[i], "--PiM") == 0 ){
      enablePiMinus = atoi( argv[i+1] );
    }
    else if( strcmp( argv[i], "-o" ) == 0 ){
      output_file_override = argv[i+1];
    }
    else if( strcmp( argv[i], "-i" ) == 0 ){
      input_file_override = argv[i+1];
    }

  }

  if( !found_fcl_file ){
    std::cout << "Error: Must provide fcl file with option '-c'" << std::endl;
    return false;
  }

  
  return true;
}
