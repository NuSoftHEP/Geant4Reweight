#ifndef FitSample_h
#define FitSample_h 1

#include <string>

struct FitSample{
  std::string theName;
  double abs, cex, inel, prod, dcex;
  bool Raw;
  std::string theFile;
};
#endif
