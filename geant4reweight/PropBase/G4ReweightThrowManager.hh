#ifndef G4ReweightThrowManager_h
#define G4ReweightThrowManager_h

#include "TDecompChol.h"
#include "TRandom3.h"

class TFile;

#include <string>
#include <vector>
#include <map>

class G4ReweightThrowManager{

  public:
    G4ReweightThrowManager( TFile & );

    bool Decomp();
    std::map< std::string, double > DoThrow(); 

    std::map< std::string, double > GetBestFit(){
      std::map< std::string, double > results;

      for( size_t i = 0; i < BestFitVals.size(); ++i )
        results[ ParNames[i] ] = BestFitVals[i];

      return results;
    }; 
  
  private:
    TDecompChol Cholesky;
    //TMatrixD *  CholMatrix; // unused
    std::vector< double >  BestFitVals;
    std::vector< std::string >  ParNames;
    bool DecompSuccess;
    TRandom3 rng;

};

#endif
