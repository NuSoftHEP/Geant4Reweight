#include "G4ReweightFitter.hh"

#include <vector>
#include <map>
#include <iomanip>
#include <sstream>

#include "TH1D.h"
#include "TCanvas.h"
#include "TVectorD.h"

#include "TMatrixD.h"


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
    delete it->second;
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
    //std::string FracFileName,
    G4ReweightParameterMaker & parMaker,
    /*const fhicl::ParameterSet & material,
    G4ReweightManager * rw_manager,*/
    bool fSave,TMatrixD *cov, std::string position, bool doFullRange) {
  //store the info needed to calculate variances from the cov matrix
  //stores the list of parameters used, the momentum ranges they apply to
  theCovStore.clear();

  std::map<std::string, std::vector<FitParameter>> pars =
      parMaker.GetParameterSet();
  std::vector<FitParameter> elast_pars = parMaker.GetElasticParameterSet();

  //inelastic parameters always go in first

  bool use_reac = false;
  if(cov != nullptr && position != "CV"){
    int i=0;
    for (auto itPar = pars.begin(); itPar != pars.end(); ++itPar) {
      for (size_t j=0; j<itPar->second.size(); j++) {
        if (!itPar->second.at( j ).Dummy) {
          std::string name1 = itPar->first;
          covElementStore S;

          S.index = i;
          S.Range = itPar->second.at(j).Range;
          S.cut = name1;

          if(name1 == "reac")
            use_reac = true;
          theCovStore.push_back(S);
          i++;
        }
      }
    }

    for (size_t i_ep=0; i_ep < elast_pars.size(); ++i_ep) {
      std::string name = elast_pars.at(i_ep).Cut;

      covElementStore S;
      S.index = i;
      S.Range = elast_pars.at(i_ep).Range;
      S.cut = name;
      theCovStore.push_back(S);
      i++;
    }
  }

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


    ranges["reac"] = max;
    ranges["abscx"] = (ranges["abs"] > ranges["cex"] ?
                       ranges["abs"] : ranges["cex"]);
    ranges["elast"] =
        parMaker.GetElasticHist()->GetXaxis()->GetBinUpEdge(
            parMaker.GetElasticHist()->GetNbinsX());
  }

  //Go through each cut and get the values for the MC
  //based off of the points from the Data
  for (auto itCut = cuts.begin(); itCut != cuts.end(); ++itCut) {
    std::string cut_name = *itCut;

    //Get the data graph for this cut
    //Add flag to draw full range
    std::vector<double> xs, ys;

    if (doFullRange) {
      //std::cout << "Full range: " << *itCut << " " << ranges[*itCut] << std::endl;
      double x = 0.;
      while (x < ranges[*itCut]) {
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
        ys.push_back(theReweighter->GetInelasticXSec(x)*
                     theReweighter->GetInelasticBias(x));

        //if no cov or position specified, just use central values
        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = NewSigmaWithCov(x, cut_name, cov, use_reac);
          ys.back() += (position == "up" ? sqrt(v) : -1.*sqrt(v));
        }
      }
    }
    else if (cut_name == "elast") {
      for (size_t i = 0; i < xs.size(); ++i) {
        double x = xs[i];
        ys.push_back(theReweighter->GetElasticXSec(x)*
                     theReweighter->GetElasticBias(x));

        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = NewSigmaWithCov(x, cut_name, cov, use_reac);
          ys.back() += (position == "up" ? sqrt(v) : -1.*sqrt(v));
        }
      }
    }
    else if (cut_name == "total") {
      for (size_t i = 0; i < xs.size(); ++i) {
        double x = xs[i];
        ys.push_back((theReweighter->GetElasticXSec(x)*
                      theReweighter->GetElasticBias(x)) +
                     (theReweighter->GetInelasticXSec(x)*
                      theReweighter->GetInelasticBias(x)));

        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = NewSigmaWithCov(x, cut_name, cov, use_reac);
          ys.back() += (position == "up" ? sqrt(v) : -1.*sqrt(v));
        }
      }
    }
    else if (cut_name == "abscx") {
      for (size_t i = 0; i < xs.size(); ++i) {
        double x = xs[i];
        ys.push_back(
            (theReweighter->GetExclusiveFactor(x, "abs")*
             theReweighter->GetExclusiveXSec(x, "abs")) +
            (theReweighter->GetExclusiveFactor(x, "cex")*
             theReweighter->GetExclusiveXSec(x, "cex")));

        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = NewSigmaWithCov(x, cut_name, cov, use_reac);
          ys.back() += (position == "up" ? sqrt(v) : -1.*sqrt(v));
        }
      }
    }
    else {
      for (size_t i = 0; i < xs.size(); ++i) {
        double x = xs[i];
        ys.push_back(
            (theReweighter->GetExclusiveFactor(x, cut_name)*
             theReweighter->GetExclusiveXSec(x, cut_name)));

        if (cov && position != "CV") {
          //new method to get +/- 1 sigma variations
          double v = NewSigmaWithCov(x, cut_name, cov, use_reac);
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

double G4ReweightFitter::NewSigmaWithCov (double x, std::string cut, TMatrixD * cov, bool use_reac) {
  double variance = 0;
  //variances of individual channels as a fraction of nominal cross section

  //key is two strings that look up which element of the cov matrix they correspond to
  std::map<std::pair<std::string,std::string>, double> cuts_and_covs;
  //setup map storing names of excl channels and cov matrix elements
  for (size_t i_cut = 0; i_cut < theCovStore.size(); ++i_cut) {
    for (size_t j_cut = 0; j_cut < theCovStore.size(); ++j_cut) {
      //if x outside the range of either parameter, set corresponding cov element to zero
      if ((x < theCovStore.at(i_cut).Range.first) ||
          (x > theCovStore.at(i_cut).Range.second) ||
          (x < theCovStore.at(j_cut).Range.first) ||
          (x > theCovStore.at(j_cut).Range.second)) {
        cuts_and_covs[{theCovStore.at(i_cut).cut, theCovStore.at(j_cut).cut}] = 0;
      }
      else {
        cuts_and_covs[{theCovStore.at(i_cut).cut,theCovStore.at(j_cut).cut}] =
            (*cov)(theCovStore.at(i_cut).index, theCovStore.at(j_cut).index);
      }
    }
  }


  //TODO: Find a way to figure out which channels to use from the params file / make this less hard coded in (eg inherited class)
  //or from how they're set in G4PionReweighter constructor
  std::string excl_channels[8] = {"abs", "cex", "inel", "prod", "dcex", "elast",
                                  "reac", "total"};
  //add any remaining data types not found in cov matrix
  for (int i_ch=0; i_ch<8; i_ch++) {
    for (int j_ch=0; j_ch<8; j_ch++) {
      //if entry in CM already exists, skip
      if (cuts_and_covs.find({excl_channels[i_ch] , excl_channels[j_ch]}) == cuts_and_covs.end()) {
        //if reac is being used as a parameter then set diagonal elements of cov matrix for abs, cex, inel, prod, dcex as equal to reac error
        if (use_reac && i_ch==j_ch && excl_channels[i_ch] != "elast" && excl_channels[i_ch] != "total") {
          cuts_and_covs[{excl_channels[i_ch], excl_channels[i_ch]}] = cuts_and_covs[{"reac","reac"}];
        }
        else
          cuts_and_covs[{excl_channels[i_ch], excl_channels[j_ch]}] = 0.0;
      }
    }
  }

  //create a map between each name of exclusive channel and its Nominal prediction
  //TODO find way to make this less hard coded in
  std::map<std::string , double> cuts_and_noms;
  for (int i_ch=0; i_ch<8; ++i_ch) {
    //std::cout << excl_channels[i_ch] << std::endl;
    if (excl_channels[i_ch] == "elast") {
      cuts_and_noms["elast"] = theReweighter->GetElasticXSec(x);
    }
    else if(excl_channels[i_ch] == "reac"){
      cuts_and_noms["reac"] = theReweighter->GetInelasticXSec(x);
    }
    else if(excl_channels[i_ch] == "total"){
      cuts_and_noms["total"] = theReweighter->GetElasticXSec(x) +
                               theReweighter->GetInelasticXSec(x);
    }
    //retrieve nominal excl channel cross sections cross sections
    else {
      cuts_and_noms[excl_channels[i_ch]] =
          theReweighter->GetExclusiveXSec(x, excl_channels[i_ch]);
    }
  }

  //now calculate the varaince depending on what sort of process you're dealing with
  //single exclusive channel
  if(cut != "total" && cut != "reac" && cut != "abscx"){
    variance = cuts_and_noms[cut]*
               cuts_and_noms[cut]*
               cuts_and_covs[{cut, cut}];
  }
  //abscx - sum of abs and cex, include cov between the two
  else if (cut == "abscx") {
    variance = (cuts_and_noms["abs"]*cuts_and_noms["abs"]*
                cuts_and_covs[{"abs", "abs"}]) +
               (cuts_and_noms["cex"]*cuts_and_noms["cex"]*
                cuts_and_covs[std::make_pair("cex","cex")]) +
               (2*cuts_and_noms["abs"]*cuts_and_noms["cex"]*
                cuts_and_covs[{"cex", "abs"}]);
  }
  // reac - sum of abs cex inel dcex and prod, include all their covariances
  else if (cut == "reac") {
    if (!use_reac) {

      std::string reac_excl_ch[5] = {"abs", "cex", "inel", "prod", "dcex"};
      variance = 0;

      for (int i=0; i<5; i++) {
        for (int j=0; j<5; j++) {
          variance += (cuts_and_noms[reac_excl_ch[i]]*
                       cuts_and_noms[reac_excl_ch[j]]*
                       cuts_and_covs[{reac_excl_ch[i],reac_excl_ch[j]}]);
        }
      }
    }// !use_reac
    else {
      variance = (cuts_and_noms["reac"]*
                  cuts_and_noms["reac"]*
                  cuts_and_covs[{"reac", "reac"}]);
    }// use_reac
  }
  else if (cut == "total") {
    if (!use_reac) {
      std::string reac_excl_ch[6] = {"abs", "cex", "inel", "prod", "dcex",
                                     "elast"};
      variance = 0;
      for (int i=0; i<6; i++) {
        for (int j=0; j<6; j++) {
          variance += (cuts_and_noms[reac_excl_ch[i]]*
                       cuts_and_noms[reac_excl_ch[j]]*
                       cuts_and_covs[{reac_excl_ch[i],reac_excl_ch[j]}]);
        }
      }
    }
    else {
      variance = (cuts_and_noms["reac"]*cuts_and_noms["reac"]*
                  cuts_and_covs[{"reac", "reac"}]) +
                 (cuts_and_noms["elast"]*cuts_and_noms["elast"]*
                  cuts_and_covs[{"elast", "elast"}]) +
                 (2*cuts_and_noms["elast"]*cuts_and_noms["reac"]*
                  cuts_and_covs[{"elast", "reac"}]);
    }
  }
  else {
    std::cout << "Unrecognized process " << cut << " assuming 0 variance" << std::endl;
    return 0;
  }

  return variance;
}
