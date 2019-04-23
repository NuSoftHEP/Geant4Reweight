#ifndef G4Reweighter_h
#define G4Reweighter_h

#include <string>
#include <utility>

#include "TFile.h"
#include "TH1D.h"
#include "TGraph.h"

#include "G4ReweightTraj.hh"

#include <cmath>
#include <map>
#include "TTree.h"

#include <iostream> 

class G4ReweightTraj;
//class G4ReweightStep;

class G4Reweighter{
  public:
    
    G4Reweighter(){};
    G4Reweighter(TFile *, std::map< std::string, TGraph*> &,/* double, double,*/ bool PiMinus = false);    
    G4Reweighter(TFile *, const std::map< std::string, TH1D*> &, bool PiMinus = false);    
    ~G4Reweighter();

    double GetWeight( std::string, double );
    double GetWeightFromGraph( std::string, double );

    double GetWeight( G4ReweightTraj * );
    double GetNominalMFP( double );
    double GetBiasedMFP( double );

    void SetTotalGraph( TFile * );

    void SetNewHists( const std::map< std::string, TH1D* > &FSScales );

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
      fInelastic = "pi-Inelastic";
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

    TGraph * totalGraph;

    double Maximum;
    double Minimum;

    std::string fInelastic = "pi+Inelastic";
  
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

    double Mass;
    double Density;
};

#endif
