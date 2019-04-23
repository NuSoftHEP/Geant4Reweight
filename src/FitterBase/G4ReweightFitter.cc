#include "G4ReweightFitter.hh"

#include <vector> 
#include <map>
#include <iomanip>
#include <sstream>

#include "TH1D.h"
#include "TCanvas.h"
#include "TVectorD.h"

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

void G4ReweightFitter::GetMCFromCurves(std::string TotalXSecFileName, std::string FracFileName, std::map< std::string, std::vector< FitParameter > > pars, bool fSave){

  TFile TotalXSecFile(TotalXSecFileName.c_str(), "OPEN");

  total_inel = (TGraph*)TotalXSecFile.Get("inel_momentum");

  std::map< std::string, TH1D* > FSHists;
  std::map< std::string, std::vector< FitParameter > >::iterator itPar;
  std::map< std::string, bool > CutIsDummy;
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


  TFile FracFile(FracFileName.c_str(), "OPEN");

  theReweighter = new G4Reweighter(&FracFile, FSHists, false);

  
  TGraph * total_var = theReweighter->GetTotalVariationGraph();

   
  for( auto itCut = cuts.begin(); itCut != cuts.end(); ++itCut ){

    std::string cut_name = *itCut;
    std::vector< double > xs,ys;

    if(cut_name == "reac"){
      for( int i = 0; i < total_inel->GetN(); ++i ){
        double x = total_inel->GetX()[i];
        double y = total_inel->GetY()[i];
        if( x > total_var->GetX()[ total_var->GetN() -1 ] ){
          break;
        }
        xs.push_back( x );
        ys.push_back( y * total_var->Eval( x ) ); 
      }   
      MC_xsec_graphs[ "reac" ] = new TGraph( xs.size(), &xs[0], &ys[0] );
    }
    else if( cut_name == "abscx" ){
      for( int i = 0; i < total_inel->GetN(); ++i ){
        double x = total_inel->GetX()[i];

        if( x > theReweighter->GetNewGraph( "abs" )->GetX()[ theReweighter->GetNewGraph( "abs" )->GetN() -1 ] ){
          break;
        }

        double y = total_inel->GetY()[i];
        xs.push_back( x );
        ys.push_back( y * ( theReweighter->GetNewGraph( "abs" )->Eval( x ) + theReweighter->GetNewGraph( "cex" )->Eval( x ) ) ); 
      }
      MC_xsec_graphs[ cut_name ] = new TGraph( xs.size(), &xs[0], &ys[0] ); 
    }
    else{

      for( int i = 0; i < total_inel->GetN(); ++i ){
        double x = total_inel->GetX()[i];
        if( x > theReweighter->GetNewGraph( cut_name )->GetX()[ theReweighter->GetNewGraph( cut_name )->GetN() -1 ] ){
          break;
        }
        double y = total_inel->GetY()[i];
        xs.push_back( x );
        ys.push_back( y * theReweighter->GetNewGraph( cut_name )->Eval( x ) ); 
      }
      MC_xsec_graphs[ cut_name ] = new TGraph( xs.size(), &xs[0], &ys[0] ); 
    }

    if( fSave ){
      fFitDir->cd();
      MC_xsec_graphs[ cut_name ]->Write(cut_name.c_str()); 
    }
  }

  std::map < std::string, TH1D *>::iterator it = 
    FSHists.begin();
  for( ; it != FSHists.end(); ++it ){
      delete it->second;
  }
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


double G4ReweightFitter::DoFit(bool fSave){

  double Chi2 = 0.;
  double Data_val, MC_val, Data_err;
  double x;

  //Go through each cut defined for the experiment
  std::map< std::string, TGraphErrors * >::iterator itXSec = Data_xsec_graphs.begin();
  for( itXSec; itXSec != Data_xsec_graphs.end(); ++itXSec ){
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
  fDataFile = new TFile( fDataFileName.c_str(), "READ"); 

  std::map< std::string, std::string >::iterator itGraphs;
  for( itGraphs = graph_names.begin(); itGraphs != graph_names.end(); ++itGraphs ){
    Data_xsec_graphs[ itGraphs->first ] = (TGraphErrors*)fDataFile->Get(itGraphs->second.c_str());
  }
}
