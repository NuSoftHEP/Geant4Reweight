#ifndef G4ReweightTraj_h
#define G4ReweightTraj_h 1

#include "G4ReweightInter.hh"
#include "G4ReweightFinalState.hh"

#include <vector>
#include <string>
#include <map>
#include <utility>
#include <iostream>

#include "TH1F.h"
#include "TGraph.h" 

// Functor for deleting pointers in vector.
template<class T> class DeleteVector
{
  public:
    // Overloaded () operator.
    // This will be called by for_each() function.
    bool operator()(T x) const{
      // Delete pointer.
      delete x;
      return true;
    }
};
//Forward Declaration to let Traj know of step
class G4ReweightStep;

class G4ReweightFinalState;

class G4ReweightTraj{

  public:

    G4ReweightTraj(int,int,int,int,std::pair<int,int>);
    ~G4ReweightTraj();
    
    int trackID;
    int eventNum;
    int PID;
    int parID;

    std::string fInelastic;
  
    double Energy; 

    std::pair<int,int> stepRange;

    G4ReweightTraj * parent;
    
    void AddStep(G4ReweightStep *); 
    size_t GetNSteps();
    G4ReweightStep * GetStep(size_t is);
  
    bool SetParent(G4ReweightTraj *);
    bool AddChild(G4ReweightTraj *);
    size_t GetNChilds();
    G4ReweightTraj * GetChild(size_t);
    std::vector<G4ReweightTraj*> HasChild(int); 

    std::string GetFinalProc();
    
    double GetTotalLength();

    double GetWeight(double bias);
    double GetWeight_Elast(double elast_bias);

    double GetWeight(TH1F *);
    double GetWeight_Elast(TH1F *);
    
    double GetWeight(TGraph *);
    double GetWeightFunc(G4ReweightInter *);
    double GetWeightFunc_Elast(G4ReweightInter *);
    
    double GetWeightFS(G4ReweightFinalState *);
    double GetWeightFS(TGraph *);

    int GetNElastic();
    //Returns a vector of the distances between elastic scatters.
    //Includes from start to first, then between all subsequent elastic scatters
    //  Start -> First, First -> Second, Second -> Third, etc.
    //No info from last elastic scatter to end of the list.
    std::vector<double> GetElastDists();

    std::vector< std::pair<double,int> > ThinSliceMethod(double);
    std::vector< std::pair<double,int> > ThinSliceBetheBloch(double);
    std::vector< std::pair<double,int> > ThinSliceBetheBlochInelastic(double);
    std::vector< std::pair<double,int> > ThinSliceMethodInelastic(double);
    double BetheBloch(double);

    void AddWeight( double w ){
       weights.push_back(w);
    };

    double GetWeight( size_t i ){
      if( i > weights.size() - 1 ){
        std::cout << "Error: out of weights vector range" << std::endl;
        return -1;
      }
    };

    const std::vector< double > & GetWeights() const{
      return weights;
    };

  protected:

  private:
    std::vector<G4ReweightStep *> steps;
    std::vector<G4ReweightTraj *> children;

    std::vector< double > weights;
};


#endif
