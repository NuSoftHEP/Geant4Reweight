#ifndef G4ReweightVarMap_h
#define G4ReweightVarMap_h

#include <string>
#include <vector>
#include <utility>

#include "fhiclcpp/ParameterSet.h"

#include "G4ReweightPar.hh"
#include <unordered_map>

class G4ReweightVarMap {

 public:
  G4ReweightVarMap() {};
  G4ReweightVarMap(const std::vector<fhicl::ParameterSet> & pars);

  double GetExclusiveVar(std::string cut, double p) const;
  double GetElasticVar(double p) const;
  double GetTotalInelVar(double p) const;

  void BuildFromPars(const std::vector<G4ReweightPar> & pars);
  void BuildFromPars(const std::vector<fhicl::ParameterSet> & pars);

  //User provides cut (i.e. reac, abs, cex) and the position in that cut's vector
  void Set(std::string cut, size_t index, double val);
  //User provides the index corresponding to the index of the original parameter vector
  void Set(size_t index, double val);

  //Reset all to 1.
  void Reset();
 private:

  void CheckCutAndIndex(std::string cut, size_t index);
  void CheckIndexToKeys(size_t index);
  double GetFromPars(const std::vector<G4ReweightPar> & pars, double p) const;

  //Double-lookup :`( but user-friendliness/flexibility :-)   [hopefully]
  std::unordered_map<std::string, std::vector<G4ReweightPar>> fVarMap;
  std::vector<std::pair<std::string, size_t>> fIndexToKeys;  

  //possibly provide override/configuration
  std::string fReacString = "reac";
  std::string fElastString = "reac";
};

#endif
