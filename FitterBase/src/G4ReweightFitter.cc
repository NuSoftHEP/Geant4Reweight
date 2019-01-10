#include "G4ReweightFitter.hh"
#include "G4ReweightInter.hh"

#include <vector> 
#include <map>
#include <iomanip>
#include <sstream>

#include "TH1D.h"


void G4ReweightFitter::GetMCGraphs(){
  
  //here: have option for if Raw
  fMCTree = GetReweightFS();

  //Get the total 
  std::string drawCommand = "sqrt(Energy*Energy - 139.57*139.57)";
  std::string totalCommand = drawCommand + ">>total" + binning;
  fMCTree->Draw( totalCommand.c_str(), "", "goff" );

  TH1D * total = (TH1D*)gDirectory->Get("total");

  //Go through the map of cuts
  std::map< std::string, std::string >::iterator itCuts = cuts.begin();
  for( itCuts; itCuts != cuts.end(); ++itCuts ){


    std::string name = itCuts->first;      
    std::string theCut = itCuts->second;
    
    std::cout << name << std::endl;
    std::string cutCommand = drawCommand + ">>" + name + binning;

    std::string cutOption = weight + theCut;
    std::cout << cutOption.c_str() << std::endl;

    fMCTree->Draw( cutCommand.c_str(), cutOption.c_str(), "goff" );
   
    TH1D * hist = (TH1D*)gDirectory->Get( name.c_str() );
    
    hist->Divide( total );
    hist->Scale( scale );
    
    std::cout << "MC cut " << name << std::endl;

    std::vector< double > the_xsec; 
    for( int i = 0; i < points.size(); ++i ){

      std::cout << "point: " << points[i] << std::endl;
      std::cout << "bin: " << hist->FindBin( points[i] ) << std::endl;
      std::cout << "content: " << hist->GetBinContent( hist->FindBin( points[i] ) ) << std::endl;

      double content = hist->GetBinContent( hist->FindBin( points[i] ) );

      std::cout << "\txsec: " << content << std::endl;
      the_xsec.push_back( content );
    }

    MC_xsec_graphs[ name ] = new TGraph( points.size(), &points[0], &the_xsec[0] );
  }

}

TTree* G4ReweightFitter::GetReweightFS( /*FitSample theSample*/ ){
  std::cout << "Sample: " << std::endl
            << "\tabs: "  << ActiveSample->abs  << std::endl
            << "\tcex: "  << ActiveSample->cex  << std::endl
            << "\tdcex: " << ActiveSample->dcex << std::endl
            << "\tinel: " << ActiveSample->inel << std::endl
            << "\tprod: " << ActiveSample->prod << std::endl
            << "\tFile: " << ActiveSample->theFile << std::endl;

  TFile * RWFile = new TFile(ActiveSample->theFile.c_str(), "READ");
  TTree * RWTree = (TTree*)RWFile->Get("tree");
  std::cout << "Got tree: " << RWTree << std::endl;
  return RWTree;
}

double G4ReweightFitter::DoFit(){
  double Chi2 = 0.;
  double Data_val, MC_val, Data_err;
  double x;

  //Go through each cut defined for the experiment
  std::map< std::string, TGraph * >::iterator itXSec = MC_xsec_graphs.begin();
  for( itXSec; itXSec != MC_xsec_graphs.end(); ++itXSec ){
    std::string name = itXSec->first;

    std::cout << "Fitting " << name << " now" << std::endl;

    TGraph * MC_xsec = itXSec->second;
    TGraphErrors * Data_xsec = Data_xsec_graphs.at(name);

    int nPoints = MC_xsec->GetN(); 

    for( int i = 0; i < nPoints; ++i ){
      Data_xsec->GetPoint(i, x, Data_val);
      Data_err = Data_xsec->GetErrorY(i);
      MC_xsec->GetPoint(i, x, MC_val);

      Chi2 += (1 / nPoints ) * ( (Data_val - MC_val) / Data_err ) * ( (Data_val - MC_val) / Data_err );
    }
    
  }

  return Chi2;
}

void G4ReweightFitter::ParseXML(std::string FileName){

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

 
  tinyxml2::XMLElement * theSample = theRoot->FirstChildElement( "Sample" );
  if( !theSample ){
    std::cout << "Could Not get element " << std::endl;
    return;
  }


  
  while( theSample ){

    double abs, cex, dcex, prod, inel;

    std::map< std::string, double > factors = {
      { "abs",  0. },
      { "cex",  0. },
      { "dcex", 0. },
      { "prod", 0. },
      { "inel", 0. }
    };

   
    std::map< std::string , double >::iterator itFactor = factors.begin();
    
    tinyxml2::XMLError attResult;

    for( itFactor; itFactor != factors.end(); ++itFactor ){
      std::string name = itFactor->first;
      attResult = theSample->QueryDoubleAttribute(name.c_str(), &(itFactor->second) );

      if( attResult != tinyxml2::XML_SUCCESS ){
        std::cout << "Could not get " << name << std::endl;
        std::cout << "Setting to 1."  << std::endl;

        itFactor->second = 1.;
      }

      else std::cout << "Got " << name << ": " << itFactor->second << std::endl;


    }
    
    bool Raw;
    attResult = theSample->QueryBoolAttribute("Raw", &Raw);
    if( attResult != tinyxml2::XML_SUCCESS ){
      std::cout << "Could not get Raw" << std::endl;
    }

    std::string reweightFile;

    if( !Raw ){ 
      const char * reweightFileText = nullptr;

      reweightFileText = theSample->Attribute("File");
      if (reweightFileText != nullptr) reweightFile = reweightFileText;

      std::cout << "File: " << reweightFile << std::endl;
    }
    else std::cout << "Need to run reweighting" << std::endl;
    
    FitSample theFitSample;
    theFitSample.Raw  = Raw;
    theFitSample.abs  = factors["abs"];
    theFitSample.cex  = factors["cex"];
    theFitSample.inel = factors["inel"];
    theFitSample.prod = factors["prod"];
    theFitSample.dcex = factors["dcex"];
    theFitSample.theFile = reweightFile;

    std::cout << "CHECKING " << std::endl;
    std::cout << "abs: "  << theFitSample.abs << std::endl;
    std::cout << "cex: "  << theFitSample.cex << std::endl;
    std::cout << "File: " << theFitSample.theFile << std::endl;
   
    samples.push_back( theFitSample );
    
    theSample = theSample->NextSiblingElement("Sample");
  }

  tinyxml2::XMLElement * theData = theRoot->FirstChildElement("Data");
  if( !theData ){
    std::cout << "Could Not get Data " << std::endl;
    return;
  }

  std::string dataFile;
  const char * dataFileText = nullptr;
  dataFileText = theData->Attribute("File");
  if (dataFileText != nullptr) dataFile = dataFileText;

  std::cout << "Data: " << dataFile << std::endl;

  fDataFileName = dataFile;
}
