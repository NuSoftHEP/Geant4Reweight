#include "G4ReweightSuite.hh"
#include "G4ReweightUtilities.hh"
#include "cetlib/filepath_maker.h"
#include "cetlib/filesystem.h"
#include "cetlib/search_path.h"

using ParSetVector = std::vector<fhicl::ParameterSet>;

G4ReweightSuite::~G4ReweightSuite() {
  for (auto & rw : fReweighters) {
    delete rw.second;
  }
  for (auto & f : fFracsFiles) {
    f.second->Close();
    delete f.second;
  }
  delete fManager;
}

G4ReweightSuite::G4ReweightSuite(fhicl::ParameterSet & pset) {
  //Get the material list and make the manager
  auto materials = pset.get<ParSetVector>("Materials");
  fManager = new G4ReweightManager(materials);

  //fParameters = pset.get<std::vector<fhicl::ParameterSet>>("ParameterSet");
  auto all_reweights = pset.get<ParSetVector>("Reweights");

  for (auto & rw : all_reweights) {
    std::string name = rw.get<std::string>("Name");
    std::cout << "Making " << name << std::endl;
    
    int pdg = rw.get<int>("PDG");
    auto this_material = rw.get<fhicl::ParameterSet>("Material");
    auto mat_name = this_material.get<std::string>("Name");
    auto this_part_mat = std::make_pair(pdg, mat_name);
    auto fracs_file = rw.get<std::string>("FracsFile");

    auto parameter_set = rw.get<ParSetVector>("ParameterSet");

    fPartMats.push_back(this_part_mat);

    fNParameters[this_part_mat] = parameter_set.size();
    fFracsFiles[this_part_mat] = g4rwutils::OpenFile(fracs_file);
    fReweighters[this_part_mat] = new G4MultiReweighter(
        pdg, *fFracsFiles[this_part_mat], parameter_set, this_material,
        fManager);
    for (const auto & par: parameter_set) {
      fParameterNames[this_part_mat].push_back(par.get<std::string>("Name"));
    }
  }
}

bool G4ReweightSuite::CheckMaterial(const std::string & material) const {
  return (
    std::find_if(
      fReweighters.begin(), fReweighters.end(),
        [&](const auto & it){
          return (it.first.second == material);
        }
    ) != fReweighters.end()
  );
}

bool G4ReweightSuite::CheckPDG(int pdg) const {
  return (
    std::find_if(
      fReweighters.begin(), fReweighters.end(),
        [&](const auto & it){
          return (it.first.first == pdg);
        }
    ) != fReweighters.end()
  );
}


std::vector<double> G4ReweightSuite::Scan(
    G4ReweightTraj & traj,
    PartMat_t part_mat, size_t param_number,
    size_t nsteps, double start, double end) {

  //TODO -- check param_number, start < end etc.
  auto * the_reweighter = fReweighters[part_mat];
  size_t npars = fNParameters[part_mat];
  double delta = (end - start)/(nsteps-1);

  //If nsteps == 0, just return 1 weight at start
  if (nsteps == 0) {
    std::vector<double> pars(npars, 1.);
    pars[param_number] = start;//Set the specific parameter
    the_reweighter->SetAllParameterValues({pars});
    return {the_reweighter->GetWeightFromSetParameters(traj)};
  }

  //If not, scan over the full range
  std::vector<double> weights;
  for (size_t i = 0; i < nsteps; ++i) {
    std::vector<double> pars(npars, 1.);
    pars[param_number] = start + i*delta;//Set the specific parameter
    the_reweighter->SetAllParameterValues({pars});
    weights.push_back(the_reweighter->GetWeightFromSetParameters(traj));
  }
  return weights;
}

std::vector<double> G4ReweightSuite::Scan(
    G4ReweightTraj & traj,
    int pdg, std::string material, size_t param_number,
    size_t nsteps, double start, double end) {
  return Scan(traj, {pdg, material}, param_number, nsteps, start, end);
}
