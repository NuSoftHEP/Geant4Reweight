#ifndef G4MultiReweighter_hh
#define G4MultiReweighter_hh

#include "fhiclcpp/ParameterSet.h"

#include "geant4reweight/PropBase/G4ReweightParameterMaker.hh"
#include "geant4reweight/ReweightBase/G4Reweighter.hh"
#include "geant4reweight/ReweightBase/G4ReweighterFactory.hh"

class G4ReweightManager;
class G4ReweightTraj;

#include "TDecompChol.h"
#include "TRandom3.h"

class TFile;

#include <map>
#include <string>
#include <utility> // std::pair
#include <vector>

class G4MultiReweighter{
 public:
  G4MultiReweighter(int pdg,
                    TFile & fracsFile,
                    const std::vector<fhicl::ParameterSet> & parSet,
                    const fhicl::ParameterSet & material,
                    G4ReweightManager * rw_manager,
                    TFile & fitResults, size_t nThrows = 100,
                    int seed = 0);

  G4MultiReweighter(int pdg,
                    TFile & fracsFile,
                    const std::vector<fhicl::ParameterSet> & parSet,
                    const fhicl::ParameterSet & material,
                    G4ReweightManager * rw_manager,
                    size_t nThrows = 100, int seed = 0);

  
  double GetWeightFromNominal(G4ReweightTraj & traj/*,
                              bool alternate_weight = false*/);

  double GetWeightFrom1DThrow(G4ReweightTraj & traj, size_t iThrow/*,
                              bool alternate_weight = false*/);
  std::vector<double> GetWeightFromAll1DThrows(G4ReweightTraj & traj/*,
                                               bool alternate_weight = false*/);

  std::pair<double, double> GetPlusMinusSigmaParWeight(
      G4ReweightTraj & traj, size_t iPar/*, bool alternate_weight = false*/);

  double GetWeightFromCorrelatedThrow(G4ReweightTraj & traj, size_t iThrow/*,
                                      bool alternate_weight = false*/);
  std::vector<double> GetWeightFromAllCorrelatedThrows(G4ReweightTraj & traj/*,
                                                       bool alternate_weight = false*/);

  bool CheckDecompSuccess() {return decompSuccess;};

  bool SetParameterValue(size_t iPar, double value); 
  bool SetAllParameterValues(std::vector<double> values);
  double GetWeightFromSetParameters(G4ReweightTraj & traj/*,
                                    bool alternate_weight = false*/);

  void SetInelasticPreBias(double bias) {reweighter->SetInelasticPreBias(bias);};
  void SetElasticPreBias(double bias) {reweighter->SetElasticPreBias(bias);};

  /*
  std::map<std::string, std::pair<double, double>> GetPlusMinusSigmaWeights();
  std::map<std::string, double> GetNominalVar();

  std::map<std::string, double> Get1DVar(size_t iThrow);
  std::map<std::string, std::vector<double>> GetAll1DVars();

  std::map<std::string, double> GetCorrelatedVar(size_t iThrow);
  std::map<std::string, std::vector<double>> GetAllCorrelatedVars();

  std::pair<double, double> GetPlusMinusSigmaParVar(size_t iPar);
  std::map<std::string, std::pair<double, double>> GetPlusMinusSigmaVars();
  */
 
 private:

  void GenerateThrows();

  G4ReweighterFactory factory;
  G4ReweightParameterMaker parMaker;
  G4Reweighter * reweighter; 
  size_t numberOfThrows;
  TRandom3 rng;

  bool decompSuccess;
  TDecompChol choleskyMatrix;    

  //std::map<std::string, std::vector<double>> paramRandVals;
  //std::map<std::string, double> paramNominalVals;
  //std::map<std::string, size_t> paramIndicies;

  std::vector<std::vector<double>> paramRandomVals;
  std::vector<double> paramNominalVals;
  std::vector<std::string> paramNames;
  std::vector<double> paramSigmas;

  std::map<std::string,double> paramVals;
};

#endif
