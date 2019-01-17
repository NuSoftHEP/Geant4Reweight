#include "G4ReweightFitter.hh"
#include "G4ReweightHandler.hh"
#include "newDUETFitter.hh"
#include "BinonFitter.hh"
#include <vector>
#include <string>
#include "TVectorD.h"

#include "TFile.h"

std::string set_prec(double);

int main(int argc, char ** argv){

  TFile * out = new TFile ("newFitter_try.root", "RECREATE");
  out->cd();
  TDirectory * data_dir = out->mkdir( "Data" );

  newDUETFitter df(out);

  G4ReweightHandler handler;  
  handler.ParseXML("../FitterBase/reweight_handler.xml");


  handler.SetFiles("C_piplus");
  std::vector< FitSample > C_piplus_samples;

  double abs_start = 0.9;
  double abs_end = 1.0;
  double delta_abs = .1;

  double cex_start = 0.9;
  double cex_end = 0.9;
  double delta_cex = .1; 


  for( double abs = abs_start; abs <= abs_end; abs += delta_abs ){
    for( double cex = cex_start; cex <= cex_end; cex += delta_cex ){
       
       std::string name = "abs" + set_prec(abs) + "_cex" + set_prec(cex);
       FitSample theSample = handler.DoReweight( name.c_str(), abs, cex);
       C_piplus_samples.push_back( theSample );

       theSample.abs = abs;
       theSample.cex = cex;
       theSample.dcex = 0.;
       theSample.inel = 0.;
       theSample.prod = 0.;

       df.AddSample( theSample );
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
  
  std::cout << "Have: " << df.GetNSamples() << " samples" << std::endl;

  for( size_t i = 0; i < df.GetNSamples(); ++i ){
    
    FitSample theSample = C_piplus_samples[i];
    std::string dir_name = theSample.theName;
    std::cout << dir_name << std::endl;

    ///NEED TO FIX THIS
    TVectorD absvec(1, &(theSample.abs));
    TVectorD cexvec(1, &(theSample.cex));
    TVectorD inelvec(1, &(theSample.inel));
    TVectorD prodvec(1, &(theSample.prod));
    TVectorD dcexvec(1, &(theSample.dcex));

    out->cd();
    //check this when have multiple fitters
    TDirectory * outdir = out->mkdir( dir_name.c_str() );
    outdir->cd();

    absvec.Write("absval");
    cexvec.Write("cexval");
    inelvec.Write("inelval");
    prodvec.Write("prodval");
    dcexvec.Write("dcexval");


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
