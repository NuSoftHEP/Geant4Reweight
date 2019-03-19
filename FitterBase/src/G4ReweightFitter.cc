#include "G4ReweightFitter.hh"
#include "G4ReweightInter.hh"

#include <vector> 
#include <map>
#include <iomanip>
#include <sstream>

#include "TH1D.h"
#include "TCanvas.h"
#include "TVectorD.h"

G4ReweightFitter::G4ReweightFitter( TFile * output_file, fhicl::ParameterSet exp ){
  fOutputFile = output_file;

  fExperimentName = exp.get< std::string >("Name");
  std::vector< std::pair< std::string, std::vector< double > > > temp_points;
  temp_points = exp.get< std::vector< std::pair< std::string, std::vector< double > > > >("Points");
  points = std::map< std::string, std::vector< double > >( temp_points.begin(), temp_points.end() );
  
  std::cout << "Points:" << std::endl;

  for( std::map< std::string, std::vector< double > >::iterator ip = points.begin(); ip != points.end(); ++ip ){
    std::cout << ip->first << " ";
    for( size_t i = 0; i < (ip->second).size(); ++i){
      std::cout << (ip->second).at(i) << " ";
    }
    std::cout << std::endl;
  }

  std::vector< std::pair< std::string, std::string > > temp_cuts = exp.get< std::vector< std::pair<std::string, std::string> > >("Cuts");
  cuts = std::map< std::string, std::string >( temp_cuts.begin(), temp_cuts.end() );

  std::cout << "Cuts: " << std::endl;
  for( std::map< std::string, std::string >::iterator it = cuts.begin(); it != cuts.end(); ++it ){ 
    std::cout << it->first << " " << it->second << std::endl;
  }

  std::vector< std::pair< std::string, std::string > > temp_graph_names = exp.get< std::vector< std::pair<std::string, std::string> > >("Graphs");
  graph_names = std::map< std::string, std::string >( temp_graph_names.begin(), temp_graph_names.end() );

  fDataFileName = exp.get< std::string >("Data");

  double thickness = exp.get< double >("Thickness");
  double mass      = exp.get< double >("Mass");
  double density   = exp.get< double >("Density");

  scale = 1.e27 / (thickness * density * 6.022e23 / mass);

  std::vector< std::string > bin_param = exp.get< std::vector< std::string > >("Binning");
  binning = "(" + bin_param[0] + "," + bin_param[1] + "," + bin_param[2] + ")"; 

  type = exp.get< std::string >("Type");
  BuildCuts();
  
  std::cout << "Cuts: " << std::endl;
  for( std::map< std::string, std::string >::iterator it = cuts.begin(); it != cuts.end(); ++it ){ 
    std::cout << it->first << " " << it->second << std::endl;
  }

  double dummyX = 0.;
  double dummyY = 1.;

  dummyGraph = new TGraph(1, &dummyX, &dummyY );


}

void G4ReweightFitter::GetMCFromCurves(std::string TotalXSecFileName, std::string FracFileName, std::map< std::string, std::vector< FitParameter > > pars){
  
  //Remove me
  TFile tryout("try.root", "RECREATE");

  TFile TotalXSecFile(TotalXSecFileName.c_str(), "OPEN");

  total_inel = (TGraph*)TotalXSecFile.Get("inel_momentum");
  tryout.cd();

  total_inel->Write("inel_momentum");

  std::map< std::string, TGraph* > FSGraphs;
  std::map< std::string, std::vector< FitParameter > >::iterator itPar;
  for( itPar = pars.begin(); itPar != pars.end(); ++itPar ){
    std::string name = itPar->first;  
    std::cout << "Cut: " << name << std::endl;
    
    bool isDummy = false;

    std::vector< std::pair< double, double > > vars;
    std::vector< double > varX, varY; 

    for( size_t i = 0; i < itPar->second.size(); ++i ){
      
      if( itPar->second.at( i ).Dummy ){
        std::cout << "Dummy" << std::endl;
        FSGraphs[name] = dummyGraph;
        std::cout << "Dummy N: " << dummyGraph->GetN() << std::endl;
        isDummy = true;
        break;
      }
  
      else{        
        double value = itPar->second.at( i ).Value;
        std::pair< double, double > range = itPar->second.at( i ).Range;

        vars.push_back( std::make_pair( range.first,  value ) );
        vars.push_back( std::make_pair( range.second, value ) );

        varX.push_back( range.first );
        varX.push_back( range.second );
        varY.push_back( value );
        varY.push_back( value );
      }
  
    }
    
    if( !isDummy ){
      for( size_t i = 0; i < vars.size(); ++i ){
        std::cout << vars.at(i).first << " " << vars.at(i).second << std::endl;
      }

      FSGraphs[name] = new TGraph(varX.size(), &varX[0], &varY[0]);
    }
  }


  TFile FracFile(FracFileName.c_str(), "OPEN");

  double max = 2000., min = 10.;
  theFS = new G4ReweightFinalState(&FracFile, FSGraphs, max, min, false);

  
  tryout.cd();
  TGraph * total_var = theFS->GetTotalVariationGraph();
  total_var->Write("TotalVar");
  for( std::map<std::string, TGraph*>::iterator itGr = FSGraphs.begin(); 
    itGr != FSGraphs.end(); ++itGr ){

    theFS->GetOldGraph( itGr->first )->Write( ("Old" + itGr->first).c_str() );
    theFS->GetNewGraph( itGr->first )->Write( ("New" + itGr->first).c_str() );
    theFS->GetExclusiveVariationGraph( itGr->first )->Write( ("Var" + itGr->first ).c_str() );
  }


  std::map< std::string, std::string >::iterator itCut = cuts.begin();
  for( itCut; itCut != cuts.end(); ++itCut ){
    tryout.cd();
    std::vector< double > xs,ys;
    std::cout << "Cut: " << itCut->first << std::endl;

    if(itCut->first == "reac"){
      for( int i = 0; i < total_inel->GetN(); ++i ){
        double x = total_inel->GetX()[i];
        double y = total_inel->GetY()[i];
        xs.push_back( x );
        if( x > total_var->GetX()[ total_var->GetN() -1 ] ){
          break;
        }
        ys.push_back( y * total_var->Eval( x ) ); 
      }   
      MC_xsec_graphs[ "reac" ] = new TGraph( xs.size(), &xs[0], &ys[0] );
      MC_xsec_graphs[ "reac" ]->Write( ("new_xsec_" + itCut->first).c_str() );
    }
    else if( itCut->first == "abscx" ){
      for( int i = 0; i < total_inel->GetN(); ++i ){
        double x = total_inel->GetX()[i];

        if( x > theFS->GetNewGraph( "abs" )->GetX()[ theFS->GetNewGraph( "abs" )->GetN() -1 ] ){
          break;
        }

        double y = total_inel->GetY()[i];
        xs.push_back( x );
        ys.push_back( y * ( theFS->GetNewGraph( "abs" )->Eval( x ) + theFS->GetNewGraph( "cex" )->Eval( x ) ) ); 
      }
      MC_xsec_graphs[ itCut->first ] = new TGraph( xs.size(), &xs[0], &ys[0] ); 
      MC_xsec_graphs[ itCut->first ]->Write( ("new_xsec_" + itCut->first).c_str() );    
    }
    else{

      for( int i = 0; i < total_inel->GetN(); ++i ){
        double x = total_inel->GetX()[i];
        if( x > theFS->GetNewGraph( itCut->first )->GetX()[ theFS->GetNewGraph( itCut->first )->GetN() -1 ] ){
          break;
        }
        double y = total_inel->GetY()[i];
        xs.push_back( x );
        ys.push_back( y * theFS->GetNewGraph( itCut->first )->Eval( x ) ); 
      }
      MC_xsec_graphs[ itCut->first ] = new TGraph( xs.size(), &xs[0], &ys[0] ); 
      MC_xsec_graphs[ itCut->first ]->Write( ("new_xsec_" + itCut->first).c_str() );
    }

    fFitDir->cd();

    MC_xsec_graphs[ itCut->first ]->Write(itCut->first.c_str()); 
  }

  tryout.Close();
}

void G4ReweightFitter::FinishUp(){
  std::map< std::string, TGraph* >::iterator it =
    MC_xsec_graphs.begin();

  for( ; it!= MC_xsec_graphs.end(); ++it ){
    delete it->second;
  }
  delete theFS;
}


void G4ReweightFitter::GetMCGraphs(){
  std::cout << "Sample: " << std::endl;
  std::cout << "Sample: " << std::endl;
  for( size_t i = 0; i < ActiveSample->Parameters.size(); ++i ){
    std::cout << "\t" << ActiveSample->Parameters.at(i).Name << " " << ActiveSample->Parameters.at(i).Value << std::endl;
  }
  std::cout << "\tFile: " << ActiveSample->theFile << std::endl;

  TFile fMCFile(ActiveSample->theFile.c_str(), "READ");
  fMCTree = (TTree*)fMCFile.Get("tree");
  std::cout << "Got tree: " << fMCTree << std::endl;

  //Get the total 
  std::string drawCommand = "sqrt(Energy*Energy - 139.57*139.57)";
  std::string totalCommand = drawCommand + ">>total" + binning;
  fMCTree->Draw( totalCommand.c_str(), "", "goff" );

  TH1D * total = (TH1D*)gDirectory->Get("total");

  //TCanvas * c1 = new TCanvas("c1", "c1");

  //Go through the map of cuts
  std::map< std::string, std::string >::iterator itCuts = cuts.begin();
  for( itCuts; itCuts != cuts.end(); ++itCuts ){


    std::string name = itCuts->first;      
    std::string theCut = itCuts->second;
    
    std::string cutCommand = drawCommand + ">>" + name + binning;

    std::string cutOption = weight + theCut;

    fMCTree->Draw( cutCommand.c_str(), cutOption.c_str(), "goff" );
   
    TH1D * hist = (TH1D*)gDirectory->Get( name.c_str() );

//    hist->Draw();
    
    
    hist->Divide( total );
    hist->Scale( scale );
//    hist->Draw();
    
    
    std::vector< double > thePoints = points.at( name );
    std::vector< double > the_xsec; 
    for( int i = 0; i < thePoints.size(); ++i ){


      double content = hist->GetBinContent( hist->FindBin( thePoints[i] ) );

      the_xsec.push_back( content );
    }

    MC_xsec_graphs[ name ] = new TGraph( thePoints.size(), &thePoints[0], &the_xsec[0] );

    fFitDir->cd();

    MC_xsec_graphs[ name ]->Write(name.c_str()); 
  }

  delete fMCTree;
  fMCFile.Close();

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

TTree* G4ReweightFitter::GetReweightFS( /*FitSample theSample*/ ){
  std::cout << "Sample: " << std::endl;
  for( size_t i = 0; i < ActiveSample->Parameters.size(); ++i ){
    std::cout << "\t" << ActiveSample->Parameters.at(i).Name << " " << ActiveSample->Parameters.at(i).Value << std::endl;
  }
  std::cout << "\tFile: " << ActiveSample->theFile << std::endl;

//To do: make these members of the class to handle memory better
  TFile * RWFile = new TFile(ActiveSample->theFile.c_str(), "READ");
  TTree * RWTree = (TTree*)RWFile->Get("tree");
  std::cout << "Got tree: " << RWTree << std::endl;
  return RWTree;
}

//To do: close file & delete pointers from GetReweightFS
//void G4ReweightFitter::CloseReweightFS(){
//
//}

double G4ReweightFitter::DoFit(){
  double Chi2 = 0.;
  double Data_val, MC_val, Data_err;
  double x;

  //Go through each cut defined for the experiment
//  std::map< std::string, TGraph * >::iterator itXSec = MC_xsec_graphs.begin();
//  for( itXSec; itXSec != MC_xsec_graphs.end(); ++itXSec ){
  std::map< std::string, TGraphErrors * >::iterator itXSec = Data_xsec_graphs.begin();
  for( itXSec; itXSec != Data_xsec_graphs.end(); ++itXSec ){
    std::string name = itXSec->first;

    std::cout << "Fitting " << name << " now" << std::endl;

    //TGraph * MC_xsec = itXSec->second;
    //TGraphErrors * Data_xsec = Data_xsec_graphs.at(name);
    TGraph * MC_xsec = MC_xsec_graphs.at(name);
    TGraphErrors * Data_xsec = itXSec->second;

    int nPoints = MC_xsec->GetN(); 
   
    double partial_chi2 = 0.;
    for( int i = 0; i < nPoints; ++i ){

      Data_xsec->GetPoint(i, x, Data_val);
      Data_err = Data_xsec->GetErrorY(i);
      MC_val = MC_xsec->Eval( x );
//      MC_xsec->GetPoint(i, x, MC_val);

      partial_chi2 += (1. / nPoints ) * ( (Data_val - MC_val) / Data_err ) * ( (Data_val - MC_val) / Data_err );
    }
    
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

void G4ReweightFitter::SetActiveSample( size_t i, TDirectory * output_dir){ 
  ActiveSample = &samples[i]; 
  fTopDir = output_dir;
  fTopDir->cd();

  fFitDir = output_dir->mkdir( fExperimentName.c_str() );
  fFitDir->cd();
}

void G4ReweightFitter::MakeFitDir( TDirectory *output_dir ){

  fTopDir = output_dir;
  fTopDir->cd();

  fFitDir = output_dir->mkdir( fExperimentName.c_str() );
  fFitDir->cd();

}

void G4ReweightFitter::LoadData(){
  fDataFile = new TFile( fDataFileName.c_str(), "READ"); 

  std::map< std::string, std::string >::iterator itGraphs;
  for( itGraphs = graph_names.begin(); itGraphs != graph_names.end(); ++itGraphs ){
    Data_xsec_graphs[ itGraphs->first ] = (TGraphErrors*)fDataFile->Get(itGraphs->second.c_str());
    std::cout << "Data: " << Data_xsec_graphs[ itGraphs->first ] << std::endl;
  }

  std::cout << "Loaded data" << std::endl;
}
