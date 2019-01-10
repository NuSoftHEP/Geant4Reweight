#ifndef G4ReweightFitter_h
#define G4ReweightFitter_h


#include "TGraph.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TFile.h"
#include "FitSample.hh"
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include "tinyxml2.h"

class G4ReweightFitter{
  public:
    G4ReweightFitter() {};
    ~G4ReweightFitter(){};

    virtual void   LoadData() = 0;
    virtual double DoFit();
    virtual void   LoadMC(){};

    void GetMCGraphs();
    TTree * GetReweightFS( );

    size_t    GetNSamples(){return samples.size();};
    FitSample GetSample( size_t i ){return samples[i];};
    void      SetActiveSample( size_t i ){ ActiveSample = &samples[i]; }
    void ParseXML(std::string);

  protected:

    TTree * fMCTree;
    std::string fDataFileName;
    TFile * fDataFile;

    std::vector< double > points;

    int nBins;
    double binLow, binHigh;
    std::string binning;

   
    std::map< std::string, std::string > cuts;
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
