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

    G4ReweightTraj(int,int,int);
    //~G4ReweightTraj();
    
    int GetTrackID()  const {return fTrackID; };
    int GetPDG()      const {return fPDG; }; 
    int GetParID()    const {return fParentID; };

    std::string GetInelasticType() {return fInelastic;};

    double GetEnergy(){return fEnergy;}; 
    void SetEnergy( double e ){ fEnergy = e; };

    void AddStep(const G4ReweightStep & step);
    size_t GetNSteps() const;
    const G4ReweightStep & GetStep (size_t is) const;
  
    void AddChild(G4ReweightTraj *);
    size_t GetNChilds() const;
    G4ReweightTraj * GetChild(size_t) const;
    const std::vector<G4ReweightTraj*> & GetChildren() const {return fChildren;};
    std::vector<G4ReweightTraj*> HasChild(int) const; 

    std::string GetFinalProc() const;
    
    double GetTotalLength();
    size_t GetNElastic();
    std::vector<double> GetElastDists();

  private:
    std::vector<G4ReweightStep> fSteps;
    std::vector<G4ReweightTraj *> fChildren;

    std::string fInelastic;
  
    int fTrackID;
    int fPDG;
    int fParentID;

    double fEnergy;
};


#endif
