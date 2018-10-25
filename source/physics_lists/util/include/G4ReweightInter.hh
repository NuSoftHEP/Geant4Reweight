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
//
//---------------------------------------------------------------------------
//
// ClassName: G4ReweightInter:
//     Class to give histogram functionality for Geant4
//
// Author: 2018 Jake Calcutt
//
// Modified:
//
//----------------------------------------------------------------------------
//
#ifndef G4ReweightInter_h
#define G4ReweightInter_h

#include <vector>
#include <string>
class G4ReweightInter
{

  public:
           G4ReweightInter(std::vector< std::pair<double, double> > pts);
	  ~G4ReweightInter();

           size_t GetNPoints(){ return content.size(); };
           double GetContent( double );
           double GetPoint( size_t );
           double GetValue( size_t ); 

  private:

           std::vector< std::pair< double, double > > content;
        
};
#endif
