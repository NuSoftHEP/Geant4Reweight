#ifndef newDUETFitter_h
#define newDUETFitter_h

#include "G4ReweightFitter.hh"
#include "FitSample.hh"

#include <iostream>
#include <string>
#include <vector>

#include "TH1D.h"
#include "TH2D.h"
#include "TMatrixD.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TFile.h"
#include "TTree.h"
#include "tinyxml2.h"

#include "G4ReweightTreeParser.hh"

class newDUETFitter : public G4ReweightFitter { 
  public:
  
    newDUETFitter();
    virtual void   LoadData();
    double DoFit() override;

//    newDUETFitter(){fOutFile = new TFile("DUET_fit.root", "RECREATE"); };
//    newDUETFitter( std::string/*, std::string*/ );
//    newDUETFitter( std::vector< std::string > );
//    //newDUETFitter( std::string, std::string );
//   ~newDUETFitter();
   
/*   
    double DoFit();
    void   LoadMC();
    void   LoadMCVector();
    void   LoadRawMC();
    void   LoadRawMCVector();
    void   LoadFracs(std::string);
    void   DoReweight(double);
    void   DoReweightFS(double,double);
    void   DoReweightFSVector(double,double);
    TTree* GetReweightFS(FitSample);
    void   SaveInfo();
    void   ClearMemory();
    void   ParseXML(std::string);

    size_t    GetNSamples(){return samples.size();};
    FitSample GetSample( size_t i ){return samples[i];};
    void      SetActiveSample( size_t i ){ ActiveSample = &samples[i]; }
*/
  private:
    TMatrixD * DUET_cov_matrix;
    TMatrixD * DUET_cov_inv;
    
/*    TFile * fDUETFile;
    TFile * fOutFile;
    TTree * fFitTree;

    std::string fMCFileName;
    std::string fRawMCFileName;
    std::vector< std::string > fRawMCFileNameVector;
    std::string fDataFileName;

//    std::string fOutputDir;

    TFile * fMCFile;
    TFile * fRawMCFile;
    TTree * fMCTree;
    G4ReweightTreeParser * fFSFracs; 
    TTree * fFSTree;
    TFile * fFrac;

    TGraphErrors * DUET_xsec_abs;
    TGraphErrors * DUET_xsec_cex;
    //TH2D * DUET_cov;

    TGraph * MC_xsec_abs;
    TGraph * MC_xsec_cex;


    //Reweight stuff
    G4ReweightTreeParser * Reweighter;

    double norm_param;
    double norm_abs_param;
    double norm_cex_param;
    double the_Chi2;
    TH2D * BinnedChi2;

    std::vector< double > norm_vector;
    std::vector< double > norm_abs_vector;
    std::vector< double > norm_cex_vector;
    std::vector< double > Chi2_vector;

    std::vector< FitSample > samples;
    FitSample * ActiveSample;
*/
 //   std::string abs_cut =  "(int == \"pi+Inelastic\" && ( (nPi0 + nPiPlus + nPiMinus)  == 0) )";
 //   std::string cex_cut =  "(int == \"pi+Inelastic\" && ( (nPiPlus + nPiMinus) == 0 ) && (nPi0 == 1))";

//    std::string weight = "weight*finalStateWeight*";


};

#endif
