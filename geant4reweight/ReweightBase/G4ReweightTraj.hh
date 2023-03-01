#ifndef G4ReweightTraj_h
#define G4ReweightTraj_h 1

class G4ReweightStep;

#include <string>
#include <utility> // std::pair
#include <vector>

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

class G4ReweightTraj{

  public:

    G4ReweightTraj(int,int,int,int,std::pair<int,int>);
    ~G4ReweightTraj();
    
    int GetTrackID()  const {return trackID; };
    int GetEventNum() const {return eventNum; };
    int GetPDG()      const {return PDG; };  
    int GetParID()    const {return parID; };

    std::string GetInelasticType() {return fInelastic;};

    double GetEnergy(){return Energy;}; 
    void SetEnergy( double e ){ Energy = e; };

    G4ReweightTraj * GetParent(){return parent;};

    std::pair<int,int> GetStepRange(){return stepRange;};

    void AddStep(G4ReweightStep *); 
    size_t GetNSteps() const;
    G4ReweightStep * GetStep (size_t is) const;
  
    bool SetParent(G4ReweightTraj *);
    bool AddChild(G4ReweightTraj *);
    size_t GetNChilds() const;
    G4ReweightTraj * GetChild(size_t) const;
    std::vector<G4ReweightTraj*> HasChild(int) const; 

    std::string GetFinalProc() const;
    
    double GetTotalLength();


    size_t GetNElastic();
    std::vector<double> GetElastDists();

/*
    void AddWeight( double w ){
       weights.push_back(w);
    };

    double GetWeight( size_t i ){
      if( i > weights.size() - 1 ){
        std::cout << "Error: out of weights vector range" << std::endl;
        return -1;
      }
      
      return weights[i];
    };

    const std::vector< double > & GetWeights() const{
      return weights;
    };
*/

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
