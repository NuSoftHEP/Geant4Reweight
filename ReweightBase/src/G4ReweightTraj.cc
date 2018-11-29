#include "G4ReweightTraj.hh"
#include "G4ReweightStep.hh"

#include <iostream>
#include <math.h>
#include <algorithm>

G4ReweightTraj::G4ReweightTraj(int tid, int pid, int parid, int eventnum, std::pair<int,int> range) :
trackID(tid), PID(pid), parID(parid), eventNum(eventnum), stepRange(range){
  if( PID == 211 ){ 
    fInelastic = "pi+Inelastic";
  }
  else if( PID == -211 ){
     fInelastic = "pi-Inelastic";  
  }

}

G4ReweightTraj::~G4ReweightTraj(){
  children.clear();

  std::for_each(steps.begin(), steps.end(), DeleteVector<G4ReweightStep*>());  
  steps.clear();
}

void G4ReweightTraj::AddStep(G4ReweightStep * inputStep){
  steps.push_back(inputStep);
}

size_t G4ReweightTraj::GetNSteps(){
  return steps.size();
}

G4ReweightStep * G4ReweightTraj::GetStep(size_t is){
  size_t NSteps = GetNSteps();
  
  if ( NSteps == 0 ){
    std::cout << "No associated steps" << std::endl;
    return NULL;
  }
  else if (is > NSteps - 1) {
    std::cout << "Requested Step Out of Range" << std::endl <<
    "Please Provide index in Range [0, " << NSteps - 1 << "]" << std::endl;

    return NULL;
  }
  else{
    return steps.at(is); 
  }
}


bool G4ReweightTraj::SetParent(G4ReweightTraj * parTraj){
  
  //Check if the event nums match, parID matches trackID
  bool check = (  (eventNum == parTraj->eventNum) 
               && (parID == parTraj->trackID) );
  
//  std::cout << "Parent check: " << check << std::endl;
  bool check_2 = parTraj->AddChild(this);
  if(check && check_2){

//    std::cout << "Setting parent" << std::endl;
    //Set the pointer  
    parent = parTraj; 
  }

  return (check && check_2);
}

bool G4ReweightTraj::AddChild(G4ReweightTraj * childTraj){

  //Check if  the event nums match, and trackID matches child's parent
  bool check = (  (eventNum == childTraj->eventNum)
               && (trackID == childTraj->parID) );

//  std::cout << "Child check: " << check << std::endl;
  if(check){
    //Set the pointer
    children.push_back(childTraj);
//    std::cout << "Added child" << std::endl;
  }

  return check;

}

size_t G4ReweightTraj::GetNChilds(){
  return children.size();
}

G4ReweightTraj * G4ReweightTraj::GetChild(size_t ic){
  size_t NChilds = GetNChilds();
  if(NChilds == 0){
    std::cout << "Has no childs" << std::endl;
    return NULL;
  }
  else if(ic > NChilds - 1){
    std::cout << "Index out of range." << std::endl <<
    "Please Provide index in Range [0, " << NChilds - 1 << "]" << std::endl;
    return NULL;
  }
  else{
    return children.at(ic);
  }

}

std::string G4ReweightTraj::GetFinalProc(){
  return steps.back()->stepChosenProc;
}

double G4ReweightTraj::GetTotalLength(){
  double total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){
    total += steps.at(is)->stepLength;  
  }

  return total;
}

double G4ReweightTraj::GetWeight(double bias){
  double total, bias_total;

  //Decrement in the case an inelastic interaction occurred.
  size_t nsteps = GetNSteps();
  if( GetFinalProc() == fInelastic )nsteps--;

  for(size_t is = 0; is < nsteps; ++is){   

    auto theStep = GetStep(is);
        
    for(size_t ip = 0; ip < theStep->GetNActivePostProcs(); ++ip){

      auto theProc = theStep->GetActivePostProc(ip);

      if( theProc.Name == fInelastic ){

        total += (10.*theStep->stepLength/theProc.MFP);
        bias_total += ( (10.*theStep->stepLength*bias) / theProc.MFP);
      }
    }
  }


  double weight = exp( total - bias_total );
  if( GetFinalProc() == fInelastic ){
    weight = weight * bias;
  }
 
  return weight;
}

double G4ReweightTraj::GetWeight(TH1F * biasHist){
  double total, bias_total;

  //Decrement in the case an inelastic interaction occurred.
  size_t nsteps = GetNSteps();
  if( GetFinalProc() == fInelastic )nsteps--;

  for(size_t is = 0; is < nsteps; ++is){   

    auto theStep = GetStep(is);
        
    for(size_t ip = 0; ip < theStep->GetNActivePostProcs(); ++ip){

      auto theProc = theStep->GetActivePostProc(ip);

      if( theProc.Name == fInelastic ){

        total += (10.*theStep->stepLength/theProc.MFP);

        double theMom = theStep->GetFullPreStepP();
        int theBin    = biasHist->FindBin(theMom);
        
        double bias;
        if(theBin < 1 || theBin > biasHist->GetNbinsX() ){
          bias = 1.;
        }
        else{
          bias = biasHist->GetBinContent(theBin); 
        }

        bias_total += ( (10.*theStep->stepLength*bias) / theProc.MFP);

      }
    }
  }


  double weight = exp( total - bias_total );
  if( GetFinalProc() == fInelastic ){
    auto lastStep = GetStep( GetNSteps() - 1 );
    double theMom = lastStep->GetFullPreStepP();
    int theBin    = biasHist->FindBin(theMom);

    double bias;
    if( theBin < 1 || theBin > biasHist->GetNbinsX() ) bias = 1.;
    else bias = biasHist->GetBinContent(theBin); 

    weight = weight * bias;
  }
 
  return weight;
}


double G4ReweightTraj::GetWeightFunc(G4ReweightInter * biasInter){
  double total, bias_total;

  //Decrement in the case an inelastic interaction occurred.
  size_t nsteps = GetNSteps();
  if( GetFinalProc() == fInelastic )nsteps--;

  for(size_t is = 0; is < nsteps; ++is){   

    auto theStep = GetStep(is);
        
    for(size_t ip = 0; ip < theStep->GetNActivePostProcs(); ++ip){

      auto theProc = theStep->GetActivePostProc(ip);

      if( theProc.Name == fInelastic ){

        total += (10.*theStep->stepLength/theProc.MFP);
        
        double theMom = theStep->GetFullPreStepP();
        double bias;
        bias = biasInter->GetContent(theMom);
        bias_total += ( (10.*theStep->stepLength*bias) / theProc.MFP);

      }
    }
  }


  double weight = exp( total - bias_total );
  if( GetFinalProc() == fInelastic ){
    auto lastStep = GetStep( GetNSteps() - 1 );
    double theMom = lastStep->GetFullPreStepP();
    double bias = biasInter->GetContent(theMom);
    weight = weight * bias;
  }
 
  return weight;
}

double G4ReweightTraj::GetWeight_Elast(double elast_bias){

  double elast_weight = 1.;
  double elast_total = 0.;
  double elast_bias_total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){   

    auto theStep = GetStep(is);

    //Check if the chosen proc was elastic. 
    //If so, then multiply by the bias and don't
    //add to the totals
    if( theStep->stepChosenProc == "hadElastic" ){
      elast_weight *=  elast_bias;
    }
    //If it wasn't elastic, then add to the totals
    else{
      for(size_t ip = 0; ip < theStep->GetNActivePostProcs(); ++ip){

        auto theProc = theStep->GetActivePostProc(ip);

        if(theProc.Name == "hadElastic"){

          elast_total += ( (10.*theStep->stepLength) / theProc.MFP );
          elast_bias_total += ( (10.*theStep->stepLength*elast_bias) / theProc.MFP );

        }
      }
    }
  }

  //Now multiply by the factor for steps without elastic scatters 
  elast_weight *= exp( elast_total - elast_bias_total ); 

  return elast_weight;

}

double G4ReweightTraj::GetWeight_Elast(TH1F * elastBiasHist){

  double elast_weight = 1.;
  double elast_total = 0.;
  double elast_bias_total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){   

    auto theStep = GetStep(is);

    double theMom = theStep->GetFullPreStepP();
    int theBin    = elastBiasHist->FindBin(theMom);

    double elastBias;
    if(theBin < 1 || theBin > elastBiasHist->GetNbinsX() ) elastBias = 1.;
    else elastBias = elastBiasHist->GetBinContent(theBin); 

    //Check if the chosen proc was elastic. 
    //If so, then multiply by the bias and don't
    //add to the totals
    if( theStep->stepChosenProc == "hadElastic" ){
      elast_weight *=  elastBias;
    }
    //If it wasn't elastic, then add to the totals
    else{
      for(size_t ip = 0; ip < theStep->GetNActivePostProcs(); ++ip){

        auto theProc = theStep->GetActivePostProc(ip);

        if(theProc.Name == "hadElastic"){

          elast_total += ( (10.*theStep->stepLength) / theProc.MFP );
          elast_bias_total += ( (10.*theStep->stepLength*elastBias) / theProc.MFP );

        }
      }
    }
  }

  //Now multiply by the factor for steps without elastic scatters 
  elast_weight *= exp( elast_total - elast_bias_total ); 

  return elast_weight;
}

double G4ReweightTraj::GetWeightFunc_Elast(G4ReweightInter * elastBiasInter){

  double elast_weight = 1.;
  double elast_total = 0.;
  double elast_bias_total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){   

    auto theStep = GetStep(is);

    double theMom = theStep->GetFullPreStepP();
    double elastBias = elastBiasInter->GetContent(theMom);

    //Check if the chosen proc was elastic. 
    //If so, then multiply by the bias and don't
    //add to the totals
    if( theStep->stepChosenProc == "hadElastic" ){
      elast_weight *=  elastBias;
    }
    //If it wasn't elastic, then add to the totals
    else{
      for(size_t ip = 0; ip < theStep->GetNActivePostProcs(); ++ip){

        auto theProc = theStep->GetActivePostProc(ip);

        if(theProc.Name == "hadElastic"){

          elast_total += ( (10.*theStep->stepLength) / theProc.MFP );
          elast_bias_total += ( (10.*theStep->stepLength*elastBias) / theProc.MFP );

        }
      }
    }
  }

  //Now multiply by the factor for steps without elastic scatters 
  elast_weight *= exp( elast_total - elast_bias_total ); 

  return elast_weight;
}

int G4ReweightTraj::GetNElastic(){
  int total = 0;

  for(size_t is = 0; is < GetNSteps(); ++is){
    auto theStep = GetStep(is);

    if(theStep->stepChosenProc == "hadElastic")total++;
  }

  return total;
}

std::vector<double> G4ReweightTraj::GetElastDists(){
  std::vector<double>  dists; 
  double total = 0.;
  for(size_t is = 0; is < GetNSteps(); ++is){
    auto theStep = GetStep(is);
    total += theStep->stepLength;
    if(theStep->stepChosenProc == "hadElastic"){
      dists.push_back(total);
      total = 0.;
    }     
  }
  if(dists.size() != GetNElastic())
    std::cout << "ERROR WRONG NUMBER OF ELASTS" << std::endl;
  return dists;
}

std::vector< std::pair<double, int> > G4ReweightTraj::ThinSliceMethod(double res){
//  std::cout <<"NEW" << std::endl;

  std::vector< std::pair<double, int> > result;

  double sliceEnergy = 0.;  
  
  //First slice position
  double sliceEdge = res;
//  double sliceEdge = res*ceil(nextPos/res);
  double lastPos = 0.;
  double nextPos = 0.;
  double px,py,pz;
  
  int interactInSlice = 0;

  for(size_t is = 0; is < GetNSteps(); ++is){
    
    auto theStep = GetStep(is);
    nextPos = lastPos + theStep->deltaZ; 
    //nextPos = lastPos + theStep->stepLength; 
    px = theStep->preStepPx; 
    py = theStep->preStepPy; 
    pz = theStep->preStepPz; 
    sliceEnergy = sqrt( (px*px + py*py + pz*pz) + 139.57*139.57);

    std::string theProc = theStep->stepChosenProc; 
//    std::cout << "StepLen: " << theStep->stepLength << " Proc: " << theProc << std::endl;
    if( (theProc == "hadElastic" || theProc == "pi+Inelastic") ) interactInSlice++;
    //std::cout << nextPos << " " << sliceEdge << " " << theProc << std::endl;
     
    //Passed the slice edge or it's the last step, save Energy
    if( nextPos > sliceEdge || is == (GetNSteps() - 1) ){
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) ); 
      interactInSlice = 0;
      sliceEdge = res*ceil(nextPos/res);
    }


    lastPos = nextPos;
  }


  
  return result;
}

std::vector< std::pair<double, int> > G4ReweightTraj::ThinSliceBetheBloch(double res){

  std::vector< std::pair<double, int> > result;
  
  //First slice position
  double sliceEdge = res;
  double lastPos = 0.;
  double nextPos = 0.;
  double px,py,pz; 
  int interactInSlice = 0;

  //Get total distance traveled in z
  double totalDeltaZ = 0.;
  double disp = 0.;
  double oldDisp = 0.;
  int crossedSlices = 0; 

  int currentSlice = 0;
  int oldSlice = 0;

  double sliceEnergy = Energy;
  for(size_t is = 0; is < GetNSteps(); ++is){
    auto theStep = GetStep(is);

    disp += theStep->deltaZ;
    currentSlice = floor(disp/res);
    
    std::string theProc = theStep->stepChosenProc; 
    
    //Check to see if in a new slice and it's not the end
    if( oldSlice != currentSlice && is < GetNSteps() - 1){ 


      //Save Interaction info of the prev slice
      //and reset
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
      interactInSlice = 0;

      //Update the energy
      sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
      if( sliceEnergy - 139.57 < 0.){
        //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
        //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
        sliceEnergy = 0.0001;
      }    
      //If it's more than 1 slice, add in non-interacting slices
      for(int ic = 1; ic < abs( oldSlice - currentSlice ); ++ic){

        result.push_back( std::make_pair(sliceEnergy, 0.) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }      
      
      if( (theProc == "hadElastic" || theProc == "pi+Inelastic") ) interactInSlice++;      
    }
    //It's crossed a slice and it's the last step. Save both info
    else if( oldSlice != currentSlice && is == GetNSteps() - 1 ){
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
      interactInSlice = 0;
      
      //Update the energy
      sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
      if( sliceEnergy - 139.57 < 0.){
        //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
        //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
        sliceEnergy = 0.0001;
      }
      //If it's more than 1 slice, add in non-interacting slices
      for(int ic = 1; ic < abs( oldSlice - currentSlice ); ++ic){

        result.push_back( std::make_pair(sliceEnergy, 0.) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }
      
      //Save the last slice
      if( (theProc == "hadElastic" || theProc == "pi+Inelastic") ) interactInSlice++;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //It's the end, so just save this last info
    else if( oldSlice == currentSlice && is == GetNSteps() - 1 ){
      if( (theProc == "hadElastic" || theProc == "pi+Inelastic") ) interactInSlice++;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //Same slice, not the end. Check for interactions
    else{
      if( (theProc == "hadElastic" || theProc == "pi+Inelastic") ) interactInSlice++;
    }

    //Update oldslice
    oldSlice = currentSlice;
  }

  return result;
}

double G4ReweightTraj::BetheBloch(double energy){
  
  double K = .307075;   
  double rho = 1.390; 
  double Z = 18;
  double A = 40;
  double I = 188E-6;
  double mass = 139.57;
  double me = .511;
  //Need to make sure this is total energy, not KE
  double gamma = energy/mass;
  double beta = sqrt( 1. - (1. / (gamma*gamma)) );  double Tmax = 2 * me * beta*beta * gamma*gamma;

  double first = K * (Z/A) * rho / (beta*beta);
  double second = .5 * log(Tmax*Tmax/(I*I)) - beta*beta;

  double dEdX = first*second;
  return dEdX;  
}

std::vector< std::pair<double, int> > G4ReweightTraj::ThinSliceMethodInelastic(double res){

  std::vector< std::pair<double, int> > result;
  double sliceEnergy = 0.;  
  
  //First slice position
  double sliceEdge = res;
  double lastPos = 0.;
  double nextPos = 0.;
  double px,py,pz;
  
  int interactInSlice = 0;

  for(size_t is = 0; is < GetNSteps(); ++is){
    
    auto theStep = GetStep(is);
    nextPos = lastPos + theStep->deltaZ; 
    px = theStep->preStepPx; 
    py = theStep->preStepPy; 
    pz = theStep->preStepPz; 
    sliceEnergy = sqrt( (px*px + py*py + pz*pz) + 139.57*139.57);

    std::string theProc = theStep->stepChosenProc; 
    if( (theProc == "pi+Inelastic") ) interactInSlice++;
     
    //Passed the slice edge or it's the last step, save Energy
    if( nextPos > sliceEdge || is == (GetNSteps() - 1) ){
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) ); 
      interactInSlice = 0;
      sliceEdge = res*ceil(nextPos/res);
    }

    lastPos = nextPos;
  }
  return result;
}

std::vector< std::pair<double, int> > G4ReweightTraj::ThinSliceBetheBlochInelastic(double res){

  std::vector< std::pair<double, int> > result;
  
  //First slice position
  double sliceEdge = res;
  double lastPos = 0.;
  double nextPos = 0.;
  double px,py,pz; 
  int interactInSlice = 0;

  //Get total distance traveled in z
  double totalDeltaZ = 0.;
  double disp = 0.;
  double oldDisp = 0.;
  int crossedSlices = 0; 

  int currentSlice = 0;
  int oldSlice = 0;

  double sliceEnergy = Energy;
  for(size_t is = 0; is < GetNSteps(); ++is){
    auto theStep = GetStep(is);

    disp += theStep->deltaZ;
    currentSlice = floor(disp/res);
    
    std::string theProc = theStep->stepChosenProc; 
    
    //Check to see if in a new slice and it's not the end
    if( oldSlice != currentSlice && is < GetNSteps() - 1){ 


      //Save Interaction info of the prev slice
      //and reset
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
      interactInSlice = 0;

      //Update the energy
      sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
      if( sliceEnergy - 139.57 < 0.){
        //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
        //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
        sliceEnergy = 0.0001;
      }    
      //If it's more than 1 slice, add in non-interacting slices
      for(int ic = 1; ic < abs( oldSlice - currentSlice ); ++ic){

        result.push_back( std::make_pair(sliceEnergy, 0.) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }      
      
      if( theProc == "pi+Inelastic" ) interactInSlice++;      
    }
    //It's crossed a slice and it's the last step. Save both info
    else if( oldSlice != currentSlice && is == GetNSteps() - 1 ){
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
      interactInSlice = 0;
      
      //Update the energy
      sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
      if( sliceEnergy - 139.57 < 0.){
        //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
        //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
        sliceEnergy = 0.0001;
      }
      //If it's more than 1 slice, add in non-interacting slices
      for(int ic = 1; ic < abs( oldSlice - currentSlice ); ++ic){

        result.push_back( std::make_pair(sliceEnergy, 0.) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }
      
      //Save the last slice
      if( theProc == "pi+Inelastic" ) interactInSlice++;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //It's the end, so just save this last info
    else if( oldSlice == currentSlice && is == GetNSteps() - 1 ){
      if( theProc == "pi+Inelastic" ) interactInSlice++;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //Same slice, not the end. Check for interactions
    else{
      if( theProc == "pi+Inelastic" ) interactInSlice++;
    }

    //Update oldslice
    oldSlice = currentSlice;
  }

  return result;
}


std::vector<G4ReweightTraj*> G4ReweightTraj::HasChild(int childPID){
  std::vector<G4ReweightTraj*> childTrajs; 
  for(size_t ic = 0; ic < GetNChilds(); ++ic){
    if( GetChild(ic)->PID == childPID){
      childTrajs.push_back(GetChild(ic));
    }
  }
  return childTrajs;
}



double G4ReweightTraj::GetWeightFS(G4ReweightFinalState * theFS){
  return 0.;
}
