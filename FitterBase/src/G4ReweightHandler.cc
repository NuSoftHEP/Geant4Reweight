#include "G4ReweightHandler.hh"

std::string set_prec( double );

G4ReweightHandler::G4ReweightHandler(bool as_graphs): enable_graphs(as_graphs){
  
  dummy = new G4ReweightInter(std::vector< std::pair<double, double> >() );
  double dummyX = 0.;
  double dummyY = 1.;
  dummyGraph = new TGraph( 1, &dummyX, &dummyY );
}

G4ReweightHandler::~G4ReweightHandler(){
  delete dummy;
  delete dummyGraph;
}

void G4ReweightHandler::DefineInters( std::vector< fhicl::ParameterSet > ps ){
  std::cout << "Defining Inters" << std::endl;
  for( size_t i = 0; i < ps.size(); ++i ){
    fhicl::ParameterSet set = ps.at(i);

    std::string name = set.get< std::string >( "Cut" );

    std::cout << "Cut: " << name;

    std::vector< std::pair< double, double > > vars;
    bool isDummy = set.get< bool >( "Dummy" );
    if( isDummy ){
      std::cout << "Dummy" << std::endl;
      FSInters[name] = dummy;
    }
    else{
      vars = set.get< std::vector< std::pair< double, double > > >( "Vars" );
      for( size_t j = 0; j < vars.size(); ++j ){
        std::cout << vars.at(j).first << " " << vars.at(j).second << std::endl;
      }
      FSInters[name]  = new G4ReweightInter(vars);
    }

  }
}

///Replace the dummy checking with giving a map< std::string, bool > isDummy
void G4ReweightHandler::DefineInters( std::map< std::string, std::vector< FitParameter > > pars ){
  ActiveParameters.clear();
  std::cout << "Defining Inters" << std::endl;


  std::map< std::string, std::vector< FitParameter > >::iterator itPar;
  for( itPar = pars.begin(); itPar != pars.end(); ++itPar ){
    std::string name = itPar->first;  
    std::cout << "Cut: " << name << std::endl;
    
    bool isDummy = false;

    std::vector< std::pair< double, double > > vars;
    std::vector< double > varX, varY; 

    for( size_t i = 0; i < itPar->second.size(); ++i ){
      
      if( itPar->second.at( i ).Dummy ){
        std::cout << "Dummy" << std::endl;
        FSInters[name] = dummy;
        FSGraphs[name] = dummyGraph;
        std::cout << "Dummy N: " << dummyGraph->GetN() << std::endl;
        isDummy = true;
        break;
      }
  
      else{        
        ActiveParameters.push_back( itPar->second.at(i) );
        double value = itPar->second.at( i ).Value;
        std::pair< double, double > range = itPar->second.at( i ).Range;

        vars.push_back( std::make_pair( range.first,  value ) );
        vars.push_back( std::make_pair( range.second, value ) );

        varX.push_back( range.first );
        varX.push_back( range.second );
        varY.push_back( value );
        varY.push_back( value );
      }
  
    }
    
    if( !isDummy ){
      for( size_t i = 0; i < vars.size(); ++i ){
        std::cout << vars.at(i).first << " " << vars.at(i).second << std::endl;
      }

      FSInters[name] = new G4ReweightInter(vars);
      FSGraphs[name] = new TGraph(varX.size(), &varX[0], &varY[0]);
    }
  }
}


void G4ReweightHandler::SetInters( std::map< std::string, G4ReweightInter* > & theInts ){
  std::map< std::string, G4ReweightInter* >::iterator it = theInts.begin(); 
  for( ; it != theInts.end(); ++it ){
    FSInters[ it->first ] = it->second;
  }
}

FitSample G4ReweightHandler::DoReweight(std::string theName, double max, double min, std::string outName, bool PiMinus){

  G4ReweightFinalState * FSReweighter;
  if( enable_graphs ){
    FSReweighter = new G4ReweightFinalState( fFSFile, FSGraphs, /*max, min,*/ PiMinus );
  }
  else{
    FSReweighter = new G4ReweightFinalState( fFSTree, FSInters, max, min, PiMinus );
  }
  //////////////////////////////

  //NEED TO FIX
  //RWFileName = theName + ".root";
  RWFileName = outName;


  //Make the reweighter pointer and do the reweighting

  std::cout << "Reweighting MC File: 0" << std::endl;

  Reweighter = new G4ReweightTreeParser( fRawMCFileNames->at(0),  RWFileName );
  Reweighter->FillAndAnalyzeFS(FSReweighter);

  std::cout << std::endl;
  for( size_t i = 1; i < fRawMCFileNames->size(); ++i ){

    Reweighter->CloseInput();

    std::cout << "MC File: " << i << std::endl;
    Reweighter->OpenNewInput( fRawMCFileNames->at(i) );
    Reweighter->FillAndAnalyzeFS(FSReweighter);
    std::cout << "Tree Entries: " << Reweighter->GetTree()->GetEntries() << std::endl;
    std::cout << std::endl;

  }   

  Reweighter->CloseInput();
  Reweighter->CloseAndSaveOutput();
  ////////////////////////////////

  
  FitSample theSample; 
  theSample.theName = theName;
  theSample.theFile = RWFileName;
  theSample.Raw = false;
  theSample.Parameters = ActiveParameters;

  //memory management
  delete FSReweighter;
  delete Reweighter;
//  ClearFSInters();

  return theSample;
}

void G4ReweightHandler::ParseFHiCL( std::vector<fhicl::ParameterSet> ps_vec){

  for( size_t i = 0; i < ps_vec.size(); ++i){
    
    fhicl::ParameterSet ps = ps_vec.at(i);

    std::string name = ps.get<std::string>("Name");

    std::vector< std::string > files = ps.get< std::vector< std::string > >("Files"); 

    fMapToFiles[name] = files;

    std::string FSFile = ps.get< std::string >("FSFile");
  
    fMapToFSFiles[ name ] = FSFile;

  }

}

void G4ReweightHandler::SetFiles( std::string name ){ 
  fRawMCFileNames = &(fMapToFiles[name]); 
  fFSFileName = fMapToFSFiles[name]; 

  fFSFile = new TFile( fFSFileName.c_str(), "READ");
  fFSTree = (TTree*)fFSFile->Get( "tree" ); 

}

void G4ReweightHandler::ClearFSInters(){
      abs_vector.clear();
      cex_vector.clear();
}
