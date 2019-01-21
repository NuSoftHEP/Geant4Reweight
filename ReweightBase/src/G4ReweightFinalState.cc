#include "G4ReweightFinalState.hh"

#include "tinyxml2.h"
#include <utility>
#include <iostream>

#include "TROOT.h"


G4ReweightFinalState::G4ReweightFinalState(TFile * FinalStateFile, std::string FSScaleFileName ){
  
  TFile * fout = new TFile( "final_state_try.root", "RECREATE" );

  std::cout << "Making hist maps" << std::endl;
  std::map< std::string, TH1D* > oldHists;
  std::map< std::string, TH1D* > newHists;
  std::map< std::string, G4ReweightInter* > theVariations;

  std::cout << "Getting Max and Min" << std::endl;
  GetMaxAndMin( FSScaleFileName );
  
  //Just for loading. Could do everything in one shot, but it's
  //more understandable if it's compartmentalized like this
  for( size_t i = 0; i < theInts.size(); ++i ){
    //Open up the FSScaleFile and load the variations
    std::cout << theInts.at(i) << std::endl;
    G4ReweightInter * theInter = GetInter( FSScaleFileName, theInts.at(i) );
    theVariations[ theInts.at(i) ] = theInter;

    //Load the Hists
    TH1D * theHist = (TH1D*)FinalStateFile->Get( theInts.at(i).c_str() );
    newHists[ theInts.at(i) ] = (TH1D*)theHist->Clone( ("new_" + theInts.at(i)).c_str() );
    oldHists[ theInts.at(i) ] = (TH1D*)theHist->Clone();
  }

  fout->cd();

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
    theHist->Write();
    oldHists.at( theInts.at(i) )->Write();
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
  oldTotal->Write();
  newTotal->Write();

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
  totalVariation->Write();

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
    exclusiveVariation->Write();

    exclusiveVariations[ theInts.at(i) ] = exclusiveVariation; 
  }

  //fout->Close();
}

G4ReweightFinalState::G4ReweightFinalState(TTree * input, std::map< std::string, G4ReweightInter* > &FSScales, double max, double min, bool PiMinus) 
: Maximum(max), Minimum(min){

  SetPiMinus();
  
//  TFile * fout = new TFile( "final_state_try.root", "RECREATE" );

  std::map< std::string, TH1D* > oldHists;
  std::map< std::string, TH1D* > newHists;
  std::map< std::string, G4ReweightInter* > theVariations;



//  input->Draw( "sqrt(Energy*Energy - 139.57*139.57)>>total(10, 200, 300)", "", "goff" ); 
//  TH1D * total = (TH1D*)gDirectory->Get("total");
//  total->Write();

  std::map< std::string, std::string >::iterator it = theCuts.begin();
  for( ; it != theCuts.end(); ++it ){
    std::string name = it->first;
    std::string cut  = it->second;

    //Set Binning
    input->Draw( ("sqrt(Energy*Energy - 139.57*139.57)>>" + name + "(10, 200, 300)").c_str(), cut.c_str(), "goff" ); 

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
    delete newHists.at( theInts.at(i) );
    delete oldHists.at( theInts.at(i) );
    gDirectory->Delete(name.c_str());
    gDirectory->Delete(new_name.c_str());
  }

  //Now go through and clear from memory all of the pointers
  delete newTotal;
  delete oldTotal;

  gDirectory->Delete("oldTotal");
  gDirectory->Delete("newTotal");

  //fout->Close();
}

void G4ReweightFinalState::GetMaxAndMin( std::string FileName ){
  tinyxml2::XMLDocument doc;

  tinyxml2::XMLError loadResult = doc.LoadFile( FileName.c_str() );
  if( loadResult != tinyxml2::XML_SUCCESS ){
    std::cout << "Could not load file" << std::endl;
    return;
  }

  tinyxml2::XMLNode * theRoot = doc.FirstChild();
  if( !theRoot ){
    std::cout << "Could Not get first child" << std::endl;
    return;
  }

  std::string max = "Maximum";
  std::string min = "Minimum";
 
  tinyxml2::XMLElement * theElement = theRoot->FirstChildElement( max.c_str() );
  //ToDo: add check?
  theElement->QueryDoubleText( &Maximum );

  theElement = theRoot->FirstChildElement( min.c_str() );
  theElement->QueryDoubleText( &Minimum ); 
}

G4ReweightInter * G4ReweightFinalState::GetInter(std::string FileName, std::string FSName){

  tinyxml2::XMLDocument doc;

  tinyxml2::XMLError loadResult = doc.LoadFile( FileName.c_str() );
  if( loadResult != tinyxml2::XML_SUCCESS ){
    std::cout << "Could not load file" << std::endl;
    return 0;
  }

  tinyxml2::XMLNode * theRoot = doc.FirstChild();
  if( !theRoot ){
    std::cout << "Could Not get first child" << std::endl;
    return 0;
  }

 
  std::vector< std::pair< double, double > > input; 
  std::vector< double > theMoms, theVars;
  double momentum, variation;

  tinyxml2::XMLElement * theElement = theRoot->FirstChildElement( FSName.c_str() );
  if( !theElement ){
    std::cout << "Could Not get element " << FSName << std::endl;
    return 0;
  }
  
  tinyxml2::XMLElement * thePoint = theElement->FirstChildElement("Point");
  while( thePoint ){
   

    tinyxml2::XMLError attResult = thePoint->QueryDoubleAttribute("Momentum", &momentum);
    if( attResult != tinyxml2::XML_SUCCESS ){
      std::cout << "Could not get momentum" << std::endl;
    }

    attResult = thePoint->QueryDoubleAttribute("Variation", &variation);
    if( attResult != tinyxml2::XML_SUCCESS ){
      std::cout << "Could not get variation" << std::endl;
    }

    std::cout << momentum << " " << variation << std::endl;
   
    std::pair< double, double > thePair(momentum, variation);
    input.push_back(thePair);

    thePoint = thePoint->NextSiblingElement("Point");
  }

  G4ReweightInter * theInter = new G4ReweightInter( input ); 

  return theInter; 
  
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

TH1D * G4ReweightFinalState::GetExclusiveVariation( std::string theInt ){
  //Add in check
  
  return exclusiveVariations.at( theInt ); 
}

G4ReweightFinalState::~G4ReweightFinalState(){ 
  for( size_t i = 0; i < theInts.size(); ++i){ 
//    delete gROOT->FindObject( theInts.at(i).c_str() ); 
    delete exclusiveVariations.at( theInts.at(i) );
  } 
}

/*G4ReweightFinalState::ClearVariations(){
  std::map< std::string, G4ReweightInter * >::iterator it = exclusiveVariations.at(
}*/
