#include <iostream>
#include <utility>
#include <string>
#include <fstream>
#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"

#include "G4ReweightStep.hh"
#include "G4ReweightTraj.hh"
#include "G4ReweightTreeParser.hh"

#include "G4ReweightFinalState.hh"

#include "G4ReweightInter.hh"

std::string fileName = "try.root"; 
std::string outFileName = "outtry.root";

double weight = 1.;
double elastWeight = 1.;

std::string inelasticBiasFile = "bias.root";
std::string elasticBiasFile   = "bias.root";
std::string inelasticBiasName = "inelastic";
std::string elasticBiasName   = "elastic";

std::string weightType        = "flat";

bool enableFS = false;

std::string FinalStateFracsFile = "FinalStateFile.root";
std::string FinalStateVarsFile  = "try.xml";


bool parseArgs(int argc, char ** argv);
G4ReweightInter * GetInter(std::string);

int main(int argc, char ** argv){

  if(!parseArgs(argc, argv)) {return 0;}
  std::cout << fileName << " " << outFileName << " " << weight << " " << elastWeight << std::endl;

  TFile * inelasticFile; 
  TFile * elasticFile; 

  TH1F * inelasticHist;
  TH1F * elasticHist;
  

  G4ReweightTreeParser * tp = new G4ReweightTreeParser(fileName.c_str(), outFileName.c_str());
  tp->SetBranches();
  if(enableFS){
    std::cout << "Enabling Final State Reweighting" << std::endl;
    std::cout << "Using FinalStateFracsFile: " << FinalStateFracsFile << std::endl;
    std::cout << "Using FinalStateVarsFile: " << FinalStateVarsFile << std::endl;

    G4ReweightFinalState * theFS = new G4ReweightFinalState( new TFile(FinalStateFracsFile.c_str()), FinalStateVarsFile ); 

    tp->FillAndAnalyzeFS(theFS);

    return 0;
  }

  if(weightType == "flat"){

    std::cout << "Doing flat reweighting" << std::endl;
    std::cout << "\t" << weight << " " << elastWeight << std::endl;

    tp->FillAndAnalyze(weight,elastWeight);
  }
  else if(weightType == "binned"){

    std::cout << "Doing binned reweight" << std::endl;
    std::cout << "Inelastic Bias: " << inelasticBiasFile << " " << inelasticBiasName << std::endl; 
    std::cout << "Elastic Bias:   " << elasticBiasFile   << " " << elasticBiasName   << std::endl; 


    //Getting inelastic bias info
    //
    inelasticFile = new TFile(inelasticBiasFile.c_str());
    if( !inelasticFile->IsOpen() ){
      std::cout << "Error: Couldn't open the inelastic bias file " << inelasticBiasFile << std::endl;
      return 0;
    }
    
    std::cout << "Opened inelastic file: " << inelasticBiasFile << std::endl;
    inelasticHist = (TH1F*)inelasticFile->Get(inelasticBiasName.c_str());
    if( !inelasticHist ){
      std::cout << "Error: Couldn't find hist " << inelasticBiasName << " in the inelastic file" << std::endl;
      return 0;
    }

    std::cout << "Got inelastic hist: " << inelasticBiasName << std::endl;
    //////////////////////////////////


    //Getting elastic bias info
    //
    elasticFile   = new TFile(elasticBiasFile.c_str());
    if( !elasticFile->IsOpen() ){
      std::cout << "Error: Couldn't open the elastic bias file " << elasticBiasFile << std::endl;
      return 0;
    }

    std::cout << "Opened elastic file: " << elasticBiasFile << std::endl;
    elasticHist = (TH1F*)elasticFile->Get(elasticBiasName.c_str());
    if( !elasticHist ){
      std::cout << "Error: Couldn't find hist " << elasticBiasName << " in the elastic file" << std::endl;
      return 0;
    }

    std::cout << "Got elastic hist: " << elasticBiasName << std::endl;
    //////////////////////////////////

    tp->FillAndAnalyze(inelasticHist, elasticHist);
    
  }
  else if(weightType == "func"){

    std::cout << "Doing funced reweight" << std::endl;
    std::cout << "Inelastic Bias: " << inelasticBiasFile << std::endl; 
    //Getting inelastic bias info
    //
    G4ReweightInter * inelasticBias   = GetInter(inelasticBiasFile);
    std::cout << "Got inelastic : " << std::endl;
    for(size_t i = 0; i < inelasticBias->GetNPoints(); ++i){
      std::cout << inelasticBias->GetPoint(i) << " " << inelasticBias->GetValue(i) << std::endl;
    } 
    //////////////////////////////////

    std::cout << "Elastic Bias:   " << elasticBiasFile   << std::endl; 
    //Getting elastic bias info
    //
    G4ReweightInter * elasticBias   = GetInter(elasticBiasFile);
    std::cout << "Got elastic : " << std::endl;
    for(size_t i = 0; i < elasticBias->GetNPoints(); ++i){
      std::cout << elasticBias->GetPoint(i) << " " << elasticBias->GetValue(i) << std::endl;
    }
    //////////////////////////////////

//    return 0;
    tp->FillAndAnalyzeFunc(inelasticBias, elasticBias);
    
  }

  tp->CloseInput();

  std::cout << "done" << std::endl;

  return 0;
}

bool parseArgs(int argc, char ** argv){
  
  for(int i = 1; i < argc; ++i){
    if( strcmp(argv[i], "--help") == 0){
      std::cout << "Usage: ./G4Reweight -f inputFile -o outFile -i inelasticWeight -e elasticWeight" << std::endl;
      return false;
    }
    if( strcmp(argv[i], "-f") == 0){
      fileName = argv[i + 1];
    }
    else if( strcmp(argv[i], "-o") == 0){
      outFileName = argv[i + 1];       
    }

    else if( strcmp(argv[i], "-i") == 0){
      weight = atof(argv[i + 1]);
    }
    else if( strcmp(argv[i], "-e") == 0){
      elastWeight = atof(argv[i + 1]); 
    }

    else if( strcmp(argv[i], "--iFile") == 0){
      inelasticBiasFile = argv[i + 1];
    }
    else if( strcmp(argv[i], "--iName") == 0){
      inelasticBiasName = argv[i + 1];
    }

    else if( strcmp(argv[i], "--eFile") == 0){
      elasticBiasFile = argv[i + 1];
    }
    else if( strcmp(argv[i], "--eName") == 0){
      elasticBiasName = argv[i + 1];
    }

    else if( strcmp(argv[i], "--type") == 0){
      weightType = argv[i + 1];
    }

    else if( strcmp(argv[i], "--FS") == 0){
      enableFS = atoi( argv[i + 1] );
    }

    else if( strcmp(argv[i], "--FSFracs") == 0){
      FinalStateFracsFile = argv[i + 1];
    }

    else if( strcmp(argv[i], "--FSVars") == 0){
      FinalStateVarsFile = argv[i + 1];
    }

  }
  return true;
}

G4ReweightInter * GetInter(std::string fileName){
  std::ifstream inputFile;  
  inputFile.open(fileName);

  std::string line;

  if (inputFile.is_open()){

    std::vector< std::pair< double, double > > result;

    while( std::getline( inputFile, line) ){
      std::string::iterator it;
      std::string pos = "", val = "";
      bool found = false;
      for( it = line.begin(); it < line.end(); ++it){
        if (*it == ',') found = true;
        else{
          if(!found) pos += *it;
          else val += *it;
        }
      }
      std::cout << pos << " " << val << std::endl;
      result.push_back( std::make_pair(std::stod(pos), std::stod(val) ));
      
    }
    inputFile.close();
   
    G4ReweightInter * theInter = new G4ReweightInter( result );
    return theInter;
  }
  else{
    std::cout << "Unable to open file" << std::endl;
    std::cout << "Returning empty Interpolater. Will return biases of 1. for all momentum values" << std::endl;

    G4ReweightInter * theInter = new G4ReweightInter( std::vector<std::pair<double, double> >() );
    return theInter;
  }

  
}
