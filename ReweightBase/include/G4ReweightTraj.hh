#ifndef G4ReweightTraj_h
#define G4ReweightTraj_h 1


#include <vector>
#include <string>
#include <map>

//Forward Declaration to let Traj know of step
class G4ReweightStep;

class G4ReweightTraj{

  public:

    G4ReweightTraj();
    ~G4ReweightTraj();
    
    int trackID;
    int eventNum;
    int PID;
    int parID;
   
    G4ReweightTraj * parent;
    
    void AddStep(G4ReweightStep *); 
    size_t GetNSteps();
    G4ReweightStep * GetStep(size_t is);
    //Possibly: 
    
    //Function: append traj
    //Function: set parent
    //Function: return index of main interaction (i.e. scattering)

  protected:

  private:
    std::vector<G4ReweightStep *> steps;
    std::vector<G4ReweightTraj *> children;

};


#endif
