#include "G4ReweightFinalState.hh"
#include "G4ReweightStep.hh"

#include <utility>
#include <algorithm>
#include <iostream>

#include "TROOT.h"
#include "TVectorD.h"


G4ReweightFinalState::G4ReweightFinalState(TTree * input, std::map< std::string, G4ReweightInter* > &FSScales, double max, double min, bool PiMinus) 
: Maximum(max), Minimum(min){

  if( PiMinus ) SetPiMinus();
  
//  TFile * fout = new TFile( "final_state_try.root", "RECREATE" );

//  std::map< std::string, TH1D* > oldHists;
//  std::map< std::string, TH1D* > newHists;
  std::map< std::string, G4ReweightInter* > theVariations;



//  input->Draw( "sqrt(Energy*Energy - 139.57*139.57)>>total(10, 200, 300)", "", "goff" ); 
//  TH1D * total = (TH1D*)gDirectory->Get("total");
//  total->Write();

  std::map< std::string, std::string >::iterator it = theCuts.begin();
  for( ; it != theCuts.end(); ++it ){
    std::string name = it->first;
    std::string cut  = it->second;

    //Set Binning
    input->Draw( ("sqrt(Energy*Energy - 139.57*139.57)>>" + name + "(600, 50, 650)").c_str(), cut.c_str(), "goff" ); 

    TH1D * theHist = (TH1D*)gDirectory->Get(name.c_str());
    //theHist->Write();

    //Load the Hists
//    TH1D * theHist = (TH1D*)FinalStateFile->Get( theInts.at(i).c_str() );
    newHists[ name ] = (TH1D*)theHist->Clone( ("new_" + name).c_str() );
    oldHists[ name ] = (TH1D*)theHist->Clone();
  }
  //fout->Close();
  //delete fout;

  
  //Just for loading. Could do everything in one shot, but it's
  //more understandable if it's compartmentalized like this
  for( size_t i = 0; i < theInts.size(); ++i ){
    G4ReweightInter * theInter = FSScales[ theInts.at(i) ]; 
    theVariations[ theInts.at(i) ] = theInter;

  }

  //fout->cd();

  //Now go through and vary the exclusive channels  
  for( size_t i = 0; i < theInts.size(); ++i ){
    G4ReweightInter * theVar = theVariations.at( theInts.at(i) );
    TH1D * theHist = newHists.at( theInts.at(i) );

    for( int bin = 1; bin <= theHist->GetNbinsX(); ++bin ){
      
      double histContent = theHist->GetBinContent( bin );
      double binCenter   = theHist->GetBinCenter( bin );
      double theScale    = theVar->GetContent( binCenter ); 

      if( ( theHist->GetBinCenter( bin ) < Minimum ) 
      ||  ( theHist->GetBinCenter( bin ) > Maximum ) ){
        theHist->SetBinContent( bin, histContent );
      }
      else{
        theHist->SetBinContent( bin, theScale * histContent ); 
      }
    }

    //Save the varied and nominal
    //theHist->Write();
    //oldHists.at( theInts.at(i) )->Write();
  }

  //Form the total cross sections from 
  //the nominal and varied exlcusive channels
  TH1D * oldTotal = (TH1D*)oldHists[ theInts.at(0) ]->Clone("oldTotal");
  TH1D * newTotal = (TH1D*)newHists[ theInts.at(0) ]->Clone("newTotal");
  
  for(size_t i = 1; i < theInts.size(); ++i){
    oldTotal->Add( oldHists[ theInts.at(i) ] );
    newTotal->Add( newHists[ theInts.at(i) ] );
  }

  //Save the Totals
  //oldTotal->Write();
  //newTotal->Write();

  //Need to make this smarter when going through the bins.
  //What if there's an empty bin?
/*
  for(size_t bin = 1; bin <= newTotal->GetNbinsX(); ++bin){

    if( oldTotal->GetBinContent( bin ) == 0. ){
      oldTotal->SetBinContent( bin, 1. );
    }

    if( ( newTotal->GetBinCenter( bin ) < Minimum ) 
    ||  ( newTotal->GetBinCenter( bin ) > Maximum ) ){
      newTotal->SetBinContent( bin, oldTotal->GetBinContent( bin ) );
    }
  }
*/

  //Form the variation from the new and old totals 
  totalVariation = (TH1D*)newTotal->Clone("totalVariation");
  totalVariation->Divide( oldTotal );
  for( int bin = 1; bin <= totalVariation->GetNbinsX(); ++bin ){

    if( ( totalVariation->GetBinCenter( bin ) < Minimum ) 
    ||  ( totalVariation->GetBinCenter( bin ) > Maximum ) ){

      if( totalVariation->GetBinContent( bin ) < .000000001 ){
        totalVariation->SetBinContent( bin, 1. );
      }

    }
  }
  //totalVariation->Write();

  //Now go back through the varied exclusive channels
  //and compute the final scale
  for( size_t i = 0; i < theInts.size(); ++i ){
    TH1D * exclusiveVariation = (TH1D*)newHists.at( theInts.at(i) )->Clone( (theInts.at(i) + "Variation").c_str() );

    exclusiveVariation->Divide( oldHists.at( theInts.at(i) ) );

    for( int bin = 1; bin <= exclusiveVariation->GetNbinsX(); ++bin ){

      if( ( exclusiveVariation->GetBinCenter( bin ) < Minimum ) 
      ||  ( exclusiveVariation->GetBinCenter( bin ) > Maximum ) ){

        if( exclusiveVariation->GetBinContent( bin ) < .000000001 ){
          exclusiveVariation->SetBinContent( bin, 1. );
        }

      }
    }

    exclusiveVariation->Divide( totalVariation );
    //exclusiveVariation->Write();

    exclusiveVariations[ theInts.at(i) ] = exclusiveVariation; 

    std::string name = theInts.at(i);
    std::string new_name = "new_" + name;

    //Delete the pointers here
//    delete newHists.at( theInts.at(i) );
//    delete oldHists.at( theInts.at(i) );
//    gDirectory->Delete(name.c_str());
//    gDirectory->Delete(new_name.c_str());
  }

  //Now go through and clear from memory all of the pointers
//  delete newTotal;
//  delete oldTotal;
//
//  gDirectory->Delete("oldTotal");
//  gDirectory->Delete("newTotal");

  //fout->Close();
}

G4ReweightFinalState::G4ReweightFinalState(TFile * input, std::map< std::string, TGraph* > &FSScales, /*double max, double min, */bool PiMinus) 
/*: Maximum(max), Minimum(min)*/{

  as_graphs = true;
  if( PiMinus ) SetPiMinus();
  
  std::map< std::string, TGraph* > theVariations;
  std::map< std::string, std::string >::iterator it = theCuts.begin();
  //TFile *fout = new TFile ("graph_weights.root", "RECREATE");
  for( ; it != theCuts.end(); ++it ){
    std::string name = it->first;

    std::cout << "Loading " << name << std::endl;

    TGraph * theGraph = (TGraph*)input->Get(name.c_str());
    //theHist->Write();

    //Load the Hists
    newGraphs[ name ] = (TGraph*)theGraph->Clone( ("new_" + name).c_str() );
    oldGraphs[ name ] = (TGraph*)theGraph->Clone();
    //fout->cd();
    //oldGraphs[ name ]->Write( ("old_" + name).c_str());
  }
  std::cout << "Loaded Graphs" << std::endl;
  //delete fout;

   
  std::vector< double > newPoints;

  std::cout << "Storing" << std::endl;
  //fout->cd();
  for( size_t i = 0; i < theInts.size(); ++i ){
    theVariations[ theInts.at(i) ] = FSScales[ theInts.at(i) ];
    //theVariations[ theInts.at(i) ]->Write( theInts.at(i).c_str() );

    //New: Accounting for parameter edges between bins in the fraction graphs
    std::cout << "Adding points around parameter edges" << std::endl;
    for( int j = 0; j < theVariations[ theInts.at(i) ]->GetN(); ++j ){
      double ptX = theVariations[ theInts.at(i) ]->GetX()[ j ];
      if( ptX == 0. ) continue;

      if( std::find( newPoints.begin(), newPoints.end(), ptX - .001 ) == newPoints.end() ){
        newPoints.push_back( ptX - .001 );
      }
      if( std::find( newPoints.begin(), newPoints.end(), ptX + .001 ) == newPoints.end() ){
        newPoints.push_back( ptX + .001 );
      }
    }
  }
  
  for( size_t i = 0; i < theInts.size(); ++i ){
    for( size_t j = 0; j < newPoints.size(); ++j ){
      
      double new_x = newPoints.at(j);
      double new_y = oldGraphs[ theInts.at(i) ]->Eval( new_x );

      if( new_x < oldGraphs[ theInts.at(i) ]->GetX()[0] ){

        double old_x = oldGraphs[ theInts.at(i) ]->GetX()[0];
        double old_y = oldGraphs[ theInts.at(i) ]->GetY()[0];

        oldGraphs[ theInts.at(i) ]->InsertPointBefore(1, old_x, old_y );
        newGraphs[ theInts.at(i) ]->InsertPointBefore(1, old_x, old_y );

        oldGraphs[ theInts.at(i) ]->SetPoint(0, new_x, new_y );
        newGraphs[ theInts.at(i) ]->SetPoint(0, new_x, new_y );

        continue;
      }
      else if( new_x > oldGraphs[ theInts.at(i) ]->GetX()[ oldGraphs[ theInts.at(i) ]->GetN() - 1]){ 
        continue;
      }
      for( int k = 0; k < oldGraphs[ theInts.at(i) ]->GetN() - 1; ++k ){
        if( new_x > oldGraphs[ theInts.at(i) ]->GetX()[k] 
        &&  new_x < oldGraphs[ theInts.at(i) ]->GetX()[k + 1] ){
          oldGraphs[ theInts.at(i) ]->InsertPointBefore(k + 1, new_x, new_y );
          newGraphs[ theInts.at(i) ]->InsertPointBefore(k + 1, new_x, new_y );
        }
      }

    }
  }

  std::cout << "Stored" << std::endl;


  //fout->cd();

  //Now go through and vary the exclusive channels  
  for( size_t i = 0; i < theInts.size(); ++i ){
    std::cout << theInts.at(i) << std::endl;
    TGraph * theVar = theVariations.at( theInts.at(i) );
    TGraph * theGraph = newGraphs.at( theInts.at(i) );
    std::cout << "Got Graphs " << theVar << " " << theGraph << std::endl;
    for( size_t bin = 0; bin < theGraph->GetN(); ++bin ){
      
      double Content = theGraph->GetY()[bin];
     // std::cout << "Content: " << Content << std::endl;
      double binCenter   = theGraph->GetX()[bin];
     // std::cout << "binCenter: " << binCenter << std::endl;
      double theScale    = theVar->Eval(binCenter); 
      //std::cout << "theScale: " << theScale << std::endl;
      
      //Check if >/< max/min of var graph
      if( /*( binCenter < Minimum ) 
      ||  ( binCenter > Maximum ) 
      ||*/  ( binCenter < theVar->GetX()[0]) 
      ||  ( binCenter > theVar->GetX()[ theVar->GetN() - 1 ] ) ){
        theGraph->SetPoint( bin, binCenter, Content );
      }
      else{
        theGraph->SetPoint( bin, binCenter, theScale * Content ); 
      }
    }
    //theGraph->Write();

    //Save the varied and nominal
    //theHist->Write();
    //oldHists.at( theInts.at(i) )->Write();
  }

  //Form the total cross sections from 
  //the nominal and varied exlcusive channels
  TGraph * oldTotal = (TGraph*)oldGraphs[ theInts.at(0) ]->Clone("oldTotal");
  TGraph * newTotal = (TGraph*)newGraphs[ theInts.at(0) ]->Clone("newTotal");
  
  for(size_t i = 1; i < theInts.size(); ++i){
    AddGraphs(oldTotal, oldGraphs[ theInts.at(i) ] );
    AddGraphs(newTotal, newGraphs[ theInts.at(i) ] );
  }
  //oldTotal->Write("oldTotal");
  //newTotal->Write("newTotal");

  //Save the Totals
  //oldTotal->Write();
  //newTotal->Write();

  //Need to make this smarter when going through the bins.
  //What if there's an empty bin?
/*
  for(size_t bin = 1; bin <= newTotal->GetNbinsX(); ++bin){

    if( oldTotal->GetBinContent( bin ) == 0. ){
      oldTotal->SetBinContent( bin, 1. );
    }

    if( ( newTotal->GetBinCenter( bin ) < Minimum ) 
    ||  ( newTotal->GetBinCenter( bin ) > Maximum ) ){
      newTotal->SetBinContent( bin, oldTotal->GetBinContent( bin ) );
    }
  }
*/

  //Form the variation from the new and old totals 
  totalVariationGraph = (TGraph*)newTotal->Clone("totalVariation");
  DivideGraphs(totalVariationGraph, oldTotal);
  //totalVariationGraph->Write("totalVar");

  //Now go back through the varied exclusive channels
  //and compute the final scale
  for( size_t i = 0; i < theInts.size(); ++i ){
    TGraph * exclusiveVariation = (TGraph*)newGraphs.at( theInts.at(i) )->Clone( (theInts.at(i) + "Variation").c_str() );

    DivideGraphs( exclusiveVariation, oldGraphs.at( theInts.at(i) ) );

    DivideGraphs( exclusiveVariation, totalVariationGraph );
    //exclusiveVariation->Write();

    exclusiveVariationGraphs[ theInts.at(i) ] = exclusiveVariation; 

    std::string name = theInts.at(i);
    std::string new_name = "new_" + name;
    //exclusiveVariation->Write((name + "Var").c_str());

    //Delete the pointers here
//    delete newHists.at( theInts.at(i) );
//    delete oldHists.at( theInts.at(i) );
//    gDirectory->Delete(name.c_str());
//    gDirectory->Delete(new_name.c_str());
  }

  //Now go through and clear from memory all of the pointers
  delete newTotal;
  delete oldTotal;
//
//  gDirectory->Delete("oldTotal");
//  gDirectory->Delete("newTotal");

  //fout->Close();
}

G4ReweightFinalState::G4ReweightFinalState(TFile * input, const std::map< std::string, TH1D* > &FSScales, bool PiMinus){
  TVectorD * m_vec = (TVectorD*)input->Get("Mass");
  Mass = (*m_vec)(0);

  TVectorD * d_vec = (TVectorD*)input->Get("Density");
  Density = (*d_vec)(0);

  as_graphs = true;
  if( PiMinus ) SetPiMinus();
  
  std::map< std::string, std::string >::iterator it = theCuts.begin();
  //TFile *fout = new TFile ("graph_weights.root", "RECREATE");
  for( ; it != theCuts.end(); ++it ){
    std::string name = it->first;

    std::cout << "Loading " << name << std::endl;

    TGraph * theGraph = (TGraph*)input->Get(name.c_str());
    //theHist->Write();

    //Load the Hists
    newGraphs[ name ] = (TGraph*)theGraph->Clone( ("new_" + name).c_str() );
    oldGraphs[ name ] = (TGraph*)theGraph->Clone();
    //fout->cd();
    //oldGraphs[ name ]->Write( ("old_" + name).c_str());
  }
  std::cout << "Loaded Graphs" << std::endl;
  //delete fout;

   

  std::cout << "Stored" << std::endl;

  std::vector< double > newPoints;

  std::cout << "Storing" << std::endl;
  //fout->cd();
  for( size_t i = 0; i < theInts.size(); ++i ){
    std::cout << "Adding points around parameter edges" << std::endl;
    TH1D * theVar = FSScales.at( theInts.at(i) );
    int nBins = theVar->GetNbinsX();
    for( int j = 1; j <= nBins; ++j ){
      double ptX = theVar->GetBinLowEdge(j);
      std::cout << j << " " << ptX << std::endl;
      if( ptX == 0. ) continue;

      if( std::find( newPoints.begin(), newPoints.end(), ptX - .001 ) == newPoints.end() ){
        newPoints.push_back( ptX - .001 );
      }
      if( std::find( newPoints.begin(), newPoints.end(), ptX + .001 ) == newPoints.end() ){
        newPoints.push_back( ptX + .001 );
      }
    }

    //Add last upper bin edge
    double ptX = theVar->GetBinLowEdge( nBins ); 
    ptX += theVar->GetBinWidth( nBins );
    std::cout << "Last bin: " << ptX << std::endl;

    if( std::find( newPoints.begin(), newPoints.end(), ptX - .001 ) == newPoints.end() ){
      newPoints.push_back( ptX - .001 );
    }
    if( std::find( newPoints.begin(), newPoints.end(), ptX + .001 ) == newPoints.end() ){
      newPoints.push_back( ptX + .001 );
    }
  }


  for( size_t i = 0; i < theInts.size(); ++i ){
    for( size_t j = 0; j < newPoints.size(); ++j ){
      
      double new_x = newPoints.at(j);
      double new_y = oldGraphs[ theInts.at(i) ]->Eval( new_x );

      if( new_x < oldGraphs[ theInts.at(i) ]->GetX()[0] ){

        double old_x = oldGraphs[ theInts.at(i) ]->GetX()[0];
        double old_y = oldGraphs[ theInts.at(i) ]->GetY()[0];

        oldGraphs[ theInts.at(i) ]->InsertPointBefore(1, old_x, old_y );
        newGraphs[ theInts.at(i) ]->InsertPointBefore(1, old_x, old_y );

        oldGraphs[ theInts.at(i) ]->SetPoint(0, new_x, new_y );
        newGraphs[ theInts.at(i) ]->SetPoint(0, new_x, new_y );

        continue;
      }
      else if( new_x > oldGraphs[ theInts.at(i) ]->GetX()[ oldGraphs[ theInts.at(i) ]->GetN() - 1]){ 
        continue;
      }
      for( int k = 0; k < oldGraphs[ theInts.at(i) ]->GetN() - 1; ++k ){
        if( new_x > oldGraphs[ theInts.at(i) ]->GetX()[k] 
        &&  new_x < oldGraphs[ theInts.at(i) ]->GetX()[k + 1] ){
          oldGraphs[ theInts.at(i) ]->InsertPointBefore(k + 1, new_x, new_y );
          newGraphs[ theInts.at(i) ]->InsertPointBefore(k + 1, new_x, new_y );
        }
      }

    }
  }


  
  std::cout << "Stored" << std::endl;


  //fout->cd();

  //Now go through and vary the exclusive channels  
  for( size_t i = 0; i < theInts.size(); ++i ){
    std::cout << theInts.at(i) << std::endl;
    TH1D * theVar = FSScales.at( theInts.at(i) );
    TGraph * theGraph = newGraphs.at( theInts.at(i) );
    std::cout << "Got Graphs " << theVar << " " << theGraph << std::endl;
    for( size_t bin = 0; bin < theGraph->GetN(); ++bin ){
      
      double Content = theGraph->GetY()[bin];
     // std::cout << "Content: " << Content << std::endl;
      double point   = theGraph->GetX()[bin];
     // std::cout << "point: " << point << std::endl;
      double theScale    = theVar->GetBinContent( theVar->FindBin( point ) ); 
      //std::cout << "theScale: " << theScale << std::endl;
      
      //Check if >/< max/min of var graph
      if( ( point < theVar->GetBinLowEdge(1)) 
      ||  ( point > ( theVar->GetBinLowEdge( theVar->GetNbinsX() ) + theVar->GetBinWidth( theVar->GetNbinsX() ) ) ) ){
        theGraph->SetPoint( bin, point, Content );
      }
      else{
        theGraph->SetPoint( bin, point, theScale * Content ); 
      }
    }
    //theGraph->Write();

    //Save the varied and nominal
    //theHist->Write();
    //oldHists.at( theInts.at(i) )->Write();
  }

  //Form the total cross sections from 
  //the nominal and varied exlcusive channels
  TGraph * oldTotal = (TGraph*)oldGraphs[ theInts.at(0) ]->Clone("oldTotal");
  TGraph * newTotal = (TGraph*)newGraphs[ theInts.at(0) ]->Clone("newTotal");
  for(size_t i = 1; i < theInts.size(); ++i){
    AddGraphs(oldTotal, oldGraphs[ theInts.at(i) ] );
    AddGraphs( newTotal,   newGraphs[ theInts.at(i) ] );
  }
  //oldTotal->Write("oldTotal");
  //newTotal->Write("newTotal");

  //Save the Totals
  //oldTotal->Write();
  //newTotal->Write();

  //Need to make this smarter when going through the bins.
  //What if there's an empty bin?
/*
  for(size_t bin = 1; bin <= newTotal->GetNbinsX(); ++bin){

    if( oldTotal->GetBinContent( bin ) == 0. ){
      oldTotal->SetBinContent( bin, 1. );
    }

    if( ( newTotal->GetBinCenter( bin ) < Minimum ) 
    ||  ( newTotal->GetBinCenter( bin ) > Maximum ) ){
      newTotal->SetBinContent( bin, oldTotal->GetBinContent( bin ) );
    }
  }
*/

  //Form the variation from the new and old totals 
  totalVariationGraph = (TGraph*)newTotal->Clone("totalVariation");
  DivideGraphs(totalVariationGraph, oldTotal);
  //totalVariationGraph->Write("totalVar");

  //Now go back through the varied exclusive channels
  //and compute the final scale
  for( size_t i = 0; i < theInts.size(); ++i ){
    TGraph * exclusiveVariation = (TGraph*)newGraphs.at( theInts.at(i) )->Clone( (theInts.at(i) + "Variation").c_str() );

    DivideGraphs( exclusiveVariation, oldGraphs.at( theInts.at(i) ) );

    DivideGraphs( exclusiveVariation, totalVariationGraph );
    //exclusiveVariation->Write();

    exclusiveVariationGraphs[ theInts.at(i) ] = exclusiveVariation; 

    std::string name = theInts.at(i);
    std::string new_name = "new_" + name;
    //exclusiveVariation->Write((name + "Var").c_str());

    //Delete the pointers here
//    delete newHists.at( theInts.at(i) );
//    delete oldHists.at( theInts.at(i) );
//    gDirectory->Delete(name.c_str());
//    gDirectory->Delete(new_name.c_str());
  }

  //Now go through and clear from memory all of the pointers
  delete newTotal;
  delete oldTotal;
//
//  gDirectory->Delete("oldTotal");
//  gDirectory->Delete("newTotal");

  //fout->Close();
}

void G4ReweightFinalState::SetNewHists(const std::map< std::string, TH1D* > &FSScales){

  std::map< std::string, std::string >::iterator it = theCuts.begin();
  for( ; it != theCuts.end(); ++it ){
    std::string name = it->first;

    //Load the Hists
    if( newGraphs[ name ] )
      delete newGraphs[ name ];
    newGraphs[ name ] = (TGraph*)oldGraphs[ name ]->Clone( ("new_" + name).c_str() );
  }

  //Now go through and vary the exclusive channels  
  for( size_t i = 0; i < theInts.size(); ++i ){
    TH1D * theVar = FSScales.at( theInts.at(i) );
    TGraph * theGraph = newGraphs.at( theInts.at(i) );
    for( size_t bin = 0; bin < theGraph->GetN(); ++bin ){
      
      double Content = theGraph->GetY()[bin];
      double point   = theGraph->GetX()[bin];
      double theScale    = theVar->GetBinContent( theVar->FindBin( point ) ); 
      
      //Check if >/< max/min of var graph
      if( ( point < theVar->GetBinLowEdge(1)) 
      ||  ( point > ( theVar->GetBinLowEdge( theVar->GetNbinsX() ) + theVar->GetBinWidth( theVar->GetNbinsX() ) ) ) ){
        theGraph->SetPoint( bin, point, Content );
      }
      else{
        theGraph->SetPoint( bin, point, theScale * Content ); 
      }
    }
  }

  //Form the total cross sections from 
  //the nominal and varied exlcusive channels
  TGraph * oldTotal = (TGraph*)oldGraphs[ theInts.at(0) ]->Clone("oldTotal");
  TGraph * newTotal = (TGraph*)newGraphs[ theInts.at(0) ]->Clone("newTotal");
  
  for(size_t i = 1; i < theInts.size(); ++i){
    AddGraphs(oldTotal, oldGraphs[ theInts.at(i) ] );
    AddGraphs(newTotal, newGraphs[ theInts.at(i) ] );
  }

  //Form the variation from the new and old totals 
  if( totalVariationGraph )
    delete totalVariationGraph;
  totalVariationGraph = (TGraph*)newTotal->Clone("totalVariation");
  DivideGraphs(totalVariationGraph, oldTotal);

  //Now go back through the varied exclusive channels
  //and compute the final scale
  for( size_t i = 0; i < theInts.size(); ++i ){
    TGraph * exclusiveVariation = (TGraph*)newGraphs.at( theInts.at(i) )->Clone( (theInts.at(i) + "Variation").c_str() );

    DivideGraphs( exclusiveVariation, oldGraphs.at( theInts.at(i) ) );
    DivideGraphs( exclusiveVariation, totalVariationGraph );

    if( exclusiveVariationGraphs[ theInts.at(i) ] )    
      delete exclusiveVariationGraphs[ theInts.at(i) ];
    exclusiveVariationGraphs[ theInts.at(i) ] = exclusiveVariation; 

    std::string name = theInts.at(i);
    std::string new_name = "new_" + name;
  }

  //Now go through and clear from memory all of the pointers
  delete newTotal;
  delete oldTotal;

}

double G4ReweightFinalState::GetWeight( std::string theInt, double theMomentum ){
  
  if( ( theMomentum < Minimum ) || ( theMomentum > Maximum ) ){
    //std::cout << "Out of bounds. Returning 1" << std::endl;
    return 1.;
  }

  //std::cout << "Getting hist for interaction: " << theInt << std::endl;
  TH1D * theHist = GetExclusiveVariation( theInt ); 
  //std::cout << "Hist exists: " << theHist << std::endl;
  int theBin = theHist->FindBin( theMomentum );
  //std::cout << "Bin: " << theBin << std::endl;
  double theWeight = theHist->GetBinContent( theBin );
  //std::cout << "Weight: " << theWeight << std::endl;

  return theWeight;
}

double G4ReweightFinalState::GetWeightFromGraph( std::string theInt, double theMomentum ){
  
  if( ( theMomentum < Minimum ) || ( theMomentum > Maximum ) ){
    //std::cout << "Out of bounds. Returning 1" << std::endl;
    return 1.;
  }

  //std::cout << "Getting hist for interaction: " << theInt << std::endl;
  TGraph * theGraph = GetExclusiveVariationGraph( theInt ); 
  //std::cout << "Hist exists: " << theHist << std::endl;
//  int theBin = theHist->FindBin( theMomentum );
  //std::cout << "Bin: " << theBin << std::endl;
  double theWeight = theGraph->Eval( theMomentum );
  //std::cout << "Weight: " << theWeight << std::endl;

  return theWeight;
}

void G4ReweightFinalState::SetTotalGraph( TFile * input ){
  totalGraph = (TGraph*)input->Get( "inel_momentum" );
}


double G4ReweightFinalState::GetNominalMFP( double theMom ){
  double xsec = totalGraph->Eval( theMom );
  return 1.e27 * Mass / ( Density * 6.022e23 * xsec );
}

double G4ReweightFinalState::GetBiasedMFP( double theMom ){
  double b = 1.;
  if( as_graphs ){
    b = totalVariationGraph->Eval( theMom );
  }

  return  GetNominalMFP( theMom ) / b;
}

double G4ReweightFinalState::GetWeight( G4ReweightTraj * theTraj ){

  double total, bias_total;

  size_t nsteps = theTraj->GetNSteps();
  if( theTraj->GetFinalProc() == fInelastic )
    --nsteps;

  for(size_t is = 0; is < nsteps; ++is){   

    auto theStep = theTraj->GetStep(is);
        
    double theMom = theStep->GetFullPreStepP();

    //Convert xsec to MFP   
    //Note: taking away the factor of 10. used in conversion
    //
    total += ( theStep->stepLength / GetNominalMFP(theMom) );
    bias_total += ( theStep->stepLength / GetBiasedMFP( theMom ) );

  }

  
  double weight = exp( total - bias_total );

  if( theTraj->GetFinalProc() == fInelastic ){
   
    auto lastStep = theTraj->GetStep( theTraj->GetNSteps() - 1 );
    double theMom = lastStep->GetFullPreStepP();

    weight *= ( 1 - exp(lastStep->stepLength / GetBiasedMFP( theMom ) ) );
    weight *= ( 1. / ( 1 - exp( lastStep->stepLength / GetNominalMFP( theMom ) ) ) );


    int nPi0     = theTraj->HasChild(111).size();  
    int nPiPlus  = theTraj->HasChild(211).size();
    int nPiMinus = theTraj->HasChild(-211).size();

    std::string cut;
    if( (nPi0 + nPiPlus + nPiMinus) == 0){
      cut = "abs";
    }
    else if( (nPiPlus + nPiMinus) == 0 && nPi0 == 1 ){
      cut = "cex";
    }
    else if( (nPiPlus + nPiMinus + nPi0) > 1 ){
      cut = "prod";
    }
    else{
      if( theTraj->PID == 211 ){
        if( (nPi0 + nPiMinus) == 0 && nPiPlus == 1 ){
          cut = "inel";
        }
        else if( (nPi0 + nPiPlus) == 0 && nPiMinus == 1 ){
          cut = "dcex"; 
        }
      }
      else if( theTraj->PID == -211 ){
        if( (nPi0 + nPiMinus) == 0 && nPiPlus == 1 ){
          cut = "dcex";
        }
        else if( (nPi0 + nPiPlus) == 0 && nPiMinus == 1 ){
          cut = "inel"; 
        }
      }
    }
    TGraph * theGraph = GetExclusiveVariationGraph( cut ); 


    double exclusive_factor = 1;

    if( theMom > theGraph->GetX()[0] && theMom < theGraph->GetX()[ theGraph->GetN() - 1 ] )
      exclusive_factor = theGraph->Eval( theMom );

    weight *= exclusive_factor;
  }
  return weight;
}

TH1D * G4ReweightFinalState::GetExclusiveVariation( std::string theInt ){
  //Add in check
  
  return exclusiveVariations.at( theInt ); 
}

TGraph * G4ReweightFinalState::GetExclusiveVariationGraph( std::string theInt ){
  //Add in check
  
  return exclusiveVariationGraphs.at( theInt ); 
}

G4ReweightFinalState::~G4ReweightFinalState(){ 
  for( size_t i = 0; i < theInts.size(); ++i){ 
//    delete gROOT->FindObject( theInts.at(i).c_str() ); 
    if( exclusiveVariations.find( theInts.at(i) ) != exclusiveVariations.end() )
      delete exclusiveVariations.at( theInts.at(i) );

    if( oldHists.find( theInts.at(i) ) != oldHists.end() )
      delete oldHists.at( theInts.at(i) );

    if( newHists.find( theInts.at(i) ) != newHists.end() )
      delete newHists.at( theInts.at(i) );

    if( exclusiveVariationGraphs.find( theInts.at(i) ) != exclusiveVariationGraphs.end() )
      delete exclusiveVariationGraphs.at( theInts.at(i) );

    if( oldGraphs.find( theInts.at(i) ) != oldGraphs.end() )
      delete oldGraphs.at( theInts.at(i) );

    if( newGraphs.find( theInts.at(i) ) != newGraphs.end() )
      delete newGraphs.at( theInts.at(i) );
  } 

}

void G4ReweightFinalState::AddGraphs( TGraph *target, TGraph* adder  ){
  int nbins = target->GetN();
  for( int i = 0; i < nbins; ++i ){
    target->SetPoint(i , target->GetX()[i], (target->GetY()[i] + adder->GetY()[i]) );
  }
}

void G4ReweightFinalState::DivideGraphs( TGraph *target, TGraph* divider  ){
  int nbins = target->GetN();
  for( int i = 0; i < nbins; ++i ){
    if( divider->GetY()[i] < 0.000001 ){
      target->SetPoint(i , target->GetX()[i], 1. );
    }
    else{
      target->SetPoint(i , target->GetX()[i], (target->GetY()[i] / divider->GetY()[i]) );
    }
  }
}

/*G4ReweightFinalState::ClearVariations(){
  std::map< std::string, G4ReweightInter * >::iterator it = exclusiveVariations.at(
}*/
