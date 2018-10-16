//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//---------------------------------------------------------------------------
//
//
// Author: 2018 Jake Calcutt 
//
// Modified:
//
//----------------------------------------------------------------------------
//

#include "G4ReweightHist.hh"

 
G4ReweightHist::G4ReweightHist(std::string name, std::string title, std::vector< double > bins) :
  histName(name), histTitle(title) {
  
  //pass the bin edges
  histBinEdges = bins;

  //initialize the bin values
  if ( bins.size() > 0 )  histBinValues = std::vector< double >(bins.size() - 1, 0.);
  else histBinValues = std::vector< double >();
}

int G4ReweightHist::FindBin( double input ){

  if(histBinEdges.size() == 0) return -1;

  for(int i = 0; i < histBinEdges.size() - 1; ++i){
    if( (input > histBinEdges[i]) && (input < histBinEdges[i+1]) ){
      return i;    
    }
  }

  return -1;

}

double G4ReweightHist::GetBinContent( int theBin ){
  if (theBin == -1) return 1.;
//  else if (theBin > histBinValues.size() - 1) return 0.;

  else return histBinValues[theBin];
}

void G4ReweightHist::SetBinContent( int theBin, double theContent){
  if ( ( theBin < 0 ) || ( theBin > histBinValues.size() - 1 ) ) return;

  histBinValues[theBin] = theContent;
}
