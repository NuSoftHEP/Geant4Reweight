#include "G4ReweighterFactory.hh"

G4Reweighter * G4ReweighterFactory::BuildReweighter(
    int PDG, TFile * totalInput, TFile * FSInput,
    const std::map<std::string, TH1D*> & FSScales, TH1D * inputElasticBiasHist,
    bool fix_total){
  switch (PDG) {
    case 211:
    {
      return new G4PiPlusReweighter(totalInput, FSInput, FSScales, inputElasticBiasHist, fix_total);
    }
    case -211:
    {
      return new G4PiMinusReweighter(totalInput, FSInput, FSScales, inputElasticBiasHist, fix_total);
    }
    case 2212:
    {
      return new G4ProtonReweighter(totalInput, FSInput, FSScales, inputElasticBiasHist, fix_total);
    }
    case 2112:
    {
      return new G4NeutronReweighter(totalInput,FSInput,FSScales,inputElasticBiasHist);
    }   
    default:
      std::cerr << "Error: Reweighter for PDG code " << PDG << " is not implemented" << std::endl;
      return 0x0;
  }

}
