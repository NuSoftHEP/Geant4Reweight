#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "CarbonBox.hh"
#include "G4SimPhysicsList.hh"
#include "G4SimPhysicsListBinned.hh"
#include "G4SimPhysicsListFunc.hh"
#include "G4SimActionInitialization.hh"

#include "G4ReweightHist.hh"
#include "G4ReweightInter.hh"

#include "TH1F.h"
#include "TFile.h"

char * n;

std::string macFileName = "../G4Sim/run1.mac";
std::string outFileName = "try.root";

std::string elasticBiasFile = "bias.root";
std::string inelasticBiasFile = "bias.root";

std::string elasticBiasName = "elastic";
std::string inelasticBiasName = "inelastic";

double inelasticBiasFlat = 1.;
double elasticBiasFlat = 1.;


std::string varType = "flat";

bool ParseArgs(int argc, char* argv[]);
G4ReweightHist * GetHist(std::string, std::string);
G4ReweightInter * GetInter(std::string);

int main(int argc, char * argv[]){
 
  std::cout << "parsing" << std::endl;
  if(!ParseArgs(argc, argv)) {return 0;}
  std::cout << macFileName << " " << outFileName << " " << elasticBiasFile << " " << inelasticBiasFile << std::endl;
  std::cout << elasticBiasName << " " << inelasticBiasName << std::endl;

  G4RunManager * runManager = new G4RunManager;
  
  //Define and create detector volume  
  runManager->SetUserInitialization(new CarbonBox);

  if(varType == "func" ){

    std::vector< std::pair< double, double > > elast_input; 

    std::cout << "Making interpolated weights" << std::endl;
    G4ReweightInter * inelasticBias = GetInter( inelasticBiasFile );
    G4ReweightInter * elasticBias = GetInter( elasticBiasFile );

    std::cout << "Made interpolated weights" << std::endl;

    std::cout << "Inelastic:" << std::endl;
    for(size_t i = 0; i < inelasticBias->GetNPoints(); ++i){
      std::cout << inelasticBias->GetPoint(i) << " " << inelasticBias->GetValue(i) << std::endl;
    }
    std::cout << "Elastic:" << std::endl;
    for(size_t i = 0; i < elasticBias->GetNPoints(); ++i){
      std::cout << elasticBias->GetPoint(i) << " " << elasticBias->GetValue(i) << std::endl;
    }


    //Define the list of particles to be simulated
    //and on which models their behaviors are based
    runManager->SetUserInitialization(new G4SimPhysicsListFunc(inelasticBias, elasticBias));

  }
  else if(varType == "binned" ){
    std::cout << "Making hists" << std::endl;
    G4ReweightHist * elasticBias   = GetHist(elasticBiasFile,   elasticBiasName);
    G4ReweightHist * inelasticBias = GetHist(inelasticBiasFile, inelasticBiasName);

    std::cout << "Made Hists" << std::endl;


    //Define the list of particles to be simulated
    //and on which models their behaviors are based
    runManager->SetUserInitialization(new G4SimPhysicsListBinned(inelasticBias, elasticBias));
  }
  else if(varType == "flat" ){
    std::cout << "inel, elast: " << inelasticBiasFlat << " " << elasticBiasFlat << std::endl;
    runManager->SetUserInitialization(new G4SimPhysicsList(inelasticBiasFlat, elasticBiasFlat));
  }
  else{
    std::cout << "Unknown variation type: " << varType << std::endl;
    std::cout << "Exiting" << std::endl;

    return 0;
  }
  
  //Define the actions taken during various stages of the run
  //i.e. generating particles
  runManager->SetUserInitialization(new G4SimActionInitialization(outFileName));

  G4cout << "Init"<<G4endl;
  runManager->Initialize();
  G4cout << "Done"<< G4endl;

  //read a macro file of commands
  G4UImanager * UI = G4UImanager::GetUIpointer();
  G4String command = "/control/execute ";
  G4String fileName = macFileName;
  G4cout <<"Applying Command" <<G4endl;
  UI->ApplyCommand(command+fileName);
  G4cout <<"Done"<<G4endl;

  delete runManager;
  return 0;
}

bool ParseArgs(int argc, char* argv[]){

  for(int i = 1; i < argc; ++i){
    if( strcmp(argv[i], "--help") == 0){
      std::cout << "Usage: -c macro file -o outfile --eFile elasticBiasFile --eName elasticBiasName --iFile inelasticBias --iName inelasticBiasName" << std::endl;
      return false;
    }
    if( strcmp(argv[i], "-c") == 0){
      macFileName = argv[i+1];      
    }
    else if( strcmp(argv[i], "-o") == 0){
      outFileName = argv[i+1];
    }

    //Binned or Functional variations
    else if( strcmp(argv[i], "--eFile") == 0){      
      elasticBiasFile = argv[i+1];
    }
    else if( strcmp(argv[i], "--eName") == 0){      
      elasticBiasName = argv[i+1];
    }
    else if( strcmp(argv[i], "--iFile") == 0){      
      inelasticBiasFile = argv[i+1];
    }
    else if( strcmp(argv[i], "--iName") == 0){      
      inelasticBiasName = argv[i+1];
    }

    //Flat Reweighting
    else if( strcmp(argv[i], "-i") == 0){
      inelasticBiasFlat = atof(argv[i+1]);
    }
    else if( strcmp(argv[i], "-e") == 0){
      elasticBiasFlat = atof(argv[i+1]);
    }


    //Options: flat(default), binned, func
    else if( strcmp(argv[i], "--type") == 0){ 
      varType = argv[i+1];
    }
  }
  return true;
}

G4ReweightHist * GetHist(std::string fileName, std::string histName){
  TFile * histFile = new TFile(fileName.c_str());
  TH1F  * hist     = (TH1F*)histFile->Get(histName.c_str());

  std::vector< double > bins = std::vector< double >();
  for(int i = 1; i <= hist->GetNbinsX(); ++i){
    bins.push_back( hist->GetBinLowEdge(i) ); 
    std::cout << "pushed back: " << bins[i - 1] << std::endl;
  }
  bins.push_back( hist->GetBinLowEdge(hist->GetNbinsX()) + hist->GetBinWidth(hist->GetNbinsX()) );
  std::cout << "pushed back last: " << bins.back() << std::endl;

  std::cout << "bins: " << bins.size() << std::endl;

  G4ReweightHist * outHist = new G4ReweightHist("","", bins);
  
  for(int i = 1; i <= hist->GetNbinsX(); ++i){
    double content = hist->GetBinContent(i);
    std::cout << i << " Setting content: " << content;
    outHist->SetBinContent(i-1, content);
    std::cout << " Set content: " << outHist->GetBinContent(i-1) << std::endl;
  }

  return outHist;

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
