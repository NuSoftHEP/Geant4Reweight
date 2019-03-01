#ifndef FitSample_h
#define FitSample_h 1

#include <string>
#include <vector>
#include "FitParameter.hh"

struct FitSample{
  std::string theName;
  std::vector< FitParameter > Parameters;
  bool Raw;
  std::string theFile;
};
#endif
