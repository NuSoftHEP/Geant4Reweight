#ifndef _G4ReweightPionFitManager_h
#define _G4ReweightPionFitManager_h

#include "G4ReweightFitManager.hh"


class G4ReweightPionFitManager : public  G4ReweightFitManager {

 using G4ReweightFitManager::G4ReweightFitManager; 

 public:

 void  SetExclusiveChannels();
 int GetNModelParam(std::string cut,bool use_reac=false) override;

};


#endif
