#include "Geant4/G4HadronInelasticProcess.hh"
#include "Geant4/G4PionPlus.hh"
#include "Geant4/G4PionMinus.hh"
#include "Geant4/G4Proton.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4StepPoint.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4ProcessVector.hh"
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4VParticleChange.hh"

#include "geant4reweight/src/PredictionBase/G4CascadeDetectorConstruction.hh"
#include "geant4reweight/src/PredictionBase/G4CascadePhysicsList.hh"

#include <utility>
#include <iostream>
#include <fstream>

#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TVectorD.h"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

//#ifdef FNAL_FHICL
#include "cetlib/filepath_maker.h"
//#endif

std::string fcl_file;

bool range_override = false;
double range_low_override = 0.;
double range_high_override = 0.;

std::string output_file_override = "empty"; 

int ncasc_override = 0;
int ndiv_override = 0;
int type_override = -999;


struct CascadeConfig{

  CascadeConfig(){};
  CascadeConfig( fhicl::ParameterSet & pset ){
    nCascades = pset.get< size_t >("NCascades");
    if( ncasc_override > 0 ) 
      nCascades = ncasc_override;

    type      = pset.get< int >("Type");
    if( type_override != -999 )
      type = type_override;

    range = pset.get< std::pair< double, double > >("Range");
    if( range_override) 
      range = std::make_pair(range_low_override, range_high_override);


    nDivisions = pset.get< size_t >("NDivisions");
    if( ndiv_override > 0 ) 
      nDivisions = ndiv_override;

    outFileName = pset.get< std::string >("Outfile");
     if( output_file_override  != "empty" ){
      outFileName = output_file_override;
    }

    fhicl::ParameterSet MaterialParameters = pset.get< fhicl::ParameterSet >("Material");
    MaterialName = MaterialParameters.get< std::string >( "Name" );
    MaterialZ = MaterialParameters.get< int >( "Z" );
    MaterialMass = MaterialParameters.get< double >( "Mass" );
    MaterialDensity = MaterialParameters.get< double >( "Density" );

  };

  size_t nCascades; 
  size_t nDivisions;
  int type;
  std::pair< double, double > range;

  std::string outFileName;

  int MaterialZ;
  double MaterialMass;
  double MaterialDensity;
  std::string MaterialName;

};

struct TrackStepPart{
  G4Track * theTrack;
  G4Step * theStep;
  G4DynamicParticle * dynamic_part;
};

CascadeConfig configure(fhicl::ParameterSet & pset);
std::vector< double > fillMomenta(CascadeConfig theConfig);
TrackStepPart initTrackAndPart(G4ParticleDefinition * part_def, G4Material * theMaterial );

bool parseArgs(int argc, char* argv[]);
void initRunMan( G4RunManager & rm );
void makeFCLParameterSet( fhicl::ParameterSet & pset);
G4HadronInelasticProcess * getInelasticProc( /*G4HadronInelasticProcess * inelastic_proc, */G4ParticleDefinition * part_def, std::string inel_name );


int main(int argc, char * argv[]){

  if( !parseArgs(argc, argv) ) 
    return 0;

  fhicl::ParameterSet pset;
  makeFCLParameterSet(pset);

  CascadeConfig theConfig( pset );

  if( theConfig.nDivisions < 1 ){
    std::cout << "Please give NDivision >= 1" << std::endl;
    return 0;
  }
  if( theConfig.range.second < theConfig.range.first ){
    std::cout << "Please give range in increasing order" << std::endl;
    return 0;
  }

  std::vector< double > momenta = fillMomenta( theConfig );

  TFile * fout = new TFile( theConfig.outFileName.c_str(), "RECREATE");

  TVectorD m_vec(1);
  m_vec[0] = theConfig.MaterialMass;
  m_vec.Write("Mass");

  TVectorD d_vec(1);
  d_vec[0] = theConfig.MaterialDensity;
  d_vec.Write("Density");

  std::cout << "Got mass & density" << std::endl;

  TTree * tree = new TTree("tree","");  
  int nPi0 = 0, nPiPlus = 0, nPiMinus = 0, nProton, nNeutron;
  double momentum;
  tree->Branch( "nPi0", &nPi0 );
  tree->Branch( "nPiPlus", &nPiPlus );
  tree->Branch( "nPiMinus", &nPiMinus );
  tree->Branch( "nProton", &nProton );
  tree->Branch( "nNeutron", &nNeutron );
  tree->Branch( "momentum", &momentum );

  std::cout << "Initializing" << std::endl;
  //Initializing
  G4RunManager rm;
  initRunMan( rm );
  ////

  std::cout << "Initialized" << std::endl;

  G4PionPlus  * piplus = 0x0;
  G4PionMinus * piminus = 0x0;
  G4Proton    * proton = 0x0;
  G4Neutron   * neutron = 0x0;
  G4ParticleDefinition * part_def = 0x0;
  std::string inel_name = "";
  switch( theConfig.type ){
    case 211:
      std::cout << "Chose PiPlus" << std::endl;
      part_def = piplus->Definition();
      inel_name = "pi+Inelastic";
      break;
    
    case -211:
      std::cout << "Chose PiMinus" << std::endl;
      part_def = piminus->Definition();
      inel_name = "pi-Inelastic";
      break;

    case 2212: 
    {
      std::cout << "Chose Proton" << std::endl;
      part_def = proton->Definition();
      inel_name = "protonInelastic";

/*
      //Default for now
      ++momenta.back(); 
      std::vector< double > total_ys(momenta.size(), 1.);
      TGraph total_gr(momenta.size(), &momenta[0], &total_ys[0]);
      fout->cd();
      total_gr.Write( "total" );
      fout->Close();
      //delete rm;
      return 1;
*/    
      //Returns before this, but... eh
      break;
    }

    case 2112:
    {
      std::cout << "Chose Neutron" << std::endl;
      part_def = neutron->Definition();
      inel_name = "neutronInelastic";
/*
      //Default for now
      ++momenta.back(); 
      std::vector< double > total_ys(momenta.size(), 1.);
      TGraph total_gr(momenta.size(), &momenta[0], &total_ys[0]);
      fout->cd();
      total_gr.Write( "total" );
      fout->Close();
      //delete rm;
      return 1;
*/    
      //Returns before this, but... eh
      break;
      
    }
    

    default:
      std::cout << "Please specify either 211, -211, 2112, or 2212" << std::endl;
      fout->cd();
      fout->Close();
      //delete rm;
      return 0;
      
  }

  //G4HadronInelasticProcess * inelastic_proc = 0x0;
  G4HadronInelasticProcess * inelastic_proc = getInelasticProc( /*inelastic_proc, */part_def, inel_name );

  std::cout << "inelastic_proc: " << inelastic_proc << std::endl;

  if( !inelastic_proc ){
    //delete rm;
    return 0;
  }

  G4Material * theMaterial = new G4Material(theConfig.MaterialName, theConfig.MaterialZ, theConfig.MaterialMass*g/mole, theConfig.MaterialDensity*g/cm3);

  auto track_par = initTrackAndPart( part_def, theMaterial );
  G4Track * theTrack = track_par.theTrack;
  G4Step * theStep   = track_par.theStep;
  G4DynamicParticle * dynamic_part = track_par.dynamic_part;

  for( size_t iM = 0; iM < momenta.size(); ++iM ){
    std::cout << "Momentum: " << momenta.at(iM) << std::endl;
    double theMomentum = momenta[iM]; 
    double KE = sqrt( theMomentum*theMomentum + part_def->GetPDGMass()*part_def->GetPDGMass() ) - part_def->GetPDGMass();
    dynamic_part->SetKineticEnergy( KE );
    for( size_t iC = 0; iC < theConfig.nCascades; ++iC ){

      if( !(iC % 1000) ) std::cout << "\tCascade: " << iC << std::endl;

      nPi0 = 0; 
      nPiPlus = 0; 
      nPiMinus = 0;
      nProton = 0;
      nNeutron = 0;
      momentum = dynamic_part->GetTotalMomentum();
      G4VParticleChange * thePC = inelastic_proc->PostStepDoIt( *theTrack, *theStep );

      size_t nSecondaries = thePC->GetNumberOfSecondaries();
      for( size_t i = 0; i < nSecondaries; ++i ){
        auto part = thePC->GetSecondary(i)->GetDynamicParticle();

        switch( part->GetPDGcode() ){
          case( 211 ):
            ++nPiPlus; break;
          case( -211 ):
            ++nPiMinus; break;
          case( 111 ):
            ++nPi0; break;          
          case( 2212 ):
            ++nProton; break;
          case( 2112 ):
            ++nNeutron; break;
          default:
            break;
        }
      }
      
      thePC->SetVerboseLevel(0);
      thePC->Initialize(*theTrack);

      tree->Fill();

    }
  }

  fout->cd();
  tree->Write();
 
  std::map< std::string, std::string > cuts;
  //Define cuts and make graphs out of the results
  //
  //
  //These can be made FHiCL-able 
  if( theConfig.type == 211 ){
    cuts["abs"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 0";
    cuts["prod"] = " (nPi0 + nPiPlus + nPiMinus) > 1";
    cuts["cex"] = "nPi0 == 1 && nPiPlus == 0 && nPiMinus == 0";
    cuts["inel"] = "nPi0 == 0 && nPiPlus == 1 && nPiMinus == 0";
    cuts["dcex"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 1";
  }
  else if( theConfig.type == -211 ){
    cuts["abs"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 0";
    cuts["prod"] = " (nPi0 + nPiPlus + nPiMinus) > 1";
    cuts["cex"] = "nPi0 == 1 && nPiPlus == 0 && nPiMinus == 0";
    cuts["inel"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 1";
    cuts["dcex"] = "nPi0 == 0 && nPiPlus == 1 && nPiMinus == 0";
  }
  if( theConfig.type == 2212 || theConfig.type == 2112 ){
    cuts["0n0p"] = "nProton == 0 && nNeutron == 0";
    cuts["1n0p"] = "nProton == 0 && nNeutron == 1";
    cuts["0n1p"] = "nProton == 1 && nNeutron == 0";
    cuts["1n1p"] = "nProton == 1 && nNeutron == 1";
    cuts["Other"] = "nProton > 1 || nNeutron > 1";
  }

  int nbins = int(theConfig.range.second) + 1;
  std::string binning = "(" + std::to_string(nbins) + ",0," + std::to_string(theConfig.range.second + 1.) + ")";

  std::string draw_total = "momentum>>total" + binning;
  tree->Draw( draw_total.c_str(), "", "goff" );
  TH1F * total = (TH1F*)gDirectory->Get("total");
  std::vector< int > bins;
  for( int i = 1; i <= total->GetNbinsX(); ++i ){
    if( total->GetBinContent( i ) > 0. ){
      bins.push_back( i );
      std::cout << i << std::endl;
    }
  }

  for( auto itCut = cuts.begin(); itCut != cuts.end(); ++itCut ){
    std::cout << itCut->first << std::endl;
    std::string name = "h" + itCut->first;
    std::string draw = "momentum>>" + name + binning;
    std::cout << "Draw: " << draw << std::endl;

    tree->Draw( draw.c_str(), itCut->second.c_str(), "goff" ); 
    TH1F * hist = (TH1F*)gDirectory->Get( name.c_str() );
    hist->Divide( total );

    std::vector< double > xs,ys;
    
    for( size_t i = 0; i < bins.size(); ++i ){
      xs.push_back( bins.at(i) );
      ys.push_back( hist->GetBinContent(bins.at(i)) );
    }
    TGraph gr(xs.size(), &xs[0], &ys[0]);
    gr.Write( itCut->first.c_str() );

  }


  fout->Close();
  //delete rm;

  return 0;
}

bool parseArgs(int argc, char ** argv){

  bool found_fcl_file = false;

  bool found_range_low = false;
  bool found_range_high = false;

  for( int i = 1; i < argc; ++i ){
    if( ( strcmp( argv[i], "--help" )  == 0 ) || ( strcmp( argv[i], "-h" ) == 0 ) ){
      std::cout << "Usage: ./G4Cascade -c <cascade_config>.fcl [options]" << std::endl;
      std::cout << std::endl;
      std::cout << "Options: " << std::endl;
      std::cout << "\t-o <output_file_override>.root" << std::endl;
      std::cout << "\t--low <range_low_value> (must be provided along with -high)" << std::endl; 
      std::cout << "\t--high <range_high_value> (must be provided along with -low)" << std::endl; 
      std::cout << "\t--NC <number_of_cascades_per_point> (must be > 0 to work)" << std::endl;
      std::cout << "\t--ND <divisor_of_range> (must be > 0 to work)" << std::endl;
      std::cout << "\t-t <probe type> (currently only works with 211 and -211)" << std::endl;

      return false;
    }

    else if( strcmp( argv[i], "-c" ) == 0 ){
      fcl_file = argv[i+1];
      found_fcl_file = true;
    }

    else if( strcmp( argv[i], "-o" ) == 0 ){
      output_file_override = argv[i+1];
    }

    else if( strcmp( argv[i], "--low" ) == 0 ){
      range_low_override = atof( argv[i+1] ); 
      found_range_low = true;
    }

    else if( strcmp( argv[i], "--high" ) == 0 ){
      range_high_override = atof( argv[i+1] ); 
      found_range_high = true;
    }

    else if( strcmp( argv[i], "--NC" ) == 0 ){
      ncasc_override = atoi( argv[i+1] ); 
    }

    else if( strcmp( argv[i], "--ND" ) == 0 ){
      ndiv_override = atoi( argv[i+1] ); 
    }

    else if( strcmp( argv[i], "-t" ) == 0 ){
      type_override = atoi( argv[i+1] );
    }
  }

  if( !found_fcl_file ){
    std::cout << "Error: Must provide fcl file with option '-c'" << std::endl;
    return false;
  }

  if( found_range_low != found_range_high ){
    std::cout << "Error: If overriding fcl range, you must provide both values with options '-l' and '-h'" << std::endl;
    return false;
  }
  else{
    range_override = found_range_low;
  }

  

  return true;
}

void initRunMan( G4RunManager & rm ){
  rm.SetUserInitialization(new G4CascadeDetectorConstruction);
  rm.SetUserInitialization(new G4CascadePhysicsList);
  rm.Initialize();
  rm.ConfirmBeamOnCondition();
  rm.ConstructScoringWorlds();
  rm.RunInitialization();
}

void makeFCLParameterSet( fhicl::ParameterSet & pset){
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
}

/*void getInelasticProc( G4HadronInelasticProcess * inelastic_proc, G4ParticleDefinition * part_def, std::string inel_name ){

  G4ProcessManager * pm = part_def->GetProcessManager();
  G4ProcessVector  * pv = pm->GetProcessList();
  
  for( int i = 0; i < pv->size(); ++i ){
    G4VProcess * proc = (*pv)(i);
    std::string theName = proc->GetProcessName();
    std::cout <<  theName << std::endl;
    if( theName == inel_name ){
      std::cout << "Found inelastic" << std::endl;
      inelastic_proc = (G4HadronInelasticProcess*)proc;
    }
  }
}
*/


G4HadronInelasticProcess * getInelasticProc( G4ParticleDefinition * part_def, std::string inel_name ){

  G4ProcessManager * pm = part_def->GetProcessManager();
  G4ProcessVector  * pv = pm->GetProcessList();
  
  //for( int i = 0; i < pv->size(); ++i ){
  for( size_t i = 0; i < (size_t)pv->size(); ++i ){
    G4VProcess * proc = (*pv)(i);
    std::string theName = proc->GetProcessName();
    std::cout <<  theName << std::endl;
    if( theName == inel_name ){
      std::cout << "Found inelastic" << std::endl;
      return (G4HadronInelasticProcess*)proc;
    }
  }
  return 0x0;
}


CascadeConfig configure(fhicl::ParameterSet & pset){
  CascadeConfig theConfig;

  theConfig.nCascades = pset.get< size_t >("NCascades");
  if( ncasc_override > 0 ) 
    theConfig.nCascades = ncasc_override;

  theConfig.type      = pset.get< int >("Type");
  if( type_override != -999 )
    theConfig.type = type_override;

  theConfig.range = pset.get< std::pair< double, double > >("Range");
  if( range_override) 
    theConfig.range = std::make_pair(range_low_override, range_high_override);


  theConfig.nDivisions = pset.get< size_t >("NDivisions");
  if( ndiv_override > 0 ) 
    theConfig.nDivisions = ndiv_override;

  theConfig.outFileName = pset.get< std::string >("Outfile");
   if( output_file_override  != "empty" ){
    theConfig.outFileName = output_file_override;
  }

  fhicl::ParameterSet MaterialParameters = pset.get< fhicl::ParameterSet >("Material");
  theConfig.MaterialName = MaterialParameters.get< std::string >( "Name" );
  theConfig.MaterialZ = MaterialParameters.get< int >( "Z" );
  theConfig.MaterialMass = MaterialParameters.get< double >( "Mass" );
  theConfig.MaterialDensity = MaterialParameters.get< double >( "Density" );


  return theConfig;
}

std::vector< double > fillMomenta( CascadeConfig theConfig ){
  std::cout << "Range: " << theConfig.range.first << " " << theConfig.range.second << std::endl;
  std::vector< double > momenta;
  double delta = theConfig.range.second - theConfig.range.first;
  double step = delta / theConfig.nDivisions; 
  for( size_t i = 0; i <= theConfig.nDivisions; ++i ){
    momenta.push_back( theConfig.range.first + i * step );
  }

  return momenta;
}

TrackStepPart initTrackAndPart(G4ParticleDefinition * part_def, G4Material * theMaterial ){
  G4DynamicParticle * dynamic_part = new G4DynamicParticle(part_def, G4ThreeVector(0.,0.,1.), 0. );
  std::cout << "PDG: " << dynamic_part->GetPDGcode() << std::endl;

  G4Track * theTrack = new G4Track( dynamic_part, 0., G4ThreeVector(0.,0.,0.) );
  G4Step * theStep = new G4Step();
  G4StepPoint * thePoint = new G4StepPoint();
  thePoint->SetMaterial( theMaterial );
  theStep->SetPreStepPoint( thePoint );
  theTrack->SetStep( theStep );

  TrackStepPart results;
  results.theTrack = theTrack;
  results.theStep = theStep;
  results.dynamic_part = dynamic_part;
  return results;
}
