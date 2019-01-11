#include "G4ReweightFitter.hh"
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
//  df.ParseXML(argv[1]);
//  df.LoadData();
//  df.SaveData(data_dir);

  
  BinonFitter bf(out);
//  bf.ParseXML(argv[1]);
//  bf.LoadData();
//  bf.SaveData(data_dir);
  
  std::vector< G4ReweightFitter* > fitters = {&df, &bf};

  for( size_t i = 0; i < fitters.size(); ++i ){
    fitters[i]->ParseXML(argv[1]);
    fitters[i]->LoadData();
    fitters[i]->SaveData(data_dir);
  }

  for( size_t i = 0; i < 1/* df.GetNSamples()*/; ++i ){
    std::string dir_name = "abs" + set_prec(df.GetSample(i).abs) + "_cex" + set_prec(df.GetSample(i).cex);

    out->cd();
    TDirectory * outdir = out->mkdir( dir_name.c_str() );

    std::cout << std::endl << "Reweighting. Abs: " << df.GetSample(i).abs << " Cex: " << df.GetSample(i).cex << std::endl;

    for( size_t iFitter = 0; iFitter < fitters.size(); ++iFitter ){
      auto theFitter = fitters[iFitter];
//      df.SetActiveSample(i, outdir);
//      df.GetMCGraphs();
//      std::cout << df.DoFit() << std::endl;
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
