#ifndef G4ReweightTraj_h
#define G4ReweightTraj_h 1


#include <vector>
#include <string>
#include <map>
#include <utility>

#include "TH1F.h"

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

class G4ReweightTraj{

  public:

    G4ReweightTraj(int,int,int,int,std::pair<int,int>);
    ~G4ReweightTraj();
    
    int trackID;
    int eventNum;
    int PID;
    int parID;
  
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
    
    double GetWeightFunc(TH1F *);
    double GetWeightFunc_Elast(TH1F *);
    
    int GetNElastic();
    //Returns a vector of the distances between elastic scatters.
    //Includes from start to first, then between all subsequent elastic scatters
    //  Start -> First, First -> Second, Second -> Third, etc.
    //No info from last elastic scatter to end of the list.
    std::vector<double> GetElastDists();

    std::vector< std::pair<double,int> > ThinSliceMethod(double);
    std::vector< std::pair<double,int> > ThinSliceBetheBloch(double);
    std::vector< std::pair<double,int> > ThinSliceMethodInelastic(double);
    double BetheBloch(double);

  protected:

  private:
    std::vector<G4ReweightStep *> steps;
    std::vector<G4ReweightTraj *> children;
    static const int nRW = 2;
    std::string reweightable[nRW] = {
      "pi+Inelastic",
      "Decay"
    };
};


#endif
