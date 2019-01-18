#ifndef G4ReweightHandler_h
#define G4ReweightHandler_h


#include "TGraph.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "FitSample.hh"
#include "G4ReweightInter.hh"
#include "G4ReweightFinalState.hh"
#include "G4ReweightTreeParser.hh"
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include "tinyxml2.h"

class G4ReweightHandler{
  public:
    G4ReweightHandler();
    ~G4ReweightHandler();

    void ParseXML(std::string, std::vector< std::string >);
    FitSample DoReweight(std::string, double, double, std::string);

    void SetFiles( std::string );


  protected:
    std::vector< std::string > * fRawMCFileNames;
    std::string fFSFileName;

    std::map< std::string, std::vector<std::string> > fMapToFiles;
    std::map< std::string, std::string >              fMapToFSFiles;

    std::string RWFileName;
    G4ReweightTreeParser * Reweighter;

    TFile * fFSFile;
    TTree * fFSTree;

    std::map< std::string, G4ReweightInter* > FSInters;
    G4ReweightInter * dummy;
    std::vector< std::pair< double, double > > abs_vector;
    std::vector< std::pair< double, double > > cex_vector;

    void ClearFSInters();


  public:
    void CloseFSFile(){ delete fFSTree; fFSFile->Close(); delete fFSFile; };
};

#endif
