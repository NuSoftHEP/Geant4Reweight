#ifndef G4Reweighter_h
#define G4Reweighter_h

#include <string>
#include <utility>

#include "TFile.h"
#include "TH1D.h"
#include "TGraph.h"

#include "G4ReweightTraj.hh"

#include <cmath>
#include <map>
#include "TTree.h"

#include <iostream>

#include "Geant4/G4CrossSectionDataStore.hh"
#include "Geant4/G4PionPlus.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4VProcess.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4HadronInelasticProcess.hh"
#include "Geant4/G4HadronElasticProcess.hh"
#include "Geant4/G4HadronCaptureProcess.hh"
#include "Geant4/G4String.hh"
#include "Geant4/G4hIonisation.hh"
#include "Geant4/G4hPairProduction.hh"
#include "Geant4/G4hBremsstrahlung.hh"
#include "Geant4/G4CoulombScattering.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4PVPlacement.hh"

#include "geant4reweight/src/PredictionBase/G4CascadeDetectorConstruction.hh"
#include "geant4reweight/src/PredictionBase/G4CascadePhysicsList.hh"
#include "geant4reweight/src/PredictionBase/G4DecayHook.hh"
#include "G4ReweightManager.hh"

#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/ParameterSet.h"
#include "cetlib/filepath_maker.h"


class G4ReweightTraj;

class G4Reweighter{
  public:

    G4Reweighter(){};
    G4Reweighter(TFile *, const std::map<std::string, TH1D*> &,
                 const fhicl::ParameterSet & material_pars,
                 G4ReweightManager * rw_manager,
                 std::vector<std::string> the_ints,
                 TH1D * inputElasticBiasHist = 0x0, bool fix = false);
    virtual ~G4Reweighter();

    double GetWeight(const G4ReweightTraj * theTraj);
    virtual std::string GetInteractionSubtype(const G4ReweightTraj &);

    void SetMomentum(double p);
    double GetDecayMFP(double p);
    double GetCoulMFP(double p);
    double GetCaptureMFP(double p);
    double GetNominalMFP(double p);
    double GetBiasedMFP(double p);
    double GetNominalElasticMFP(double p);
    double GetBiasedElasticMFP(double p);
    double GetInelasticBias(double p);
    double GetElasticBias(double p);
    double GetExclusiveFactor(double p, std::string cut);

    double GetInelasticXSec(double p);
    double GetExclusiveXSec(double p, std::string cut);
    double GetElasticXSec(double p);

    void SetNewHists(const std::map< std::string, TH1D* > &FSScales);
    void SetNewElasticHists(TH1D * inputElasticBiasHist);

    void SetInelasticPreBias(double bias) {fInelasticPreBias = bias;};
    void SetElasticPreBias(double bias) {fElasticPreBias = bias;};

  protected:

    bool fix_total = false;

    std::map<std::string, TGraph *> exclusiveFracs;
    std::map<std::string, TH1D *> inelScales;

    fhicl::ParameterSet MaterialParameters;
    G4ReweightManager * RWManager;
    TH1D * elasticBias;

    // These should be set in the constructor of the actual reweighter you use (e.g. G4PiPlusReweighter/G4PiMinusReweighter/G4ProtonReweighter)
    std::string fInelastic/* = "pi+Inelastic"*/;

    G4RunManager * rm;
    G4Track * testTrack;
    G4Step * testStep;
    G4StepPoint * testPoint;
    G4Material * testMaterial;
    G4Box * solidWorld;
    G4LogicalVolume * logicWorld;
    G4VPhysicalVolume * physWorld;
    G4CascadeDetectorConstruction * detector;
    G4CascadePhysicsList * physList;

    G4PionPlus  * piplus;
    G4ParticleDefinition * part_def;
    G4DynamicParticle * dynamic_part;

    G4DecayHook * decay_hook;
    G4HadronElasticProcess * elastic_proc;
    G4HadronInelasticProcess * inelastic_proc;
    G4CoulombScattering * coul_proc = 0x0;
    G4HadronCaptureProcess * cap_proc = 0x0;

    void SetupProcesses();
    void SetupWorld();
    void SetupParticle();

    double fInelasticPreBias = 1.;
    double fElasticPreBias = 1.;

};

#endif
