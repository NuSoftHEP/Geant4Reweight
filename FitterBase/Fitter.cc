#include "DUETFitter.hh"

int main(int argc, char ** argv){
  DUETFitter df; 
  df.LoadData(); 
  std::cout << df.DoFit() << std::endl;

  return 0;
}
