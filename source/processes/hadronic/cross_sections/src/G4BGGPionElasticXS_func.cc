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
// $Id: G4BGGPionElasticXS.cc 93682 2015-10-28 10:09:49Z gcosmo $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4BGGPionElasticXS
//
// Author:        Vladimir Ivanchenko
//
// Creation date: 01.10.2003
// Modifications:
//
// -------------------------------------------------------------------
//

#include "G4BGGPionElasticXS_func.hh"
#include "G4SystemOfUnits.hh"
#include "G4ComponentGGHadronNucleusXsc.hh"
#include "G4UPiNuclearCrossSection.hh"
#include "G4HadronNucleonXsc.hh"
#include "G4ComponentSAIDTotalXS.hh"
#include "G4Proton.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4NistManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4BGGPionElasticXS_func::G4BGGPionElasticXS_func(const G4ParticleDefinition*, G4ReweightHist * bias_hist) 
 : G4VCrossSectionDataSet("Barashenkov-Glauber"), theBias(bias_hist) 
{
  verboseLevel = 0;
  fGlauberEnergy = 91.*GeV;
  fLowEnergy = 20.*MeV;
  fSAIDHighEnergyLimit = 2.6*GeV;
  SetMinKinEnergy(0.0);
  SetMaxKinEnergy(100*TeV);

  for (G4int i = 0; i < 93; i++) {
    theGlauberFac[i] = 0.0;
    theCoulombFac[i] = 0.0;
    theA[i] = 1;
  }
  fPion = 0;
  fGlauber = 0;
  fHadron  = 0;
  fSAID    = 0;
  particle = 0;
  theProton= G4Proton::Proton();
  isPiplus = false;
  isInitialized = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4BGGPionElasticXS_func::~G4BGGPionElasticXS_func()
{
  delete fSAID;
  delete fHadron;
  delete fPion;
  delete fGlauber;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool 
G4BGGPionElasticXS_func::IsElementApplicable(const G4DynamicParticle*, G4int,
					   const G4Material*)
{
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool G4BGGPionElasticXS_func::IsIsoApplicable(const G4DynamicParticle*, 
					      G4int Z, G4int A,  
					      const G4Element*,
					      const G4Material*)
{
  return (1 == Z && 2 >= A);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double
G4BGGPionElasticXS_func::GetElementCrossSection(const G4DynamicParticle* dp,
					   G4int ZZ, const G4Material*)
{
  // this method should be called only for Z > 1

  G4double cross = 0.0;
  G4double ekin = dp->GetKineticEnergy();
  G4int Z = ZZ;
  if(1 == Z) {
    cross = 1.0115*GetIsoCrossSection(dp,1,1);
  } else {
    if(Z > 92) { Z = 92; }

    if(ekin <= fLowEnergy) {
      cross = theCoulombFac[Z];
    } else if(ekin > fGlauberEnergy) {
      cross = theGlauberFac[Z]*fGlauber->GetElasticGlauberGribov(dp, Z, theA[Z]);
    } else {
      cross = fPion->GetElasticCrossSection(dp, Z, theA[Z]);
    }
  }
  if(verboseLevel > 1) {
    G4cout << "G4BGGPionElasticXS_func::GetElementCrossSection  for "
	   << dp->GetDefinition()->GetParticleName()
	   << "  Ekin(GeV)= " << dp->GetKineticEnergy()
	   << " in nucleus Z= " << Z << "  A= " << theA[Z]
	   << " XS(b)= " << cross/barn 
	   << G4endl;
  }
  //G4cout << "Returning cross" << G4endl;
  //G4cout << "Bias: " << theBias << G4endl;
  //return cross;
  
  G4double momentum = dp->GetTotalMomentum() / MeV;
  int theBin = theBias->FindBin(momentum);
//  if( theBin < theBias->GetNbinsX() || theBin > theBias->GetNbinsX() ){
  if( theBin == -1 ){ 
    std::cout << "Out of momentum range. Should scale by x1" << std::endl
              << theBias->GetBinContent( theBin )             << std::endl;
  }
 
//funchere
  //Just scale by 1
  if (theBin == -1){
    return cross;
  }
  else if(theBin == 0){

    //Interpolate from 1 at the low bin edge
    if(momentum < theBias->GetBinCenter(theBin)){
      double deltaX = momentum - theBias->GetBinLowEdge( theBin ); 
      double width  = theBias->GetBinCenter( theBin ) - theBias->GetBinLowEdge( theBin );
      double deltaY = theBias->GetBinContent( theBin ) - 1.;

      double val = (deltaY / width) * deltaX + 1.;
      return val * cross;
    }    
    //Interpolate towards the next bin edge
    else{
      
      //Check if there's only 1 bin
      //If so, interpolate towards 1 at the high bin edge
      if(theBias->GetNBins() == 1){
        double deltaX = momentum - theBias->GetBinCenter( theBin ); 
        double width  = theBias->GetBinHighEdge( theBin ) - theBias->GetBinCenter( theBin);
        double deltaY = 1. - theBias->GetBinContent( theBin );

        double val = (deltaY / width) * deltaX + theBias->GetBinContent( theBin );
        return val * cross;
      }

      //else interpolate towards the higher bin's center       
      double deltaX = momentum - theBias->GetBinCenter( theBin );
      double width  = theBias->GetBinCenter( theBin + 1 )  - theBias->GetBinCenter( theBin ); 
      double deltaY = theBias->GetBinContent( theBin + 1 ) - theBias->GetBinContent( theBin );

      double val = (deltaY / width) * deltaX + theBias->GetBinContent( theBin );
      return val * cross;
    }
  }
  //If we're at the highest bin
  else if(theBin == theBias->GetNBins() - 1){

    //Interpolate towards 1 at the highest bin edge
    if( momentum > theBias->GetBinCenter( theBin ) ){     
      double deltaX = momentum - theBias->GetBinCenter( theBin );
      double width  = theBias->GetBinHighEdge( theBin ) - theBias->GetBinCenter( theBin);
      double deltaY = 1. - theBias->GetBinContent( theBin );

      double val = (deltaY / width) * deltaX + theBias->GetBinContent( theBin );
      return val * cross;
    }

    //Else interpolate towards the lower bin's center
    double deltaX = momentum - theBias->GetBinCenter( theBin - 1 );
    double width  = theBias->GetBinCenter( theBin )  - theBias->GetBinCenter( theBin - 1 ); 
    double deltaY = theBias->GetBinContent( theBin ) - theBias->GetBinContent( theBin - 1);

    double val = (deltaY / width) * deltaX + theBias->GetBinContent( theBin - 1 ); 
    return val * cross;
  }
  //We're somewhere in the middle
  else{
    
    //Interpolate from center of this bin 
    //towards center of next
    if( momentum > theBias->GetBinCenter( theBin ) ){
      double deltaX = momentum - theBias->GetBinCenter( theBin );
      double width  = theBias->GetBinCenter( theBin + 1 )  - theBias->GetBinCenter( theBin );
      double deltaY = theBias->GetBinContent( theBin + 1 ) - theBias->GetBinContent( theBin );

      double val = (deltaY / width) * deltaX + theBias->GetBinContent( theBin );
      return val * cross;
    }
    //Interpolate to center of this bin
    //from center of lower
    else{
      double deltaX = momentum - theBias->GetBinCenter( theBin - 1 );
      double width  = theBias->GetBinCenter( theBin )  - theBias->GetBinCenter( theBin - 1);
      double deltaY = theBias->GetBinContent( theBin ) - theBias->GetBinContent( theBin - 1);  

      double val = (deltaY / width) * deltaX + theBias->GetBinContent( theBin - 1 );
      return val * cross;
    }
  }

 // return theBias->GetBinContent( theBin )*cross;
}

G4double
G4BGGPionElasticXS_func::GetIsoCrossSection(const G4DynamicParticle* dp, 
				       G4int Z, G4int A, 
				       const G4Isotope*,
				       const G4Element*,
				       const G4Material*)
{
  // this method should be called only for Z = 1

  G4double cross = 0.0;
  G4double ekin = dp->GetKineticEnergy();

  if(ekin <= fSAIDHighEnergyLimit) {
    cross = fSAID->GetElasticIsotopeCrossSection(particle, ekin, 1, 1);
  } else {
    fHadron->GetHadronNucleonXscPDG(dp, theProton);
    cross = theCoulombFac[1]*fHadron->GetElasticHadronNucleonXsc();
  } 
  cross *= A;
  /*
  if(ekin <= fLowEnergy) {
    cross = theCoulombFac[1];

  } else if( A < 2) {
    fHadron->GetHadronNucleonXscNS(dp, G4Proton::Proton());
    cross = fHadron->GetElasticHadronNucleonXsc();
  } else {
    fHadron->GetHadronNucleonXscNS(dp, G4Proton::Proton());
    cross = fHadron->GetElasticHadronNucleonXsc();
    fHadron->GetHadronNucleonXscNS(dp, G4Neutron::Neutron());
    cross += fHadron->GetElasticHadronNucleonXsc();
  }
  */
  if(verboseLevel > 1) {
    G4cout << "G4BGGPionElasticXS_func::GetIsoCrossSection  for "
	   << dp->GetDefinition()->GetParticleName()
	   << "  Ekin(GeV)= " << dp->GetKineticEnergy()
	   << " in nucleus Z= " << Z << "  A= " << A
	   << " XS(b)= " << cross/barn 
	   << G4endl;
  }
  return cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4BGGPionElasticXS_func::BuildPhysicsTable(const G4ParticleDefinition& p)
{

  G4cout << "BUILT BGG TABLE" << G4endl;

  if(&p == G4PionPlus::PionPlus() || &p == G4PionMinus::PionMinus()) {
    particle = &p;
  } else {
    G4cout << "### G4BGGPionElasticXS_func WARNING: is not applicable to " 
	   << p.GetParticleName()
	   << G4endl;
    throw G4HadronicException(__FILE__, __LINE__,
	  "G4BGGPionElasticXS_func::BuildPhysicsTable is used for wrong particle");
    return;
  }

  if(isInitialized) { return; }
  isInitialized = true;

  fPion    = new G4UPiNuclearCrossSection();
  fGlauber = new G4ComponentGGHadronNucleusXsc();
  fHadron  = new G4HadronNucleonXsc();
  fSAID    = new G4ComponentSAIDTotalXS();

  fPion->BuildPhysicsTable(*particle);
  fGlauber->BuildPhysicsTable(*particle);

  if(particle == G4PionPlus::PionPlus()) { isPiplus = true; }

  G4ThreeVector mom(0.0,0.0,1.0);
  G4DynamicParticle dp(particle, mom, fGlauberEnergy);

  G4NistManager* nist = G4NistManager::Instance();

  G4double csup, csdn;
  G4int A;

  if(verboseLevel > 0) {
    G4cout << "### G4BGGPionElasticXS_func::Initialise for "
	   << particle->GetParticleName() << G4endl;
  }
  for(G4int iz=2; iz<93; iz++) {

    A = G4lrint(nist->GetAtomicMassAmu(iz));
    theA[iz] = A;

    csup = fGlauber->GetElasticGlauberGribov(&dp, iz, A);
    csdn = fPion->GetElasticCrossSection(&dp, iz, A);

    theGlauberFac[iz] = csdn/csup;
    if(verboseLevel > 0) {
      G4cout << "Z= " << iz <<  "  A= " << A 
	     << " factor= " << theGlauberFac[iz] << G4endl; 
    }
  }
  /*
  dp.SetKineticEnergy(fLowEnergy);
  fHadron->GetHadronNucleonXscNS(&dp, G4Proton::Proton());
  theCoulombFac[1] = fHadron->GetElasticHadronNucleonXsc();
  */
  dp.SetKineticEnergy(fSAIDHighEnergyLimit);
  fHadron->GetHadronNucleonXscPDG(&dp, theProton);
  theCoulombFac[1] = 
    fSAID->GetElasticIsotopeCrossSection(particle,fSAIDHighEnergyLimit,1,1)
    /fHadron->GetElasticHadronNucleonXsc();

  dp.SetKineticEnergy(fLowEnergy);
  for(G4int iz=2; iz<93; iz++) {
    theCoulombFac[iz] = fPion->GetElasticCrossSection(&dp, iz, theA[iz]);
    if(verboseLevel > 0) {
      G4cout << "Z= " << iz <<  "  A= " << A 
	     << " factor= " << theCoulombFac[iz] << G4endl; 
    }
  }
}

void
G4BGGPionElasticXS_func::CrossSectionDescription(std::ostream& outFile) const 
{
  outFile << "The Barashenkov-Glauber-Gribov cross section handles elastic\n"
          << "scattering of pions from nuclei at all energies. The\n"
          << "Barashenkov parameterization is used below 91 GeV and the\n"
          << "Glauber-Gribov parameterization is used above 91 GeV.\n";
}
