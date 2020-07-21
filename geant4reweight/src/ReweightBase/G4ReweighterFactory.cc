#include "G4ReweighterFactory.hh"

//argument list:
//
//PDG Code
//totalInput = XSection File
//FSInput = FracsFile
//FSScales = ParMaker->GetFSGHists()
//inputElasticBiasHist = ParMaker->GetElasticHist()
//
//Check all of these things are initialised properly and how to get the right input data.

G4Reweighter * G4ReweighterFactory::BuildReweighter(int PDG, TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D*> & FSScales, TH1D * inputElasticBiasHist ){
  switch( PDG ){

    case  211:
    {
      return new G4PiPlusReweighter(totalInput, FSInput, FSScales, inputElasticBiasHist);
    }
    case -211:
    {
      return new G4PiMinusReweighter(totalInput, FSInput, FSScales, inputElasticBiasHist);
    }
    case 2212:
    {
      return new G4ProtonReweighter(totalInput, FSInput, FSScales, inputElasticBiasHist);
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
