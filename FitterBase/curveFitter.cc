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

  std::string outFileName = "curveFitter_try.root";
  G4ReweightCurveFitManager FitMan( outFileName );

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(argv[1]);
  ///Making Fit Parameters

//  std::map< std::string, std::vector< FitParameter > > FullParameterSet;


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
  FitMan.RunFitAndSave();
 return 0;
}
