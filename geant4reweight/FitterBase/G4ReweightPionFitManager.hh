#ifndef _G4ReweightPionFitManager_h
#define _G4ReweightPionFitManager_h

#include "geant4reweight/FitterBase/G4ReweightFitManager.hh"

#include <string>

class G4ReweightPionFitManager : public G4ReweightFitManager {

 using G4ReweightFitManager::G4ReweightFitManager; 

 public:

 void  SetExclusiveChannels();
 int GetNModelParam(std::string cut,bool use_reac=false) override;

};


#endif
