#include "G4ReweighterFactory.hh"

G4Reweighter * G4ReweighterFactory::BuildReweighter(int PDG, TFile * input, const std::map< std::string, TH1D*> & FSScales, TH1D * inputElasticBiasHist ){
  switch( PDG ){

    case  211:
    {
      return new G4PiPlusReweighter(input, FSScales, inputElasticBiasHist);
    }
    case -211:
    {
      return new G4PiMinusReweighter(input, FSScales, inputElasticBiasHist);
    }
    case 2212:
    {
      return new G4ProtonReweighter(input, FSScales, inputElasticBiasHist);
    }
    default:
      std::cerr << "Error: Reweighter for PDG code " << PDG << " is not implemented" << std::endl;
      return 0x0;
  }

}
