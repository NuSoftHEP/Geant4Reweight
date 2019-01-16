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

    void SetFiles( std::vector<std::string> * theFiles ){ fRawMCFileNames = theFiles; };
    void SetFiles_C_piplus(){ fRawMCFileNames = &C_piplus_FilesVector; fFSFileName = C_piplus_FSFile; };
    void SetFiles_C_piminus(){ fRawMCFileNames = &C_piminus_FilesVector; fFSFileName = C_piminus_FSFile; };
//    void SetFiles_O_piplus(){ fRawMCFileNames = &O_piplus_FilesVector; };
//    void SetFiles_O_piminus(){ fRawMCFileNames = &O_piminus_FilesVector; };


  protected:
    std::vector< std::string > * fRawMCFileNames;
    std::string fFSFileName;
    std::vector< std::string > C_piplus_FilesVector;
    std::vector< std::string > C_piminus_FilesVector;

    std::string C_piplus_FSFile;
    std::string C_piminus_FSFile;

    std::string RWFileName;
    G4ReweightTreeParser * Reweighter;
  public:
};

#endif
