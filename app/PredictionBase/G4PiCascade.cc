#include "G4CascadeInterface.hh"
#include "G4HadronInelasticProcess.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Material.hh"
#include "G4Nucleus.hh"
#include "G4SystemOfUnits.hh"
#include "G4CascadeInterface.hh"
#include "G4HadProjectile.hh"
#include "G4HadFinalState.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "G4RunManager.hh"
#include "G4PiCascadeDetectorConstruction.hh"
#include "G4PiCascadePhysicsList.hh"
#include "G4HadronicException.hh"
#include "G4VParticleChange.hh"


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

std::string fcl_file;

bool range_override = false;
double range_low_override = 0.;
double range_high_override = 0.;

std::string output_file_override = "empty"; 

int ncasc_override = 0;
int ndiv_override = 0;
int type_override = -999;

bool parseArgs(int argc, char* argv[]);

int main(int argc, char * argv[]){

  if( !parseArgs(argc, argv) ) 
    return 0;

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(fcl_file);

  int nCascades = ps.get< int >("NCascades");
  if( ncasc_override > 0 ) 
    nCascades = ncasc_override;

  int type      = ps.get< int >("Type");
  if( type_override != -999 )
    type = type_override;
    

  
  std::pair< double, double > range = ps.get< std::pair< double, double > >("Range");
  if( range_override) 
    range = std::make_pair(range_low_override, range_high_override);


  int nDivisions = ps.get< int >("NDivisions");
  if( ndiv_override > 0 ) 
    nDivisions = ndiv_override;

  if( nDivisions < 1 ){
    std::cout << "Please give NDivision >= 1" << std::endl;
    return 0;
  }
  if( range.second < range.first ){
    std::cout << "Please give range in increasing order" << std::endl;
    return 0;
  }

  std::cout << "Range: " << range.first << " " << range.second << std::endl;
  std::vector< double > momenta;
  double delta = range.second - range.first;
  double step = delta / nDivisions; 
  for( size_t i = 0; i <= nDivisions; ++i ){
    momenta.push_back( range.first + i * step );
  }

  std::string outFileName = ps.get< std::string >("Outfile");
   if( output_file_override  != "empty" ){
    outFileName = output_file_override;
  }


  TFile * fout = new TFile( outFileName.c_str(), "RECREATE");
  TTree * tree = new TTree("tree","");  
  int nPi0 = 0, nPiPlus = 0, nPiMinus = 0;
  double momentum;
  tree->Branch( "nPi0", &nPi0 );
  tree->Branch( "nPiPlus", &nPiPlus );
  tree->Branch( "nPiMinus", &nPiMinus );
  tree->Branch( "momentum", &momentum );

  //Initializing
  G4RunManager * rm = new G4RunManager();
  rm->SetUserInitialization(new G4PiCascadeDetectorConstruction);
  rm->SetUserInitialization(new G4PiCascadePhysicsList);
  rm->Initialize();
  rm->ConfirmBeamOnCondition();
  rm->ConstructScoringWorlds();
  rm->RunInitialization();
  ////

  G4PionPlus  * piplus;
  G4PionMinus * piminus;
  G4ParticleDefinition * part_def;
  std::string inel_name;
  if( type == 211 ){
    std::cout << "Chose PiPlus" << std::endl;
    part_def = piplus->Definition();
    inel_name = "pi+Inelastic";
  }
  else if( type == -211 ){
    std::cout << "Chose PiMinus" << std::endl;
    part_def = piminus->Definition();
    inel_name = "pi-Inelastic";
  }
  else{
    std::cout << "Please specify either 211 or -211" << std::endl;
    return 0;
  }

  G4ProcessManager * pm = part_def->GetProcessManager();
  G4ProcessVector  * pv = pm->GetProcessList();
  
  G4HadronInelasticProcess * inelastic_proc;

  for( int i = 0; i < pv->size(); ++i ){
    G4VProcess * proc = (*pv)(i);
    std::string theName = proc->GetProcessName();
    std::cout <<  theName << std::endl;
    if( theName == inel_name ){
      std::cout << "Found inelastic" << std::endl;
      inelastic_proc = (G4HadronInelasticProcess*)proc;
    }
  }

  if( !inelastic_proc ) return 0;

  fhicl::ParameterSet MaterialParameters = ps.get< fhicl::ParameterSet >("Material");
  std::string MaterialName = MaterialParameters.get< std::string >( "Name" );
  int MaterialZ = MaterialParameters.get< int >( "Z" );
  double MaterialMass = MaterialParameters.get< double >( "Mass" );
  double MaterialDensity = MaterialParameters.get< double >( "Density" );
  G4Material * theMaterial = new G4Material(MaterialName, MaterialZ, MaterialMass*g/mole, MaterialDensity*g/cm3);
  G4Nucleus * theNucleus = new G4Nucleus( theMaterial );


//  G4CascadeInterface * theCascade = new G4CascadeInterface( "BertiniCascade" );
  
  G4DynamicParticle * dynamic_part = new G4DynamicParticle(part_def, G4ThreeVector(0.,0.,1.), 0. );
  std::cout << "PDG: " << dynamic_part->GetPDGcode() << std::endl;

  G4Track * theTrack = new G4Track( dynamic_part, 0., G4ThreeVector(0.,0.,0.) );
  G4Step * theStep = new G4Step();
  G4StepPoint * thePoint = new G4StepPoint();
  thePoint->SetMaterial( theMaterial );
  theStep->SetPreStepPoint( thePoint );
  theTrack->SetStep( theStep );

  for( size_t iM = 0; iM < momenta.size(); ++iM ){
    std::cout << "Momentum: " << momenta.at(iM) << std::endl;
    double theMomentum = momenta[iM]; 
    double KE = sqrt( theMomentum*theMomentum + part_def->GetPDGMass()*part_def->GetPDGMass() ) - part_def->GetPDGMass();
    dynamic_part->SetKineticEnergy( KE );
    for( size_t iC = 0; iC < nCascades; ++iC ){

      if( !(iC % 1000) ) std::cout << "\tCascade: " << iC << std::endl;

      nPi0 = 0; 
      nPiPlus = 0; 
      nPiMinus = 0;
      momentum = dynamic_part->GetTotalMomentum();
      //G4HadFinalState * theFS;
      G4VParticleChange * thePC; 
      thePC = inelastic_proc->PostStepDoIt( *theTrack, *theStep );
      //try{
      //  //theFS = theCascade->ApplyYourself( *dynamic_part, *theNucleus );
      //}
      //catch( G4HadronicException aR ){
      //  std::cout << "Something went wrong" <<  std::endl;
      //}
//      std::cout << "Secondaries: " << theFS->GetNumberOfSecondaries() << std::endl;

      //size_t nSecondaries = theFS->GetNumberOfSecondaries();
      size_t nSecondaries = thePC->GetNumberOfSecondaries();
      for( size_t i = 0; i < nSecondaries; ++i ){
        //auto part = theFS->GetSecondary(i)->GetParticle();
        auto part = thePC->GetSecondary(i)->GetDynamicParticle();
      //  std::cout << i << " " << part->GetPDGcode() << " " << part->GetTotalMomentum() << std::endl;

        switch( part->GetPDGcode() ){
          case( 211 ):
            ++nPiPlus; break;
          case( -211 ):
            ++nPiMinus; break;
          case( 111 ):
            ++nPi0; break;          
          default:
            break;
        }
      }
      
      thePC->SetVerboseLevel(0);
      thePC->Initialize(*theTrack);

      //std::cout << "nPiPlus: " << nPiPlus << std::endl;
      //std::cout << "nPiMinus: " << nPiMinus << std::endl;
      //std::cout << "nPi0: " << nPi0 << std::endl;
      tree->Fill();

//      std::cout << std::endl;
    }
  }
  fout->cd();


  TVectorD m_vec(1);
  m_vec[0] = MaterialMass;
  m_vec.Write("Mass");

  TVectorD d_vec(1);
  d_vec[0] = MaterialDensity;
  d_vec.Write("Density");

  tree->Write();
 
  std::map< std::string, std::string > cuts;
  //Define cuts and make graphs out of the results
  cuts["abs"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 0";
  cuts["prod"] = " (nPi0 + nPiPlus + nPiMinus) > 1";
  cuts["cex"] = "nPi0 == 1 && nPiPlus == 0 && nPiMinus == 0";
  if( type == 211 ){
    cuts["inel"] = "nPi0 == 0 && nPiPlus == 1 && nPiMinus == 0";
    cuts["dcex"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 1";
  }
  if( type == -211 ){
    cuts["inel"] = "nPi0 == 0 && nPiPlus == 0 && nPiMinus == 1";
    cuts["dcex"] = "nPi0 == 0 && nPiPlus == 1 && nPiMinus == 0";
  }

  int nbins = int(range.second) + 1;
  std::string binning = "(" + std::to_string(nbins) + ",0," + std::to_string(range.second + 1.) + ")";

  std::string draw = "momentum>>total" + binning;
  tree->Draw( draw.c_str(), "", "goff" );
  TH1F * total = (TH1F*)gDirectory->Get("total");
  std::vector< int > bins;
  for( int i = 1; i <= total->GetNbinsX(); ++i ){
    if( total->GetBinContent( i ) > 0. ){
      bins.push_back( i );
      std::cout << i << std::endl;
    }
  }

  std::map< std::string, std::string >::iterator itCut = cuts.begin();
  for( itCut; itCut != cuts.end(); ++itCut ){
    std::cout << itCut->first << std::endl;
    std::string name = "h" + itCut->first;
    std::string draw = "momentum>>" + name + binning;
    std::cout << "Draw: " << draw << std::endl;

    tree->Draw( draw.c_str(), itCut->second.c_str(), "goff" ); 
    TH1F * hist = (TH1F*)gDirectory->Get( name.c_str() );
    hist->Divide( total );

    std::vector< double > xs,ys;
    
    for( int i = 0; i < bins.size(); ++i ){
      xs.push_back( bins.at(i) );
      ys.push_back( hist->GetBinContent(bins.at(i)) );
    }
    TGraph gr(xs.size(), &xs[0], &ys[0]);
    gr.Write( itCut->first.c_str() );

  }


  fout->Close();
  delete rm;

  return 0;
}

bool parseArgs(int argc, char ** argv){

  bool found_fcl_file = false;

  bool found_range_low = false;
  bool found_range_high = false;

  for( int i = 1; i < argc; ++i ){
    if( ( strcmp( argv[i], "--help" )  == 0 ) || ( strcmp( argv[i], "-h" ) == 0 ) ){
      std::cout << "Usage: ./G4PiCascade -c <cascade_config>.fcl [options]" << std::endl;
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
