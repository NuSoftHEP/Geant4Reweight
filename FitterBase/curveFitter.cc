#include "G4ReweightCurveFitManager.hh"
#include "FitParameter.hh"
#include <vector>
#include <string>
#include "TVectorD.h"

#include "TFile.h"
#include "TH2D.h"
#include "TGraph2D.h"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

std::string set_prec(double);

int main(int argc, char ** argv){

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(argv[1]);
  bool fSave    = ps.get< bool >( "Save", false );

  std::string outFileName = ps.get< std::string >( "OutputFile" );
  G4ReweightCurveFitManager FitMan( outFileName, fSave);

  std::vector< fhicl::ParameterSet > FitParSets = ps.get< std::vector< fhicl::ParameterSet > >("ParameterSet");
  FitMan.MakeFitParameters( FitParSets );


  ///Defining MC Sets
  std::vector< fhicl::ParameterSet > FCLSets = ps.get< std::vector< fhicl::ParameterSet > >("Sets");
  FitMan.DefineMCSets( FCLSets );
  ///////////////////////////////////////////

  ///Defining experiments
  FitMan.DefineExperiments( ps );
  ///////////////////////////////////////////


  FitMan.GetAllData();

  FitMan.MakeMinimizer( ps );

  bool fFitScan = ps.get< bool >( "FitScan", false );
  FitMan.RunFitAndSave(fFitScan/*, fSave*/);
  return 0;
}
