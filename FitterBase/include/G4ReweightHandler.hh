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

    void ParseXML(std::string);
    FitSample DoReweight(std::string, double, double);

    void SetFiles( std::string name ){ fRawMCFileNames = &(fMapToFiles[name]); fFSFileName = fMapToFSFiles[name]; };


  protected:
    std::vector< std::string > * fRawMCFileNames;
    std::string fFSFileName;

    std::map< std::string, std::vector<std::string> > fMapToFiles;
    std::map< std::string, std::string >              fMapToFSFiles;

    std::string RWFileName;
    G4ReweightTreeParser * Reweighter;
  public:
};

#endif
