#include "G4NeutronReweighter.hh"
#include "G4ReweightStep.hh"


G4NeutronReweighter::G4NeutronReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales){
  fInelastic = "neutronInelastic";
  theInts = {"total"};
  Initialize(totalInput, FSInput, FSScales);
}

G4NeutronReweighter::G4NeutronReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist){
  fInelastic = "neutronInelastic";
  theInts = {"total"};
  Initialize(totalInput,FSInput,FSScales,inputElasticBiasHist);
}

std::string G4NeutronReweighter::GetInteractionSubtype( G4ReweightTraj & theTraj ){
    return "total";
}


double G4NeutronReweighter::GetGenericWeight( G4ReweightTraj * theTraj, bool weight_elastic) {
  double total_weight = 1.0;
  double dL = 1.0;

  size_t nsteps = theTraj->GetNSteps();

  for (size_t i = 0; i < nsteps; ++i) {
    auto * theStep = theTraj->GetStep(i);
    //std::cout << "Kinetic energy of neutron segment: " << seg.KE << "Length of neutron segment: " << seg.length << std::endl;
    double L_remaining = theStep->GetStepLength();
    double p = theStep->GetFullPreStepP();

    double nom_xsec = 1./(weight_elastic ?
                          GetNominalElasticMFP(p) :
                          GetNominalMFP(p));
    if (nom_xsec < 1.e-8) continue;
    double var_xsec = 1./(weight_elastic ?
                          GetBiasedElasticMFP(p) :
                          GetBiasedMFP(p));
    double delta_sigma = var_xsec - nom_xsec;
    // std::cout << p << " Nominal Cross Section: " << nom_xsec << " Varied: " << var_xsec << std::endl;

    // Loop over steps of fixed length dL within segment
    while (L_remaining > 0) {
      double step = std::min(dL, L_remaining);
      L_remaining -= step;


      bool is_final_step = (L_remaining <= 0);
      bool interacted = (
        weight_elastic ?
        (is_final_step && theStep->GetStepChosenProc() == "hadElastic") :
        (is_final_step && (theStep->GetStepChosenProc() == "neutronInelastic"))
      );

      if (interacted) {
	// Reweight based on interaction probabilities at final point
        double nom_int_prob = 1.0 - std::exp(-nom_xsec * step);
        double var_int_prob = 1.0 - std::exp(-var_xsec * step);
        double w = var_int_prob / nom_int_prob;
	      // std::cout << "\tSegment Weight (interaction): " << w << " Probs: " << nom_int_prob << " " << var_int_prob << std::endl;
        total_weight *= w;
      } else {
	// Reweight survival probability along the path
        double w = std::exp(-delta_sigma * step);
        //std::cout << "Segment Weight (no interaction): " << w << "\n" << "delta_sigma * step / inverse_n: " << delta_sigma * step / inverse_n << std::endl;
        total_weight *= w;
      }
    }
  }
  return total_weight;
}

double G4NeutronReweighter::GetWeight( G4ReweightTraj * theTraj) {
  return GetGenericWeight(theTraj, false);
}

double G4NeutronReweighter::GetElasticWeight( G4ReweightTraj * theTraj) {
  return GetGenericWeight(theTraj, true);
}

G4NeutronReweighter::~G4NeutronReweighter(){}
