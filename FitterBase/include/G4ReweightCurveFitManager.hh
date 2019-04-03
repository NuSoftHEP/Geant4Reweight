#ifndef G4ReweightCurveFitManager_h
#define G4ReweightCurveFitManager_h


#include "TGraph.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TFile.h"
#include "Math/Minimizer.h"
#include "TDirectory.h"
#include "FitSample.hh"
#include "FitParameter.hh"
#include "G4ReweightFitter.hh"
#include "G4ReweightHandler.hh"

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include "tinyxml2.h"
#include <utility>
#include "Math/Factory.h"
#include "Math/Functor.h"


#include "fhiclcpp/ParameterSet.h"

class G4ReweightCurveFitManager{
  public:
    G4ReweightCurveFitManager(std::string &);
    void MakeFitParameters( std::vector< fhicl::ParameterSet > &);

    bool CheckIsDummy( std::string theCut ){
      //Maybe throw an exception if not in the map?
      return CutIsDummy.at( theCut );
    };

    void DefineMCSets( std::vector< fhicl::ParameterSet > &);
    void DefineExperiments( fhicl::ParameterSet &);
    void GetAllData();
    void RunFitAndSave(bool fFitScan=false, bool fSave = false);
    void MakeMinimizer( fhicl::ParameterSet & );
    void DrawFitResults();
    
    bool IsSetActive( std::string theSetName ){
      return ( std::find( sets.begin(), sets.end(), theSetName ) != sets.end() );
    };

    void DefineFCN(bool fSave=false);


  protected:
    TFile      * out;
    TDirectory * data_dir;

    std::map< std::string, std::vector< G4ReweightFitter* > > mapSetsToFitters;
    std::map< std::string, std::string > mapSetsToFracs;
    std::map< std::string, std::string > mapSetsToNominal;
    std::vector< G4ReweightFitter* > allFitters;

    std::map< std::string, std::vector< FitParameter > > FullParameterSet;
    std::map< std::string, bool > CutIsDummy;

    std::vector< std::string > sets;

    size_t nSamples;

    std::unique_ptr< ROOT::Math::Minimizer > fMinimizer;

    std::vector< std::string > thePars;
    std::vector< double > theVals;
    std::vector< double > theScanStarts, theScanDeltas;
    std::vector< int > theScanSteps;
    
    int nDOF;

    ROOT::Math::Functor theFCN;

    TTree fit_tree;
    double tree_chi2 = 0.;
    std::map< std::string, double > parameter_values;


};

#endif
