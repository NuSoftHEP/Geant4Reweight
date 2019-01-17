#include "G4ReweightHandler.hh"

std::string set_prec( double );

G4ReweightHandler::G4ReweightHandler(){

}

G4ReweightHandler::~G4ReweightHandler(){

}

FitSample G4ReweightHandler::DoReweight(std::string theName, double norm_abs, double norm_cex ){

  std::string norm_abs_str = set_prec(norm_abs);
  std::string norm_cex_str = set_prec(norm_cex);

  G4ReweightInter * dummy = new G4ReweightInter(std::vector< std::pair<double, double> >() );
  std::map< std::string, G4ReweightInter* > FSInters;

  std::vector< std::pair< double, double > > abs_vector;
  abs_vector.push_back( std::make_pair(200., norm_abs) );  
  abs_vector.push_back( std::make_pair(300., norm_abs) );  
  FSInters["abs"]  = new G4ReweightInter(abs_vector);

  std::vector< std::pair< double, double > > cex_vector;
  cex_vector.push_back( std::make_pair(200., norm_cex) );  
  cex_vector.push_back( std::make_pair(300., norm_cex) );       
  FSInters["cex"]  = new G4ReweightInter(cex_vector);

  FSInters["inel"] = dummy;
  FSInters["prod"] = dummy;
  FSInters["dcex"] = dummy;


  TFile * fFSFile = new TFile( fFSFileName.c_str(), "READ");
  TTree * fFSTree = (TTree*)fFSFile->Get( "tree" ); 

  G4ReweightFinalState * FSReweighter = new G4ReweightFinalState( fFSTree, FSInters, 300., 200. );
  //////////////////////////////

  //NEED TO FIX
  RWFileName = theName + ".root";


  //Make the reweighter pointer and do the reweighting
  Reweighter = new G4ReweightTreeParser( fRawMCFileNames->at(0),  RWFileName );
  Reweighter->FillAndAnalyzeFS(FSReweighter);
  for( size_t i = 1; i < fRawMCFileNames->size(); ++i ){

    Reweighter->CloseInput();

    std::cout << "MC File: " << i << std::endl;
    Reweighter->OpenNewInput( fRawMCFileNames->at(i) );
    Reweighter->FillAndAnalyzeFS(FSReweighter);
    std::cout << "Tree Entries: " << Reweighter->GetTree()->GetEntries() << std::endl;

  }   

  Reweighter->CloseAndSaveOutput();
  ////////////////////////////////

  
  FitSample theSample; 
  theSample.theName = theName;
  theSample.theFile = RWFileName;
  theSample.Raw = false;

  return theSample;
}

void G4ReweightHandler::ParseXML(std::string FileName){

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


  std::vector< std::string > vector_names = {"C_piplus"};

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

/*
  ////////////////////////////////////////////
  tinyxml2::XMLElement * C_piminus_Files = theRoot->FirstChildElement( "C_piminus_Files" );
  if( !C_piminus_Files ){
    std::cout << "Could Not get C_piminus_Files" << std::endl;
    return;
  }

  theFile = C_piminus_Files->FirstChildElement( "File" );

  while(theFile){
    std::string reweightFile;

    const char * reweightFileText = nullptr;

    reweightFileText = theFile->Attribute("Name");
    if (reweightFileText != nullptr) reweightFile = reweightFileText;

    std::cout << "File: " << reweightFile << std::endl;

    C_piminus_FilesVector.push_back( reweightFile );
    
    theFile = theFile->NextSiblingElement("File");
  } 

  theFSFile = C_piminus_Files->FirstChildElement( "FSFile");
  FSFileText = nullptr;

  FSFileText = theFSFile->Attribute("Name");
  if (FSFileText != nullptr) FSFile = FSFileText;

  std::cout << "File: " << FSFile << std::endl;

  C_piminus_FSFile = FSFile;
  ////////////////////////
  */


  //O Files
//  tinyxml2::XMLElement * O_Files = theRoot->FirstChildElement( "O_Files" );
//  if( !O_Files ){
//    std::cout << "Could Not get O_Files" << std::endl;
//    return;
//  }
//
//  tinyxml2::XMLElement * theFile = O_Files->FirstChildElement( "File" );
//
//  while(theFile){
//    std::string reweightFile;
//
//    const char * reweightFileText = nullptr;
//
//    reweightFileText = theFile->Attribute("File");
//    if (reweightFileText != nullptr) reweightFile = reweightFileText;
//
//    std::cout << "File: " << reweightFile << std::endl;
//
//    OFilesVector.push_back( reweightFile );
//    
//    theFile = theFile->NextSiblingElement("File")
//  }
  //




  /*{ 
  //Sample loops
  tinyxml2::XMLElement * theSample = theRoot->FirstChildElement( "Sample" );
  if( !theSample ){
    std::cout << "Could Not get element " << std::endl;
    return;
  }
  while( theSample ){

    double abs, cex, dcex, prod, inel;

    std::map< std::string, double > factors = {
      { "abs",  0. },
      { "cex",  0. },
      { "dcex", 0. },
      { "prod", 0. },
      { "inel", 0. }
    };

   
    std::map< std::string , double >::iterator itFactor = factors.begin();
    
    tinyxml2::XMLError attResult;

    for( itFactor; itFactor != factors.end(); ++itFactor ){
      std::string name = itFactor->first;
      attResult = theSample->QueryDoubleAttribute(name.c_str(), &(itFactor->second) );

      if( attResult != tinyxml2::XML_SUCCESS ){
        std::cout << "Could not get " << name << std::endl;
        std::cout << "Setting to 1."  << std::endl;

        itFactor->second = 1.;
      }

      else std::cout << "Got " << name << ": " << itFactor->second << std::endl;


    }
    
    bool Raw;
    attResult = theSample->QueryBoolAttribute("Raw", &Raw);
    if( attResult != tinyxml2::XML_SUCCESS ){
      std::cout << "Could not get Raw" << std::endl;
    }

    std::string reweightFile;

    if( !Raw ){ 
      const char * reweightFileText = nullptr;

      reweightFileText = theSample->Attribute("File");
      if (reweightFileText != nullptr) reweightFile = reweightFileText;

      std::cout << "File: " << reweightFile << std::endl;
    }
    else std::cout << "Need to run reweighting" << std::endl;
    
    FitSample theFitSample;
    theFitSample.Raw  = Raw;
    theFitSample.abs  = factors["abs"];
    theFitSample.cex  = factors["cex"];
    theFitSample.inel = factors["inel"];
    theFitSample.prod = factors["prod"];
    theFitSample.dcex = factors["dcex"];
    theFitSample.theFile = reweightFile;

    //std::cout << "CHECKING " << std::endl;
    //std::cout << "abs: "  << theFitSample.abs << std::endl;
    //std::cout << "cex: "  << theFitSample.cex << std::endl;
    //std::cout << "File: " << theFitSample.theFile << std::endl;
   
    samples.push_back( theFitSample );
    
    theSample = theSample->NextSiblingElement("Sample");
  }

  tinyxml2::XMLElement * theData = theRoot->FirstChildElement("Data");
  if( !theData ){
    std::cout << "Could Not get Data " << std::endl;
    return;
  }

  std::string dataFile;
  const char * dataFileText = nullptr;
  dataFileText = theData->Attribute("File");
  if (dataFileText != nullptr) dataFile = dataFileText;
  */

}
