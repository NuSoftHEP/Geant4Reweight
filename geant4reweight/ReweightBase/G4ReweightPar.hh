#ifndef G4ReweightPar_h
#define G4ReweightPar_h 1

#include <string>
#include <utility> // std::pair

struct G4ReweightPar {
  std::string Name;
  std::string Cut;
  double Value;
  std::pair<double, double> Range;
};
#endif
