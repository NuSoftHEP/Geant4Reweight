#include "geant4reweight/FitterBase/G4ReweightFitManager.hh"
#include "geant4reweight/FitterBase/DUETFitter.hh"

#include "Math/Factory.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TPad.h"
#include "TMatrixD.h"
#include "TROOT.h"
#include "TVectorD.h"

#include <string>
#include <vector>

G4ReweightFitManager::G4ReweightFitManager(std::string & fOutFileName, bool do_save,
                                           G4ReweightManager * rw_manager,
                                           double total_xsec_bias)
  : fit_tree("FitTree", ""),
    fSave( do_save ),
    fRWManager(rw_manager) {
  out = new TFile( fOutFileName.c_str(), "RECREATE" );
  data_dir = out->mkdir( "Data" );
  nDOF = 0;
  fit_tree.Branch( "Chi2", &tree_chi2 );

  //scales contribution to chi2 from total cross section data
  total_mix = total_xsec_bias;
}

void G4ReweightFitManager::MakeFitParameters(std::vector< fhicl::ParameterSet > & FitParSets) {

  parMaker = G4ReweightParameterMaker( FitParSets, true );

  //TODO nDOF calculation no longer valid with new fit method
  nDOF -= parMaker.GetNParameters();
  theParVals = parMaker.GetParametersAsPairs();

  //add elastic parameters as well
  for(std::string::size_type i=0;i<parMaker.GetElasticParameterSet().size();i++){
    theElastParVals.push_back(std::pair(parMaker.GetElasticParameterSet().at(i).Name,parMaker.GetElasticParameterSet().at(i).Value));
  }

  for( size_t i = 0; i < theParVals.size(); ++i ){
    std::string branch_name = theParVals[i].first;
    parameter_values[ branch_name ] = 0.;
    fit_tree.Branch( branch_name.c_str(), &parameter_values.at( branch_name ), (branch_name + "/D").c_str() );
  }

  //get elastic parameter set as well
  for(size_t i = 0;i< theElastParVals.size(); ++i){
    std::string branch_name = theElastParVals[i].first;
    parameter_values[ branch_name ] = 0.;
    fit_tree.Branch( branch_name.c_str() , &parameter_values.at( branch_name ) , (branch_name + "/D").c_str() );
  }

}

void G4ReweightFitManager::DefineMCSets(std::vector< fhicl::ParameterSet > &MCSets) {
  for( size_t i = 0; i < MCSets.size(); ++i ){
    std::string theSet = MCSets[i].get<std::string>("Name");
    sets.push_back(theSet);
    mapSetsToFracs[theSet] = MCSets[i].get<std::string>("FSFile");
    mapSetsToMaterial[theSet] = MCSets[i].get<fhicl::ParameterSet>("Material");
  }
}

void G4ReweightFitManager::DefineExperiments(fhicl::ParameterSet &ps) {
    std::vector<fhicl::ParameterSet> exps =
        ps.get<std::vector<fhicl::ParameterSet>>("Experiments");

  for(size_t i = 0; i < exps.size(); ++i){
    std::string the_type = exps.at(i).get<std::string >("Type");
    if (IsSetActive(the_type)) {
      G4ReweightFitter * exp = new G4ReweightFitter(out, exps.at(i),
                                                    mapSetsToFracs[the_type],
                                                    parMaker,
                                                    mapSetsToMaterial[the_type],
                                                    fRWManager);
      mapSetsToFitters[exp->GetType()].push_back(exp);
    }
  }

  if( IsSetActive( "C_PiPlus" ) ){
    bool includeDUET = ps.get< bool >("IncludeDUET");
    std::string DUET_data = ps.get< std::string >( "DUETDataFile" );

    fhicl::ParameterSet DUET_set = ps.get<fhicl::ParameterSet>("DUETSet");
    DUETFitter * df = new DUETFitter(out, DUET_set, mapSetsToFracs["C_PiPlus"],
                                     parMaker, mapSetsToMaterial["C_PiPlus"],
                                     fRWManager);
    if( includeDUET ){
      mapSetsToFitters["C_PiPlus"].push_back( df );
    }
  }

  std::cout << "Done Getting Experiments" << std::endl;
}

void G4ReweightFitManager::GetAllData(){
  
  for( auto itSet = mapSetsToFitters.begin(); itSet != mapSetsToFitters.end(); ++itSet ){
    std::vector< G4ReweightFitter* > fitters = itSet->second;
    for( size_t i = 0; i < fitters.size(); ++i ){
      fitters[i]->LoadData();
      fitters[i]->SaveData(data_dir);
      nDOF += fitters[i]->GetNDOF();
      allFitters.push_back( fitters[i] );
    }
  }
}


void G4ReweightFitManager::DefineFCN(){

  theFCN = ROOT::Math::Functor(
      [&](double const *coeffs) {
          //if you use reac as a parameter, change this to true. this then gets used when deciding how many fit
          //params you have when computing the ndof
          bool use_reac=false;


          std::string dir_name = "";
          for(size_t i = 0; i < theParVals.size(); ++i){
            theParVals[i].second = coeffs[i];
            dir_name += theParVals[i].first + std::to_string( coeffs[i] );
            parameter_values[ theParVals[i].first ] = coeffs[i];
            if(theParVals[i].first == "fReac") use_reac=true;
          }

          //elast params get added onto end of parameter vector
          for(size_t i=0;i< theElastParVals.size(); ++i){
            theElastParVals[i].second = coeffs[i+theParVals.size()];
            parameter_values[ theElastParVals[i].first] = coeffs[i+theParVals.size() ];
            dir_name += theElastParVals[i].first + std::to_string( coeffs[i+theParVals.size()] );
          }


          //new method for setting params
          if(theElastParVals.size()) {
            parMaker.SetNewValsWithElast( theParVals , theElastParVals);
          }
          else {
            parMaker.SetNewVals( theParVals) ;
          }
          //////////////////////////////////////////////////////

          //Saving Extra Information//////////////////////////
          TDirectory * outdir = nullptr;
          if( fSave ){
            if( !out->Get( dir_name.c_str() ) ){
              outdir = out->mkdir( dir_name.c_str() );
            }
            else{
              outdir = (TDirectory*)out->Get( dir_name.c_str() );
            }
          }
          //////////////////////////////////////////////////////

          std::vector<Chi2Store> theStore;
          //zero everything at start of fit
          for(size_t i_dt=0;i_dt<AllExclChannels.size();i_dt++){
            Chi2Store thisStore(AllExclChannels.at(i_dt),0,0);
            theStore.push_back(thisStore);
          }


          double chi2 = 0.;
          double n = 0;
          //for each type of measurement, abs, elast, cex, reac, total etc.
          //store the num of points and the resulting chi2


          for(auto itSet = mapSetsToFitters.begin(); itSet != mapSetsToFitters.end(); ++itSet ){
            for( size_t i = 0; i < itSet->second.size(); ++i ){
              auto theFitter = itSet->second.at(i);
              std::string FracsFile = mapSetsToFracs[ itSet->first ];
              auto material = mapSetsToMaterial[itSet->first];

              if( fSave )
                theFitter->MakeFitDir( outdir );

              //get MC predictions
              theFitter->GetMCValsWithCov(parMaker, fSave);
              //perform fit
              theFitter->DoFitModified(fSave);

              for(size_t i_dt=0;i_dt<AllExclChannels.size();i_dt++){
                //Chi2Store object inside fitter stores n data points and chi2 for each exclusive channel fitted
                Chi2Store thisStore = theFitter->GetNDataPointsAndChi2(AllExclChannels.at(i_dt).c_str());
                theStore[i_dt].nPoints += thisStore.nPoints;
                theStore[i_dt].chi2 += thisStore.chi2;
              }
              theFitter->FinishUp();
            }
          }

          int p = 0; //num of model parameters process i_dt is senstive to

          //go through exclusive channels, calculate chi2 from each and num dof
          for(size_t i_dt=0;i_dt<AllExclChannels.size();i_dt++){

            p = GetNModelParam(theStore.at(i_dt).cut,use_reac);
            //make sure you don't divide by zero!
            if(theStore.at(i_dt).nPoints - p >0){
              double this_chi2 = theStore.at(i_dt).chi2/(theStore.at(i_dt).nPoints-p);

              //if this is a total cross section, multiply by bias parameter
              if(i_dt == 0){
                this_chi2 *= total_mix;
                n += total_mix;
              }
              else {
                n++;
              }

              chi2 += this_chi2;
            }
          }

          tree_chi2 = chi2/n;
          fit_tree.Fill();
          return chi2/n;
     },
     theParVals.size()+theElastParVals.size());
}




void G4ReweightFitManager::SaveFitTree(){
  out->cd();
  fit_tree.Write();
  out->Close();
}


void G4ReweightFitManager::RunFitAndSave( bool fFitScan ){

  TMatrixD *cov = new TMatrixD( theParVals.size() + theElastParVals.size() , theParVals.size() + theElastParVals.size() );
  //correlation matrix
  TMatrixD *corr = new TMatrixD( theParVals.size() + theElastParVals.size() , theParVals.size() + theElastParVals.size() );

  TH1D parsHist( "parsHist", ";; Value", theParVals.size()+theElastParVals.size() , 0 ,theParVals.size()+theElastParVals.size() );

  TH2D covHist( "covHist", "" , theParVals.size()+theElastParVals.size() , 0 , theParVals.size()+theElastParVals.size() , theParVals.size()+theElastParVals.size() , 0 , theParVals.size()+theElastParVals.size() );


  //Correlation matrix histogram
  TH2D corrHist( "corrHist" , "" , theParVals.size()+theElastParVals.size() , 0 , theParVals.size()+theElastParVals.size() , theParVals.size()+theElastParVals.size() , 0 , theParVals.size()+theElastParVals.size() );


  for( auto it = parMaker.GetParameterSet().begin(); it != parMaker.GetParameterSet().end(); ++it ){
    std::cout << it->first << std::endl;
    for( size_t i = 0; i < it->second.size(); ++i ){
      std::cout << "\t" << it->second.at(i).Name << " " << it->second.at(i).Value << std::endl;
    }
  }

  //Added by C Thorpe  print elastic params
  auto elastParam = parMaker.GetElasticParameterSet();
  for(std::string::size_type i=0;i<elastParam.size();i++){
    std::cout << elastParam.at(i).Cut << std::endl;
    std::cout << "\t" <<  elastParam.at(i).Name << " " << elastParam.at(i).Value <<   std::endl;
  }


  //OutlierScan();
  DefineFCN();

  if( !fFitScan ){
    std::cout << "Doing minimizing" << std::endl;
    fMinimizer->SetFunction( theFCN );
    std::cout << "function set" << std::endl;
    int fitstatus = fMinimizer->Minimize();
    std::cout << fMinimizer->MinValue() << std::endl;
    std::cout << "fitstatus: " << fitstatus << std::endl;
    if(!fitstatus){
      std::cout << "Failed to find minimum: " << std::endl;
    }
    else{
      std::vector< double > vals, errs;
      std::cout << "Found minimum: " << std::endl;

      for( size_t i = 0; i < theParVals.size() + theElastParVals.size(); ++i ){
        //add elastic params
        if(i < theParVals.size())
          std::cout << theParVals[i].first << " " << fMinimizer->X()[i] << std::endl;
        else
          std::cout << theElastParVals[i - theParVals.size() ].first << " " << fMinimizer->X()[i] << std::endl;


        vals.push_back( fMinimizer->X()[i] );
        errs.push_back( sqrt( fMinimizer->CovMatrix(i,i) ) );

        parsHist.SetBinContent( i+1, vals.back() );

        if(i < theParVals.size())    parsHist.GetXaxis()->SetBinLabel( i+1, theParVals[i].first.c_str() );
        else parsHist.GetXaxis()->SetBinLabel(i+1 ,   theElastParVals[i-theParVals.size()].first.c_str() );

        parsHist.SetBinError( i+1, errs.back() );

        if(i < theParVals.size()){
          covHist.GetXaxis()->SetBinLabel( i+1, theParVals[i].first.c_str() );
          corrHist.GetXaxis()->SetBinLabel( i+1, theParVals[i].first.c_str() );
        }
        else {
          covHist.GetXaxis()->SetBinLabel( i+1, theElastParVals[i-theParVals.size()].first.c_str() );
          corrHist.GetXaxis()->SetBinLabel( i+1, theElastParVals[i-theParVals.size()].first.c_str() );
        }


        if(i < theParVals.size()){
          covHist.GetYaxis()->SetBinLabel( i+1, theParVals[i].first.c_str() );
          corrHist.GetYaxis()->SetBinLabel( i+1, theParVals[i].first.c_str() );
        }
        else {
          corrHist.GetYaxis()->SetBinLabel( i+1, theElastParVals[i-theParVals.size()].first.c_str() );
          covHist.GetYaxis()->SetBinLabel( i+1, theElastParVals[i-theParVals.size()].first.c_str() );
        }

        for( size_t j = 0; j < theParVals.size() + theElastParVals.size(); ++j ){
          (*cov)(i,j) = fMinimizer->CovMatrix(i,j);
          (*corr)(i,j) = fMinimizer->Correlation(i,j);

          covHist.SetBinContent(i+1, j+1, fMinimizer->CovMatrix(i,j));
          corrHist.SetBinContent(i+1, j+1, fMinimizer->Correlation(i,j));
        }
      }
      DoScans();

      std::string dir_names[4] = {"MinusSigma", "BestFit", "PlusSigma", "Nominal"};

      for( int sigma_it = 0; sigma_it < 4; ++sigma_it ){
        //Setting parameters
        for( size_t i = 0; i < theParVals.size() + theElastParVals.size(); ++i ){
          int j = i - theParVals.size();
          //C Thorpe: Setting of min max curves now handled by separate method that uses covariance matrix
          //Set params to their central values here, pm 1 sigma added later
          if( sigma_it < 3 ){
            if(i<theParVals.size())
              theParVals[i].second = vals[i];
            else{
              theElastParVals[j].second = vals[i];
            }

          }
          else if( sigma_it == 3 ){
            if(i < theParVals.size())
              theParVals[i].second = 1.;
            else
              theElastParVals[j].second = 1.;
          }
        }

        if(theElastParVals.size())
          parMaker.SetNewValsWithElast( theParVals , theElastParVals );
        else
          parMaker.SetNewVals( theParVals );

        TDirectory * outdir = out->mkdir( dir_names[sigma_it].c_str() );

        for (auto itSet = mapSetsToFitters.begin();
             itSet != mapSetsToFitters.end(); ++itSet) {
          //for (size_t i = 0; i < itSet->second.size(); ++i) {
            auto theFitter = itSet->second.at(0/*i*/);

            theFitter->MakeFitDir( outdir );
            std::string position;
            //C Thorpe: this tells GetMCFromCurvesWithCovariance if you want +/- sigma or central val
            if(sigma_it == 0) position = "down";
            else if(sigma_it == 2) position = "up";
            else position = "CV";

            //new method to get MC predictions, supply cov matrix
            theFitter->GetMCValsWithCov(parMaker, true, &covHist, position, true, &AllExclChannels);
            theFitter->FinishUp();
          //}
        }
      }
      DrawFitResults();
      //DoScans();
    }
  }
  else{
    std::cout << "Doing scan" << std::endl;

    //Build the input to the FCN
    int total_steps = 1;

    std::vector< std::pair< std::string, FitParameter > > active_pars = parMaker.GetActiveParametersAsPairs();
    for( auto it = active_pars.begin(); it != active_pars.end(); ++it )
      total_steps *= it->second.ScanSteps;


    std::cout << "Factors: " << std::endl;
    std::vector< int > theFactors;
    for( size_t i = 0; i < active_pars.size(); ++i ){
      std::cout << i << " Steps: " << active_pars[i].second.ScanSteps << std::endl;
      if( i == 0 )
        theFactors.push_back( total_steps / active_pars[i].second.ScanSteps );
      else
        theFactors.push_back( theFactors[i - 1] / active_pars[i].second.ScanSteps );

      std::cout << theFactors.back() << std::endl;
    }

    std::cout << "Steps: " << std::endl;
    for( int z = 0; z < total_steps; ++z ){

      std::vector< int > input;
      std::vector< double > input_coeffs;
      for( size_t i = 0; i < theFactors.size(); ++i ){

        int val = z;

        for( size_t j = 0; j < i; ++j )
          val -= ( theFactors[j] * input[j] );

        input.push_back( val / theFactors[i] );
        input_coeffs.push_back( active_pars[i].second.ScanStart + ( input.back() * active_pars[i].second.ScanDelta ) );

        std::cout << input.back() << " " << input_coeffs.back() << " ";
      }
      std::cout << std::endl;

      std::cout << "FCN output: " << z << " " << theFCN(&input_coeffs[0]) << std::endl;
    }

  }


  out->cd();
  fit_tree.Write();
  cov->Write( "FitCovariance" );
  corr->Write( "FitCorrelation" );
  covHist.Write( "FitCovHist" );
  corrHist.Write( "FitCorrHist" );
  parsHist.Write();
  out->Close();
  std::cout << "Done" << std::endl;


}

void G4ReweightFitManager::MakeMinimizer( fhicl::ParameterSet & ps ){
  fMinimizer = std::unique_ptr<ROOT::Math::Minimizer>
    ( ROOT::Math::Factory::CreateMinimizer( "Minuit2", "Migrad" ) );


  //Configure
  fMinimizer->SetMaxFunctionCalls( ps.get< int >("MaxCalls") );
  fMinimizer->SetTolerance( ps.get< double >("Tolerance") );
  double LowerLimit = ps.get< double >("LowerLimit");
  double UpperLimit = ps.get< double >("UpperLimit");

  for( size_t i = 0; i < theParVals.size(); ++i ){
    fMinimizer->SetVariable( i, theParVals[i].first.c_str(), theParVals[i].second, 0.01 );
    fMinimizer->SetVariableLimits( i, LowerLimit, UpperLimit );
  }

  for(size_t i=0;i< theElastParVals.size(); ++i){
    fMinimizer->SetVariable( i + theParVals.size(), theElastParVals[i].first.c_str() , theElastParVals[i].second , 0.01);
    fMinimizer->SetVariableLimits( i + theParVals.size() , LowerLimit , UpperLimit );
  }
}

void G4ReweightFitManager::DrawFitResults(){

  std::vector<std::string> types;
  for(auto itSets = mapSetsToFitters.begin(); itSets != mapSetsToFitters.end();
      ++itSets){
    types.push_back(itSets->first);
  }
  //add elastic parameters
  std::map< std::string, std::string > titles = {
    {"reac", "Reaction"},
    {"inel", "Quasielastic"},
    {"abs", "Absorption"},
    {"cex", "Charge Exchange"},
    {"abscx", "Abs. + Ch. Ex."},
    {"total", "Total"},
    {"elast", "Elastic"}
  };

  gROOT->SetBatch(1);

  TDirectory * fitdir = (TDirectory*)out->mkdir("Fit");
  fitdir->cd();

  TLegend * leg = nullptr;
  for( auto itType = types.begin(); itType != types.end(); ++itType ){

    TDirectory * typedir = (TDirectory*)fitdir->mkdir( (*itType).c_str() );
    typedir->cd();

    std::vector< std::string > all_cuts, dirs;

    std::map< std::string, std::vector< TGraphErrors * > > all_data;

    TList * data_keys = data_dir->GetListOfKeys();
    for( int i = 0; i < data_keys->GetSize(); ++i ){

      std::string data_name = data_keys->At(i)->GetName();
      if( data_name.find( *itType ) == std::string::npos ) continue;
      TDirectory * sub_dir = (TDirectory*)data_dir->Get( data_name.c_str() );

      TList * exp_keys = sub_dir->GetListOfKeys();
      std::vector< std::string > data_graph_names;
      for( int j = 0; j < exp_keys->GetSize(); ++j ){
        std::string key_name = exp_keys->At(j)->GetName();
        //  std::cout << "key_name: " << key_name << std::endl;
        //Excluding Covariance from DUET
        if( key_name.find( "cov" ) != std::string::npos )
          continue;

        all_data[ key_name ].push_back( (TGraphErrors*)sub_dir->Get( key_name.c_str() ) );

        bool found = false;
        for( size_t k = 0; k < all_cuts.size(); ++k ){
          if( all_cuts[k] == key_name ){
            found = true;
            break;
          }
        }

        if( !found ){
          all_cuts.push_back( key_name );
          dirs.push_back( data_name );
        }
      }

    }


    //for( size_t i = 0; i < all_cuts.size(); ++i ){
    //  std::string cut_name = all_cuts[i];
    for( size_t i = 0; i < AllExclChannels.size(); ++i ){
      std::string cut_name = AllExclChannels[i];
      std::cout << "cut name " << cut_name << std::endl;
      double max = 0.;
      std::vector< TGraphErrors * > data_vec = all_data[ cut_name ];
      std::cout << "Data vec size: " << data_vec.size() << std::endl;
      for( size_t j = 0; j < data_vec.size(); ++j ){

        TGraphErrors * data_gr = data_vec[j];
        data_gr->SetMarkerColor(1);
        data_gr->SetMarkerStyle(20);
        for( int k = 0; k < data_gr->GetN(); ++k ){
          double val;
          val = data_gr->GetY()[k] + data_gr->GetEY()[k];
          if( val > max) max = val;
        }
      }

      //PlusSigma
      std::string plus_name = "PlusSigma/" + mapSetsToFitters[*itType].at(0)->GetName() + "/" + cut_name;
      TGraph * plus_sigma  = (TGraph*)out->Get( plus_name.c_str() );

      double old_x = plus_sigma->GetX()[0];
      double old_y = plus_sigma->GetY()[0];
      plus_sigma->InsertPointBefore(1, old_x, old_y);
      plus_sigma->SetPoint(0, old_x/*-.001*/, 0.);

      old_x = plus_sigma->GetX()[ plus_sigma->GetN() - 1 ];
      old_y = plus_sigma->GetY()[ plus_sigma->GetN() - 1 ];
      plus_sigma->InsertPointBefore( (plus_sigma->GetN() - 1), old_x, old_y );
      plus_sigma->SetPoint( (plus_sigma->GetN() - 1), old_x/*+.001*/, 0. );

      for( int j = 0; j < plus_sigma->GetN(); ++j ){
        if( plus_sigma->GetY()[j] > max ) max = plus_sigma->GetY()[j];
      }

      //Nominal
      std::string nominal_name = "Nominal/" + mapSetsToFitters[*itType].at(0)->GetName() + "/" + cut_name;
      TGraph * nominal = (TGraph*)out->Get( nominal_name.c_str() );
      for( int j = 0; j < nominal->GetN(); ++j ){
        if( nominal->GetY()[j] > max ) max = nominal->GetY()[j];
      }
      ///////////


      //MinusSigma
      std::string minus_name = "MinusSigma/" + mapSetsToFitters[*itType].at(0)->GetName() + "/" + cut_name;
      TGraph * minus_sigma  = (TGraph*)out->Get( minus_name.c_str() );

      old_x = minus_sigma->GetX()[0];
      old_y = minus_sigma->GetY()[0];
      minus_sigma->InsertPointBefore(1, old_x, old_y);
      minus_sigma->SetPoint(0, old_x/*-.001*/, 0.);

      old_x = minus_sigma->GetX()[ minus_sigma->GetN() - 1 ];
      old_y = minus_sigma->GetY()[ minus_sigma->GetN() - 1 ];
      minus_sigma->InsertPointBefore( (minus_sigma->GetN() - 1), old_x, old_y );
      minus_sigma->SetPoint( (minus_sigma->GetN() - 1), old_x/*+.001*/, 0. );
      ////////////



      TCanvas c1("c1","c1", 500, 400);

      c1.SetTickx(1);
      c1.SetTicky(1);
      c1.SetBottomMargin( c1.GetBottomMargin() * 1.1 );
      c1.SetLeftMargin( c1.GetLeftMargin() * 1.1 );

      plus_sigma->SetFillColor(2);
      plus_sigma->SetMaximum( 1.2 * max );
      plus_sigma->SetMinimum(0.);
      std::string title = titles[cut_name] + ";Pion Momentum (MeV/c);#sigma (mb)";
      plus_sigma->SetTitle( title.c_str() );
      plus_sigma->GetXaxis()->SetTitleSize(.06);
      plus_sigma->GetXaxis()->SetTitleOffset(.75);
      plus_sigma->GetYaxis()->SetTitleSize(.06);
      plus_sigma->GetYaxis()->SetTitleOffset(.75);
      plus_sigma->GetXaxis()->SetLabelSize(.04);
      plus_sigma->GetYaxis()->SetLabelSize(.04);

      minus_sigma->SetFillColor(0);

      plus_sigma->GetXaxis()->SetRangeUser(0., old_x);
      plus_sigma->Draw("AF");
      minus_sigma->Draw("F same");

      //BestFit
      std::string best_fit_name = "BestFit/" + mapSetsToFitters[*itType].at(0)->GetName() + "/" + cut_name;
      TGraph * best_fit = (TGraph*)out->Get( best_fit_name.c_str() );
      best_fit->SetLineStyle(10);
      best_fit->SetLineColor(1);
      best_fit->Draw("same");
      best_fit->SetFillColor(kRed);

      nominal->SetLineStyle(10);
      nominal->SetLineColor(4);
      nominal->Draw("same");

      for( size_t j = 0; j < data_vec.size(); ++j ){
        data_vec[j]->Draw( "P same" );
        if (!leg) {
          leg = new TLegend();
          leg->AddEntry(data_vec[j], "Data", "l");
          leg->AddEntry(best_fit, "Best Fit #pm1#sigma", "lf");
          leg->AddEntry(nominal, "Nominal", "l");
          leg->Write("legend");
        }
      }


      gPad->RedrawAxis();

      typedir->cd();
      c1.Write( cut_name.c_str() );
    }
  }

  delete leg;
  TVectorD theChi2(3);
  theChi2[0] = fMinimizer->MinValue();
  //TODO nDOF calculation no longer valid, needs updating
  theChi2[1] = nDOF;
  theChi2[2] = fMinimizer->MinValue() / nDOF;
  std::cout << "Minimum Chi-squared: " << fMinimizer->MinValue() << std::endl;

  out->cd("Fit");
  theChi2.Write( "Chi2" );

}

void G4ReweightFitManager::DoScans() {
  TDirectory * scans = (TDirectory *)out->mkdir("Scans");
  scans->cd();

  std::cout << "Scanning parameters" << std::endl;
  size_t total_parameters = theParVals.size()+theElastParVals.size();

  unsigned int fNScanSteps = 100;
  double * x = new double[fNScanSteps] {};
  double * y = new double[fNScanSteps] {};
  for (size_t i = 0; i < total_parameters; ++i) {
    std::cout << "\tParameter " << fMinimizer->VariableName(i) << std::endl;
    bool scanned = fMinimizer->Scan(i, fNScanSteps, x, y);
    if (scanned) {
      TGraph gr(fNScanSteps - 1, x, y);
      std::string title_name = /*"#chi^{2} Scan: " + fMinimizer->VariableName(i)
                               + */";Parameter Value;#chi^{2}";
      gr.SetTitle(title_name.c_str());
      gr.GetXaxis()->SetTitleSize(.04);
      gr.GetYaxis()->SetTitleSize(.04);
      gr.SetLineColor(kBlue);
      gr.SetLineWidth(2);
      gr.Write(fMinimizer->VariableName(i).c_str());
    }
  }

  delete[] x;
  delete[] y;

}
