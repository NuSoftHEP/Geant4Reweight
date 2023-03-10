#ifndef G4ReweightFitter_h
#define G4ReweightFitter_h

class G4PiPlusReweighter;
class G4ReweightManager;
class G4ReweightParameterMaker;

#include "fhiclcpp/ParameterSet.h"

#include "geant4reweight/FitterBase/util/FitStore.hh"

class TDirectory;
class TFile;
class TGraph;
class TGraphErrors;
class TH2D;
class TTree;

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <utility> // std::pair
#include <vector>

// stores elements of covariance matrix produced in fit
/*
struct covElementStore {
  std::string cut; //name of process (abs, cex etc)
  int index; //index in CM
  std::pair<double , double> Range; //range of momenta parameter applies to
};*/



class G4ReweightFitter{
  public:
    G4ReweightFitter() {};
    G4ReweightFitter(TFile * output_file, fhicl::ParameterSet exp,
                     std::string frac_file_name,
                     G4ReweightParameterMaker & parMaker,
                     const fhicl::ParameterSet & material,
                     G4ReweightManager * rw_manager);
    ~G4ReweightFitter(){};

    virtual void   LoadData();
    virtual void   SaveData( TDirectory * );
    virtual double DoFit(bool fSave=true);
    virtual void DoFitModified(bool fSave=true);
    virtual void   LoadMC(){};

    //TODO: nDOF calculation needs updating 
    double GetNDOF(){ return nDOF; };

    Chi2Store GetNDataPointsAndChi2(std::string cut);
    void GetMCValsWithCov(//std::string FracFileName,
                          G4ReweightParameterMaker & parMaker,
                          /*const fhicl::ParameterSet & material,
                          G4ReweightManager * rw_manager,*/
                          bool fSave = false, TH2D/*TMatrixD*/ * cov = nullptr,
                          std::string position = "CV", bool doFullRange = false,
                          std::vector<std::string> * channels = nullptr);
    double SigmaWithCov(
        double x, std::string cut, /*TH2D * cov,*/
        std::map<std::pair<std::string, std::string>, double> & cov_vals_map,
        G4ReweightParameterMaker & parMaker);

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

    TFile * fFracFile;
    G4PiPlusReweighter * theReweighter;

    TGraph * total_inel;

    //added by C Thorpe - store total inelastic cross section
    TGraph * total_el;

    int nDOF;
    //double nDOF;
    
};


#endif
