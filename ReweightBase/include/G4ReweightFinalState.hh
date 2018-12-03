#ifndef G4ReweightFinalState_h
#define G4ReweightFinalState_h

#include <string>
#include <utility>

#include "TFile.h"
#include "TH1D.h"

#include "G4ReweightInter.hh"

#include <cmath>
#include <map>
#include "TTree.h"

class G4ReweightFinalState{
  public:
    
    G4ReweightFinalState(){};
    G4ReweightFinalState(TFile * , std::string );    
    //G4ReweightFinalState(TFile * FinalStateFinal, std::string FSScaleFileName);    
    G4ReweightFinalState(TTree *, std::map< std::string, G4ReweightInter*> &, double, double);    
    ~G4ReweightFinalState();

    G4ReweightInter * GetInter(std::string, std::string);
    void GetMaxAndMin( std::string );
    double GetWeight( std::string, double );

    TH1D * GetTotalVariation(){ return totalVariation; };
    TH1D * GetExclusiveVariation( std::string );

  private:
    
    std::map< std::string, TH1D* > exclusiveVariations; 

    TH1D * totalVariation;

    double Maximum;
    double Minimum;
  
    std::vector< std::string > theInts = {"inel", "cex", "abs", "dcex", "prod"};

    std::string abs_cut =  "(int == \"pi+Inelastic\" && ( (nPi0 + nPiPlus + nPiMinus)  == 0) )";
    std::string inel_cut = "(int == \"pi+Inelastic\" && ( (nPi0 + nPiMinus) == 0 ) && (nPiPlus == 1))";
    std::string cex_cut =  "(int == \"pi+Inelastic\" && ( (nPiPlus + nPiMinus) == 0 ) && (nPi0 == 1))";
    std::string dcex_cut = "(int == \"pi+Inelastic\" && ( (nPiPlus + nPi0) == 0 ) && (nPiMinus == 1))";
    std::string prod_cut = "(int == \"pi+Inelastic\" && ( (nPiPlus + nPi0 + nPiMinus) > 1) )";

    std::map< std::string, std::string > theCuts = {
      {"inel", inel_cut},
      {"abs",  abs_cut},
      {"cex",  cex_cut},
      {"dcex", dcex_cut},
      {"prod", prod_cut},
    };

};

#endif
