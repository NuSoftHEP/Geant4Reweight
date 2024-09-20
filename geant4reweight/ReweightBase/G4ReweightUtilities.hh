#ifndef G4ReweightUtilities_h
#define G4ReweightUtilities_h

#include "fhiclcpp/ParameterSet.h"
#include "TFile.h"
namespace g4rwutils {
fhicl::ParameterSet MakeFCLPars(std::string & fcl_file);
TFile * OpenFile(const std::string & fracs_file);
}
#endif
