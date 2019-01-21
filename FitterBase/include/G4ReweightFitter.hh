#ifndef G4ReweightFitter_h
#define G4ReweightFitter_h


#include "TGraph.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TFile.h"
#include "TDirectory.h"
#include "FitSample.hh"
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include "tinyxml2.h"

#include "fhiclcpp/ParameterSet.h"

class G4ReweightFitter{
  public:
    G4ReweightFitter() {};
    G4ReweightFitter(TFile*, fhicl::ParameterSet);
    ~G4ReweightFitter(){};

    virtual void   LoadData()/* = 0*/;
    virtual void   SaveData( TDirectory * );
    virtual double DoFit();
    virtual void   LoadMC(){};


    void GetMCGraphs();
    TTree * GetReweightFS( );

    size_t    GetNSamples(){return samples.size();};
    FitSample GetSample( size_t i ){return samples[i];};
/*    void      SetActiveSample( size_t i, TDirectory * output_dir){ 
      ActiveSample = &samples[i]; 
      output_dir->cd();
    }
    */
    void SetActiveSample( size_t, TDirectory * );
    void AddSample( FitSample theSample ){ samples.push_back( theSample); };
    void ParseXML(std::string);

    std::string GetType(){ return type; };

    void BuildCuts(){
      for( std::map< std::string, std::string >::iterator itCuts = cuts.begin(); itCuts != cuts.end(); ++itCuts ){
        if( type.find("piplus") != std::string::npos ){
          std::string old_cut = itCuts->second;
          itCuts->second = "(int == \"pi+Inelastic\" && " + old_cut + ")";
        }         
        if( type.find("piminus") != std::string::npos ){
          std::string old_cut = itCuts->second;
          itCuts->second = "(int == \"pi-Inelastic\" && " + old_cut + ")";
        }         
      }
    }

/*    void BuildScale(){
      scale
    }*/
  protected:

    std::string fExperimentName = "Generic_Experiment_Name";

    TTree * fMCTree;
    std::string fDataFileName;
    TFile * fDataFile;
    TFile * fOutputFile;
    TDirectory * fFitDir;
    TDirectory * fTopDir;

    std::vector< double > points;

    int nBins;
    double binLow, binHigh;
    std::string binning;

    std::string type;

   
    std::map< std::string, std::string > cuts;
    std::map< std::string, std::string > graph_names;

    std::map< std::string, TGraph* > MC_xsec_graphs;
    std::map< std::string, TGraphErrors* > Data_xsec_graphs;
    const std::string weight = "weight*finalStateWeight*";

    FitSample * ActiveSample;
    std::vector< FitSample > samples;

    double scale;

    std::string set_prec(double input){
      std::stringstream stream_in; 
      stream_in << std::fixed << std::setprecision(2) << input;
      return stream_in.str();
    };
};

#endif
