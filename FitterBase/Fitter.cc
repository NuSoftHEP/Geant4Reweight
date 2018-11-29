#include "DUETFitter.hh"

int main(int argc, char ** argv){

  DUETFitter df(argv[1], argv[2]); 
  df.LoadData(); 
  df.DoReweight();
  df.LoadMC();
  std::cout << df.DoFit() << std::endl;

  return 0;
}
