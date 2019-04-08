#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

#include "TFile.h" 

int main(int argc, char ** argv){
  fhicl::ParameterSet ps = fhicl::make_ParameterSet( argv[1] );

  std::string outFileName = ps.get< std::string >( "OutputFile" );
  std::string inFileName  = ps.get< std::string >( "InputFile" ); 

  G4ReweightTreeParser * tp = new G4ReweightTreeParser(fileName.c_str(), outFileName.c_str());


  std::string FracsFileName = ps.get< std::string >( "Fracs" );
  TFile FracsFile( FracsFileName.c_str(), "OPEN" );

  std::string FitResultsFileName = ps.get< std::string >( "FitResults" );
  TFile FitResultsFile( FitResultsFileName.c_str(), "OPEN" );
  G4ReweightThrowManager ThrowMan( FitResultsFile );


  //Input the ParameterSets somehow (might have to change how it's normally created) 
  //Then create the paramater maker
  //G4ReweightParameterMaker( input );


  //Implement this in the tree parser   
  //Will have to do the throws. Save them (maybe in a vector of vectors/maps?, map of vectors?).
  //Then for each set of throws, create teh parameters (hists) and do the reweighting.
  //Weights from these will need to be saved in a tree branch of a vector<double> 
  tp->FillAndAnalyzeFSThrows( &FracsFile, G4ReweightParameterMaker, G4ReweightThrowManager );
  tp->CloseAndSaveOutput();

  return 0;
}
