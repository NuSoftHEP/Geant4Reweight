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

}

void G4ReweightFitter::GetMCGraphs(){
  std::cout << "Sample: " << std::endl
          << "\tabs: "  << ActiveSample->abs  << std::endl
          << "\tcex: "  << ActiveSample->cex  << std::endl
          << "\tdcex: " << ActiveSample->dcex << std::endl
          << "\tinel: " << ActiveSample->inel << std::endl
          << "\tprod: " << ActiveSample->prod << std::endl
          << "\tFile: " << ActiveSample->theFile << std::endl;

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
  std::cout << "Sample: " << std::endl
            << "\tabs: "  << ActiveSample->abs  << std::endl
            << "\tcex: "  << ActiveSample->cex  << std::endl
            << "\tdcex: " << ActiveSample->dcex << std::endl
            << "\tinel: " << ActiveSample->inel << std::endl
            << "\tprod: " << ActiveSample->prod << std::endl
            << "\tFile: " << ActiveSample->theFile << std::endl;

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
  std::map< std::string, TGraph * >::iterator itXSec = MC_xsec_graphs.begin();
  for( itXSec; itXSec != MC_xsec_graphs.end(); ++itXSec ){
    std::string name = itXSec->first;

    std::cout << "Fitting " << name << " now" << std::endl;

    TGraph * MC_xsec = itXSec->second;
    TGraphErrors * Data_xsec = Data_xsec_graphs.at(name);

    int nPoints = MC_xsec->GetN(); 
   
    double partial_chi2 = 0.;
    for( int i = 0; i < nPoints; ++i ){

      Data_xsec->GetPoint(i, x, Data_val);
      Data_err = Data_xsec->GetErrorY(i);
      MC_xsec->GetPoint(i, x, MC_val);


      partial_chi2 += (1. / nPoints ) * ( (Data_val - MC_val) / Data_err ) * ( (Data_val - MC_val) / Data_err );
    }
    
    /*
    fFitDir->cd();
    TVectorD chi2_val(1);
    chi2_val[0] = partial_chi2;
    chi2_val.Write( (name + "_chi2").c_str() );
    */

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

    //std::cout << "CHECKING " << std::endl;
    //std::cout << "abs: "  << theFitSample.abs << std::endl;
    //std::cout << "cex: "  << theFitSample.cex << std::endl;
    //std::cout << "File: " << theFitSample.theFile << std::endl;
   
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

  //std::cout << "Data: " << dataFile << std::endl;

  //fDataFileName = dataFile;
}

void G4ReweightFitter::SetActiveSample( size_t i, TDirectory * output_dir){ 
  ActiveSample = &samples[i]; 
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
