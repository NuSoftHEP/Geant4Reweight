#ifndef G4DifferentialReweighter_h
#define G4DifferentialReweighter_h
#include "G4Reweighter.hh"
#include "G4ReweightTraj.hh"
#include "G4ReweightStep.hh"
#include "TH1D.h"

class G4ReweightTraj;
class G4ReweightStep;

class G4DifferentialReweighter : public G4Reweighter {
 public:
  G4DifferentialReweighter() {};
  G4DifferentialReweighter(TFile *, const std::map<std::string, TH1D*> &,
               const fhicl::ParameterSet & material_pars,
               G4ReweightManager * rw_manager,
               std::vector<std::string> the_ints,
               TH1D * inputElasticBiasHist = nullptr, bool fix = false);

  double GetWeight(const G4ReweightTraj * theTraj) override;

  void OpenCosFile(std::string input_file);
 private:
  std::vector<TH1D*> fInputCosHists;
  std::vector<TH1D*> fFlattenedCosHists;
};
#endif
