#include "geant4reweight/ReweightBase/G4MultiReweighter.hh"

#include "TFile.h"
#include "TH1D.h"
#include "TMatrixD.h"
#include "TVectorD.h"

#include <iostream>

G4MultiReweighter::G4MultiReweighter(
    int pdg, TFile & fracsFile,
    const std::vector<fhicl::ParameterSet> & parSet,
    const fhicl::ParameterSet & material,
    G4ReweightManager * rw_manager,
    TFile & fitResults,
    size_t nThrows, int seed)
    : parMaker(parSet, false, pdg),
      reweighter(factory.BuildReweighter(pdg, &fracsFile,
                                         parMaker.GetFSHists(),
                                         material,
                                         rw_manager,
                                         parMaker.GetElasticHist())),
      numberOfThrows(nThrows),
      rng(seed) {
    
  //Get fit covariance matrix and 
  //attempt to get Cholesky Decomposition
  TMatrixD * cov = (TMatrixD*)fitResults.Get("FitCovariance");
  choleskyMatrix = TDecompChol( *cov );
  decompSuccess = choleskyMatrix.Decompose();

  //Get the parameters and set the values
  TH1D * parsHist = (TH1D*)fitResults.Get("parsHist");
  for (int i = 1; i <= parsHist->GetNbinsX(); ++i) {
    paramNames.push_back(parsHist->GetXaxis()->GetBinLabel(i));
    paramNominalVals.push_back(parsHist->GetBinContent(i));
    paramSigmas.push_back(parsHist->GetBinError(i));
    paramRandomVals.push_back(std::vector<double>()); 
    paramVals[paramNames.back()] = paramNominalVals.back();

    //Check that this exists in the parameter set defined by the fcl,
    //Throw an exception if not
    std::map<std::string, double> fcl_params = parMaker.GetActiveParameterValues();
    if (fcl_params.find(paramNames.back()) == fcl_params.end()) {
      std::cerr << "Found parameter in input fit results which was not in" <<
                   " the input fcl parameters" << std::endl;
      std::exception e;
      throw e;
    }
  }

  GenerateThrows();
}

G4MultiReweighter::G4MultiReweighter(
    int pdg, TFile & fracsFile,
    const std::vector<fhicl::ParameterSet> & parSet,
    const fhicl::ParameterSet & material,
    G4ReweightManager * rw_manager,
    size_t nThrows, int seed)
    : parMaker(parSet, false, pdg),
      reweighter(factory.BuildReweighter(pdg, &fracsFile,
                                         parMaker.GetFSHists(),
                                         material,
                                         rw_manager,
                                         parMaker.GetElasticHist())),
      numberOfThrows(nThrows),
      rng(seed) {

  //Get the parameters and set the values
  for (size_t i = 0; i < parSet.size(); ++i) {
    paramNames.push_back(parSet[i].get<std::string>("Name"));
    paramNominalVals.push_back(parSet[i].get<double>("Nominal"));
    paramSigmas.push_back(parSet[i].get<double>("Sigma"));
    paramRandomVals.push_back(std::vector<double>()); 
    paramVals[paramNames.back()] = paramNominalVals.back();
  }

  GenerateThrows();
}



void G4MultiReweighter::GenerateThrows() {
  //Generate the random numbers used for the throws 
  for (size_t i = 0; i < numberOfThrows; ++i) {
    for (size_t j = 0; j < paramNames.size(); ++j) {
      paramRandomVals[j].push_back(rng.Gaus());
    }
  }
}

double G4MultiReweighter::GetWeightFromNominal(G4ReweightTraj & traj) {
  //Build the map with the nominal vals
  std::map<std::string, double> paramMap;
  for (size_t i = 0; i < paramNames.size(); ++i) {
    paramMap[paramNames[i]] = paramNominalVals[i];
  }

  //Give the variables to the parameter maker 
  //and use these for the reweighter
  parMaker.SetNewVals(paramMap);
  reweighter->SetNewHists(parMaker.GetFSHists());
  reweighter->SetNewElasticHists(parMaker.GetElasticHist());
  
  //Get the weight from the trajectory
  return reweighter->GetWeight(&traj);
}

double G4MultiReweighter::GetWeightFrom1DThrow(G4ReweightTraj & traj,
                                               size_t iThrow) {
  if (iThrow+1 > numberOfThrows) {
    std::cerr << "Requested throw out of bounds" << std::endl;
    return -1.; 
  }

  //Build the map with the nominal + r*sigma
  //r --> Random gaussian number produced previously
  std::map<std::string, double> paramMap;
  for (size_t i = 0; i < paramNames.size(); ++i) {
    double this_throw = paramNominalVals[i] +
                        paramRandomVals[i][iThrow]*paramSigmas[i];

    paramMap[paramNames[i]] = (this_throw > 0. ? this_throw : 0.); 
  }

  //Give the variables to the parameter maker 
  //and use these for the reweighter
  parMaker.SetNewVals(paramMap);
  reweighter->SetNewHists(parMaker.GetFSHists());
  reweighter->SetNewElasticHists(parMaker.GetElasticHist());
  
  //Get the weight from the trajectory
  return reweighter->GetWeight(&traj);
}

std::vector<double> G4MultiReweighter::GetWeightFromAll1DThrows(
    G4ReweightTraj & traj) {

  std::vector<double> results;
  for (size_t i = 0; i < numberOfThrows; ++i) {
    results.push_back(GetWeightFrom1DThrow(traj, i));
  }
  return results;
}

double G4MultiReweighter::GetWeightFromCorrelatedThrow(G4ReweightTraj & traj,
                                                       size_t iThrow) {
  if (!decompSuccess) {
    std::cerr << "Cholesky decomposition was not formed." <<
                 "Returning weight of 1." << std::endl;
    return 1.;
  }

  if (iThrow+1 > numberOfThrows) {
    std::cerr << "Requested throw out of bounds" << std::endl;
    return -1.;
  }

  //Form a vector from the previously produced random numbers
  TVectorD rand(paramNames.size());
  for (size_t i = 0; i < paramNames.size(); ++i) {
    rand[i] = paramRandomVals[i][iThrow];
  }

  //Multiply by the Cholesky decomposed matrix
  TVectorD rand_times_chol = choleskyMatrix.GetU()*rand;

  //Build map with nominal + cholesky*rand vector
  std::map<std::string, double> paramMap;
  for (size_t i = 0; i < paramNames.size(); ++i) {
    double this_throw = paramNominalVals[i] + rand_times_chol[i];
    paramMap[paramNames[i]] = (this_throw > 0. ? this_throw : 0.);
  }

  //Give the variables to the parameter maker 
  //and use these for the reweighter
  parMaker.SetNewVals(paramMap);
  reweighter->SetNewHists(parMaker.GetFSHists());
  reweighter->SetNewElasticHists(parMaker.GetElasticHist());
  
  //Get the weight from the trajectory
  return reweighter->GetWeight(&traj);
}

std::vector<double> G4MultiReweighter::GetWeightFromAllCorrelatedThrows(
    G4ReweightTraj & traj) {
  std::vector<double> results;

  if (!decompSuccess) {
    std::cerr << "Cholesky decomposition was not formed." <<
                 "Returning empty weight vector" << std::endl;
    return results;
  }

  for (size_t i = 0; i < numberOfThrows; ++i) {
    results.push_back(GetWeightFromCorrelatedThrow(traj, i));
  }
  return results; 
}

std::pair<double, double> G4MultiReweighter::GetPlusMinusSigmaParWeight(
    G4ReweightTraj & traj,
    size_t iPar) {

  if (iPar+1 > paramNames.size()) {
    std::cerr << "Requested parameter index out of bounds" << std::endl;
    return {1., 1.};
  }

  //Build map with nominal +1 sigma for the requested parameter 
  std::map<std::string, double> paramMap;
  for (size_t i = 0; i < paramNames.size(); ++i) {
    paramMap[paramNames[i]] = paramNominalVals[i];
  }

  paramMap[paramNames[iPar]] += paramSigmas[iPar];

  //Give the variables to the parameter maker 
  //and use these for the reweighter
  parMaker.SetNewVals(paramMap);
  reweighter->SetNewHists(parMaker.GetFSHists());
  reweighter->SetNewElasticHists(parMaker.GetElasticHist());
  double plus_weight = reweighter->GetWeight(&traj);

  //Do the same for the -1 sigma variation
  //-2 to account for +1
  paramMap[paramNames[iPar]] -= 2.*paramSigmas[iPar];

  if (paramMap[paramNames[iPar]] < 0.)
    paramMap[paramNames[iPar]] = 0.;

  parMaker.SetNewVals(paramMap);
  reweighter->SetNewHists(parMaker.GetFSHists());
  reweighter->SetNewElasticHists(parMaker.GetElasticHist());
  double minus_weight = reweighter->GetWeight(&traj);

  return {plus_weight, minus_weight};
}

bool G4MultiReweighter::SetParameterValue(size_t iPar, double value) {
  if (iPar+1 > paramNames.size()) {
    std::cerr << "Requested parameter index out of bounds" << std::endl;
    return false;
  } 

  //std::cout << "Setting parameter value for " << iPar << " " <<
  //             paramNames[iPar] << std::endl;

  paramVals[paramNames[iPar]] = value;
  //std::cout << "Set value" << std::endl;
  parMaker.SetNewVals(paramVals);
  //Give the variables to the parameter maker 
  //and use these for the reweighter
  parMaker.SetNewVals(paramVals);
  reweighter->SetNewHists(parMaker.GetFSHists());
  reweighter->SetNewElasticHists(parMaker.GetElasticHist());
  //std::cout << "Set" << std::endl;

  return true;
}

bool G4MultiReweighter::SetAllParameterValues(std::vector<double> values) {
  if (values.size() != paramNames.size()) {
    std::cerr << "Input value vector differs from number of parameters" <<
                 std::endl;
    return false;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    bool set_value = SetParameterValue(i, values[i]);
    if (!set_value) {
      return false;
    }
  }
  return true;
}

double G4MultiReweighter::GetWeightFromSetParameters(G4ReweightTraj & traj) {
  return reweighter->GetWeight(&traj);
}
