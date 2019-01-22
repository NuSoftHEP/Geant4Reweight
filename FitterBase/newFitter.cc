#include "G4ReweightFitter.hh"
#include "G4ReweightHandler.hh"
#include "newDUETFitter.hh"
#include "BinonFitter.hh"
#include "Meirav_C_PiPlusFitter.hh"
#include "Meirav_C_PiMinusFitter.hh"
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

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(argv[2]);
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

  //BinonFitter bf(out);
  //Meirav_C_PiPlusFitter mcpf(out);
  //Meirav_C_PiMinusFitter mcmf(out);

  std::vector< std::string > sets = ps.get< std::vector< std::string > >("Sets");
  //mapSetsToFitters["C_piplus"] = {&df, &mcpf};
  //mapSetsToFitters["C_piminus"] = {&bf, &mcmf};

  std::map< std::string, bool > mapSetsToPiMinus;
  mapSetsToPiMinus["C_piminus"] = true;
  mapSetsToPiMinus["C_piplus"]  = false;


  G4ReweightHandler handler;  

  std::vector< fhicl::ParameterSet > new_sets = ps.get< std::vector< fhicl::ParameterSet > >("New_Sets"); 
  handler.ParseFHiCL( new_sets );

  double abs_start = 1.0;
  double delta_abs = .1;
  int n_abs = 1;

  double cex_start = 1.0;
  double delta_cex = .1; 
  int n_cex = 1;

  int nSamples = n_abs * n_cex;

  //replace vector of samples from fcl
  //each vector will contain the name of the sample, 
  //the abs and cex values
  //the file name
  //and whether it's already reweighted 
  //
  //if reweighted, add to the sample vector of the experiments
  //if raw, do reweight like normal
  for( std::vector<std::string>::iterator itSet = sets.begin(); itSet != sets.end(); ++itSet ){

    std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
    std::cout << "Set: " << *itSet << std::endl;

    handler.SetFiles( *itSet );

    for( int i = 0; i < n_abs; ++i ){
      for( int j = 0; j < n_cex; ++j ){

         double abs = abs_start + delta_abs*i;
         double cex = cex_start + delta_cex*j;
         
         std::string name = "abs" + set_prec(abs) + "_cex" + set_prec(cex);
         bool pim = mapSetsToPiMinus[ *itSet ];
         FitSample theSample = handler.DoReweight( name.c_str(), abs, cex, (*itSet + "_" + name + ".root"), pim );

         theSample.abs = abs;
         theSample.cex = cex;
         theSample.dcex = 0.;
         theSample.inel = 0.;
         theSample.prod = 0.;

         std::vector< G4ReweightFitter* > theFitters = mapSetsToFitters[ *itSet ]; 
         for( size_t iFit = 0; iFit < theFitters.size(); ++iFit){
           std::cout << "Adding sample to " << *itSet << std::endl;
           theFitters[iFit]->AddSample( theSample );
         }
      }
    }

    handler.CloseFSFile();

    std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
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
