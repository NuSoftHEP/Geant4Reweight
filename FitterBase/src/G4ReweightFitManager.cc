#include "G4ReweightFitManager.hh"
#include "DUETFitter.hh"
#include "TVectorD.h"
#include "TTree.h"
#include "TMatrixD.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TROOT.h"

G4ReweightFitManager::G4ReweightFitManager(std::string & fOutFileName, bool do_save) : 
  fit_tree("FitTree", ""),
  fSave( do_save )
{
  out = new TFile( fOutFileName.c_str(), "RECREATE" );
  data_dir = out->mkdir( "Data" );
  nDOF = 0;
  fit_tree.Branch( "Chi2", &tree_chi2 );
}

void G4ReweightFitManager::MakeFitParameters( std::vector< fhicl::ParameterSet > & FitParSets ){
  for( size_t i = 0; i < FitParSets.size(); ++i ){
    fhicl::ParameterSet theSet = FitParSets.at(i);
    std::string theCut = theSet.get< std::string >("Cut");

    if( FullParameterSet.find( theCut ) == FullParameterSet.end() ){  
      FullParameterSet[ theCut ] = std::vector< FitParameter  >();
    }   

    bool isDummy = theSet.get< bool >("Dummy");
    if( isDummy ){
      FitParameter dummyPar;
      dummyPar.Name = "dummy";
      dummyPar.Cut = theCut;
      dummyPar.Value = 1.; 
      dummyPar.Range = std::make_pair( 0., 0.);
      dummyPar.Dummy = true;
          
      FullParameterSet[ theCut ].push_back( dummyPar );
      CutIsDummy[ theCut ] = true;
    }   
    else{ 
      std::cout << "Making parameters for " << theCut << std::endl;
        
      std::vector< fhicl::ParameterSet > theParameters = theSet.get< std::vector< fhicl::ParameterSet > >("Parameters");
      for( size_t j = 0; j < theParameters.size(); ++j ){
        fhicl::ParameterSet thePar = theParameters.at(j);


        std::string theName = thePar.get< std::string >("Name");
        std::cout << theName << std::endl;
            
        std::pair< double, double > theRange = thePar.get< std::pair< double, double > >("Range");
        std::cout << "Range Low: " << theRange.first << " High: " << theRange.second << std::endl;

        double nominal = thePar.get< double >("Nominal",1.);

        FitParameter par;
        par.Name = theName;
        par.Cut = theCut;
        par.Dummy = false;
        par.Value = nominal; 
        par.Range = theRange;
        FullParameterSet[ theCut ].push_back( par );
        thePars.push_back( par.Name );
        theVals.push_back( nominal );

        double scan_start = thePar.get< double >("ScanStart", 1.);
        int    nsteps =     thePar.get< int >("NScanSteps", 10);
        double scan_delta = thePar.get< double >("ScanDelta", .1);

        theScanStarts.push_back( scan_start );
        theScanSteps.push_back( nsteps );
        theScanDeltas.push_back( scan_delta );



        //Remove 1 DOF for each non-dummy parameter
        --nDOF;
      }   
      CutIsDummy[ theCut ] = false;
    }   
  } 

  for( int i = 0; i < thePars.size(); ++i ){
    std::string branch_name = thePars.at(i);
    parameter_values[ branch_name ] = 0.;
    fit_tree.Branch( branch_name.c_str(), &parameter_values.at( branch_name ), (branch_name + "/D").c_str() );   
  }

}

void G4ReweightFitManager::DefineMCSets( std::vector< fhicl::ParameterSet > &MCSets ){

  for( size_t i = 0; i < MCSets.size(); ++i ){
    std::string theSet = MCSets[i].get< std::string >("Name");
    sets.push_back( theSet );
    mapSetsToFracs[ theSet ] = MCSets[i].get< std::string >( "FSFile" );
    mapSetsToNominal[ theSet ] = MCSets[i].get< std::string >( "File" );
  }

}

void G4ReweightFitManager::DefineExperiments( fhicl::ParameterSet &ps){
  std::vector< fhicl::ParameterSet > exps = ps.get< std::vector< fhicl::ParameterSet > >("Experiments");
  std::cout << "Getting Experiments: "  << exps.size() << std::endl;
  for(size_t i = 0; i < exps.size(); ++i){
    std::cout << std::endl;

    if( IsSetActive( exps.at(i).get<std::string >( "Type" ) ) ){
      G4ReweightFitter * exp = new G4ReweightFitter(out, exps.at(i));
      mapSetsToFitters[ exp->GetType() ].push_back( exp );
    }
  }

  if( IsSetActive( "C_PiPlus" ) ){
    bool includeDUET = ps.get< bool >("IncludeDUET");
    std::string DUET_data = ps.get< std::string >( "DUETDataFile" ); 
    DUETFitter * df = new DUETFitter(out, DUET_data);
    if( includeDUET ){
      mapSetsToFitters["C_PiPlus"].push_back( df );
    }
  }
}

void G4ReweightFitManager::GetAllData(){
  std::map< std::string, std::vector< G4ReweightFitter* > >::iterator
    itSet = mapSetsToFitters.begin();
  for( ; itSet != mapSetsToFitters.end(); ++itSet ){

    std::cout << "Loading Data for Set: " << itSet->first << std::endl;;
    std::vector< G4ReweightFitter* > fitters = itSet->second;
    for( size_t i = 0; i < fitters.size(); ++i ){
      fitters[i]->LoadData();
      fitters[i]->SaveData(data_dir);
      std::cout << "NDOF from Set: " << fitters[i]->GetNDOF() << std::endl;
      nDOF += fitters[i]->GetNDOF();
      allFitters.push_back( fitters[i] );
    }
  }

}

void G4ReweightFitManager::DefineFCN(/*bool fSave*/){
  theFCN = ROOT::Math::Functor(
      [&](double const *coeffs) {
        
        std::string dir_name = "";

        int a = 0;
        //Setting parameters
        for(size_t i = 0; i < thePars.size(); ++i){

          std::map< std::string, std::vector< FitParameter > >::iterator it;
          for( it = FullParameterSet.begin(); it != FullParameterSet.end(); ++it ){
            for( size_t j = 0; j < it->second.size(); ++j ){
              if( it->second[j].Name == thePars[i] ){
                it->second[j].Value = coeffs[a];
                dir_name += thePars[i] + std::to_string( coeffs[a] );

                parameter_values[ thePars[i] ] = coeffs[a];

                ++a;
              }
            }
          }   
        }

        TDirectory * outdir;
        if( fSave ){
          if( !out->Get( dir_name.c_str() ) ){
            outdir = out->mkdir( dir_name.c_str() );
          }
          else{
            outdir = (TDirectory*)out->Get( dir_name.c_str() );
          }
        }
  
        double chi2 = 0.;

	std::map< std::string, std::vector< G4ReweightFitter* > >::iterator
          itSet = mapSetsToFitters.begin();
    
        for( ; itSet != mapSetsToFitters.end(); ++itSet ){
          for( size_t i = 0; i < itSet->second.size(); ++i ){
            auto theFitter = itSet->second.at(i); 
    
            std::string NominalFile = mapSetsToNominal[ itSet->first ];
            std::string FracsFile = mapSetsToFracs[ itSet->first ];
    
            if( fSave )
              theFitter->MakeFitDir( outdir );

            theFitter->GetMCFromCurves( NominalFile, FracsFile, FullParameterSet, fSave);
            double fit_chi2 = theFitter->DoFit(fSave);
    
            chi2 += fit_chi2;
    
            theFitter->FinishUp();
    
          }
        }	
	
        tree_chi2 = chi2;
        fit_tree.Fill();
        return chi2;
      },
      thePars.size() 
    );
}

void G4ReweightFitManager::RunFitAndSave( bool fFitScan/*, bool fSave*/ ){

  TMatrixD *cov = new TMatrixD( thePars.size(), thePars.size() );
  TH1D parsHist("parsHist", "", thePars.size(), 0,thePars.size());
  TH2D covHist("covHist", "", thePars.size(), 0,thePars.size(), thePars.size(), 0,thePars.size());

  std::cout << "Start: " << std::endl;
  std::map< std::string, std::vector< FitParameter > >::iterator it;
  for( it = FullParameterSet.begin(); it != FullParameterSet.end(); ++it ){
    std::cout << it->first << std::endl;
    for( size_t i = 0; i < it->second.size(); ++i ){
      std::cout << "\t" << it->second.at(i).Name << " " << it->second.at(i).Value << std::endl;
    }
  }


  DefineFCN(/*fSave*/);


  if( !fFitScan ){
    std::cout << "Doing minimizing" << std::endl;

    fMinimizer->SetFunction( theFCN );
    int fitstatus = fMinimizer->Minimize();



    std::cout << "fitstatus: " << fitstatus << std::endl;
    if( !fitstatus ){
      std::cout << "Failed to find minimum: " << std::endl;
    }
    else{
      std::vector< double > vals, errs;
      std::cout << "Found minimum: " << std::endl;    
      for( size_t i = 0; i < thePars.size(); ++i ){
        std::cout << thePars[i] << " " << fMinimizer->X()[i] << std::endl;

        vals.push_back( fMinimizer->X()[i] );
        errs.push_back( sqrt( fMinimizer->CovMatrix(i,i) ) );

        parsHist.SetBinContent( i+1, vals.back() );
        parsHist.GetXaxis()->SetBinLabel( i+1, thePars[i].c_str() );
        parsHist.SetBinError( i+1, errs.back() );

        covHist.GetXaxis()->SetBinLabel( i+1, thePars[i].c_str() );
        covHist.GetYaxis()->SetBinLabel( i+1, thePars[i].c_str() );


        for( size_t j = 0; j < thePars.size(); ++j ){
          (*cov)(i,j) = fMinimizer->CovMatrix(i,j);
          covHist.SetBinContent(i+1, j+1, fMinimizer->CovMatrix(i,j));
        }
      }


      std::string dir_names[4] = {"MinusSigma", "BestFit", "PlusSigma", "Nominal"};

      for( int sigma_it = 0; sigma_it < 4; ++sigma_it ){
        //Setting parameters
        for(size_t i = 0; i < thePars.size(); ++i){

          std::map< std::string, std::vector< FitParameter > >::iterator it;
          for( it = FullParameterSet.begin(); it != FullParameterSet.end(); ++it ){
            for( size_t j = 0; j < it->second.size(); ++j ){
              if( it->second[j].Name == thePars[i] ){
                if( sigma_it < 3 )
                  it->second[j].Value = vals[i] + (sigma_it - 1)*errs[i];
                else if( sigma_it == 3 )
                   it->second[j].Value = 1.;               
              }
            }
          }   
        }


        TDirectory * outdir = out->mkdir( dir_names[sigma_it].c_str() );

        std::map< std::string, std::vector< G4ReweightFitter* > >::iterator
          itSet = mapSetsToFitters.begin();
        
        for( ; itSet != mapSetsToFitters.end(); ++itSet ){
          for( size_t i = 0; i < itSet->second.size(); ++i ){
            auto theFitter = itSet->second.at(i); 
        
            std::string NominalFile = mapSetsToNominal[ itSet->first ];
            std::string FracsFile = mapSetsToFracs[ itSet->first ];
        
            theFitter->MakeFitDir( outdir );
            theFitter->GetMCFromCurves( NominalFile, FracsFile, FullParameterSet, true);
        
            theFitter->FinishUp();
        
          }
        }	
      }

      DrawFitResults();
    }
  }
  else{
   std::cout << "Doing scan" << std::endl;
   
   //Build the input to the FCN
   int total_steps = 1;
   for( size_t i = 0; i < theScanSteps.size(); ++i )
     total_steps *= theScanSteps[i];

   std::cout << "Factors: " << std::endl;
   std::vector< int > theFactors;
   for( size_t i = 0; i < theScanSteps.size(); ++i ){
     std::cout << i << " Steps: " << theScanSteps[i] << std::endl;
     if( i == 0 )
       theFactors.push_back( total_steps / theScanSteps[i] );
     else
       theFactors.push_back( theFactors[i - 1] / theScanSteps[i] );

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
       input_coeffs.push_back( theScanStarts[i] + ( input.back() * theScanDeltas[i] ) );

       std::cout << input.back() << " " << input_coeffs.back() << " ";
     }
     std::cout << std::endl;

     std::cout << "FCN output: " << z << " " << theFCN(&input_coeffs[0]) << std::endl; 
   }

  }

  
  out->cd();
  fit_tree.Write();
  cov->Write( "FitCovariance" );
  covHist.Write( "FitCovHist" );
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
  
  for( size_t i = 0; i < thePars.size(); ++i ){
    fMinimizer->SetVariable( i, thePars[i].c_str(), theVals[i], 0.1 );
    fMinimizer->SetVariableLimits( i, LowerLimit, UpperLimit );
  }

}

void G4ReweightFitManager::DrawFitResults(){

  std::vector< std::string > types;
  for( auto itSets = mapSetsToFitters.begin(); itSets != mapSetsToFitters.end(); ++itSets ){
    types.push_back( itSets->first );
  }

  std::map< std::string, std::string > titles = {
    {"reac", "Reactive"},
    {"inel", "Quasi-Elastic"},
    {"abs", "Absorption"},
    {"cex", "Charge Exchange"},
    {"abscx", "Abs + CEx"}
  };

  gROOT->SetBatch(1);

  TDirectory * fitdir = (TDirectory*)out->mkdir("Fit"); 
  fitdir->cd();

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

    for( size_t i = 0; i < all_cuts.size(); ++i ){
      std::string cut_name = all_cuts[i];
     
      double max = 0.;     
      std::vector< TGraphErrors * > data_vec = all_data[ cut_name ];
      for( size_t j = 0; j < data_vec.size(); ++j ){

        TGraphErrors * data_gr = data_vec[j];
        data_gr->SetMarkerColor(1);
        data_gr->SetMarkerStyle(20);
        
        for( int k = 0; k < data_gr->GetN(); ++k ){
          double val = data_gr->GetY()[k] + data_gr->GetEY()[k];
          if( val > max) max = val;
        }
      }

      //PlusSigma
      std::string plus_name = "PlusSigma/" + dirs[i] + "/" + cut_name;
      TGraph * plus_sigma  = (TGraph*)out->Get( plus_name.c_str() );

      double old_x = plus_sigma->GetX()[0];
      double old_y = plus_sigma->GetY()[0];
      plus_sigma->InsertPointBefore(1, old_x, old_y);
      plus_sigma->SetPoint(0, old_x-.001, 0.);

      old_x = plus_sigma->GetX()[ plus_sigma->GetN() - 1 ];
      old_y = plus_sigma->GetY()[ plus_sigma->GetN() - 1 ];
      plus_sigma->InsertPointBefore( (plus_sigma->GetN() - 1), old_x, old_y );
      plus_sigma->SetPoint( (plus_sigma->GetN() - 1), old_x+.001, 0. );

      for( int j = 0; j < plus_sigma->GetN(); ++j ){
        if( plus_sigma->GetY()[j] > max ) max = plus_sigma->GetY()[j];
      }

      ////////////

      //Nominal
      std::string nominal_name = "Nominal/" + dirs[i] + "/" + cut_name;
      TGraph * nominal = (TGraph*)out->Get( nominal_name.c_str() );
      for( int j = 0; j < nominal->GetN(); ++j ){
        if( nominal->GetY()[j] > max ) max = nominal->GetY()[j];
      }
      ///////////



      //MinusSigma
      std::string minus_name = "MinusSigma/" + dirs[i] + "/" + cut_name;
      TGraph * minus_sigma  = (TGraph*)out->Get( minus_name.c_str() );

      old_x = minus_sigma->GetX()[0];
      old_y = minus_sigma->GetY()[0];
      minus_sigma->InsertPointBefore(1, old_x, old_y);
      minus_sigma->SetPoint(0, old_x-.001, 0.);

      old_x = minus_sigma->GetX()[ minus_sigma->GetN() - 1 ];
      old_y = minus_sigma->GetY()[ minus_sigma->GetN() - 1 ];
      minus_sigma->InsertPointBefore( (minus_sigma->GetN() - 1), old_x, old_y );
      minus_sigma->SetPoint( (minus_sigma->GetN() - 1), old_x+.001, 0. );
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

      plus_sigma->Draw("AF");
      minus_sigma->Draw("F same");

      //BestFit
      std::string best_fit_name = "BestFit/" + dirs[i] + "/" + cut_name;
      TGraph * best_fit = (TGraph*)out->Get( best_fit_name.c_str() );
      best_fit->SetLineStyle(10);
      best_fit->SetLineColor(1);
      best_fit->Draw("same");

      nominal->SetLineStyle(10);
      nominal->SetLineColor(4);
      nominal->Draw("same");

      for( size_t j = 0; j < data_vec.size(); ++j ){
        data_vec[j]->Draw( "P same" );
      }

      gPad->RedrawAxis();

      typedir->cd();
      c1.Write( cut_name.c_str() );

    }
  }

  TVectorD theChi2(3);
  theChi2[0] = fMinimizer->MinValue();
  theChi2[1] = nDOF;
  theChi2[2] = fMinimizer->MinValue() / nDOF;
  std::cout << "Minimum Chi-squared: " << fMinimizer->MinValue() << std::endl;

  out->cd("Fit");
  theChi2.Write( "Chi2" );

}
