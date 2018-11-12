#include "G4ReweightFinalState.hh"

#include "tinyxml2.h"
#include <utility>
#include <iostream>

G4ReweightFinalState::G4ReweightFinalState(TFile * FinalStateFile, std::string FSScaleFileName ){
  
  std::vector< std::string > theInts = {"inel", "cex", "abs", "dcex", "prod"};

  TFile * fout = new TFile( "final_state_try.root", "RECREATE" );

  std::map< std::string, TH1D* > oldFracs;
  std::map< std::string, TH1D* > newFracs;

  GetMaxAndMin( FSScaleFileName );
  
  //Open up the FSScaleFile and load the variations
  for( size_t i = 0; i < theInts.size(); ++i ){
    G4ReweightInter * theInter = GetInter( FSScaleFileName, theInts.at(i) );
    TH1D * theHist = (TH1D*)FinalStateFile->Get( theInts.at(i).c_str() );
    
    std::string newName = "new_" + theInts.at(i);
    TH1D * newHist = (TH1D*)theHist->Clone(newName.c_str());
    TH1D * oldHist = (TH1D*)theHist->Clone();

    oldFracs[ theInts.at(i) ] = oldHist;

    for( int bin = 1; bin <= theHist->GetNbinsX(); ++bin ){
      
      double histContent = theHist->GetBinContent( bin );
      double binCenter   = theHist->GetBinCenter( bin );
      double theScale    = theInter->GetContent( binCenter ); 

      if( ( newHist->GetBinCenter( bin ) < Minimum ) 
      ||  ( newHist->GetBinCenter( bin ) > Maximum ) ){
        newHist->SetBinContent( bin, histContent );
      }
      else{
        newHist->SetBinContent( bin, theScale * histContent ); 
      }
    }
    
    newFracs[ theInts.at(i) ] = newHist; 

    fout->cd();
    oldHist->Write();
  }

  //Need to make this smarter when going through the bins.
  //What if there's an empty bin?

  TH1D * oldTotal = (TH1D*)oldFracs[ theInts.at(0) ]->Clone("oldTotal");
  TH1D * newTotal = (TH1D*)newFracs[ theInts.at(0) ]->Clone("newTotal");
  
  for(size_t i = 1; i < theInts.size(); ++i){
    oldTotal->Add( oldFracs[ theInts.at(i) ] );
    newTotal->Add( newFracs[ theInts.at(i) ] );
  }

  for(size_t bin = 1; bin <= newTotal->GetNbinsX(); ++bin){

    if( oldTotal->GetBinContent( bin ) == 0. ){
      oldTotal->SetBinContent( bin, 1. );
    }

    if( ( newTotal->GetBinCenter( bin ) < Minimum ) 
    ||  ( newTotal->GetBinCenter( bin ) > Maximum ) ){
      newTotal->SetBinContent( bin, oldTotal->GetBinContent( bin ) );
    }
  }

  totalRatio = (TH1D*)newTotal->Clone("totalRatio");
  totalRatio->Divide( oldTotal );

  oldTotal->Write();
  newTotal->Write();
  totalRatio->Write();

  for( size_t i = 0; i < theInts.size(); ++i ){
     newFracs[ theInts.at(i) ]->Divide( newTotal );    
     newFracs[ theInts.at(i) ]->Write();

     std::string ratioName = "ratio_" + theInts.at(i);
     TH1D * ratio = (TH1D*)newFracs[ theInts.at(i) ]->Clone(ratioName.c_str());
     ratio->Divide(oldFracs.at( theInts.at(i) ) );
     ratios[ theInts.at(i) ] = ratio;
     ratio->Write();
 
  }

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
  TH1D * theHist = ratios.at( theInt );
  //std::cout << "Hist exists: " << theHist << std::endl;
  int theBin = theHist->FindBin( theMomentum );
  //std::cout << "Bin: " << theBin << std::endl;
  double theWeight = theHist->GetBinContent( theBin );
  //std::cout << "Weight: " << theWeight << std::endl;

  return theWeight;
}

TH1D * G4ReweightFinalState::GetIntRatio( std::string theInt ){
  //Add in check
  
  return ratios.at( theInt ); 
}

G4ReweightFinalState::~G4ReweightFinalState(){}
