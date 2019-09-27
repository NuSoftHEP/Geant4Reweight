#ifndef G4ReweightTraj_h
#define G4ReweightTraj_h 1

#include "G4Reweighter.hh"

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

class G4Reweighter;

class G4ReweightTraj{

  public:

    G4ReweightTraj(int,int,int,int,std::pair<int,int>);
    ~G4ReweightTraj();
    
    int GetTrackID()  {return trackID; };
    int GetEventNum() {return eventNum; };
    int GetPDG()      {return PDG; };  
    int GetParID()    {return parID; };

    std::string GetInelasticType() {return fInelastic;};

    double GetEnergy(){return Energy;}; 
    void SetEnergy( double e ){ Energy = e; };

    G4ReweightTraj * GetParent(){return parent;};

    std::pair<int,int> GetStepRange(){return stepRange;};

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


    int GetNElastic();
    std::vector<double> GetElastDists();

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

    std::string fInelastic;
  
    int trackID;
    int eventNum;
    int PDG;
    int parID;

    double Energy; 

    std::pair<int,int> stepRange;

    G4ReweightTraj * parent;
    
};


#endif
