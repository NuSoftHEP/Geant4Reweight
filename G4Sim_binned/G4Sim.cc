#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4SimDetectorConstruction.hh"
#include "G4SimPhysicsList.hh"
#include "G4SimActionInitialization.hh"

#include "G4ReweightHist.hh"


#include "TH1F.h"
#include "TFile.h"

char * n;

std::string macFileName = "../G4Sim/run1.mac";
std::string outFileName = "try.root";

std::string elasticBiasFile = "bias.root";
std::string inelasticBiasFile = "bias.root";

std::string elasticBiasName = "elastic";
std::string inelasticBiasName = "inelastic";

bool ParseArgs(int argc, char* argv[]);
G4ReweightHist * GetHist(std::string, std::string);

int main(int argc, char * argv[]){

  std::cout << "parsing" << std::endl;
  if(!ParseArgs(argc, argv)) {return 0;}
  std::cout << macFileName << " " << outFileName << " " << elasticBiasFile << " " << inelasticBiasFile << std::endl;
  std::cout << elasticBiasName << " " << inelasticBiasName << std::endl;

  std::cout << "Making vectors" << std::endl;
  std::vector<double> elasticBiasBins;
  std::vector<double> inelasticBiasBins = {0.0,500,1200.};
  
  std::cout << "Making hists" << std::endl;
  //G4ReweightHist* elasticBias = new G4ReweightHist("","",elasticBiasBins);
  //G4ReweightHist* inelasticBias = new G4ReweightHist("","",inelasticBiasBins);

  G4ReweightHist * elasticBias   = GetHist(elasticBiasFile,   elasticBiasName);
  G4ReweightHist * inelasticBias = GetHist(inelasticBiasFile, inelasticBiasName);

  std::cout << "Made Hists" << std::endl;

//  std::cout << "Setting bins" << std::endl;
//  inelasticBias->SetBinContent(0, .75);
//  inelasticBias->SetBinContent(1, 1.25);
//  std::cout << "Set Bins" << std::endl;


  G4RunManager * runManager = new G4RunManager;
  
  //Define and create detector volume  
  runManager->SetUserInitialization(new G4SimDetectorConstruction);

  //Define the list of particles to be simulated
  //and on which models their behaviors are based
  runManager->SetUserInitialization(new G4SimPhysicsList(inelasticBias, elasticBias));

  //Define the actions taken during various stages of the run
  //i.e. generating particles
  runManager->SetUserInitialization(new G4SimActionInitialization(outFileName));

  G4cout << "Init"<<G4endl;
  runManager->Initialize();
  G4cout << "Done"<< G4endl;


 /* G4UImanager * UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/verbose 1");
  UI->ApplyCommand("/event/verbose 1");*/

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
