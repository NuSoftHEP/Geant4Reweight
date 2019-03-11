#ifndef G4ReweightFinalState_h
#define G4ReweightFinalState_h

#include <string>
#include <utility>

#include "TFile.h"
#include "TH1D.h"
#include "TGraph.h"

#include "G4ReweightInter.hh"

#include <cmath>
#include <map>
#include "TTree.h"

#include <iostream> 

class G4ReweightFinalState{
  public:
    
    G4ReweightFinalState(){};
    G4ReweightFinalState(TFile * , std::string );    
    //G4ReweightFinalState(TFile * FinalStateFinal, std::string FSScaleFileName);    
    G4ReweightFinalState(TTree *, std::map< std::string, G4ReweightInter*> &, double, double, bool PiMinus = false);    
    G4ReweightFinalState(TFile *, std::map< std::string, TGraph*> &, double, double, bool PiMinus = false);    
    ~G4ReweightFinalState();

    G4ReweightInter * GetInter(std::string, std::string);
    void GetMaxAndMin( std::string );
    double GetWeight( std::string, double );
    double GetWeightFromGraph( std::string, double );

    TH1D * GetTotalVariation(){ return totalVariation; };
    TGraph * GetTotalVariationGraph(){ return totalVariationGraph; };
    TH1D * GetExclusiveVariation( std::string );
    TGraph * GetExclusiveVariationGraph( std::string );
    TH1D * GetOldHist( std::string cut ){ return oldHists[cut]; };
    TH1D * GetNewHist( std::string cut ){ return newHists[cut]; };
    TGraph * GetOldGraph( std::string cut ){ return oldGraphs[cut]; };
    TGraph * GetNewGraph( std::string cut ){ return newGraphs[cut]; };

    bool IsPiMinus(){return PiMinusFlag;};

    void AddGraphs(TGraph*, TGraph*);
    void DivideGraphs(TGraph*, TGraph*);
    bool AsGraph(){return as_graphs; };

    void SetPiMinus(){

      abs_cut =  "(int == \"pi-Inelastic\" && ( (nPi0 + nPiPlus + nPiMinus)  == 0) )";
      inel_cut = "(int == \"pi-Inelastic\" && ( (nPi0 + nPiPlus) == 0 ) && (nPiMinus == 1))";
      cex_cut =  "(int == \"pi-Inelastic\" && ( (nPiPlus + nPiMinus) == 0 ) && (nPi0 == 1))";
      dcex_cut = "(int == \"pi-Inelastic\" && ( (nPiMinus + nPi0) == 0 ) && (nPiPlus == 1))";
      prod_cut = "(int == \"pi-Inelastic\" && ( (nPiPlus + nPi0 + nPiMinus) > 1) )";             

      PiMinusFlag = true;

      theCuts["abs"]  = abs_cut;
      theCuts["inel"] = inel_cut;
      theCuts["cex"]  = cex_cut;
      theCuts["dcex"] = dcex_cut;
      theCuts["prod"] = prod_cut;


    };

  private:
    
    bool PiMinusFlag = false;
    bool as_graphs = false;

    std::map< std::string, TH1D* > exclusiveVariations; 
    TH1D * totalVariation;

    std::map< std::string, TGraph* > exclusiveVariationGraphs; 
    TGraph * totalVariationGraph;

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

    std::map< std::string, TH1D* > oldHists;
    std::map< std::string, TH1D* > newHists;
    std::map< std::string, TGraph* > oldGraphs;
    std::map< std::string, TGraph* > newGraphs;
};

#endif
