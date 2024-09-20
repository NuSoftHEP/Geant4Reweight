#ifndef G4ReweightPar_h
#define G4ReweightPar_h 1

#include <string>
#include <utility> // std::pair

#include "fhiclcpp/ParameterSet.h"

struct G4ReweightPar {

  G4ReweightPar() : Name("none"), Cut("none"), Value(1.), Range({0., 1.}) {};
  G4ReweightPar(const fhicl::ParameterSet & par)
    : Name(par.get<std::string>("Name")), Cut(par.get<std::string>("Cut")),
      Value(par.get<double>("Value")),
      Range(par.get<std::pair<double, double>>("Range")) {};

  std::string Name;
  std::string Cut;
  double Value;
  std::pair<double, double> Range;
};
#endif
