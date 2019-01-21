#include "G4ReweightHandler.hh"

std::string set_prec( double );

G4ReweightHandler::G4ReweightHandler(){
  
  dummy = new G4ReweightInter(std::vector< std::pair<double, double> >() );
}

G4ReweightHandler::~G4ReweightHandler(){
  delete dummy;
 
//  std::map<std::string, G4ReweightInter*>::iterator itInter;
//  for( itInter = FSInters.begin(); itInter != FSInters.end(); ++itInter ){
//    delete itInter->second;
//  }
}

FitSample G4ReweightHandler::DoReweight(std::string theName, double norm_abs, double norm_cex, std::string outName, bool PiMinus ){


  std::string norm_abs_str = set_prec(norm_abs);
  std::string norm_cex_str = set_prec(norm_cex);



  abs_vector.push_back( std::make_pair(200., norm_abs) );  
  abs_vector.push_back( std::make_pair(300., norm_abs) );  
  FSInters["abs"]  = new G4ReweightInter(abs_vector);

  cex_vector.push_back( std::make_pair(200., norm_cex) );  
  cex_vector.push_back( std::make_pair(300., norm_cex) );       
  FSInters["cex"]  = new G4ReweightInter(cex_vector);

  FSInters["inel"] = dummy;
  FSInters["prod"] = dummy;
  FSInters["dcex"] = dummy;


  G4ReweightFinalState * FSReweighter = new G4ReweightFinalState( fFSTree, FSInters, 300., 200., PiMinus );
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


  //memory management
  delete FSReweighter;
  delete Reweighter;
  ClearFSInters();

  return theSample;
}

void G4ReweightHandler::ParseXML(std::string FileName, std::vector< std::string > vector_names){

  tinyxml2::XMLDocument doc;

  tinyxml2::XMLError loadResult = doc.LoadFile( FileName.c_str() );
  if( loadResult != tinyxml2::XML_SUCCESS ){
    std::cout << "Could not load file" << std::endl;
    return;
  }

  tinyxml2::XMLNode * theRoot = doc.FirstChild();
  if( !theRoot ){
    std::cout << "Could Not get first child" << std::endl;
    return;
  }


  //std::vector< std::string > vector_names = {"C_piplus"};

  for( std::vector< std::string >::iterator vector_name = vector_names.begin(); vector_name != vector_names.end(); ++vector_name){

    //Create the entry in the map 
    fMapToFiles[ *vector_name ]; 

    std::string element_name = *vector_name + "_Files";

    tinyxml2::XMLElement * Files = theRoot->FirstChildElement( element_name.c_str() );
    if( !Files ){
      std::cout << "Could Not get " << *vector_name + "_Files" << std::endl;
      return;
    }
  
    tinyxml2::XMLElement * theFile = Files->FirstChildElement( "File" );
  
    while(theFile){
      std::string reweightFile;
  
      const char * reweightFileText = nullptr;
  
      reweightFileText = theFile->Attribute("Name");
      if (reweightFileText != nullptr) reweightFile = reweightFileText;
  
      std::cout << "File: " << reweightFile << std::endl;
  
      fMapToFiles[ *vector_name ].push_back( reweightFile );
      
      theFile = theFile->NextSiblingElement("File");
    }
  
    tinyxml2::XMLElement * theFSFile = Files->FirstChildElement( "FSFile");
    std::string FSFile;
  
    const char * FSFileText = nullptr;
  
    FSFileText = theFSFile->Attribute("Name");
    if (FSFileText != nullptr) FSFile = FSFileText;
  
    std::cout << "File: " << FSFile << std::endl;
  
    fMapToFSFiles[ *vector_name ] = FSFile;

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

//      std::map<std::string, G4ReweightInter*>::iterator itInter;
//      for( itInter = FSInters.begin(); itInter != FSInters.end(); ++itInter ){
//        delete itInter->second;
//      }
}
