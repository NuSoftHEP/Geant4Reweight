#include "G4ReweightFitter.hh"
#include "newDUETFitter.hh"
#include <vector>
#include <string>

int main(int argc, char ** argv){

  newDUETFitter df;
  df.ParseXML(argv[1]);
  df.LoadData();


  for( size_t i = 0; i < df.GetNSamples(); ++i ){
    std::cout << std::endl << "Reweighting. Abs: " << df.GetSample(i).abs << " Cex: " << df.GetSample(i).cex << std::endl;
    df.SetActiveSample(i);
    df.GetMCGraphs();
    std::cout << df.DoFit() << std::endl;
  }


  return 0;
}
