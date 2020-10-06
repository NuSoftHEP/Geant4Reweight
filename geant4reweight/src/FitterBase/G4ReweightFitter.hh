#ifndef G4ReweightFitter_h
#define G4ReweightFitter_h

#include "TMatrixD.h"
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

//#include "geant4reweight/src/ReweightBase/G4Reweighter.hh"
#include "geant4reweight/src/ReweightBase/G4PiPlusReweighter.hh"
#include "geant4reweight/src/FitterBase/FitParameter.hh"

#include "fhiclcpp/ParameterSet.h"

#include "util/FitStore.hh"

// stores elements of covariance matrix produced in fit
struct covElementStore {
  std::string cut; //name of process (abs, cex etc)
  int index; //index in CM
  std::pair<double , double> Range; //range of momenta parameter applies to
};



class G4ReweightFitter{
  public:
    G4ReweightFitter() {};
    G4ReweightFitter( TFile* , fhicl::ParameterSet );
    ~G4ReweightFitter(){};

    virtual void   LoadData();
    virtual void   SaveData( TDirectory * );
    virtual double DoFit(bool fSave=true);
    virtual void DoFitModified(bool fSave=true);
    virtual void   LoadMC(){};

    //TODO: nDOF calculation needs updating 
    double GetNDOF(){ return nDOF; };

    Chi2Store GetNDataPointsAndChi2(std::string cut);
    //void GetMCFromCurves( std::string, std::string, std::map< std::string, std::vector< FitParameter > >,std::vector<FitParameter>, bool fSave=false);

    //arguments : Total xsec filename , Fracs file name , parameters , elastic parameters , fSave , covariance matrix , position(central value,+1 sigma,-1 sigma) 
    void GetMCFromCurvesWithCovariance(std::string, std::string, std::map< std::string, std::vector< FitParameter > >,std::vector<FitParameter>, bool fSave=false,TMatrixD *cov=nullptr, std::string position = "CV");

    //Calculates SD using full covariance matrix. use_reac = true if fit was using a single parameter for reac instead of individual exclusive channels
    double SigmaWithCovariance(double x , std::string cut , TMatrixD *cov , bool use_reac);

    void SaveExpChi2( double &, std::string & );
    void FinishUp();

    void MakeFitDir( TDirectory * );

    std::string GetType(){ return type; };
    std::string GetName(){ return fExperimentName; };

    //weighting for total cross section in chi^2 calculation
    double total_mix=1;

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


    //stores number of data points and resulting chi2 for each experiment
    std::vector<Chi2Store> fitDataStore;


    std::string set_prec(double input){
      std::stringstream stream_in; 
      stream_in << std::fixed << std::setprecision(2) << input;
      return stream_in.str();
    };

    G4PiPlusReweighter * theReweighter;
    TGraph * dummyGraph;
    TH1D * dummyHist;

    TGraph * total_inel;

    //added by C Thorpe - store total inelastic cross section
    TGraph * total_el;

    int nDOF;
    //double nDOF;
    
    std::vector<covElementStore> theCovStore;

};


#endif
