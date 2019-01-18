#include "G4ReweightFitter.hh"
#include "G4ReweightHandler.hh"
#include "newDUETFitter.hh"
#include "BinonFitter.hh"
#include <vector>
#include <string>
#include "TVectorD.h"

#include "TFile.h"
#include "TH2D.h"
#include "TGraph2D.h"

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

  double abs_start = 0.5;
  double delta_abs = .1;
  int n_abs = 10;
  double abs_end = abs_start + n_abs*delta_abs;

  double cex_start = 0.5;
  double delta_cex = .1; 
  int n_cex = 10;
  double cex_end = cex_start + n_cex*delta_cex;

 std::vector< double > abs_vector; 
 std::vector< double > cex_vector; 
 std::vector< double > chi2_vector; 


  for( int i = 0; i < n_abs; ++i ){
    for( int j = 0; j < n_cex; ++j ){

       double abs = abs_start + delta_abs*i;
       double cex = cex_start + delta_cex*j;
       
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
    
    FitSample theSample = df.GetSample(i);
    std::string dir_name = theSample.theName;
    std::cout << dir_name << std::endl;

    double abs =  theSample.abs;
    double cex =  theSample.cex;
    std::cout << "Vals: " << abs << " " << cex << std::endl;
    double inel = theSample.inel;
    double prod = theSample.prod;
    double dcex = theSample.dcex;

    out->cd();
    //check this when have multiple fitters
    TDirectory * outdir = out->mkdir( dir_name.c_str() );
    outdir->cd();

    double chi2 = 0.;

    for( size_t iFitter = 0; iFitter < fitters.size(); ++iFitter ){
      auto theFitter = fitters[iFitter];
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


  return 0;
}

/*std::string set_prec(double input){
  std::stringstream stream_in; 
  stream_in << std::fixed << std::setprecision(2) << input;
  return stream_in.str();
}*/
