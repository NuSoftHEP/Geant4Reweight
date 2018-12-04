#include "DUETFitter.hh"

int main(int argc, char ** argv){

  DUETFitter df(argv[1], argv[3]); 
  df.ParseXML(argv[2]);
  df.LoadData(); 

  double step = .1;

/*  for( int i = -5; i < 6; ++i ){
    double norm = 1. + i * step;
    df.DoReweight(norm);
    df.LoadMC();
    std::cout << norm << " " << df.DoFit() << std::endl;
  }
*/

  df.LoadRawMC();

//  for( int i = -7; i < 2; ++i ){
//    for(int j = -7; j < 2; ++j ){
//
//      double norm_abs = 1. + i * step;
//      double norm_cex = 1. + j * step;
//
//      std::cout << std::endl << "Reweighting. Abs: " << norm_abs << " Cex: " << norm_cex << std::endl;
//
//      df.DoReweightFS( norm_abs, norm_cex );
//      df.LoadMC();
//      std::cout << df.DoFit() << std::endl;
//
//    }
//  }

  for( size_t i = 0; i < df.GetNSamples(); ++i ){
    std::cout << std::endl << "Reweighting. Abs: " << df.GetSample(i).abs << " Cex: " << df.GetSample(i).cex << std::endl;
    df.SetActiveSample(i);
    df.LoadMC();
    std::cout << df.DoFit() << std::endl;
  }

  return 0;
}
