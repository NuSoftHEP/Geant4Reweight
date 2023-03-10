#include "geant4reweight/FitterBase/G4ReweightFitter.hh"

#include "geant4reweight/ReweightBase/G4PiPlusReweighter.hh"
#include "geant4reweight/PropBase/G4ReweightParameterMaker.hh"

#include "TDirectory.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TVectorD.h"

#include <map>
#include <string>
#include <utility> // std::pair
#include <vector>

G4ReweightFitter::G4ReweightFitter(TFile * output_file,
                                   fhicl::ParameterSet exp,
                                   std::string frac_file_name,
                                   G4ReweightParameterMaker & parMaker,
                                   const fhicl::ParameterSet & material,
                                   G4ReweightManager * rw_manager) {
  fOutputFile = output_file;

  //Increase later
  nDOF = 0;

  fExperimentName = exp.get<std::string>("Name");
  std::vector<std::pair<std::string, std::string>> temp_graph_names =
      exp.get<std::vector<std::pair<std::string, std::string>>>("Graphs");
  graph_names = std::map<std::string, std::string>(temp_graph_names.begin(),
                                                   temp_graph_names.end());
  for (auto it = graph_names.begin(); it != graph_names.end(); ++it) {
    cuts.push_back(it->first);
    ++nDOF;
  }

  fDataFileName = exp.get<std::string>("Data");
  type = exp.get<std::string>("Type");

  //Eventually: reweighter factory
  fFracFile = new TFile(frac_file_name.c_str(), "OPEN");
  theReweighter = new G4PiPlusReweighter(
      fFracFile, parMaker.GetFSHists(), material, rw_manager,
      parMaker.GetElasticHist());

}

void G4ReweightFitter::FinishUp() {
  for (auto it = MC_xsec_graphs.begin(); it!= MC_xsec_graphs.end(); ++it) {
    if (it->second) delete it->second;
  }

  //delete theReweighter;
}


void G4ReweightFitter::SaveData(TDirectory * data_dir){
  data_dir->cd();

  TDirectory * experiment_dir;

  //Check if the directory already exists. If so, delete it and remake
  if( data_dir->Get( fExperimentName.c_str() ) ){
     data_dir->cd();
     data_dir->rmdir( fExperimentName.c_str() );
  }
  experiment_dir = data_dir->mkdir( fExperimentName.c_str() );
  experiment_dir->cd();

  std::map< std::string, TGraphErrors * >::iterator itData;
  for( itData = Data_xsec_graphs.begin(); itData != Data_xsec_graphs.end(); ++itData ){
    std::string name = itData->first;
    itData->second->Write( name.c_str() );
  }

}


//new fit method - calculates chi2 and num of data points for each process assoc with this fitter, stores them.
void G4ReweightFitter::DoFitModified(bool fSave){

  //empty fit data store
  fitDataStore.clear();

  double Data_val, MC_val, Data_err;
  double x;

  //Go through each cut defined for the experiment
  for( auto itXSec = Data_xsec_graphs.begin(); itXSec != Data_xsec_graphs.end(); ++itXSec ){
    std::string name = itXSec->first;

    //record how many points there are for each cut
    TGraph * MC_xsec = MC_xsec_graphs.at(name);
    TGraphErrors * Data_xsec = itXSec->second;

    int nPoints = Data_xsec->GetN();

    double partial_chi2 = 0.;

    for( int i = 0; i < nPoints; ++i ){
      Data_xsec->GetPoint(i, x, Data_val);
      Data_err = Data_xsec->GetErrorY(i);
      MC_val = MC_xsec->Eval( x );
      partial_chi2 += ( (Data_val - MC_val) / Data_err ) * ( (Data_val - MC_val) / Data_err );
    }

    //TODO: This probably needs updating to use new method for calculaing chi2
    if( fSave )
      SaveExpChi2( partial_chi2, name );

    Chi2Store thisStore(name,nPoints,partial_chi2);
    fitDataStore.push_back(thisStore);
  }
}


//old fit method
double G4ReweightFitter::DoFit(bool fSave){
  double Chi2 = 0.;
  double Data_val, MC_val, Data_err;
  double x;

  //Go through each cut defined for the experiment
  //std::map< std::string, TGraphErrors * >::iterator itXSec = Data_xsec_graphs.begin();
  for( auto itXSec = Data_xsec_graphs.begin(); itXSec != Data_xsec_graphs.end(); ++itXSec ){
    std::string name = itXSec->first;

    TGraph * MC_xsec = MC_xsec_graphs.at(name);
    TGraphErrors * Data_xsec = itXSec->second;

    int nPoints = Data_xsec->GetN();
    double partial_chi2 = 0.;
    for( int i = 0; i < nPoints; ++i ){
      Data_xsec->GetPoint(i, x, Data_val);
      Data_err = Data_xsec->GetErrorY(i);
      MC_val = MC_xsec->Eval( x );
      partial_chi2 += (1. / nPoints ) * ( (Data_val - MC_val) / Data_err ) * ( (Data_val - MC_val) / Data_err );
    }
    if( fSave )

      SaveExpChi2( partial_chi2, name );
    Chi2 += partial_chi2;
  }

  return Chi2;
}

void G4ReweightFitter::SaveExpChi2( double &theChi2, std::string &name ){
  fFitDir->cd();
  TVectorD chi2_val(1);
  chi2_val[0] = theChi2;
  chi2_val.Write( (name + "_chi2").c_str() );
}

void G4ReweightFitter::MakeFitDir( TDirectory *output_dir ){

  fTopDir = output_dir;
  fTopDir->cd();

  if( !output_dir->Get( fExperimentName.c_str() ) ){
    fFitDir = output_dir->mkdir( fExperimentName.c_str() );
  }
  else{
    fFitDir = (TDirectory*)output_dir->Get( fExperimentName.c_str() );
  }
  fFitDir->cd();

}

void G4ReweightFitter::LoadData(){
std::cout << "Loading data" << std::endl;
std::cout << fDataFileName << std::endl;
  fDataFile = new TFile( fDataFileName.c_str(), "READ");

  std::map< std::string, std::string >::iterator itGraphs;
  for( itGraphs = graph_names.begin(); itGraphs != graph_names.end(); ++itGraphs ){
    std::cout << itGraphs->first << " " << itGraphs->second << std::endl;
    Data_xsec_graphs[ itGraphs->first ] = (TGraphErrors*)fDataFile->Get(itGraphs->second.c_str());
  }
}



//gets num of data points and associated chi2 for a specific kind of cross section for this fitter
Chi2Store G4ReweightFitter::GetNDataPointsAndChi2(std::string cut){

  //zero everything
  Chi2Store thisStore(cut,0,0);
  for(size_t i=0;i<fitDataStore.size();i++){
    if(fitDataStore.at(i).cut == cut) thisStore = fitDataStore.at(i);
  }

  //if not found, return empty fit store
  return thisStore;
}

void G4ReweightFitter::GetMCValsWithCov(
    G4ReweightParameterMaker & parMaker,
    bool fSave, TH2D * cov, std::string position, bool doFullRange,
    std::vector<std::string> * channels) {

  std::map<std::string, std::vector<FitParameter>> pars =
      parMaker.GetParameterSet();
  std::vector<FitParameter> elast_pars = parMaker.GetElasticParameterSet();

  //inelastic parameters always go in first

  //Set the new values for the parameters
  theReweighter->SetNewHists(parMaker.GetFSHists());
  theReweighter->SetNewElasticHists(parMaker.GetElasticHist());

  std::map<std::string, double> ranges;
  if (doFullRange) {
    double max = -1.;

    for (auto it = parMaker.GetFSHists().begin();
         it != parMaker.GetFSHists().end(); ++it) {
      std::string name = it->first;
      //std::cout << "cut: " << name << std::endl;
      ranges[name] =
          it->second->GetXaxis()->GetBinUpEdge(it->second->GetNbinsX());
      //std::cout << "Set range: " << name << " " << ranges[name] << std::endl;
      if (ranges[name] > max)
        max = ranges[name];
      //std::cout << "max: " << max << std::endl;
    }

    for (auto it = parMaker.GetFSHists().begin();
         it != parMaker.GetFSHists().end(); ++it) {
      std::string name = it->first;
      //std::cout << "cut: " << name << std::endl;
      ranges[name] = max;
    }

    ranges["reac"] = max;
    ranges["abscx"] = max;
    ranges["elast"] = max;
    ranges["total"] = max;
  }

  //Go through each cut and get the values for the MC
  //based off of the points from the Data
  //First transform the cov into a map
  //Transform cov into map for easier use
  std::map<std::pair<std::string, std::string>, double> cov_vals_map;
  if (cov) {
    for (int i = 1; i <= cov->GetNbinsX(); ++i) {
      for (int j = 1; j <= cov->GetNbinsY(); ++j) {
        std::string x_label = cov->GetXaxis()->GetBinLabel(i);
        std::string y_label = cov->GetYaxis()->GetBinLabel(j);

        cov_vals_map[{x_label, y_label}] = cov->GetBinContent(i, j);
      }
    }
  }

  auto itCut = (channels == nullptr ? cuts.begin() : channels->begin());
  auto endCut = (channels == nullptr ? cuts.end() : channels->end());
  for (; itCut != endCut; ++itCut) {
    std::string cut_name = *itCut;

    //Get the data graph for this cut
    //Add flag to draw full range
    std::vector<double> xs, ys;

    if (doFullRange) {
      //std::cout << "Full range: " << *itCut << " " << ranges[*itCut] << std::endl;
      double x = 0.;
      while (x < ranges[*itCut]-.1) {
        xs.push_back(x);
        x += .1;
      }
      //Get the last one for good measure
      xs.push_back(x);
    }
    else {
      auto data_graph = Data_xsec_graphs[cut_name];
      for (int i = 0; i < data_graph->GetN(); ++i) {
        xs.push_back(data_graph->GetX()[i]);
      }
    }

    //Get the biased cross section from the reweighter
    if (cut_name == "reac") {
      for (size_t i = 0; i < xs.size(); ++i) {
        double x = xs[i];
        ys.push_back(theReweighter->GetInelasticXSec(x, true)*
                     theReweighter->GetInelasticBias(x));

        //if no cov or position specified, just use central values
        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = SigmaWithCov(x, cut_name, cov_vals_map, parMaker);
          ys.back() += (position == "up" ? sqrt(v) : -1.*sqrt(v));
        }
      }
    }
    else if (cut_name == "elast") {
      for (size_t i = 0; i < xs.size(); ++i) {
        double x = xs[i];
        ys.push_back(theReweighter->GetElasticXSec(x, true)*
                     theReweighter->GetElasticBias(x));

        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = SigmaWithCov(x, cut_name, cov_vals_map, parMaker);
          ys.back() += (position == "up" ? sqrt(v) : -1.*sqrt(v));
        }
      }
    }
    else if (cut_name == "total") {
      for (size_t i = 0; i < xs.size(); ++i) {
        double x = xs[i];
        ys.push_back((theReweighter->GetElasticXSec(x, true)*
                      theReweighter->GetElasticBias(x)) +
                     (theReweighter->GetInelasticXSec(x, true)*
                      theReweighter->GetInelasticBias(x)));

        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = SigmaWithCov(x, cut_name, cov_vals_map, parMaker);
          ys.back() += (position == "up" ? sqrt(v) : -1.*sqrt(v));
        }
      }
    }
    else if (cut_name == "abscx") {
      for (size_t i = 0; i < xs.size(); ++i) {
        double x = xs[i];
        ys.push_back(
            (theReweighter->GetExclusiveFactor(x, "abs")*
             theReweighter->GetExclusiveXSec(x, "abs", true)) +
            (theReweighter->GetExclusiveFactor(x, "cex")*
             theReweighter->GetExclusiveXSec(x, "cex", true)));

        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = SigmaWithCov(x, cut_name, cov_vals_map, parMaker);
          ys.back() += (position == "up" ? sqrt(v) : -1.*sqrt(v));
        }
      }
    }
    else {
      for (size_t i = 0; i < xs.size(); ++i) {
        double x = xs[i];
        ys.push_back(
            (theReweighter->GetExclusiveFactor(x, cut_name)*
             theReweighter->GetExclusiveXSec(x, cut_name, true)));

        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = SigmaWithCov(x, cut_name, cov_vals_map, parMaker);
          ys.back() += (position == "up" ? sqrt(v) : -1.*sqrt(v));
        }
      }
    }
    MC_xsec_graphs[cut_name] = new TGraph(xs.size(), &xs[0], &ys[0]);
  }
  //write all of the graphs to file, inc total and elast
  if( fSave ){
    for(auto it = MC_xsec_graphs.begin(); it!=MC_xsec_graphs.end(); ++it){
      fFitDir->cd();
      it->second->Write(it->first.c_str());
    }
  }
}

double G4ReweightFitter::SigmaWithCov(
    double x, std::string cut,
    std::map<std::pair<std::string, std::string>, double> & cov_vals_map,
    G4ReweightParameterMaker & parMaker) {
  std::map<std::string, std::vector<FitParameter>> pars =
      parMaker.GetParameterSet();
  std::vector<FitParameter> reac_pars = pars.at("reac");
  std::vector<FitParameter> elast_pars = parMaker.GetElasticParameterSet();
  std::map<std::string, double> par_vals;

  std::vector<std::string> all_cuts = {"elast"};
  for (auto it = pars.begin(); it != pars.end(); ++it) {
    all_cuts.push_back(it->first);
  }

  //go through the different vectors to find the ranges of the parameters
  //and check against x
  std::map<std::pair<std::string, std::string>, double> cov_map_cuts;
  for (size_t i = 0; i < all_cuts.size(); ++i) {
    std::string cut1 = all_cuts[i];
    bool is_dummy = false;
    bool is_out_of_range = true;
    size_t index1 = 0;
    if (cut1 == "elast") {
      if (!elast_pars.size()) {
        is_dummy = true;
      }
      else {
        for (size_t j = 0; j < elast_pars.size(); ++j) {
          FitParameter par = elast_pars[j];
          if ((x >= par.Range.first) && (x <= par.Range.second)) {
            index1 = j;
            is_out_of_range = false;
            break;
          }
        }
      }
    }
    else if (pars.at(cut1)[0].Dummy) {
      is_dummy = true;
    }
    else {
      std::vector<FitParameter> cut_pars = pars.at(cut1);
      for (size_t j = 0; j < cut_pars.size(); ++j) {
        FitParameter par = cut_pars[j];
        if ((x >= par.Range.first) && (x <= par.Range.second)) {
          index1 = j;
          is_out_of_range = false;
          break;
        }
      }
    }
    
    if (is_dummy || is_out_of_range) {
      par_vals[cut1] = 1.;
      for (size_t j = 0; j < all_cuts.size(); ++j) {
        std::string cut2 = all_cuts[j];
        cov_map_cuts[{cut1, cut2}] = 0.;
      }
    }
    else {
      if (cut1 == "elast") {
        par_vals[cut1] = elast_pars[index1].Value;
      }
      else {
        par_vals[cut1] = pars.at(cut1)[index1].Value;
      }
      for (size_t j = 0; j < all_cuts.size(); ++j) {
        bool is_out_of_range_2 = true;
        std::string cut2 = all_cuts[j];
        if (cut2 == "elast") {
          if (!elast_pars.size()) {
            cov_map_cuts[{cut1, cut2}] = 0.;
          }
          else {
            for (size_t k = 0; k < elast_pars.size(); ++k) {
              FitParameter par = elast_pars[k];
              if ((x >= par.Range.first) && (x <= par.Range.second)) {
                is_out_of_range_2 = false;
                if (cut1 == "elast") {
                  cov_map_cuts[{cut1, cut2}] =
                      fabs(cov_vals_map[{elast_pars[index1].Name, par.Name}]);
                }
                else {
                  cov_map_cuts[{cut1, cut2}] =
                      fabs(cov_vals_map[{pars.at(cut1)[index1].Name, par.Name}]);
                }
                break;
              }
              if (is_out_of_range_2) {
                cov_map_cuts[{cut1, cut2}] = 0.;
              }
            }
          }
        }
        else {
          std::vector<FitParameter> pars2 = pars[cut2];
          if (pars2[0].Dummy) {
            cov_map_cuts[{cut1, cut2}] = 0.;      
          }
          else {
            for (size_t k = 0; k < pars2.size(); ++k) {
              FitParameter par = pars2[k];
              if ((x >= par.Range.first) && (x <= par.Range.second)) {
                is_out_of_range_2 = false;
                if (cut1 == "elast") {
                  cov_map_cuts[{cut1, cut2}] =
                      fabs(cov_vals_map[{elast_pars[index1].Name, par.Name}]);
                }
                else {
                  cov_map_cuts[{cut1, cut2}] =
                      fabs(cov_vals_map[{pars.at(cut1)[index1].Name, par.Name}]);
                }
                break;
              }
              if (is_out_of_range_2) {
                cov_map_cuts[{cut1, cut2}] = 0.;
              }
            }
          }
        }
      }
    }
  }
  
  //Just do the elastic error. simple
  if (cut == "elast") {
    return (std::pow(theReweighter->GetElasticXSec(x, true), 2)*cov_map_cuts[{cut, cut}]);
  }
  else if (cut != "reac" && cut != "total" && cut != "abscx") {
    return ((std::pow(par_vals[cut]*
                      theReweighter->GetExclusiveXSec(x, cut, true), 2)*
             cov_map_cuts[{"reac", "reac"}]) +
            (std::pow(par_vals["reac"]*
                      theReweighter->GetExclusiveXSec(x, cut, true), 2)*
             cov_map_cuts[{cut, cut}]) +
            (2*par_vals["reac"]*
             par_vals[cut]*
             std::pow(theReweighter->GetExclusiveXSec(x, cut, true), 2)*
             cov_map_cuts[{cut, "reac"}]));
  }
  else if (cut == "abscx") {
    //reac term
    double variance = std::pow((par_vals["abs"]*
                                theReweighter->GetExclusiveXSec(x, "abs", true)) +
                               (par_vals["cex"]*
                                theReweighter->GetExclusiveXSec(x, "cex", true)), 2)*
                      cov_map_cuts[{"reac", "reac"}];

    //abs term
    variance += std::pow(theReweighter->GetExclusiveXSec(x, "abs", true)*par_vals["reac"], 2)*
                cov_map_cuts[{"abs", "abs"}];

    //cex term
    variance += std::pow(theReweighter->GetExclusiveXSec(x, "cex", true)*par_vals["reac"], 2)*
                cov_map_cuts[{"cex", "cex"}];

    //cross terms: abs/cex with reac
    variance += 2*((par_vals["abs"]*
                    theReweighter->GetExclusiveXSec(x, "abs", true)) +
                   (par_vals["cex"]*
                    theReweighter->GetExclusiveXSec(x, "cex", true)))*
                  ((par_vals["reac"]*
                    theReweighter->GetExclusiveXSec(x, "abs", true)*
                    cov_map_cuts[{"abs", "reac"}]) +
                   (par_vals["reac"]*
                    theReweighter->GetExclusiveXSec(x, "cex", true)*
                    cov_map_cuts[{"cex", "reac"}]));

    //cross term: abs with cex
    variance += 2*std::pow(par_vals["reac"], 2)*
                theReweighter->GetExclusiveXSec(x, "abs", true)*
                theReweighter->GetExclusiveXSec(x, "cex", true)*
                cov_map_cuts[{"abs", "cex"}];
    return variance;
  }
  else if (cut == "total" || cut == "reac") {

    //reac term
    double reac_term = 0.;
    for (size_t i = 0; i < all_cuts.size(); ++i) {
      if (all_cuts[i] == "elast" || all_cuts[i] == "reac") continue;
      reac_term += (par_vals[all_cuts[i]]*
                  theReweighter->GetExclusiveXSec(x, all_cuts[i], true));
    }
    double variance = reac_term*reac_term*cov_map_cuts[{"reac", "reac"}];

    //exclusive terms
    for (size_t i = 0; i < all_cuts.size(); ++i) {
      if (all_cuts[i] == "elast" || all_cuts[i] == "reac") continue;
      variance += cov_map_cuts[{all_cuts[i], all_cuts[i]}]*
                  std::pow(theReweighter->GetExclusiveXSec(x, all_cuts[i], true), 2)*
                  par_vals["reac"]*par_vals["reac"];
    }

    //Cross terms: reac with exclusive
    double sub_var = (2.*par_vals["reac"]*reac_term);

    for (size_t i = 0; i < all_cuts.size(); ++i) {
      if (all_cuts[i] == "elast" || all_cuts[i] == "reac") continue;
      variance += sub_var*theReweighter->GetExclusiveXSec(x, all_cuts[i], true)*
                  cov_map_cuts[{"reac", all_cuts[i]}];
    }

    //Cross terms: mixed exclusive
    for (size_t i = 0; i < all_cuts.size(); ++i) {
      if (all_cuts[i] == "elast" || all_cuts[i] == "reac") continue;
      for (size_t j = 0; j < all_cuts.size(); ++j) {
        if ((i == j) || (all_cuts[j] == "elast") || (all_cuts[j] == "reac"))
          continue;
        variance += par_vals["reac"]*par_vals["reac"]*
                    theReweighter->GetExclusiveXSec(x, all_cuts[i], true)*
                    theReweighter->GetExclusiveXSec(x, all_cuts[j], true)*
                    cov_map_cuts[{all_cuts[i], all_cuts[j]}];
      }
    }

    //If total, add in terms proportional to elast
    if (cut == "total") {
      //Elast term
      variance += std::pow(theReweighter->GetElasticXSec(x, true), 2)*
                  cov_map_cuts[{"elast", "elast"}];
      
      //Cross term: elast with reac
      variance += 2.*reac_term*cov_map_cuts[{"elast", "reac"}];

      //Cross term: elast with exclusive
      for (size_t i = 0; i < all_cuts.size(); ++i) {
        if (all_cuts[i] == "reac" || all_cuts[i] == "elast") continue;
        variance += 2.*theReweighter->GetElasticXSec(x, true)*par_vals["reac"]*
                    theReweighter->GetExclusiveXSec(x, all_cuts[i], true)*
                    cov_map_cuts[{"elast", all_cuts[i]}];
      }
    }

    return variance;
  }
  return 0.;
}
