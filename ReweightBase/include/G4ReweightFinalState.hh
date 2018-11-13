#ifndef G4ReweightFinalState_h
#define G4ReweightFinalState_h

#include <string>
#include <utility>

#include "TFile.h"
#include "TH1D.h"

#include "G4ReweightInter.hh"

#include <cmath>
#include <map>

class G4ReweightFinalState{
  public:
    
    G4ReweightFinalState(TFile * FinalStateFinal, std::string FSScaleFileName);    
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

    //std::map< std::string, G4ReweightInter* > FSInters;
};

#endif
