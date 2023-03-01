#include "geant4reweight/ReweightBase/G4ReweighterFactory.hh"

#include "geant4reweight/ReweightBase/G4PiPlusReweighter.hh"
#include "geant4reweight/ReweightBase/G4PiMinusReweighter.hh"
#include "geant4reweight/ReweightBase/G4ProtonReweighter.hh"
#include "geant4reweight/ReweightBase/G4NeutronReweighter.hh"
#include "geant4reweight/ReweightBase/G4KPlusReweighter.hh"
#include "geant4reweight/ReweightBase/G4KMinusReweighter.hh"
#include "geant4reweight/ReweightBase/G4Reweighter.hh"

#include <iostream>

G4Reweighter * G4ReweighterFactory::BuildReweighter(
    int PDG, TFile * FSInput,
    const std::map<std::string, TH1D*> & FSScales,
    const fhicl::ParameterSet & material_pars,
    G4ReweightManager * rw_manager,
    TH1D * inputElasticBiasHist,
    bool fix_total){
  switch (PDG) {
    case 211:
    {
      return new G4PiPlusReweighter(FSInput, FSScales, material_pars,
                                    rw_manager, inputElasticBiasHist,
                                    fix_total);
    }
    case -211:
    {
      return new G4PiMinusReweighter(FSInput, FSScales, material_pars,
                                     rw_manager, inputElasticBiasHist,
                                     fix_total);
    }
    case 2212:
    {
      return new G4ProtonReweighter(FSInput, FSScales, material_pars,
                                    rw_manager, inputElasticBiasHist,
                                    fix_total);
    }
    case 2112:
    {
      return new G4NeutronReweighter(FSInput, FSScales, material_pars,
                                     rw_manager, inputElasticBiasHist,
                                     fix_total);
    }   
    case 321:
    {
      return new G4KPlusReweighter(FSInput, FSScales, material_pars,
                                   rw_manager, inputElasticBiasHist,
                                   fix_total);
    }
    case -321:
    {
      return new G4KMinusReweighter(FSInput, FSScales, material_pars,
                                   rw_manager, inputElasticBiasHist,
                                   fix_total);
    }
    default:
      std::cerr << "Error: Reweighter for PDG code " << PDG << " is not implemented" << std::endl;
      return nullptr;
  }

}
