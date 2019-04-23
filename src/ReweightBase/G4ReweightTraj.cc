#include "G4ReweightTraj.hh"
#include "G4ReweightStep.hh"

#include <iostream>
#include <math.h>
#include <algorithm>

G4ReweightTraj::G4ReweightTraj(int tid, int pdg, int parid, int eventnum, std::pair<int,int> range) :
trackID(tid), PDG(pdg), parID(parid), eventNum(eventnum), stepRange(range){
  if( PDG == 211 ){ 
    fInelastic = "pi+Inelastic";
  }
  else if( PDG == -211 ){
     fInelastic = "pi-Inelastic";  
  }

}

G4ReweightTraj::~G4ReweightTraj(){
  children.clear();

//  std::for_each(steps.begin(), steps.end(), DeleteVector<G4ReweightStep*>());  
  for( size_t i = 0; i < steps.size(); ++i ){
    delete steps.at(i);
  }
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
  return steps.back()->GetStepChosenProc();
}

double G4ReweightTraj::GetTotalLength(){
  double total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){
    total += steps.at(is)->GetStepLength();  
  }

  return total;
}

int G4ReweightTraj::GetNElastic(){
  int total = 0;

  for(size_t is = 0; is < GetNSteps(); ++is){
    auto theStep = GetStep(is);

    if(theStep->GetStepChosenProc() == "hadElastic")
      total++;
  }

  return total;
}

std::vector<double> G4ReweightTraj::GetElastDists(){
  std::vector<double>  dists; 
  double total = 0.;
  for(size_t is = 0; is < GetNSteps(); ++is){
    auto theStep = GetStep(is);
    total += theStep->GetStepLength();
    if(theStep->GetStepChosenProc() == "hadElastic"){
      dists.push_back(total);
      total = 0.;
    }     
  }
  if(dists.size() != GetNElastic())
    std::cout << "ERROR WRONG NUMBER OF ELASTS" << std::endl;
  return dists;
}

std::vector< std::pair<double, int> > G4ReweightTraj::ThinSliceMethod(double res){

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
    nextPos = lastPos + theStep->GetDeltaZ(); 
    px = theStep->GetPreStepPx(); 
    py = theStep->GetPreStepPy(); 
    pz = theStep->GetPreStepPz(); 
    sliceEnergy = sqrt( (px*px + py*py + pz*pz) + 139.57*139.57);

    std::string theProc = theStep->GetStepChosenProc(); 
//    std::cout << "StepLen: " << theStep->stepLength << " Proc: " << theProc << std::endl;
    if( (theProc == "hadElastic" || theProc == fInelastic) ) interactInSlice++;
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

    disp += theStep->GetDeltaZ();
    currentSlice = floor(disp/res);
    
    std::string theProc = theStep->GetStepChosenProc(); 
    
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
      
      if( (theProc == "hadElastic" || theProc == fInelastic) ) interactInSlice++;      
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
    nextPos = lastPos + theStep->GetDeltaZ(); 
    px = theStep->GetPreStepPx(); 
    py = theStep->GetPreStepPy(); 
    pz = theStep->GetPreStepPz(); 
    sliceEnergy = sqrt( (px*px + py*py + pz*pz) + 139.57*139.57);

    std::string theProc = theStep->GetStepChosenProc(); 
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

    disp += theStep->GetDeltaZ();
    currentSlice = floor(disp/res);
    
    std::string theProc = theStep->GetStepChosenProc(); 
    
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


std::vector<G4ReweightTraj*> G4ReweightTraj::HasChild(int childPDG){
  std::vector<G4ReweightTraj*> childTrajs; 
  for(size_t ic = 0; ic < GetNChilds(); ++ic){
    if( GetChild(ic)->GetPDG() == childPDG){
      childTrajs.push_back(GetChild(ic));
    }
  }
  return childTrajs;
}

