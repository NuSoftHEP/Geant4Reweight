#include "G4ReweightUtilities.hh"
#include "cetlib/filepath_maker.h"
#include "cetlib/filesystem.h"
#include "cetlib/search_path.h"


namespace g4rwutils {
fhicl::ParameterSet MakeFCLPars(std::string & fcl_file) {
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
  return fhicl::ParameterSet::make(fcl_file, lookupPolicy);
};

TFile * OpenFile(const std::string & filename) {
  TFile * the_file = 0x0;
  std::cout << "Searching for " << filename << std::endl;
  if (cet::file_exists(filename)) {
    std::cout << "File exists. Opening " << filename << std::endl;
    the_file = new TFile(filename.c_str());
    if (!the_file ||the_file->IsZombie() || !the_file->IsOpen()) {
      delete the_file;
      the_file = 0x0;
      throw cet::exception("PDSPAnalyzer_module.cc") << "Could not open " << filename;
    }
  }
  else {
    std::cout << "File does not exist here. Searching FW_SEARCH_PATH" << std::endl;
    cet::search_path sp{"FW_SEARCH_PATH"};
    std::string found_filename;
    auto found = sp.find_file(filename, found_filename);
    if (!found) {
      throw cet::exception("PDSPAnalyzer_module.cc") << "Could not find " << filename;
    }

    std::cout << "Found file " << found_filename << std::endl;
    the_file = new TFile(found_filename.c_str());
    if (!the_file ||the_file->IsZombie() || !the_file->IsOpen()) {
      delete the_file;
      the_file = 0x0;
      throw cet::exception("PDSPAnalyzer_module.cc") << "Could not open " << found_filename;
    }
  }
  return the_file;
}

}
