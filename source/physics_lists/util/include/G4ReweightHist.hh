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
// ClassName: G4ReweightHist:
//     Class to give histogram functionality for Geant4
//
// Author: 2018 Jake Calcutt
//
// Modified:
//
//----------------------------------------------------------------------------
//
#ifndef G4ReweightHist_h
#define G4ReweightHist_h

#include <vector>
#include <string>
class G4ReweightHist
{

  public:
           G4ReweightHist(std::string name, std::string title, std::vector< double > bins);
	  ~G4ReweightHist();

    int    FindBin( double ); 
    double GetBinContent( int );
    void   SetBinContent( int, double );

  private:
          // no instance needed
//  	G4ReweightHist(std::string name, std::string title, int nBins, double binLow, double binHigh);

        std::string histName;
        std::string histTitle;

        std::vector< double > histBinEdges;
        std::vector< double > histBinValues;

        
};
#endif
