#include "G4ReweightFitter.hh"
#include "G4ReweightHandler.hh"
#include "newDUETFitter.hh"
#include <vector>
#include <string>
#include "TVectorD.h"

#include "TFile.h"
#include "TH2D.h"
#include "TGraph2D.h"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

std::string set_prec(double);

int main(int argc, char ** argv){

  TFile * out = new TFile ("newFitter_try.root", "RECREATE");
  out->cd();
  TDirectory * data_dir = out->mkdir( "Data" );

  std::map< std::string, std::vector< G4ReweightFitter* > > mapSetsToFitters;

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(argv[1]);
  std::vector< fhicl::ParameterSet > exps = ps.get< std::vector< fhicl::ParameterSet > >("Experiments");

  std::cout << "Getting Experiments: "  << exps.size() << std::endl;
  for(size_t i = 0; i < exps.size(); ++i){

    G4ReweightFitter * exp = new G4ReweightFitter(out, exps.at(i));

    std::cout << std::endl;

    mapSetsToFitters[ exp->GetType() ].push_back( exp ); 

  }

  std::cout << "Sizes: " << mapSetsToFitters[ "C_piplus" ].size() << " " << mapSetsToFitters[ "C_piminus" ].size() << std::endl;

  newDUETFitter df(out);
  mapSetsToFitters["C_piplus"].push_back( &df );

  std::vector< std::string > sets;
  std::vector< fhicl::ParameterSet > FCLSets = ps.get< std::vector< fhicl::ParameterSet > >("Sets");
  
  for( size_t i = 0; i < FCLSets.size(); ++i ){
    sets.push_back( FCLSets[i].get< std::string >("Name") );
  }

  G4ReweightHandler handler;  
  handler.ParseFHiCL( FCLSets );

  std::vector< fhicl::ParameterSet > samples = ps.get< std::vector< fhicl::ParameterSet > >("Samples"); 
  std::cout << "Got " << samples.size() << " samples" << std::endl;
  size_t nSamples = samples.size();

  for( size_t i = 0; i < nSamples; ++i ){

    fhicl::ParameterSet sampleSet = samples[i];

    std::vector<std::pair<std::string, bool>>   tempRaw = sampleSet.get<std::vector<std::pair<std::string, bool>>>("Raw");
    std::map< std::string, bool > Raw = std::map< std::string, bool >(tempRaw.begin(), tempRaw.end());
    

    double abs = sampleSet.get<double>("abs");
    double cex = sampleSet.get<double>("cex");
    std::string Name = sampleSet.get<std::string>("Name");

    std::cout << Name << std::endl;
    std::cout << "abs: " << abs << std::endl;
    std::cout << "cex: " << cex << std::endl;

    
    std::vector< std::string >::iterator itSet = sets.begin();
    //std::map< std::string, bool >::iterator itRaw = Raw.begin();
//    for( itRaw; itRaw != Raw.end(); ++itRaw){
    for( itSet; itSet != sets.end(); ++itSet ){
      
      std::string theSet = *itSet; 
      
      std::cout << "Checking " << theSet << std::endl;

      FitSample theSample;
      theSample.abs = abs;
      theSample.cex = cex;
      theSample.dcex = 0.;
      theSample.inel = 0.;
      theSample.prod = 0.;

      if( Raw[ theSet ] ){

        handler.SetFiles( theSet );

        bool pim = ( (theSet).find("minus") != std::string::npos );
        std::cout << "PiMinus? " << pim << std::endl;
        theSample = handler.DoReweight( Name.c_str(), abs, cex, (theSet + "_" + Name + ".root"), pim );

      }
      else{
        
        std::vector< std::pair< std::string, std::string > > tempFiles;
        tempFiles = sampleSet.get< std::vector< std::pair< std::string, std::string > > >("Files");
        std::map< std::string, std::string > theFiles = std::map< std::string, std::string >(tempFiles.begin(), tempFiles.end());

        theSample.theFile = theFiles[ theSet ];
        theSample.theName = Name;

      }

      std::vector< G4ReweightFitter* > theFitters = mapSetsToFitters[ theSet ]; 
      for( size_t iFit = 0; iFit < theFitters.size(); ++iFit){
        std::cout << "Adding sample to " << theSet << std::endl;
        theFitters[iFit]->AddSample( theSample );
      }

    }
  }

 
  //Flatten the map to fitters while loading data for ease 
  std::vector< G4ReweightFitter* > allFitters;
 
  std::map< std::string, std::vector< G4ReweightFitter* > >::iterator itSet;
  itSet = mapSetsToFitters.begin();
  for( ; itSet != mapSetsToFitters.end(); ++itSet ){
 
    std::cout << "Loading Data for Set: " << itSet->first;
    std::vector< G4ReweightFitter* > fitters = itSet->second;
    for( size_t i = 0; i < fitters.size(); ++i ){
      fitters[i]->LoadData();
      fitters[i]->SaveData(data_dir);
      allFitters.push_back( fitters[i] );
    }
  }
  
  std::cout << "Have: " << nSamples << " samples" << std::endl;

  std::vector< double > abs_vector; 
  std::vector< double > cex_vector; 
  std::vector< double > chi2_vector; 

  for( size_t i = 0; i < nSamples; ++i ){

    double chi2 = 0.;
    
    //Just for getting the parameter values
    auto tempFitter = allFitters[0];
    FitSample tempSample = tempFitter->GetSample(i);
    std::string dir_name = tempSample.theName;
    std::cout << dir_name << std::endl;

    double abs =  tempSample.abs;
    double cex =  tempSample.cex;
    std::cout << "Vals: " << abs << " " << cex << std::endl;
    double inel = tempSample.inel;
    double prod = tempSample.prod;
    double dcex = tempSample.dcex;

    TDirectory * outdir = out->mkdir( dir_name.c_str() );

    for( size_t j = 0; j < allFitters.size(); ++j ){
    
        auto theFitter = allFitters[j];

        out->cd();
        outdir->cd();

        theFitter->SetActiveSample(i, outdir);
        theFitter->GetMCGraphs();
        double fit_chi2 = theFitter->DoFit();
        std::cout << fit_chi2 << std::endl;

        chi2 += fit_chi2;
    }

    abs_vector.push_back( abs );
    cex_vector.push_back( cex );
    chi2_vector.push_back( chi2 );
  }

  out->cd();

  TGraph2D * fitSurf = new TGraph2D( abs_vector.size(), &abs_vector[0], &cex_vector[0], &chi2_vector[0]); 
  
  fitSurf->Write("chi2_surf");
  out->Close();

  std::cout << "Done" << std::endl;

  return 0;
}

/*std::string set_prec(double input){
  std::stringstream stream_in; 
  stream_in << std::fixed << std::setprecision(2) << input;
  return stream_in.str();
}*/
