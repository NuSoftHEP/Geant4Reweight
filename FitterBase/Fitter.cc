#include "DUETFitter.hh"
#include <vector>
#include <string>

int main(int argc, char ** argv){

  std::vector< std::string > files = {
    "/pnfs/dune/scratch/users/calcuttj/C_thin_gps_0.root",
    "/pnfs/dune/scratch/users/calcuttj/C_thin_gps_1.root",
    "/pnfs/dune/scratch/users/calcuttj/C_thin_gps_2.root",
    "/pnfs/dune/scratch/users/calcuttj/C_thin_gps_3.root",
    "/pnfs/dune/scratch/users/calcuttj/C_thin_gps_4.root"
  };

  //DUETFitter df(argv[1]); 
  DUETFitter df(files);
  df.ParseXML(argv[2]);
  df.LoadData(); 

  double step = .1;

  //df.LoadRawMC();
  df.LoadRawMCVector();

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
//    df.LoadMC();
    df.LoadMCVector();
    std::cout << df.DoFit() << std::endl;
  }


  return 0;
}
