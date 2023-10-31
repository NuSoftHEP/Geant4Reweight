#include "G4DifferentialReweighter.hh"
#include "TFile.h"
#include "Geant4/G4PionPlus.hh"


double G4DifferentialReweighter::GetWeight(const G4ReweightTraj * theTraj) {
  double weight = G4Reweighter::GetWeight(theTraj);

  auto theStep = theTraj->GetStep(theTraj->GetNSteps()-1);
  double p = theStep->GetFullPreStepP();
  if (p < 30. || p > 1000. || theTraj->HasChild(2212).size() == 0) {
    weight *= 1.;
  }
  else {
    //Attempt one: round to nearest hist
    int rounded = int(std::round(p));
    int hist_number = (rounded - 10)/10;
    std::cout << "p: " << p << " rounded: " << rounded << " hist number: " <<
                 hist_number << std::endl;
    auto * hist = fFlattenedCosHists[hist_number];

    //iterate over 
    double max_p = -999., max_px = -999., max_py = -999., max_pz = -999.;

    for (const auto * child : theTraj->HasChild(2212)) {

      auto * c_step = child->GetStep(0);
      if (c_step->GetFullPreStepP() > max_p) {
        max_p = c_step->GetFullPreStepP();
        max_px = c_step->GetPreStepPx();
        max_py = c_step->GetPreStepPy();
        max_pz = c_step->GetPreStepPz();
        std::cout << max_p << " " <<
                     max_px << " " <<
                     max_py << " " <<
                     max_pz << std::endl;
      }
    }

    double px = theStep->GetPreStepPx();
    double py = theStep->GetPreStepPy();
    double pz = theStep->GetPreStepPz();
    std::cout << "child: " << max_p << " " <<
                 max_px << " " <<
                 max_py << " " <<
                 max_pz << std::endl;
    std::cout << p << " " <<
                 px << " " <<
                 py << " " <<
                 pz << std::endl;

    double costheta = (px*max_px + py*max_py + pz*max_pz)/(max_p*p);

    int bin = hist->FindBin(costheta);
    std::cout << "costheta: " << costheta << " bin: " << bin << " preweight: " << weight << " weight: " << hist->GetBinContent(bin) << std::endl;;
    weight *= hist->GetBinContent(bin);
  }


  return weight;
}

G4DifferentialReweighter::G4DifferentialReweighter(
    TFile * FSInput,
    const std::map<std::string, TH1D*> &FSScales,
    const fhicl::ParameterSet & material_pars,
    G4ReweightManager * rw_manager,
    std::vector<std::string> the_ints,
    TH1D * inputElasticBiasHist, bool fix)
  : G4Reweighter(FSInput, FSScales, material_pars, rw_manager, the_ints,
                 inputElasticBiasHist, fix) {
  part_def = piplus->Definition();
  fInelastic = "pi+Inelastic";
  std::cout << "Part def: " << part_def << std::endl;
  SetupProcesses();
}

void G4DifferentialReweighter::OpenCosFile(std::string input_file) {
  auto * input = TFile::Open(input_file.c_str());
  for (int i = 0; i < 100; ++i) {
    std::string name = "h_proton_cos_" + std::to_string(i);
    auto * h = (TH1D*)input->Get(name.c_str());
    fInputCosHists.push_back(h);
    h->SetDirectory(0);
    h->Scale(1./h->Integral());
    auto * flat = (TH1D*)h->Clone();
    fFlattenedCosHists.push_back(flat);
    flat->SetDirectory(0);

    for (int i = 1; i < h->GetNbinsX()+1; ++i) {
      flat->SetBinContent(i, 1.);
    }
    flat->Scale(1./flat->Integral());

    if (h->Integral() > 1.e-12)
      flat->Divide(h);
  }


  input->Close();
}
