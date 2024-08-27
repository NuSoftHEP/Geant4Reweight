#ifndef G4ReweightVarMap_h
#define G4ReweightVarMap_h

#include <string>
#include <vector>
#include <utility>

#include "G4ReweightPar.hh"
#include <unordered_map>

class G4ReweightVarMap {

 public:
  G4ReweightVarMap() {};

  double GetExclusiveVar(std::string cut, double p) const;
  double GetElasticVar(double p) const;
  double GetTotalInelVar(double p) const;

  void BuildFromPars(const std::vector<G4ReweightPar> & pars);

  //User provides cut (i.e. reac, abs, cex) and the position in that cut's vector
  void Set(std::string cut, size_t index, double val);
  //User provides the index corresponding to the index of the original parameter vector
  void Set(size_t index, double val);
 private:

  void CheckCutAndIndex(std::string cut, size_t index);
  void CheckIndexToKeys(size_t index);
  double GetFromPars(const std::vector<G4ReweightPar> & pars, double p) const;

  //Double-lookup :`( but user-friendliness/flexibility :-)   [hopefully]
  std::unordered_map<std::string, std::vector<G4ReweightPar>> fVarMap;
  std::vector<std::pair<std::string, size_t>> fIndexToKeys;  

  //For now, make these const -- possibly provide override/configuration
  const std::string fReacString = "reac";
  const std::string fElastString = "reac";
};

#endif
