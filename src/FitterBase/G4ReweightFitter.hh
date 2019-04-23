#ifndef G4ReweightFitter_h
#define G4ReweightFitter_h


#include "TGraph.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TFile.h"
#include "TDirectory.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>

#include "G4Reweighter.hh"
#include "FitParameter.hh"

#include "fhiclcpp/ParameterSet.h"

class G4ReweightFitter{
  public:
    G4ReweightFitter() {};
    G4ReweightFitter(TFile*, fhicl::ParameterSet);
    ~G4ReweightFitter(){};

    virtual void   LoadData();
    virtual void   SaveData( TDirectory * );
    virtual double DoFit(bool fSave=true);
    virtual void   LoadMC(){};

    double GetNDOF(){ return nDOF; };


    void GetMCFromCurves( std::string, std::string, std::map< std::string, std::vector< FitParameter > >, bool fSave=false);

    void SaveExpChi2( double &, std::string & );
    void FinishUp();

    void MakeFitDir( TDirectory * );

    std::string GetType(){ return type; };
    std::string GetName(){ return fExperimentName; };

  protected:

    std::string fExperimentName = "Generic_Experiment_Name";

    TTree * fMCTree;
    std::string fDataFileName;
    TFile * fDataFile;
    TFile * fOutputFile;
    TDirectory * fFitDir;
    TDirectory * fTopDir;

    std::string type;

    std::vector< std::string > cuts;
    std::map< std::string, std::string > graph_names;

    std::map< std::string, TGraph* > MC_xsec_graphs;
    std::map< std::string, TGraphErrors* > Data_xsec_graphs;

    std::string set_prec(double input){
      std::stringstream stream_in; 
      stream_in << std::fixed << std::setprecision(2) << input;
      return stream_in.str();
    };

    G4Reweighter * theReweighter;
    TGraph * dummyGraph;
    TH1D * dummyHist;
    TGraph * total_inel;

    int nDOF;
};

#endif
