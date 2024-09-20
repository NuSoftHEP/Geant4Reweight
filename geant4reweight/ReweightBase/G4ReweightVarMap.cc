#include "G4ReweightVarMap.hh"
#include <stdexcept>
#include <map>


G4ReweightVarMap::G4ReweightVarMap(
    const std::vector<fhicl::ParameterSet> & pars) {
  BuildFromPars(pars);
}

void G4ReweightVarMap::BuildFromPars(
    const std::vector<fhicl::ParameterSet> & pars) {
  std::vector<G4ReweightPar> rw_pars;
  for (const auto & par : pars) {
    rw_pars.push_back(G4ReweightPar(par));
  }
  BuildFromPars(rw_pars);
}

void G4ReweightVarMap::BuildFromPars(const std::vector<G4ReweightPar> & pars) {

  //Iterate over the incoming parameters
  for (const auto & new_par : pars) {
    const auto & the_cut = new_par.Cut;
    double new_low = new_par.Range.first;
    double new_high = new_par.Range.second;

    auto & old_pars = fVarMap.at(the_cut);
    //Check if they overlap with already-added pars
    //The to-be-added parameter can have the same low as a previously-added parameter's high
    //and/or it can have the same high as a previously-added's low
    for (const auto & old_par : old_pars) {
      auto old_low = old_par.Range.first;
      auto old_high = old_par.Range.second;
      if ((new_low <= old_low && old_low < new_high) ||
          (new_low < old_high && old_high <= new_high)) {
        std::string message
            = "Attempting to set overlapping G4ReweightPars with ranges\n\t" +
              std::to_string(new_low) + " " + std::to_string(new_high) + " " +
              std::to_string(old_low) + " " + std::to_string(old_high);
        throw std::runtime_error(message);
      }
    }

    //Add the parameter to variations
    fVarMap[the_cut].push_back(new_par);
    fIndexToKeys.push_back({the_cut, fVarMap[the_cut].size()-1});
  }
}

void G4ReweightVarMap::Set(std::string cut, size_t index, double val) {
  CheckCutAndIndex(cut, index);

  fVarMap[cut][index].Value = val;
}
void G4ReweightVarMap::CheckCutAndIndex(std::string cut, size_t index) {
  if (fVarMap.find(cut) == fVarMap.end()) {
    std::string message = "Trying to access nonexistent parameter with cut " + cut;
    throw std::runtime_error(message);
  }

  if (index >= fVarMap[cut].size()) {
    std::string message = "Out of range for cut " + cut + ". Has size " +
                          std::to_string(fVarMap[cut].size()) +
                          ", requested " + std::to_string(index);
    throw std::runtime_error(message);
  }
}

void G4ReweightVarMap::CheckIndexToKeys(size_t index) {
  if (index >= fIndexToKeys.size()) {
    std::string message = "Trying to access a parameter. Have " +
                          std::to_string(fIndexToKeys.size()) + ", requested " +
                          std::to_string(index);
    throw std::runtime_error(message);
  }
}

void G4ReweightVarMap::Set(size_t index, double val) {
  CheckIndexToKeys(index);

  auto & key = fIndexToKeys[index];
  fVarMap[key.first][key.second].Value = val;
}

void G4ReweightVarMap::Reset() {
  for (auto & [cut, vec] : fVarMap) {
    for (auto & v : vec) v.Value = 1.;
  }
}

double G4ReweightVarMap::GetFromPars(const std::vector<G4ReweightPar> & pars,
                                     double p) const {
  //Loop over pars in the vector
  for (const auto & par : pars) {
    //Check if the momentum is in range
    if ((par.Range.first <= p) && (p < par.Range.second)) {
      return par.Value;
    }
  }

  //Return 1 if not in range of any
  return 1.;
}

double G4ReweightVarMap::GetExclusiveVar(std::string cut, double p) const {
  //For this, don't throw an exception if the cut is not in the map,
  //just return 1
  if (fVarMap.find(cut) == fVarMap.end()) return 1.;

  //Get the vector of parameters for this cut
  const auto & pars = fVarMap.at(cut);
  return GetFromPars(pars, p);
}

//Elastic and Reaction/Total Inelastic are stored in the same map, 
//But provide these for clarity for users
double G4ReweightVarMap::GetElasticVar(double p) const {
  return GetExclusiveVar(fElastString, p);
}
double G4ReweightVarMap::GetTotalInelVar(double p) const {
  return GetExclusiveVar(fReacString, p);
}

