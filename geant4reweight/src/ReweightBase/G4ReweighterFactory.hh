#ifndef G4ReweighterFactory_h
#define G4ReweighterFactory_h

#include "G4Reweighter.hh"
#include "G4ProtonReweighter.hh"
#include "G4PiPlusReweighter.hh"
#include "G4PiMinusReweighter.hh"

#include "TFile.h"
#include <map>
#include "TH1D.h"

class G4Reweighter;

class G4ReweighterFactory{

  public:
   
    G4Reweighter * BuildReweighter( int PDG, TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D*> & FSScales, TH1D * inputElasticBiasHist=0x0 );
    
};

#endif
