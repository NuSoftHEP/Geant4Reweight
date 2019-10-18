#include <iostream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"

#include "geant4reweight/src/ReweightBase/G4ReweightTraj.hh"
#include "geant4reweight/src/ReweightBase/G4ReweightStep.hh"
#include "geant4reweight/src/ReweightBase/G4PiPlusReweighter.hh"

#include "geant4reweight/src/PropBase/G4ReweightParameterMaker.hh"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

//#ifdef FNAL_FHICL
#include "cetlib/filepath_maker.h"
//#endif

//Defines parseArgs and the command line options
#include "parse_reweight_args.hh"

void ProcessFlatTree(std::string &inFileName, std::string &outFileName, G4PiPlusReweighter &theReweighter );
std::vector< std::pair<double, int> > ThinSliceBetheBloch(G4ReweightTraj * theTraj, double res);
double BetheBloch(double);

int main(int argc, char ** argv){

  if(!parseArgs(argc, argv)) 
    return 0;

  fhicl::ParameterSet pset;
  // Configuration file lookup policy.
  char const* fhicl_env = getenv("FHICL_FILE_PATH");
  std::string search_path;

  if (fhicl_env == nullptr) {
    std::cerr << "Expected environment variable FHICL_FILE_PATH is missing or empty: using \".\"\n";
    search_path = ".";
  }
  else {
    search_path = std::string{fhicl_env};
  }

  cet::filepath_first_absolute_or_lookup_with_dot lookupPolicy{search_path};

  fhicl::make_ParameterSet(fcl_file, lookupPolicy, pset);


  std::string outFileName = pset.get< std::string >( "OutputFile" );
  if( output_file_override  != "empty" ){
    outFileName = output_file_override;
  }

  std::string inFileName  = pset.get< std::string >( "InputFile" ); 
  if( input_file_override  != "empty" ){
    inFileName = input_file_override;
  }

  std::string FracsFileName = pset.get< std::string >( "Fracs" );
  TFile FracsFile( FracsFileName.c_str(), "OPEN" );

  std::vector< fhicl::ParameterSet > FitParSets = pset.get< std::vector< fhicl::ParameterSet > >("ParameterSet");

  try{
    G4ReweightParameterMaker ParMaker( FitParSets );

    std::string XSecFileName = pset.get< std::string >( "XSec" );
    TFile XSecFile( XSecFileName.c_str(), "OPEN" );

    G4PiPlusReweighter * theReweighter = new G4PiPlusReweighter( &XSecFile, &FracsFile, ParMaker.GetFSHists(), ParMaker.GetElasticHist() ); 
  
    ProcessFlatTree(inFileName, outFileName, *theReweighter);

    std::cout << "done" << std::endl;
  }
  catch( const std::exception &e ){
    std::cout << "Caught exception" << std::endl;
  }

  return 0;
}

void ProcessFlatTree( std::string &inFileName, std::string &outFileName, G4PiPlusReweighter &theReweighter ){
  
  //Input variables
  int input_event;
  std::vector< double > * input_X  = new std::vector< double >();
  std::vector< double > * input_Y  = new std::vector< double >();
  std::vector< double > * input_Z  = new std::vector< double >();
  std::vector< double > * input_PX = new std::vector< double >();
  std::vector< double > * input_PY = new std::vector< double >();
  std::vector< double > * input_PZ = new std::vector< double >();
  std::vector< int > * elastic_indices = new std::vector< int >();
  int input_ID, input_PDG;
  std::string *input_EndProcess = new std::string();

  //Output variables
  int output_event;
  double weight, elastic_weight;
  double track_length;
  int PDG;
  std::string final_proc;
  double init_momentum;
  double final_momentum;
  std::vector< double > energies;
  std::vector< int > sliceInts;
  int nElasticScatters;
  //////////////////



  //Get input tree and set branches
  TFile inputFile( inFileName.c_str(), "OPEN" );
  TTree * inputTree = (TTree*)inputFile.Get("flattree/FlatTree");
  inputTree->SetBranchAddress( "event",         &input_event );
  inputTree->SetBranchAddress( "true_beam_X",   &input_X );
  inputTree->SetBranchAddress( "true_beam_Y",   &input_Y );
  inputTree->SetBranchAddress( "true_beam_Z",   &input_Z );
  inputTree->SetBranchAddress( "true_beam_PX",  &input_PX );
  inputTree->SetBranchAddress( "true_beam_PY",  &input_PY );
  inputTree->SetBranchAddress( "true_beam_PZ",  &input_PZ );
  inputTree->SetBranchAddress( "true_beam_PDG", &input_PDG );
  inputTree->SetBranchAddress( "true_beam_ID",  &input_ID );
  inputTree->SetBranchAddress( "true_beam_EndProcess", &input_EndProcess );
  inputTree->SetBranchAddress( "elastic_indices", &elastic_indices );


  //Create output
  TFile outputFile( outFileName.c_str(), "RECREATE" );
  TTree outputTree = TTree( "output", "" );
  outputTree.Branch( "event",        &output_event );
  outputTree.Branch( "weight",       &weight );
  outputTree.Branch( "elastic_weight",       &elastic_weight );
  outputTree.Branch( "track_length", &track_length );
  outputTree.Branch( "PDG", &PDG );
  outputTree.Branch( "final_proc", &final_proc );
  outputTree.Branch( "init_momentum", &init_momentum );
  outputTree.Branch( "final_momentum", &final_momentum );
  outputTree.Branch( "energies", &energies );
  outputTree.Branch( "sliceInts", &sliceInts );
  outputTree.Branch( "nElasticScatters", &nElasticScatters );


  for( int i = 0; i < inputTree->GetEntries(); ++i ){
    inputTree->GetEvent(i);
    PDG = input_PDG;
    final_proc = *input_EndProcess;

    double mass = 0.;
    if( PDG == 211 ) mass = 139.57;
    else if( PDG == 2212 ) mass = 938.28;

    //Make a G4ReweightTraj -- This is the reweightable object
    G4ReweightTraj theTraj(input_ID, input_PDG, 0, input_event, std::make_pair(0,0));

    //Create its set of G4ReweightSteps and add them to the Traj
    std::vector< G4ReweightStep* > allSteps;

    size_t nSteps = input_PX->size();
    if( nSteps < 2 ) continue;

    nElasticScatters = elastic_indices->size();

    for( size_t j = 1; j < nSteps; ++j ){
      
      if( j == input_PX->size() - 1 && std::find( elastic_indices->begin(), elastic_indices->end(), j ) != elastic_indices->end() )
        std::cout << "Warning: last step and elastic process" << std::endl;

      std::string proc = "default";
      if( j == input_PX->size() - 1 )
        proc = *input_EndProcess; 
      else if( std::find( elastic_indices->begin(), elastic_indices->end(), j ) != elastic_indices->end() )
        proc = "hadElastic";
        


      double deltaX = ( input_X->at(j) - input_X->at(j-1) );
      double deltaY = ( input_Y->at(j) - input_Y->at(j-1) );
      double deltaZ = ( input_Z->at(j) - input_Z->at(j-1) );

      double len = sqrt(
        std::pow( deltaX, 2 )  + 
        std::pow( deltaY, 2 )  + 
        std::pow( deltaZ, 2 )   
      );

      double preStepP[3] = { 
        input_PX->at(j-1)*1.e3,
        input_PY->at(j-1)*1.e3,
        input_PZ->at(j-1)*1.e3
      };

      double postStepP[3] = { 
        input_PX->at(j)*1.e3,
        input_PY->at(j)*1.e3,
        input_PZ->at(j)*1.e3
      };

      if( j == 1 ){
        theTraj.SetEnergy( sqrt( preStepP[0]*preStepP[0] + preStepP[1]*preStepP[1] + preStepP[2]*preStepP[2] + mass*mass ) );
      }              

      G4ReweightStep * theStep = new G4ReweightStep( input_ID, input_PDG, 0, input_event, preStepP, postStepP, len, proc ); 
      theStep->SetDeltaX( deltaX );
      theStep->SetDeltaY( deltaY );
      theStep->SetDeltaZ( deltaZ );

      theTraj.AddStep( theStep );
    }
    if( !(i % 1000) ) std::cout << "Entry: " << i << std::endl;

    //Get the weight from the G4ReweightTraj
    weight = theReweighter.GetWeight( &theTraj );
    elastic_weight = theReweighter.GetElasticWeight( &theTraj );
    track_length = theTraj.GetTotalLength();

    init_momentum = sqrt( theTraj.GetEnergy()*theTraj.GetEnergy() - mass*mass );
    final_momentum = sqrt( 
        std::pow( theTraj.GetStep( theTraj.GetNSteps() - 1 )->GetPreStepPx(), 2 ) + 
        std::pow( theTraj.GetStep( theTraj.GetNSteps() - 1 )->GetPreStepPy(), 2 ) + 
        std::pow( theTraj.GetStep( theTraj.GetNSteps() - 1 )->GetPreStepPz(), 2 ) 
    );
    
    output_event = input_event;

    std::vector< std::pair< double, int > > thin_slice = ThinSliceBetheBloch( &theTraj, .5 );

    energies.clear();
    sliceInts.clear();
    for( size_t j = 0; j < thin_slice.size(); ++j ){
      energies.push_back( thin_slice[j].first );
      sliceInts.push_back( thin_slice[j].second );
    }


    outputTree.Fill();
  }
  
  outputFile.cd();
  outputTree.Write();
  outputFile.Close();
}

std::vector< std::pair<double, int> > ThinSliceBetheBloch(G4ReweightTraj * theTraj, double res){

  std::vector< std::pair<double, int> > result;
  
  //First slice position
//  double sliceEdge = res;
//  double lastPos = 0.;
//  double nextPos = 0.;
//  double px,py,pz; 
  int interactInSlice = 0;

  //Get total distance traveled in z
//  double totalDeltaZ = 0.;
  double disp = 0.;
//  double oldDisp = 0.;
//  int crossedSlices = 0; 

  int currentSlice = 0;
  int oldSlice = 0;

  double sliceEnergy = theTraj->GetEnergy();
  size_t nSteps = theTraj->GetNSteps();
  for(size_t is = 0; is < nSteps; ++is){
    auto theStep = theTraj->GetStep(is);

    disp += theStep->GetStepLength();
    currentSlice = floor(disp/res);
    
    std::string theProc = theStep->GetStepChosenProc(); 
    
    //Check to see if in a new slice and it's not the end
    if( oldSlice != currentSlice && is < nSteps - 1){ 


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
        //std::cout << ic << std::endl;

        result.push_back( std::make_pair(sliceEnergy, 0) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }      
      
      if( ( theProc == "pi+Inelastic") ) interactInSlice = 1;      
    }
    //It's crossed a slice and it's the last step. Save both info
    else if( oldSlice != currentSlice && is == nSteps - 1 ){
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
        //std::cout << ic << std::endl;

        result.push_back( std::make_pair(sliceEnergy, 0) );

        //Update the energy again
        sliceEnergy = sliceEnergy - res*BetheBloch(sliceEnergy);
        if( sliceEnergy - 139.57 < 0.){
          //std::cout << "Warning! Negative energy " << sliceEnergy - 139.57 << std::endl;
          //std::cout << "Crossed " << oldSlice - currentSlice << std::endl;
          sliceEnergy = 0.0001;
        }
      }
      
      //Save the last slice
      if( (theProc == "pi+Inelastic") ) interactInSlice = 1;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //It's the end, so just save this last info
    else if( oldSlice == currentSlice && is == nSteps - 1 ){
      if( (theProc == "pi+Inelastic") ) interactInSlice = 1;
      result.push_back( std::make_pair(sliceEnergy, interactInSlice) );
    }
    //Same slice, not the end. Check for interactions
    else{
      if( (theProc == "pi+Inelastic") ) interactInSlice = 1;
    }

    //Update oldslice
    oldSlice = currentSlice;
  }

  return result;
}

double BetheBloch(double energy){
  
  //Need to make this configurable? Or delete...
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
