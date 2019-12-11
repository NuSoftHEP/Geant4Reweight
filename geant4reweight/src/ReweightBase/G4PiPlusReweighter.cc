#include "G4PiPlusReweighter.hh"

G4PiPlusReweighter::G4PiPlusReweighter(TFile * totalInput, TFile * FSInput, std::map< std::string, TGraph* > &FSScales){
  fInelastic = "pi+Inelastic";
  theInts = {"inel", "cex", "abs", "dcex", "prod"};
  Initialize(totalInput, FSInput, FSScales);
}

G4PiPlusReweighter::G4PiPlusReweighter(TFile * totalInput, TFile * FSInput, const std::map< std::string, TH1D* > &FSScales, TH1D * inputElasticBiasHist){
  fInelastic = "pi+Inelastic";
  theInts = {"inel", "cex", "abs", "dcex", "prod"};
  Initialize(totalInput,FSInput,FSScales,inputElasticBiasHist);
}

std::string G4PiPlusReweighter::GetInteractionSubtype( G4ReweightTraj & theTraj ){
    int nPi0     = theTraj.HasChild(111).size();
    int nPiPlus  = theTraj.HasChild(211).size();
    int nPiMinus = theTraj.HasChild(-211).size();

    if( (nPi0 + nPiPlus + nPiMinus) == 0){
      return "abs";
    }
    else if( (nPiPlus + nPiMinus) == 0 && nPi0 == 1 ){
      return "cex";
    }
    else if( (nPiPlus + nPiMinus + nPi0) > 1 ){
      return "prod";
    }
    else if( (nPi0 + nPiMinus) == 0 && nPiPlus == 1 ){
      return "inel";
    }
    else if( (nPi0 + nPiPlus) == 0 && nPiMinus == 1 ){
      return "dcex";
    }

    return "";
}

G4PiPlusReweighter::~G4PiPlusReweighter(){}
