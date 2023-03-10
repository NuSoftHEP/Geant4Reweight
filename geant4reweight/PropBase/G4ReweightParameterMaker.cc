#include "geant4reweight/PropBase/G4ReweightParameterMaker.hh"

#include "TH1D.h"

#include <algorithm>
#include <exception>
#include <vector>

G4ReweightParameterMaker::G4ReweightParameterMaker( const std::map< std::string, std::vector< FitParameter > > & pars ) :
  FullParameterSet( pars ), nParameters(0) {

  BuildHistsFromPars();
}

G4ReweightParameterMaker::G4ReweightParameterMaker( const std::vector< fhicl::ParameterSet > & FitParSets, bool check_overlap, int pdg){

  std::vector< std::string > all_cuts;
  switch (pdg) {
    case 211: {
      all_cuts = {"abs", "cex", "dcex", "prod", "inel", "reac"};
      break;
    }
    case -211: {
      all_cuts = {"abs", "cex", "dcex", "prod", "inel", "reac"};
      break;
    }
    case 2212: {
      all_cuts = {"total", "reac"};
      //all_cuts = {"0n0p", "1n0p", "0n1p", "1n1p", "Other", "reac"};
      break;
    }
    case 2112: {
      all_cuts = {"total", "reac"};
      break;
    }
    case 321: {
      all_cuts = {"total", "reac"};
      break;
    }
    case -321: {
      all_cuts = {"total", "reac"};
      break;
    }
    /*
    case 2112: {
      //all_cuts = {"0n0p", "1n0p", "0n1p", "1n1p", "Other", "reac"};
      break;
    }
    */
    default: {
      break;
    }
  }

  for( size_t i = 0; i < all_cuts.size(); ++i ){
    FullParameterSet[ all_cuts[i] ] = std::vector< FitParameter >();
  }

  for( size_t i = 0; i < FitParSets.size(); ++i ){
    fhicl::ParameterSet theSet = FitParSets.at(i);
    std::string theCut = theSet.get< std::string >("Cut");

    if( theCut != "elast" && std::find( all_cuts.begin(), all_cuts.end(), theCut ) == all_cuts.end() ){
      std::cout << "Error: found parameter with bad cut " << theCut << std::endl;
      std::exception e;
      throw e;
    }

    if( theCut != "elast" ) ++nParameters;
    else ++nElastParameters;

    std::string theName = theSet.get< std::string >("Name");

    std::pair< double, double > theRange = theSet.get< std::pair< double, double > >("Range");

    double nominal = theSet.get< double >("Nominal",1.);

    FitParameter par;
    par.Name = theName;
    par.Cut = theCut;
    par.Dummy = false;
    par.Value = nominal;
    par.Range = theRange;

    double scan_start = theSet.get< double >("ScanStart", 1.);
    int    nsteps =     theSet.get< int >("NScanSteps", 2);
    double scan_delta = theSet.get< double >("ScanDelta", .1);

    par.ScanStart = scan_start;
    par.ScanSteps = nsteps;
    par.ScanDelta = scan_delta;

    ///////Add into the parameters themselves
    if( theCut == "elast" ){
      ElasticParameterSet.push_back( par );
    }
    else {
      FullParameterSet[ theCut ].push_back( par );
    }
  }

  for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar){
    if( !( itPar->second.size() ) ){
      FitParameter dummyPar;

      dummyPar.Name = "dummy";
      dummyPar.Cut = itPar->first;
      dummyPar.Value = 1.;
      dummyPar.Range = std::make_pair( 0., 0.);
      dummyPar.Dummy = true;

      FullParameterSet[ itPar->first ].push_back( dummyPar );
    }
  }

  //Check range here
  if (check_overlap) {
    for (auto it = FullParameterSet.begin();
         it != FullParameterSet.end(); ++it) {
      for (size_t i = 0; i < it->second.size(); ++i) {
        std::string i_name = it->second.at(i).Name;
        std::pair<double, double> i_range = it->second.at(i).Range;
        for (size_t j = 0; j < it->second.size(); ++j) {
          if (i == j) continue;
          std::pair<double, double> j_range = it->second.at(j).Range;
          std::string j_name = it->second.at(j).Name;
          if ((i_range.first >= j_range.first) &&
              (i_range.first < j_range.second)) {
            std::cerr << "Error: found overlapping parameters\n" << i_name << " ("
                      << i_range.first << ", " << i_range.second << ")" << "\n"
                      << j_name << " (" << j_range.first << ", "
                      << j_range.second << ")" << std::endl;
            std::exception e;
            throw e;
          }
          if ((i_range.second <= j_range.second) &&
              (i_range.second > j_range.first)) {
            std::cerr << "Error: found overlapping parameters\n" << i_name << " ("
                      << i_range.first << ", " << i_range.second << ")" << "\n"
                      << j_name << " (" << j_range.first << ", "
                      << j_range.second << ")" << std::endl;
            std::exception e;
            throw e;
          }
        }
      }
    }
  }

  BuildHistsFromPars();
  BuildElasticHist();
}


void G4ReweightParameterMaker::BuildElasticHist(){

  std::vector< double > varX, varY;

  for( size_t i = 0; i < ElasticParameterSet.size(); ++i ){
    double value = ElasticParameterSet[i].Value;
    std::pair< double, double > range = ElasticParameterSet[i].Range;

    bool addDummyBin = false;
    if( varX.size() ){
      //If the end of last bin == start of this bin
      //don't need to add a dummy
      if( varX.back() < range.first ){
        varX.push_back( range.first );
        addDummyBin = true;
      }
    }
    else
      varX.push_back(range.first);

    varX.push_back( range.second );

    if( addDummyBin )
      varY.push_back( 1. );
    varY.push_back( value );
  }

  if( varX.size() ){
    ElasticHist = new TH1D( "varElast","", varX.size()-1, &varX[0]);
    for( size_t i = 0; i < varY.size(); ++i ){
      ElasticHist->SetBinContent(i+1, varY[i]);
    }
  }
  else{
   ElasticHist = new TH1D( "varElast", "", 1, 0, 0 );
   ElasticHist->SetBinContent( 1, 1. );
  }
  //Set under/overflow
  ElasticHist->SetBinContent( 0, 1. );
  ElasticHist->SetBinContent( ElasticHist->GetNbinsX()+1, 1. );

}


void G4ReweightParameterMaker::BuildHistsFromPars(){

  std::map< std::string, bool > CutIsDummy;
  for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){
    std::string name = itPar->first;
    if( name == "reac" ) continue;

    bool isDummy = false;

    std::vector< std::pair< double, double > > vars;
    std::vector< double > varX, varY;

    for( size_t i = 0; i < itPar->second.size(); ++i ){

      if( itPar->second.at( i ).Dummy ){
        FSHists[name]  = new TH1D( ("dummy_" + name).c_str(), "", 1,0,0);
        FSHists[name]->SetBinContent(0,1.);
        FSHists[name]->SetBinContent(1,1.);
        FSHists[name]->SetBinContent(2,1.);
        isDummy = true;
        break;
      }

      else{

        double value = itPar->second.at( i ).Value;
        std::pair< double, double > range = itPar->second.at( i ).Range;

        vars.push_back( std::make_pair( range.first,  value ) );
        vars.push_back( std::make_pair( range.second, value ) );

        bool addDummyBin = false;
        if (varX.size()){
          //If the end of last bin == start of this bin
          //don't need to add a dummy
          if( varX.back() < range.first ){
            varX.push_back( range.first );
            addDummyBin = true;
          }
        }
        else{
          varX.push_back(range.first);
        }
        varX.push_back( range.second );

        if (addDummyBin)
          varY.push_back( 1. );
        varY.push_back( value );
      }

    }

    CutIsDummy[ name ] = isDummy;

    if( !isDummy ){

      FSHists[name] = new TH1D( ("var"+name).c_str(),"", varX.size()-1, &varX[0]);
      for( size_t i = 0; i < varY.size(); ++i ){
        FSHists[name]->SetBinContent(i+1, varY[i]);
      }
      //Set under/overflow
      FSHists[name]->SetBinContent( 0, 1. );
      FSHists[name]->SetBinContent( FSHists[name]->GetNbinsX()+1, 1. );
    }
  }

  if( FullParameterSet.find( "reac" ) != FullParameterSet.end() ){
    if( !FullParameterSet.at( "reac" ).at(0).Dummy ){
      //If reac exists and is not a dummy, go through the exclusive channels
      //and vary each by the reac variations

      //Build the reac graph
      std::vector< double > reac_bins, varY, reacBins;
      for( size_t i = 0; i < FullParameterSet.at( "reac" ).size(); ++i ){
        double value = FullParameterSet.at( "reac" ).at( i ).Value;
        std::pair< double, double > range = FullParameterSet.at( "reac" ).at( i ).Range;

        bool addDummyBin = false;
        if( reac_bins.size() ){
          //If the end of last bin == start of this bin
          //don't need to add a dummy
          if( reac_bins.back() < range.first ){
            reac_bins.push_back( range.first );
            addDummyBin = true;
          }
        }
        else
          reac_bins.push_back(range.first);

        reac_bins.push_back( range.second );

        if( addDummyBin )
          varY.push_back( 1. );
        varY.push_back( value );

      }

      TH1D reac_hist( "var_reac","", reac_bins.size()-1, &reac_bins[0]);
      for( size_t i = 0; i < varY.size(); ++i ){
        reac_hist.SetBinContent(i+1, varY[i]);
      }
      //Set under/overflow
      reac_hist.SetBinContent( 0, 1. );
      reac_hist.SetBinContent( reac_hist.GetNbinsX()+1, 1. );

      for( auto itGr = FSHists.begin(); itGr != FSHists.end(); ++itGr ){
        std::string name = itGr->first;

        auto excHist = itGr->second;
        std::vector< double > exc_bins;
        if( !CutIsDummy[name] ){
          for( int i = 1; i <= excHist->GetNbinsX(); ++i ){
            exc_bins.push_back( excHist->GetBinLowEdge(i) );
          }
          exc_bins.push_back( excHist->GetBinLowEdge(excHist->GetNbinsX()) + excHist->GetBinWidth(excHist->GetNbinsX()) );
        }

        std::vector< double > new_bins = exc_bins;
        for( size_t i = 0; i < reac_bins.size(); ++i ){
          if( std::find( new_bins.begin(), new_bins.end(), reac_bins[i] )
          == new_bins.end() ){
            new_bins.push_back( reac_bins[i] );
          }
        }

        std::sort( new_bins.begin(), new_bins.end() );

        TH1D new_hist( "new_hist", "", new_bins.size()-1, &new_bins[0] );
        for( int i = 1; i <= new_hist.GetNbinsX(); ++i ){
          double x = new_hist.GetBinCenter( i );
          int reac_bin = reac_hist.FindBin( x );
          int exc_bin = excHist->FindBin( x );

          double content = reac_hist.GetBinContent( reac_bin );
          content *= excHist->GetBinContent( exc_bin );

          new_hist.SetBinContent(i, content );
        }

        std::string exc_name = excHist->GetName();
        (*excHist) = new_hist;
        excHist->SetName( exc_name.c_str() );

      }

    }
  }
}

//allows one to have a separate parameter set for the Elastic Params
void G4ReweightParameterMaker::SetNewValsWithElast (
    const std::vector<std::pair<std::string, double>> & input,
    const std::vector<std::pair<std::string, double>> & input_elast) {
  std::map< std::string, double > new_input;
  for( auto i = input.begin(); i != input.end(); ++i ){
    new_input[ i->first ] = i->second;
  }

  std::map< std::string, double > new_input_elast;
  //std::cout << "THERE" << std::endl;
  for( auto i = input_elast.begin(); i != input_elast.end(); ++i ){
    new_input_elast[ i->first ] = i->second;
  }
  SetNewValsWithElast( new_input , new_input_elast);
}

void G4ReweightParameterMaker::SetNewValsWithElast( const std::map< std::string, double > & input ,  const std::map< std::string, double > & input_elast  ){
  //std::cout << "THERE 3" << std::endl;
  SetParamValsWithElast( input , input_elast );

  for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){

    std::string name = itPar->first;
    if( name == "reac" ) continue;

    //Go through and set all bins to 1. as a reset
    TH1D * excHist = FSHists[name];
    for( int i = 0; i <= excHist->GetNbinsX()+1; ++i )
      excHist->SetBinContent(i, 1.);

    auto thePars = itPar->second;
    for( size_t i = 0; i < thePars.size(); ++i ){

      //First check if it is a dummy
      if( thePars[i].Dummy ){
        for( int j = 1; j <= excHist->GetNbinsX(); ++j ){
          excHist->SetBinContent(j,1.);
        }
        break;
      }

      double start = thePars[i].Range.first;
      double end   = thePars[i].Range.second;

      for( int j = 1; j <= excHist->GetNbinsX(); ++j ){
        //double bin_low = excHist->GetBinLowEdge(j);
        //double bin_high = excHist->GetBinLowEdge(j+1);

        if( ( start <= excHist->GetBinLowEdge(j) ) && ( end >= excHist->GetBinLowEdge(j+1) ) ){
          excHist->SetBinContent(j, thePars[i].Value);
        }
      }
    }
  }

  //std::cout << "Here 2" << std::endl;

  //Now Get the reactive and go back and vary all others
  if( FullParameterSet.find( "reac" ) != FullParameterSet.end() ){

    auto reacPars = FullParameterSet.at( "reac" );
    if( !reacPars[0].Dummy ){
      for( size_t i = 0; i < reacPars.size(); ++i ){
        double reac_start = reacPars[i].Range.first;
        double reac_end   = reacPars[i].Range.second;
        double reac_val   = reacPars[i].Value;

        for( auto itHist = FSHists.begin(); itHist != FSHists.end(); ++itHist ){

          TH1D * excHist = itHist->second;
          for( int j = 1; j <= excHist->GetNbinsX(); ++j ){
            //double bin_low = excHist->GetBinLowEdge(j);
            //double bin_high = excHist->GetBinLowEdge(j+1);

            if( ( reac_start <= excHist->GetBinLowEdge(j) ) && ( reac_end >= excHist->GetBinLowEdge(j+1) ) ){
              double exc_val = excHist->GetBinContent(j);
              excHist->SetBinContent(j, reac_val * exc_val);
            }
          }
        }
      }
    }
  }

  //std::cout << "Here 3" << std::endl;

  // Finally regenerate elastic histogram

  //Go through and set all bins to 1. as a reset
  for( int i = 0; i <= ElasticHist->GetNbinsX()+1; ++i )
    ElasticHist->SetBinContent(i, 1.);

  // Now set bin contents according to parameter value
  for( auto itPar = ElasticParameterSet.begin(); itPar != ElasticParameterSet.end(); ++itPar ){

    double start = itPar->Range.first;
    double end   = itPar->Range.second;

    for( int j = 1; j <= ElasticHist->GetNbinsX(); ++j ){
      if( ( start <= ElasticHist->GetBinLowEdge(j) ) && ( end >= ElasticHist->GetBinLowEdge(j+1) ) ){
        double formerval = ElasticHist->GetBinContent(j);
        ElasticHist->SetBinContent(j, formerval*itPar->Value);
      }
    }
  }

  //std::cout << "Here 4" << std::endl;
}

void G4ReweightParameterMaker::SetNewVals( const std::vector< std::pair< std::string, double > > & input ){
  std::map< std::string, double > new_input;
  //std::cout << "THERE" << std::endl;
  for( auto i = input.begin(); i != input.end(); ++i ){
    new_input[ i->first ] = i->second;
  }

  SetNewVals( new_input );
}

void G4ReweightParameterMaker::SetNewVals( const std::map< std::string, double > & input ){
  //std::cout << "THERE 2" << std::endl;
  SetParamVals( input );

  for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){

    std::string name = itPar->first;
    if( name == "reac" ) continue;

    //Go through and set all bins to 1. as a reset
    TH1D * excHist = FSHists[name];
    //for( int i = 1; i <= excHist->GetNbinsX(); ++i )
    for( int i = 0; i <= excHist->GetNbinsX()+1; ++i )
      excHist->SetBinContent(i, 1.);

    auto thePars = itPar->second;
    for( size_t i = 0; i < thePars.size(); ++i ){

      //First check if it is a dummy
      if( thePars[i].Dummy ){
        for( int j = 1; j <= excHist->GetNbinsX(); ++j ){
          excHist->SetBinContent(j,1.);
        }
        break;
      }

      double start = thePars[i].Range.first;
      double end   = thePars[i].Range.second;

      for( int j = 1; j <= excHist->GetNbinsX(); ++j ){
        //double bin_low = excHist->GetBinLowEdge(j);
        //double bin_high = excHist->GetBinLowEdge(j+1);

        if( ( start <= excHist->GetBinLowEdge(j) ) && ( end >= excHist->GetBinLowEdge(j+1) ) ){
          excHist->SetBinContent(j, thePars[i].Value);
        }


      }
    }

  }

  //Now Get the reactive and go back and vary all others
  if( FullParameterSet.find( "reac" ) != FullParameterSet.end() ){

    auto reacPars = FullParameterSet.at( "reac" );
    if( !reacPars[0].Dummy ){
      for( size_t i = 0; i < reacPars.size(); ++i ){
        double reac_start = reacPars[i].Range.first;
        double reac_end   = reacPars[i].Range.second;
        double reac_val   = reacPars[i].Value;

        for( auto itHist = FSHists.begin(); itHist != FSHists.end(); ++itHist ){

          TH1D * excHist = itHist->second;
          for( int j = 1; j <= excHist->GetNbinsX(); ++j ){
            //double bin_low = excHist->GetBinLowEdge(j);
            //double bin_high = excHist->GetBinLowEdge(j+1);

            if( ( reac_start <= excHist->GetBinLowEdge(j) ) && ( reac_end >= excHist->GetBinLowEdge(j+1) ) ){
              double exc_val = excHist->GetBinContent(j);
              excHist->SetBinContent(j, reac_val * exc_val);
            }
          }
        }
      }
    }
  }

  // Finally regenerate elastic histogram

  //Go through and set all bins to 1. as a reset
  for( int i = 0; i <= ElasticHist->GetNbinsX()+1; ++i )
    ElasticHist->SetBinContent(i, 1.);

  // Now set bin contents according to parameter value
  for( auto itPar = ElasticParameterSet.begin(); itPar != ElasticParameterSet.end(); ++itPar ){

    double start = itPar->Range.first;
    double end   = itPar->Range.second;

    for( int j = 1; j <= ElasticHist->GetNbinsX(); ++j ){

      if( ( start <= ElasticHist->GetBinLowEdge(j) ) && ( end >= ElasticHist->GetBinLowEdge(j+1) ) ){
        double formerval = ElasticHist->GetBinContent(j);
        ElasticHist->SetBinContent(j, formerval*itPar->Value);
      }
    }
  }

}

void G4ReweightParameterMaker::SetParamVals( const std::map< std::string, double > & input ){
  for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){
    if( !itPar->second.at(0).Dummy ){
      for( size_t j = 0; j < itPar->second.size(); ++j ){
        itPar->second.at(j).Value = input.at(itPar->second.at(j).Name);
      }
    }
  }

  // Also set new elastic parameter value
  for (auto itPar = ElasticParameterSet.begin(); itPar != ElasticParameterSet.end(); ++itPar){
    if (!itPar->Dummy){
      itPar->Value = input.at(itPar->Name);
    }
  }
}






void G4ReweightParameterMaker::SetParamValsWithElast( const std::map< std::string, double > & input ,    const std::map< std::string, double > & input_elast  ){
  for( auto itPar = FullParameterSet.begin(); itPar != FullParameterSet.end(); ++itPar ){
    if( !itPar->second.at(0).Dummy ){
      for( size_t j = 0; j < itPar->second.size(); ++j ){
        itPar->second.at(j).Value = input.at(itPar->second.at(j).Name);
      }
    }
  }

  // Also set new elastic parameter value
  for (auto itPar = ElasticParameterSet.begin(); itPar != ElasticParameterSet.end(); ++itPar){
    if (!itPar->Dummy){
      itPar->Value = input_elast.at(itPar->Name);
      // itPar->Value = 10000;   
    }
  }
}
