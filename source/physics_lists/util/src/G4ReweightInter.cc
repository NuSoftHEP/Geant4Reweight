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

#include "G4ReweightInter.hh"

 
G4ReweightInter::G4ReweightInter(std::vector< std::pair< double, double > > input) : content(input) { }

double G4ReweightInter::GetContent( double inputPoint){

  if( GetNPoints() <= 1 ) return 1.;

  if( inputPoint < GetPoint(0) || inputPoint > GetPoint( GetNPoints() - 1 ) ) return 1.;


  for(size_t i = 0; i < (GetNPoints() - 1); ++i){
    if ( inputPoint > GetPoint(i) && inputPoint < GetPoint(i+1) ){
      double lowerPoint = GetPoint(i);
      double upperPoint = GetPoint(i+1);

      double lowerValue = GetValue(i);
      double upperValue = GetValue(i+1);

      double slope = upperValue - lowerValue;
      slope = slope / ( upperPoint - lowerPoint );

      double deltaX = inputPoint - lowerPoint;

      return lowerValue + slope * deltaX;
    }
  }
  

  return 1.;
}

double G4ReweightInter::GetPoint( size_t i ){
  if( i >= GetNPoints() ) return -1.;

  return content[i].first;
}

double G4ReweightInter::GetValue( size_t i ){
  if( i >= GetNPoints() ) return -1.;

  return content[i].second;
}
