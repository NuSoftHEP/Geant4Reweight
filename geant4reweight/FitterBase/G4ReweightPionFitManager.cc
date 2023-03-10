#include "geant4reweight/FitterBase/G4ReweightPionFitManager.hh"

#include <string>
#include <vector>

void G4ReweightPionFitManager::SetExclusiveChannels(){
  AllExclChannels = {"total","elast","reac","abscx","cex","inel","dcex","abs","prod"};
}

          
int G4ReweightPionFitManager::GetNModelParam(std::string cut,bool use_reac){

  //find the exclusive channel in the AllExclChannelsVector
  std::vector<std::string>::iterator it;
  it = find(AllExclChannels.begin(),AllExclChannels.end(),cut);
  int i_dt = distance(AllExclChannels.begin(),it);
  int p = 0;
  //number of model parameters depends on the exclusive channel being studied
  switch(i_dt){
    case 0: 

      p = theElastParVals.size() + theParVals.size(); //total, use all of the parameters
      break;
    case 1:
      p = theElastParVals.size(); //elastic, use all of the elastic parameters num of elast params
      break;
    case 2: 
      p = theParVals.size(); //reac, use all the inelastic parameters
      break;
    case 3:
      if(!use_reac)        
        p = 2; //abscx - 2 parameters
      else
        p = theParVals.size(); //abscx but setting all reac using a single parameter
      break;
    default: 
      p = 1; //everything else is 1 parameter
  }
  return p;
}
