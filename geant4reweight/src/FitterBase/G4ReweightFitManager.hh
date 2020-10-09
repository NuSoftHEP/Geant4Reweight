#ifndef G4ReweightFitManager_h
#define G4ReweightFitManager_h


#include "TGraph.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TFile.h"
#include "Math/Minimizer.h"
#include "TDirectory.h"

#include "geant4reweight/src/PropBase/G4ReweightParameterMaker.hh"
#include "geant4reweight/src/FitterBase/FitParameter.hh"
#include "G4ReweightFitter.hh"
#include "geant4reweight/src/ReweightBase/G4ReweightManager.hh"

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include "Math/Factory.h"
#include "Math/Functor.h"


#include "fhiclcpp/ParameterSet.h"


class G4ReweightFitManager{
  public:
    explicit  G4ReweightFitManager(std::string & fOutFileName, bool do_save,
                                   G4ReweightManager * rw_manager,
                                   double total_xsec_bias=1.0);
    //G4ReweightFitManager(std::string &, bool , int particle);
    void MakeFitParameters( std::vector< fhicl::ParameterSet > &);
    bool CheckIsDummy( std::string theCut ){
      //Maybe throw an exception if not in the map?
      return CutIsDummy.at( theCut );
    };

    void DefineMCSets( std::vector< fhicl::ParameterSet > &);
    void DefineExperiments( fhicl::ParameterSet &);
    void GetAllData();
    void RunFitAndSave(bool fFitScan=false/*, bool fSave = false*/);
    void SaveFitTree();


    //void DoParameterScan(fhicl::ParameterSet & ps);

    void MakeMinimizer( fhicl::ParameterSet & );
    void DrawFitResults();

    bool IsSetActive( std::string theSetName ){
      return ( std::find( sets.begin(), sets.end(), theSetName ) != sets.end() );
    };

    void DefineFCN();

    double total_mix=1;

  protected:
    TFile      * out;
    TDirectory * data_dir;

    G4ReweightParameterMaker parMaker;

    std::map< std::string, std::vector< G4ReweightFitter* > > mapSetsToFitters;
    std::map< std::string, std::string > mapSetsToFracs;
    std::map<std::string, fhicl::ParameterSet> mapSetsToMaterial;
    std::vector< G4ReweightFitter* > allFitters;

    std::map< std::string, std::vector< FitParameter > > FullParameterSet;
    std::map< std::string, bool > CutIsDummy;

    std::vector< std::string > sets;


    std::unique_ptr< ROOT::Math::Minimizer > fMinimizer;

    std::vector< std::string > thePars;
    std::vector< double > theVals;
    std::vector< std::pair< std::string, double > > theParVals;
    //added by C Thorpe, store elastic param info as paris
    std::vector<std::pair<std::string,double>> theElastParVals;

    std::vector< double > theScanStarts, theScanDeltas;
    std::vector< int > theScanSteps;

    //int nDOF;

    double nDOF;
    int pdg;

    std::vector<std::string> AllExclChannels;
    virtual int GetNModelParam(std::string cut,bool use_reac=false){ return 1;}


    ROOT::Math::Functor theFCN;

    TTree fit_tree;
    double tree_chi2 = 0.;
    std::map< std::string, double > parameter_values;

    bool fSave;
    G4ReweightManager * fRWManager;
};

#endif
