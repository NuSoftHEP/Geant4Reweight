#include "G4ReweightFitter.hh"
#include "G4ReweightHandler.hh"
#include "newDUETFitter.hh"
#include "BinonFitter.hh"
#include <vector>
#include <string>

#include "TFile.h"

std::string set_prec(double);

int main(int argc, char ** argv){

  TFile * out = new TFile ("newFitter_try.root", "RECREATE");
  out->cd();
  TDirectory * data_dir = out->mkdir( "Data" );

  newDUETFitter df(out);

  G4ReweightHandler handler;  
  handler.ParseXML("../FitterBase/reweight_handler.xml");


  handler.SetFiles_C_piplus();
  std::vector< FitSample > C_piplus_samples;
  for( int i = 0; i < 1; ++i ){
    for( int j = 0; j < 1; ++j ){
       
       FitSample theSample = handler.DoReweight( "abs1.00_cex1.00", 1., 1.);
       C_piplus_samples.push_back( theSample );
       df.AddSample( theSample );
       //add samples here
       //need to move fitters above this loop
    }
  }

  std::vector< FitSample > C_piminus_samples;
  
  
//  BinonFitter bf(out);
//  
//  std::vector< G4ReweightFitter* > fitters = {&df, &bf};
  std::vector< G4ReweightFitter* > fitters = {&df};
//
  for( size_t i = 0; i < fitters.size(); ++i ){
//    fitters[i]->ParseXML(argv[1]);
    fitters[i]->LoadData();
    fitters[i]->SaveData(data_dir);
  }
//
  for( size_t i = 0; i < 1/* df.GetNSamples()*/; ++i ){
//    std::string dir_name = "abs" + set_prec(df.GetSample(i).abs) + "_cex" + set_prec(df.GetSample(i).cex);
    std::string dir_name = C_piplus_samples[i].theName;
    std::cout << dir_name << std::endl;

    out->cd();
    TDirectory * outdir = out->mkdir( dir_name.c_str() );

//    std::cout << std::endl << "Reweighting. Abs: " << df.GetSample(i).abs << " Cex: " << df.GetSample(i).cex << std::endl;

    for( size_t iFitter = 0; iFitter < fitters.size(); ++iFitter ){
      auto theFitter = fitters[iFitter];
      theFitter->SetActiveSample(i, outdir);
      theFitter->GetMCGraphs();
      std::cout << theFitter->DoFit() << std::endl;
    }
  }

  out->Close();


  return 0;
}

/*std::string set_prec(double input){
  std::stringstream stream_in; 
  stream_in << std::fixed << std::setprecision(2) << input;
  return stream_in.str();
}*/
