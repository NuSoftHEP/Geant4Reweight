#include "G4ReweightFitter.hh"
#include "G4ReweightHandler.hh"
#include "newDUETFitter.hh"
#include "FitParameter.hh"
#include <vector>
#include <string>
#include "TVectorD.h"

#include "TFile.h"
#include "TH2D.h"
#include "TGraph2D.h"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"

std::string set_prec(double);

int main(int argc, char ** argv){

  TFile * out = new TFile ("newFitter_try.root", "RECREATE");
  out->cd();
  TDirectory * data_dir = out->mkdir( "Data" );

  std::map< std::string, std::vector< G4ReweightFitter* > > mapSetsToFitters;

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(argv[1]);
  ///Making Fit Parameters

  std::map< std::string, std::vector< FitParameter > > FullParameterSet;

  std::vector< fhicl::ParameterSet > FitParSets = ps.get< std::vector< fhicl::ParameterSet > >("ParameterSet");
  std::cout << "Making parameter sets" << std::endl;
  for( size_t i = 0; i < FitParSets.size(); ++i ){
    fhicl::ParameterSet theSet = FitParSets.at(i);
    std::string theCut = theSet.get< std::string >("Cut");

    if( FullParameterSet.find( theCut ) == FullParameterSet.end() ){ 
      FullParameterSet[ theCut ] = std::vector< FitParameter  >();
    }
    bool isDummy = theSet.get< bool >("Dummy");
    if( isDummy ){
      FitParameter dummyPar;
      dummyPar.Name = "dummy";
      dummyPar.Cut = theCut;
      dummyPar.Value = 1.;
      dummyPar.Range = std::make_pair( 0., 0.);
      dummyPar.Dummy = true;
      
      FullParameterSet[ theCut ].push_back( dummyPar );
    } 
    else{ 
      std::cout << "Making parameters for " << theCut << std::endl;
    
      std::vector< fhicl::ParameterSet > theParameters = theSet.get< std::vector< fhicl::ParameterSet > >("Parameters");
      for( size_t j = 0; j < theParameters.size(); ++j ){
        fhicl::ParameterSet thePar = theParameters.at(j);


        std::string theName = thePar.get< std::string >("Name");
        std::cout << theName << std::endl;
        
        std::pair< double, double > theRange = thePar.get< std::pair< double, double > >("Range");
        std::cout << "Range Low: " << theRange.first << " High: " << theRange.second << std::endl;

        FitParameter par;
        par.Name = theName;
        par.Cut = theCut;
        par.Dummy = false;
        par.Value = 1.;
        par.Range = theRange;
        FullParameterSet[ theCut ].push_back( par );
      }
    }
  }
  /////////////////////////////////////////////////

  ///Defining experiments
  std::vector< fhicl::ParameterSet > exps = ps.get< std::vector< fhicl::ParameterSet > >("Experiments");
  std::cout << "Getting Experiments: "  << exps.size() << std::endl;
  for(size_t i = 0; i < exps.size(); ++i){
    G4ReweightFitter * exp = new G4ReweightFitter(out, exps.at(i));
    std::cout << std::endl;
    mapSetsToFitters[ exp->GetType() ].push_back( exp ); 
  }

  bool includeDUET = ps.get< bool >("IncludeDUET");
  newDUETFitter df(out);
  if( includeDUET ){ 
    std::cout << "Including DUET" << std::endl;
    mapSetsToFitters["C_piplus"].push_back( &df );
  }
  ///////////////////////////////////////////

  ///Defining MC Sets
  std::vector< std::string > sets;
  std::vector< fhicl::ParameterSet > FCLSets = ps.get< std::vector< fhicl::ParameterSet > >("Sets");
  
  for( size_t i = 0; i < FCLSets.size(); ++i ){
    sets.push_back( FCLSets[i].get< std::string >("Name") );
  }

  G4ReweightHandler handler(true);  
  handler.ParseFHiCL( FCLSets );
  ///////////////////////////////////////////

  //Defining varied samples
  std::vector< fhicl::ParameterSet > samples = ps.get< std::vector< fhicl::ParameterSet > >("Samples"); 
  std::cout << "Got " << samples.size() << " samples" << std::endl;
  size_t nSamples = samples.size();

  for( size_t i = 0; i < nSamples; ++i ){

    fhicl::ParameterSet sampleSet = samples[i];

    std::vector<std::pair<std::string, bool>>   tempRaw = sampleSet.get<std::vector<std::pair<std::string, bool>>>("Raw");
    std::map< std::string, bool > Raw = std::map< std::string, bool >(tempRaw.begin(), tempRaw.end());
    

    //Get Rid of this
//    double abs = sampleSet.get<double>("abs");
//    double cex = sampleSet.get<double>("cex");
    std::string Name = sampleSet.get<std::string>("Name");

    std::cout << Name << std::endl;
//    std::cout << "abs: " << abs << std::endl;
//    std::cout << "cex: " << cex << std::endl;

    
    std::vector< std::string >::iterator itSet = sets.begin();
    for( itSet; itSet != sets.end(); ++itSet ){
      
      std::string theSet = *itSet; 
      
      std::cout << "Checking " << theSet << std::endl;

      //At this point: Replace the static members with 2 vectors:
      //a) Parameter Names
      //b) Parameter Values
      FitSample theSample;
//      theSample.abs = abs;
//      theSample.cex = cex;
//      theSample.dcex = 0.;
//      theSample.inel = 0.;
//      theSample.prod = 0.;

      fhicl::ParameterSet variations = sampleSet.get< fhicl::ParameterSet >( "Variations" );
      //Go through each of the pre-defined parameters and see if there is a value in Variations
      std::map< std::string, std::vector< FitParameter > >::iterator itPar = FullParameterSet.begin();
      for( itPar; itPar != FullParameterSet.end(); ++itPar ){
        for( size_t iP = 0; iP < itPar->second.size(); ++ iP ){

          if( itPar->second.at(iP).Dummy ){
            std::cout << "This is a dummy. Continuing" << std::endl;
            break;
          }

          std::string parName = itPar->second.at( iP ).Name;
          std::cout << "Trying to get value for parameter " << parName << std::endl;
          double value = 1.;
          try{ 
            value = variations.get< double >( parName );
            std:: cout << "Its value: " << value << std::endl;
          }
          catch ( const std::exception &e ){
            std::cout << "Could not find it. Setting to 1." << std::endl;
          }
          
          itPar->second.at( iP ).Value = value;
          theSample.Parameters.push_back( itPar->second.at( iP ) );
        }
      }

      if( Raw[ theSet ] ){

        handler.SetFiles( theSet );
        bool pim = ( (theSet).find("minus") != std::string::npos );


        handler.DefineInters( FullParameterSet );
        double max = sampleSet.get< double >("Max");
        double min = sampleSet.get< double >("Min");
        theSample = handler.DoReweight( Name.c_str(), max, min, (theSet + "_" + Name + ".root"), pim );

      }
      else{
        
        std::vector< std::pair< std::string, std::string > > tempFiles;
        tempFiles = sampleSet.get< std::vector< std::pair< std::string, std::string > > >("Files");
        std::map< std::string, std::string > theFiles = std::map< std::string, std::string >(tempFiles.begin(), tempFiles.end());

        theSample.theFile = theFiles[ theSet ];
        theSample.theName = Name;

      }

      std::vector< G4ReweightFitter* > theFitters = mapSetsToFitters[ theSet ]; 
      for( size_t iFit = 0; iFit < theFitters.size(); ++iFit){
        std::cout << "Adding sample to " << theSet << std::endl;
        theFitters[iFit]->AddSample( theSample );
      }

    }
  }

 
  //Flatten the map to fitters while loading data for ease 
  std::vector< G4ReweightFitter* > allFitters;
 
  std::map< std::string, std::vector< G4ReweightFitter* > >::iterator itSet;
  itSet = mapSetsToFitters.begin();
  for( ; itSet != mapSetsToFitters.end(); ++itSet ){
 
    std::cout << "Loading Data for Set: " << itSet->first;
    std::vector< G4ReweightFitter* > fitters = itSet->second;
    for( size_t i = 0; i < fitters.size(); ++i ){
      fitters[i]->LoadData();
      fitters[i]->SaveData(data_dir);
      allFitters.push_back( fitters[i] );
    }
  }
  
  std::cout << "Have: " << nSamples << " samples" << std::endl;

  std::vector< double > abs_vector; 
  std::vector< double > cex_vector; 
  std::vector< double > chi2_vector; 

  for( size_t i = 0; i < nSamples; ++i ){

    double chi2 = 0.;
    
    //Just for getting the parameter values
    auto tempFitter = allFitters[0];
    FitSample tempSample = tempFitter->GetSample(i);
    std::string dir_name = tempSample.theName;
    std::cout << dir_name << std::endl;

    //double abs =  tempSample.abs;
    //double cex =  tempSample.cex;
    //std::cout << "Vals: " << abs << " " << cex << std::endl;
    //double inel = tempSample.inel;
    //double prod = tempSample.prod;
    //double dcex = tempSample.dcex;

    TDirectory * outdir = out->mkdir( dir_name.c_str() );

    outdir->cd();
    std::cout << "Saving Parameters" << std::endl;
    for( size_t i = 0; i < tempSample.Parameters.size(); ++i ){
      std::cout << "\t" << tempSample.Parameters.at(i).Name << " " << tempSample.Parameters.at(i).Value << std::endl;
      TVectorD par_val(1);
      par_val[0] = tempSample.Parameters.at(i).Value;
      par_val.Write( tempSample.Parameters.at(i).Name.c_str() );
    }

    for( size_t j = 0; j < allFitters.size(); ++j ){
    
        auto theFitter = allFitters[j];

        out->cd();
        outdir->cd();

        theFitter->SetActiveSample(i, outdir);
        theFitter->GetMCGraphs();
        double fit_chi2 = theFitter->DoFit();

        std::cout << fit_chi2 << std::endl;

        chi2 += fit_chi2;
    }

    TVectorD chi2_result(1);
    chi2_result[0] = chi2;
    outdir->cd();
    chi2_result.Write("Chi2");

    //abs_vector.push_back( abs );
    //cex_vector.push_back( cex );
    chi2_vector.push_back( chi2 );
  }

  //out->cd();

  //Get Rid of this
//  TGraph2D * fitSurf = new TGraph2D( abs_vector.size(), &abs_vector[0], &cex_vector[0], &chi2_vector[0]); 
  
 // fitSurf->Write("chi2_surf");
  out->Close();

  std::cout << "Done" << std::endl;

  return 0;
}

/*std::string set_prec(double input){
  std::stringstream stream_in; 
  stream_in << std::fixed << std::setprecision(2) << input;
  return stream_in.str();
}*/
