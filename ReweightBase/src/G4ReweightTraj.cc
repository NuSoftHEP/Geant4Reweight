#include "G4ReweightTraj.hh"
#include "G4ReweightStep.hh"

#include <iostream>
#include <math.h>
#include <algorithm>

G4ReweightTraj::G4ReweightTraj(int tid, int pid, int parid, int eventnum, std::pair<int,int> range){
  trackID = tid;
  PID = pid;
  parID = parid;
  eventNum = eventnum;
  stepRange = range;
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
  //double total = 0.;
  //double bias_total = 0.;
  std::map<std::string,double> total; 
  std::map<std::string,double> bias_total;  

  for(int i = 0; i < nRW; ++i){
    total[reweightable[i]] = 0.;
    bias_total[reweightable[i]] = 0.;
  }

  for(size_t is = 0; is < GetNSteps(); ++is){   
    auto theStep = GetStep(is);
        
/*    std::cout << "P: " << theStep->preStepPx << " " << theStep->preStepPy << " " << theStep->preStepPz << " " << 

    sqrt(theStep->preStepPx*theStep->preStepPx + theStep->preStepPy*theStep->preStepPy + theStep->preStepPz*theStep->preStepPz) <<std::endl;
    std::cout << "L: " << theStep->stepLength << std::endl;*/
    for(size_t ip = 0; ip < theStep->GetNActivePostProcs(); ++ip){
      auto theProc = theStep->GetActivePostProc(ip);

//      if(theProc.Name == "pi+Inelastic"){
//        total += (theStep->stepLength/theProc.MFP);
//        bias_total += ( (theStep->stepLength*bias) / theProc.MFP);
//      }
      
//      std::cout << theProc.Name << " " << theProc.MFP << std::endl; 

      if (total.count(theProc.Name)){
//        std::cout << theProc.Name << " " << theProc.MFP << std::endl;
        total[theProc.Name] += (10.*theStep->stepLength/theProc.MFP);
        
        if(theProc.Name == "pi+Inelastic"){
          bias_total[theProc.Name] += ( (10.*theStep->stepLength*bias) / theProc.MFP);
        }
        else{
          bias_total[theProc.Name] += ( (10.*theStep->stepLength) / theProc.MFP);
        }
      }

    }
  }
//  std::cout << "total: " << total["pi+Inelastic"] << std::endl;
  //std::cout << "? " << exp(-.5*total["pi+Inelastic"]) << std::endl;
  double xsecTotal = 0.;
  double bias_xsecTotal = 0.;
  for(int i = 0; i < nRW; ++i){
//    std::cout << reweightable[i] << " " << total[reweightable[i]] << " " << bias_total[reweightable[i]] << std::endl;
    xsecTotal += total[reweightable[i]];
    bias_xsecTotal += bias_total[reweightable[i]];
  }
//  std::cout << "Totals: " << xsecTotal << " " << bias_xsecTotal << std::endl;

  double weight;
/*  if(GetFinalProc() == "pi+Inelastic"){
    weight = (1 - exp( -1*bias_total ));
    weight = weight / (1 - exp( -1*total ));
  }*/
  double num, denom;  
  if(total.count(GetFinalProc()) && GetStep(GetNSteps() - 1)->stepLength > 0){
//     std::cout << "Found final proc " << GetFinalProc() << std::endl;
//     std::cout << "Pi+inel: " << total["pi+Inelastic"]  << " " << bias_total["pi+Inelastic"] << 
//     std::endl << "coulomb: " << total["CoulombScat"] << " " << bias_total["CoulombScat"] << std::endl;
//
//     std::cout << "Total: " << xsecTotal << " " << bias_xsecTotal << std::endl;
//     std::cout << "R: " << total[GetFinalProc()]/xsecTotal << " " << bias_total[GetFinalProc()]/bias_xsecTotal << std::endl;
//     std::cout << "p: " << (1 - exp( -1*xsecTotal ) ) << " " << (1 - exp( -1*bias_xsecTotal ) ) << std::endl;  

     num = bias_total[GetFinalProc()];
     num = num / bias_xsecTotal;
     num = num * (1 - exp( -1*bias_xsecTotal ) );

     denom = total[GetFinalProc()];
     denom = denom / xsecTotal;
     denom = denom * (1 - exp( -1*xsecTotal ) );


     weight = num/denom;
//     std::cout << GetFinalProc() << " " << weight << std::endl;

  }
  else{
    //weight = exp( total - bias_total );
    weight = exp( xsecTotal - bias_xsecTotal );
  }
//  std::cout <<"weight " << weight << std::endl; 
  return weight;
}

double G4ReweightTraj::GetWeight_Elast(double elast_bias){

  double elast_weight = 1.;
  double elast_total = 0.;
  double elast_bias_total = 0.;

  for(size_t is = 0; is < GetNSteps(); ++is){   
    auto theStep = GetStep(is);
        
    for(size_t ip = 0; ip < theStep->GetNActivePostProcs(); ++ip){
      auto theProc = theStep->GetActivePostProc(ip);

      //Elastic
      if(theProc.Name == "hadElastic"){
        elast_total += ( (10.*theStep->stepLength) / theProc.MFP );
        elast_bias_total += ( (10.*theStep->stepLength*elast_bias) / theProc.MFP );
      }
      /////////

    }

    //Assign the weight then reset the totals
  //  std::cout << theStep->stepChosenProc << std::endl;
    if(theStep->stepChosenProc == "hadElastic"){
//        std::cout << "Weighting Elast" << std::endl;

       elast_weight *= ( ( 1 - exp(-1.*elast_bias_total) ) / ( 1 - exp(-1.*elast_total) ) ); 
       elast_total = 0.;
       elast_bias_total = 0.;
    }

  }


  //Now multiply by the weight from the last elastic scatter
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
    nextPos = lastPos + theStep->stepLength; 
    px = theStep->preStepPx; 
    py = theStep->preStepPy; 
    pz = theStep->preStepPz; 
    sliceEnergy = sqrt( (px*px + py*py + pz*pz) + 139.57*139.57);

    std::string theProc = theStep->stepChosenProc; 
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
