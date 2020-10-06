#include "G4ReweightFitter.hh"

#include <vector>
#include <map>
#include <iomanip>
#include <sstream>

#include "TH1D.h"
#include "TCanvas.h"
#include "TVectorD.h"

#include "TMatrixD.h"


G4ReweightFitter::G4ReweightFitter( TFile * output_file, fhicl::ParameterSet exp ){
  fOutputFile = output_file;

  //Increase later
  nDOF = 0;

  fExperimentName = exp.get< std::string >("Name");
  std::vector< std::pair< std::string, std::string > > temp_graph_names = exp.get< std::vector< std::pair<std::string, std::string> > >("Graphs");
  graph_names = std::map< std::string, std::string >( temp_graph_names.begin(), temp_graph_names.end() );
  for( auto it = graph_names.begin(); it != graph_names.end(); ++it ){
    cuts.push_back( it->first );
    ++nDOF;
  }

  fDataFileName = exp.get< std::string >("Data");
  type = exp.get< std::string >("Type");

  double dummyX = 0.;
  double dummyY = 1.;

  dummyGraph = new TGraph(1, &dummyX, &dummyY );
  dummyHist  = new TH1D("dummy", "", 1,0,0);
  //Set the over/underflow bins for the dummy
  dummyHist->SetBinContent(0,1.);
  dummyHist->SetBinContent(1,1.);
  dummyHist->SetBinContent(2,1.);
}

void G4ReweightFitter::FinishUp(){
  std::map< std::string, TGraph* >::iterator it =
    MC_xsec_graphs.begin();

  for( ; it!= MC_xsec_graphs.end(); ++it ){
    delete it->second;
  }

  delete theReweighter;
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
//std::cout << "Loading data" << std::endl;
//std::cout << fDataFileName << std::endl;
  fDataFile = new TFile( fDataFileName.c_str(), "READ");

  std::map< std::string, std::string >::iterator itGraphs;
  for( itGraphs = graph_names.begin(); itGraphs != graph_names.end(); ++itGraphs ){
//std::cout << itGraphs->first << std::endl;
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
    std::string FracFileName, /*std::map<std::string,
    std::vector<FitParameter>> pars,
    std::vector<FitParameter> elast_pars,*/
    G4ReweightParameterMaker & parMaker,
    const fhicl::ParameterSet & material,
    bool fSave,TMatrixD *cov, std::string position) {
  //store the info needed to calculate variances from the cov matrix
  //stores the list of parameters used, the momentum ranges they apply to
  theCovStore.clear();

  std::map<std::string, std::vector<FitParameter>> pars =
      parMaker.GetParameterSet();
  std::vector<FitParameter> elast_pars = parMaker.GetElasticParameterSet();

  //inelastic parameters always go in first

/*
  bool use_reac=false;
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

          if(name1 == "reac") use_reac = true;
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
  */


  //Eventually: reweighter factory
  TFile FracFile(FracFileName.c_str(), "OPEN");
  theReweighter = new G4PiPlusReweighter(
      &FracFile, parMaker.GetFSHists(), material, parMaker.GetElasticHist());

  //Go through each cut and get the values for the MC
  //based off of the points from the Data 
  for (auto itCut = cuts.begin(); itCut != cuts.end(); ++itCut) {
    std::string cut_name = *itCut;

    //Get the data graph for this cut
    auto data_graph = Data_xsec_graphs[cut_name];
    std::vector<double> xs, ys;
    for (int i = 0; i < data_graph->GetN(); ++i) {
      xs.push_back(data_graph->GetX()[i]);
    }

    //Get the biased cross section from the reweighter
    if (cut_name == "reac") {
      for (size_t i = 0; i < xs.size(); ++i) {
        ys.push_back(theReweighter->GetInelasticXSec(xs[i])*
                     theReweighter->GetInelasticBias(xs[i]));
      }
    }
    else if (cut_name == "elast") {
      for (size_t i = 0; i < xs.size(); ++i) {
        ys.push_back(theReweighter->GetElasticXSec(xs[i])*
                     theReweighter->GetElasticBias(xs[i]));
      }
    }
    else if (cut_name == "total") {
      for (size_t i = 0; i < xs.size(); ++i) {
        ys.push_back((theReweighter->GetElasticXSec(xs[i])*
                      theReweighter->GetElasticBias(xs[i])) +
                     (theReweighter->GetInelasticXSec(xs[i])*
                      theReweighter->GetInelasticBias(xs[i])));
      }     
    }
    else if (cut_name == "abscx") {
      for (size_t i = 0; i < xs.size(); ++i) {
        ys.push_back(
            (theReweighter->GetExclusiveFactor(xs[i], "abs")*
             theReweighter->GetExclusiveXSec(xs[i], "abs")) +
            (theReweighter->GetExclusiveFactor(xs[i], "cex")*
             theReweighter->GetExclusiveXSec(xs[i], "cex")));
      }
    }
    else {
      for (size_t i = 0; i < xs.size(); ++i) {
        ys.push_back(
            (theReweighter->GetExclusiveFactor(xs[i], cut_name)*
             theReweighter->GetExclusiveXSec(xs[i], cut_name)));
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


//If not given a covariance matrix and position, will default to using central values that have been set
//Uses elements of provided coariance matrix to calculate pm 1 sigma curves for drawing
/*void G4ReweightFitter::GetMCFromCurvesWithCovariance(std::string FracFileName, std::map<std::string, std::vector<FitParameter>> pars, std::vector<FitParameter> elast_pars, bool fSave,TMatrixD *cov, std::string position){

  //store the info needed to calculate variances from the cov matrix
  //stores the list of parameters used, the momentum ranges they apply to
  theCovStore.clear();

  std::map<std::string, std::vector<FitParameter>>::iterator itPar;
  //inelastic parameters always go in first

  bool use_reac=false;
  if(cov != nullptr && position != "CV"){
    int i=0;
    for( itPar = pars.begin(); itPar != pars.end(); ++itPar ){
      for(size_t j=0;j<itPar->second.size();j++){
        if( !itPar->second.at( j ).Dummy ){
          std::string name1 = itPar->first;
          covElementStore S;

          S.index = i;
          S.Range = itPar->second.at(j).Range;
          S.cut = name1;

          if(name1 == "reac") use_reac = true;
          theCovStore.push_back(S);
          i++;
        }
      }
    }

    for(size_t i_ep=0;i_ep<elast_pars.size();i_ep++){
      std::string name = elast_pars.at(i_ep).Cut;

      covElementStore S;
      S.index = i;
      S.Range = elast_pars.at(i_ep).Range;
      S.cut = name;
      theCovStore.push_back(S);
      i++;
    }
  }

  TFile TotalXSecFile(TotalXSecFileName.c_str(), "OPEN");

  std::map< std::string, TH1D* > FSHists;
  std::map< std::string, bool > CutIsDummy;

  //iterate through the different processes, set MC predictions for the current values of fit parameters
  for( itPar = pars.begin(); itPar != pars.end(); ++itPar ){
    std::string name = itPar->first;
    if( name == "reac" ) continue;

    bool isDummy = false;

    std::vector< std::pair< double, double > > vars;
    std::vector< double > varX, varY;

    for( size_t i = 0; i < itPar->second.size(); ++i ){

      if( itPar->second.at( i ).Dummy ){
        FSHists[name] = (TH1D*)dummyHist->Clone();
        isDummy = true;
        break;
      }

      else{

        double value = itPar->second.at( i ).Value;
        std::pair< double, double > range = itPar->second.at( i ).Range;

        vars.push_back( std::make_pair( range.first,  value ) );
        vars.push_back( std::make_pair( range.second, value ) );

        bool addDummyBin = false;
        if( varX.size() ){
          //If the end of last bin == start of this bin
          //don't need to add a dummy
          if( varX.back() < range.first ){
            varX.push_back( range.first );
            addDummyBin = true;
          }
        }
        else
          varX.push_back(range.first);

        varX.push_back( range.second );

        if( addDummyBin )
          varY.push_back( 1. );
        varY.push_back( value );
      }

    }

    CutIsDummy[ name ] = isDummy;

    if( !isDummy ){
      FSHists[name] = new TH1D( ("var"+name).c_str(),"", varX.size()-1, &varX[0]);
      for( size_t i = 0; i < varY.size(); ++i ){
        FSHists[name]->SetBinContent(i+1, varY[i]);
      }
      //Set under/overflow
      FSHists[name]->SetBinContent( 0, 1. );
      FSHists[name]->SetBinContent( FSHists[name]->GetNbinsX()+1, 1. );
    }
  }
  //loop ends here

  if( pars.find( "reac" ) != pars.end() ){
    if( !pars[ "reac" ].at(0).Dummy ){
      //If reac exists and is not a dummy, go through the exclusive channels
      //and vary each by the reac variations

      //Build the reac graph
      std::vector< double > reac_bins, varY, reacBins;
      for( size_t i = 0; i < pars["reac"].size(); ++i ){
        double value = pars["reac"].at( i ).Value;
        std::pair< double, double > range = pars["reac"].at( i ).Range;

        bool addDummyBin = false;
        if( reac_bins.size() ){
          //If the end of last bin == start of this bin
          //don't need to add a dummy
          if( reac_bins.back() < range.first ){
            reac_bins.push_back( range.first );
            addDummyBin = true;
          }
        }
        else
          reac_bins.push_back(range.first);

        reac_bins.push_back( range.second );

        if( addDummyBin )
          varY.push_back( 1. );
        varY.push_back( value );

      }

      TH1D reac_hist( "var_reac","", reac_bins.size()-1, &reac_bins[0]);
      for( size_t i = 0; i < varY.size(); ++i ){
        reac_hist.SetBinContent(i+1, varY[i]);
      }
      //Set under/overflow
      reac_hist.SetBinContent( 0, 1. );
      reac_hist.SetBinContent( reac_hist.GetNbinsX()+1, 1. );

      for( auto itGr = FSHists.begin(); itGr != FSHists.end(); ++itGr ){
        std::string name = itGr->first;

        auto excHist = itGr->second;
        std::vector< double > exc_bins;
        if( !CutIsDummy[name] ){
          for( int i = 1; i <= excHist->GetNbinsX(); ++i ){
            exc_bins.push_back( excHist->GetBinLowEdge(i) );
          }
          exc_bins.push_back( excHist->GetBinLowEdge(excHist->GetNbinsX()) + excHist->GetBinWidth(excHist->GetNbinsX()) );
        }

        std::vector< double > new_bins = exc_bins;
        for( size_t i = 0; i < reac_bins.size(); ++i ){
          if( std::find( new_bins.begin(), new_bins.end(), reac_bins[i] )
          == new_bins.end() ){
            new_bins.push_back( reac_bins[i] );
          }
        }

        std::sort( new_bins.begin(), new_bins.end() );

        TH1D new_hist( "new_hist", "", new_bins.size()-1, &new_bins[0] );
        for( int i = 1; i <= new_hist.GetNbinsX(); ++i ){
          double x = new_hist.GetBinCenter( i );
          int reac_bin = reac_hist.FindBin( x );
          int exc_bin = excHist->FindBin( x );

          double content = reac_hist.GetBinContent( reac_bin );
          content *= excHist->GetBinContent( exc_bin );

          new_hist.SetBinContent(i, content );
        }

        std::string exc_name = excHist->GetName();
        (*excHist) = new_hist;
        excHist->SetName( exc_name.c_str() );

      }

    }
  }

  //setup elastic prediction
  TH1D *ElasticHist;
  std::vector< double > varX, varY;
  for( size_t i = 0; i < elast_pars.size(); ++i ){
    double value = elast_pars[i].Value;
    //std::cout << "elast_pars: " << elast_pars[i].Value << std::endl;

    std::pair< double, double > range = elast_pars[i].Range;
    bool addDummyBin = false;
    if( varX.size() ){
      //If the end of last bin == start of this bin
      //don't need to add a dummy
      if( varX.back() < range.first ){
        varX.push_back( range.first );
        addDummyBin = true;
      }
    }
    else
      varX.push_back(range.first);

    varX.push_back( range.second );

    if( addDummyBin )
      varY.push_back( 1. );
    varY.push_back( value );
  }
  if( varX.size() ){
    ElasticHist = new TH1D( "varElast","", varX.size()-1, &varX[0]);
    for( size_t i = 0; i < varY.size(); ++i ){
      ElasticHist->SetBinContent(i+1, varY[i]);
    }
  }
  else{
    ElasticHist = new TH1D( "varElast", "", 1, 0, 0 );
    ElasticHist->SetBinContent( 1, 1. );
  }
  //Set under/overflow
  ElasticHist->SetBinContent( 0, 1. );
  ElasticHist->SetBinContent( ElasticHist->GetNbinsX()+1, 1. );

  TFile FracFile(FracFileName.c_str(), "OPEN");
  theReweighter = new G4PiPlusReweighter(&FracFile, FSHists, ElasticHist);

  total_inel = theReweighter->GetTotalGraph();
  total_el = theReweighter->GetElastGraph();

  //total_var is a tgraph storing the change in the (total or inelastic cross section? )
  TGraph * total_var = theReweighter->GetTotalVariationGraph();

  for( auto itCut = cuts.begin(); itCut != cuts.end(); ++itCut ){
    std::string cut_name = *itCut;

    if(cut_name == "total") continue;

    std::vector< double > xs,ys;

    if(cut_name == "reac"){
      for( int i = 0; i < total_inel->GetN(); ++i ){
        double x = total_inel->GetX()[i];
        double y = total_inel->GetY()[i] * total_var->Eval( x );
        if( x > total_var->GetX()[ total_var->GetN() -1 ] ){
          break;
        }

        double y_var;

        //if no cov or position specified, just use central values
        if(cov != nullptr && position != "CV"){
        
          //new method to get +/- 1 sigma variations
          double v = SigmaWithCovariance(x,"reac",cov , use_reac);
          //std::cout << "reac: " << y << "  +/-  " <<  sqrt(v) << std::endl;
          
          if(position == "up")
            y_var = y + sqrt(v); //plus 1 sigma
          else
            y_var = y-sqrt(v); //minus 1 sigma
        }
        else
          y_var = y;

        ys.push_back( y_var);
        xs.push_back( x );
      }
      MC_xsec_graphs[ "reac" ] = new TGraph( xs.size(), &xs[0], &ys[0] );
    }//cut_name == "reac"
    else if( cut_name == "abscx" ){
      for( int i = 0; i < total_inel->GetN(); ++i ){
        double x = total_inel->GetX()[i];

        if( x > theReweighter->GetNewGraph( "abs" )->GetX()[ theReweighter->GetNewGraph( "abs" )->GetN() -1 ] ){
          break;
        }
        double y = total_inel->GetY()[i]*( theReweighter->GetNewGraph( "abs" )->Eval( x ) + theReweighter->GetNewGraph( "cex" )->Eval( x ) );

        double y_var;

        if(cov != nullptr && position != "CV"){
          double v = SigmaWithCovariance(x,cut_name,cov , use_reac);
          //std::cout <<"abscx: " << x << " , " <<y << "  +/-  " <<  sqrt(v)  << std::endl;

          if(position == "up")
            y_var = y + sqrt(v); //plus 1 sigma
          else
            y_var = y - sqrt(v); //minus 1 sigma
        }

        else
          y_var = y;

        ys.push_back( y_var);
        xs.push_back( x );
      }

      MC_xsec_graphs[ cut_name ] = new TGraph( xs.size(), &xs[0], &ys[0] );
    }//cut_name == "abscx"
    else if(cut_name != "elast"){
      for( int i = 0; i < total_inel->GetN(); ++i ){
        double x = total_inel->GetX()[i];
        if( x > theReweighter->GetNewGraph( cut_name )->GetX()[ theReweighter->GetNewGraph( cut_name )->GetN() -1 ] ){
          break;
        }
        double y = total_inel->GetY()[i];
        double y_var;

        if(cov != nullptr && position != "CV"){

          double v = SigmaWithCovariance(x,cut_name,cov , use_reac);
          if(position == "up")
            y_var =y*(theReweighter->GetNewGraph(cut_name.c_str())->Eval(x)) + sqrt(v); //plus 1 sigma
          else
            y_var =y*(theReweighter->GetNewGraph(cut_name.c_str())->Eval(x)) - sqrt(v); //minus 1 sigma
        }
        else
          y_var = y*theReweighter->GetNewGraph(cut_name.c_str())->Eval(x);

        ys.push_back(y_var);
        xs.push_back( x );
      }
      MC_xsec_graphs[ cut_name ] = new TGraph( xs.size(), &xs[0], &ys[0] );
    }//cut_name != "elast" -- anythig else that isn't elastic
  }//itCut

  std::vector<double>xs,ys;
  //Do elastic cross section
  for(int i = 0; i< total_el->GetN(); ++i){
    double x = total_el->GetX()[i];
    double y = total_el->GetY()[i];

    if(x> theReweighter->GetNewElastGraph()->GetX()[ theReweighter->GetNewElastGraph()->GetN() -1 ]){
      break;
    }

    double y_var;

    if(cov != nullptr && position != "CV"){
      double v = SigmaWithCovariance(x,"elast" ,cov , use_reac);
      if(position == "up")
        y_var = y*theReweighter->GetNewElastGraph()->Eval(x)+sqrt(v); //plus 1 sigma
      else
        y_var = y*theReweighter->GetNewElastGraph()->Eval(x)-sqrt(v); //minus 1 sigma
    }
    else
      y_var = y*(theReweighter->GetNewElastGraph()->Eval(x));

    xs.push_back(x);
    ys.push_back(y_var);
  }

  MC_xsec_graphs["elast"] = new TGraph(xs.size() , &xs[0], &ys[0] );

  //compute the total cross section as well
  xs.clear();
  ys.clear();

  for(int i = 0; i< total_el->GetN(); ++i){
    double x = total_el->GetX()[i];

    if(x> theReweighter->GetNewElastGraph()->GetX()[ theReweighter->GetNewElastGraph()->GetN() -1 ] || x > total_var->GetX()[ total_var->GetN() -1 ]) {
      break;
    }

    double y_el = total_el->GetY()[i];
    double y_inel = total_inel->GetY()[i];

    double y = y_el*theReweighter->GetNewElastGraph()->Eval(x) + y_inel*total_var->Eval(x);
    double y_var;

    if(cov != nullptr && position != "CV"){
      double v = SigmaWithCovariance(x,"total",cov , use_reac);
      if(position == "up") y_var = y + sqrt(v); //plus 1 sigma
      else y_var = y - sqrt(v); //minus 1 sigma
    }
    else
      y_var = y;

    xs.push_back(x);
    ys.push_back( y_var);
  }

  MC_xsec_graphs["total"] = new TGraph(xs.size() , &xs[0], &ys[0] );
  //write all of the graphs to file, inc total and elast
  if( fSave ){
    for(auto it = MC_xsec_graphs.begin(); it!=MC_xsec_graphs.end(); ++it){
      fFitDir->cd();
      it->second->Write(it->first.c_str());
    }
  }

  //clear memory
  std::map < std::string, TH1D *>::iterator it =
    FSHists.begin();
  for( ; it != FSHists.end(); ++it ){
    delete it->second;
  }

  delete ElasticHist;
  delete total_inel;
  delete total_el;
  delete total_var;
}


//arguments cut = type of cross section you want SD for , cov = covariance matrix for fit par
//use_reac -> Set to true if you want to fit all of the reaction channels with a single parameter
double G4ReweightFitter::SigmaWithCovariance(double x , std::string cut , TMatrixD *cov  , bool use_reac){
  double variance = 0;
  //variances of individual channels as a fraction of nominal cross section

  //key is two strings that look up which element of the cov matrix they correspond to
  std::map<std::pair<std::string,std::string> ,double   > cuts_and_covs;
  //setup map storing names of excl channels and cov matrix elements
  for(size_t i_cut=0; i_cut<theCovStore.size(); i_cut++){
    for(size_t j_cut=0; j_cut<theCovStore.size(); j_cut++){
      //if x outside the range of either parameter, set corresponding cov element to zero
      if(x < theCovStore.at(i_cut).Range.first || x > theCovStore.at(i_cut).Range.second || x < theCovStore.at(j_cut).Range.first || x > theCovStore.at(j_cut).Range.second){
        cuts_and_covs[std::make_pair(theCovStore.at(i_cut).cut,theCovStore.at(j_cut).cut)] = 0;
      }
      else {

        cuts_and_covs[std::make_pair(theCovStore.at(i_cut).cut,theCovStore.at(j_cut).cut)] = (*cov)(theCovStore.at(i_cut).index , theCovStore.at(j_cut).index );
      }
    }
  }


  //TODO: Find a way to figure out which channels to use from the params file / make this less hard coded in (eg inherited class)
  //or from how they're set in G4PionReweighter constructor
  std::string excl_channels[8] = {"abs","cex","inel","prod","dcex","elast","reac","total"};
  //add any remaining data types not found in cov matrix
  for(int i_ch=0; i_ch<8; i_ch++){
    for(int j_ch=0; j_ch<8; j_ch++){
      //if entry in CM already exists, skip
      if ( cuts_and_covs.find(std::make_pair(excl_channels[i_ch] , excl_channels[j_ch] )) == cuts_and_covs.end() ){
        //if reac is being used as a parameter then set diagonal elements of cov matrix for abs, cex, inel, prod, dcex as equal to reac error
        if(use_reac && i_ch==j_ch && excl_channels[i_ch] != "elast" && excl_channels[i_ch] != "total"){
          cuts_and_covs[std::make_pair(excl_channels[i_ch] , excl_channels[i_ch])] = cuts_and_covs[std::make_pair("reac","reac")];
        }
        else
          cuts_and_covs[std::make_pair(excl_channels[i_ch] , excl_channels[j_ch])] = 0.0;
      }
    }
  }
  
  //create a map between each name of exclusive channel and its Nominal prediction
  //TODO find way to make this less hard coded in
  std::map<std::string , double> cuts_and_noms;

  for(int i_ch=0;i_ch<8;i_ch++){
    //std::cout << excl_channels[i_ch] << std::endl;
    if(excl_channels[i_ch] == "elast"){
      double y =theReweighter->GetOldElastGraph()->Eval(x);
      cuts_and_noms["elast"] = y;
    }
    else if(excl_channels[i_ch] == "reac"){
      //quantity in brackets will just be 1 if nominal for reac is set to 1, but calculate anyway just in case
      double y =  total_inel->Eval(x)*( theReweighter->GetOldGraph( "abs" )->Eval( x ) + theReweighter->GetOldGraph( "cex" )->Eval( x )
                                + theReweighter->GetOldGraph( "inel" )->Eval( x ) + theReweighter->GetOldGraph( "prod" )->Eval( x )
                                + theReweighter->GetOldGraph( "dcex" )->Eval( x ) );
      cuts_and_noms["reac"] = y;
    }
    else if(excl_channels[i_ch] == "total"){
      double y =  total_inel->Eval(x)*( theReweighter->GetOldGraph( "abs" )->Eval( x ) + theReweighter->GetOldGraph( "cex" )->Eval( x )
                                + theReweighter->GetOldGraph( "inel" )->Eval( x ) + theReweighter->GetOldGraph( "prod" )->Eval( x )
                                + theReweighter->GetOldGraph( "dcex" )->Eval( x ) );
      y += theReweighter->GetOldElastGraph()->Eval(x);
      cuts_and_noms["total"] = y;
    }
    //retrieve nominal excl channel cross sections cross sections
    else {
      double y = total_inel->Eval(x)*theReweighter->GetOldGraph( excl_channels[i_ch] )->Eval( x );
      cuts_and_noms[excl_channels[i_ch]] = y;
    }
  }

  //now calculate the varaince depending on what sort of process you're dealing with
  //single exclusive channel
  if(cut != "total" && cut != "reac" && cut != "abscx"){                
    variance = cuts_and_noms[cut] * cuts_and_noms[cut] * cuts_and_covs[std::make_pair(cut,cut)];
  }

  //abscx - sum of abs and cex, include cov between the two
  else if(cut == "abscx"){
    variance = cuts_and_noms["abs"] * cuts_and_noms["abs"] * cuts_and_covs[std::make_pair("abs","abs")]
             + cuts_and_noms["cex"] * cuts_and_noms["cex"] * cuts_and_covs[std::make_pair("cex","cex")]
             + 2 * cuts_and_noms["abs"] * cuts_and_noms["cex"] * cuts_and_covs[std::make_pair("cex","abs")];        
  }
  // reac - sum of abs cex inel dcex and prod, include all their covariances
  else if(cut == "reac"){
    if(!use_reac){

      std::string reac_excl_ch[5] = {"abs","cex","inel","prod","dcex"};

      variance = 0;

      for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
          variance +=  cuts_and_noms[reac_excl_ch[i]] * cuts_and_noms[reac_excl_ch[j]] * cuts_and_covs[std::make_pair(reac_excl_ch[i],reac_excl_ch[j])];
        }
      }
    }// !use_reac
    else {
      variance = cuts_and_noms["reac"] * cuts_and_noms["reac"] * cuts_and_covs[std::make_pair("reac","reac")];
    }// use_reac
  }
  else if(cut == "total"){
    if(!use_reac){
      std::string reac_excl_ch[6] = {"abs","cex","inel","prod","dcex","elast"};
      variance = 0;
      for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
          variance +=  cuts_and_noms[reac_excl_ch[i]] * cuts_and_noms[reac_excl_ch[j]] * cuts_and_covs[std::make_pair(reac_excl_ch[i],reac_excl_ch[j])];
        }
      }
    }
    else {
      variance = cuts_and_noms["reac"] * cuts_and_noms["reac"] * cuts_and_covs[std::make_pair("reac","reac")]
               + cuts_and_noms["elast"] * cuts_and_noms["elast"] * cuts_and_covs[std::make_pair("elast","elast")]
               + 2*cuts_and_noms["elast"] * cuts_and_noms["reac"] * cuts_and_covs[std::make_pair("elast","reac")];
    }
  }
  else {
    std::cout << "Unrecognized process " << cut << " assuming 0 variance" << std::endl;
    return 0;
  }
  return variance;

}*/

double G4ReweightFitter::NewSigmaWithCov (double x, std::string cut, TMatrixD * cov, bool use_reac) {
  double variance = 0;
  //variances of individual channels as a fraction of nominal cross section

  //key is two strings that look up which element of the cov matrix they correspond to
  std::map<std::pair<std::string,std::string>, double> cuts_and_covs;
  //setup map storing names of excl channels and cov matrix elements
  for (size_t i_cut = 0; i_cut<theCovStore.size(); ++i_cut) {
    for (size_t j_cut = 0; j_cut<theCovStore.size(); ++j_cut) {
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
