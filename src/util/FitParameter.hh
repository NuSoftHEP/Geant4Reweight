#ifndef FitParameter_h
#define FitParameter_h 1

#include <string>

struct FitParameter{
  std::string Name;
  std::string Cut;
  double Value;
  bool Dummy;
  std::pair< double, double > Range;


  double ScanStart;
  double ScanDelta;
  int    ScanSteps;
};
#endif
