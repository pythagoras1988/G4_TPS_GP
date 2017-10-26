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
// Hadrontherapy advanced example for Geant4
// See more at: https://twiki.cern.ch/twiki/bin/view/Geant4/AdvancedExamplesHadrontherapy

#include "ProtontherapyPrimaryGeneratorAction.hh"
#include "ProtontherapyPrimaryGeneratorMessenger.hh"
#include "ScanningProtonBeamSpecification.hh"
#include "ScanningProtonBeamLine.hh"
#include "ProtontherapyRunAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "globals.hh"

#include <math.h>
#include <fstream>
#define pi 3.14159265358979323846;

using namespace std;
G4int ProtontherapyPrimaryGeneratorAction::totalEvents;

/////////////////////////////////////////////////////////////////////////////
ProtontherapyPrimaryGeneratorAction::ProtontherapyPrimaryGeneratorAction()
: yPos(0),
  zPos(0)
{
  messenger = new ProtontherapyPrimaryGeneratorMessenger(this);
  // Definition of the General particle Source
  //particleGun = new G4GeneralParticleSource();
  particleGun   = new G4ParticleGun(1);
  G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("proton"); //can be overwrite in mac file
  particleGun->SetParticleDefinition(particle);

  // Define Twiss parameters
  /*
  epsilon_y = 2.2/pi; // in pi * mm * mrad
  epsilon_z = 2.0/pi;
  alpha     = 2.4;
  beta      = 6.6;
  */
  scanSpecification = new ScanningProtonBeamSpecification();
  weightData = scanSpecification->GetWeightDataMap();
  energyLayerData = scanSpecification->GetEnergyList();
  totalEvents = scanSpecification->GetNumberOfEvents();
  G4cout<<totalEvents<<G4endl;
}

/////////////////////////////////////////////////////////////////////////////
ProtontherapyPrimaryGeneratorAction::~ProtontherapyPrimaryGeneratorAction()
{
  delete particleGun;
  delete scanSpecification;
  delete messenger;

}

void ProtontherapyPrimaryGeneratorAction::SetEnergyAndField(G4int eventID) {
  vector<G4double> weightDataPerMap;
  G4double field_x, field_y;
  G4int k;
  int fluenceCounter(0);

  for (k=0;k<weightData.size();k++) {
    weightDataPerMap = weightData[k];
    fluenceCounter += weightDataPerMap[3];
    if (eventID<=fluenceCounter) {break;}
  }

  yPos = weightDataPerMap[1]*mm;
  zPos = weightDataPerMap[2]*mm;
  field_x = yPos/10*sqrt(weightDataPerMap[0])/180;
  field_y = zPos/10*sqrt(weightDataPerMap[0])/180;

  particleGun->SetParticleEnergy((weightDataPerMap[0])*MeV);

}

  /////////////////////////////////////////////////////////////////////////////
  void ProtontherapyPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
  {
    xPos      = -236.1*cm;
    // Implement Scanning
    if (ScanningProtonBeamLine::scanOption == "scanning") {
      G4int eventID = anEvent->GetEventID();
      SetEnergyAndField(eventID);
      xPos      = -770.0*mm;
    }

    // Determine Covariance matrix
    gamma     = (1 + alpha*alpha) / beta;

    var_y = epsilon_y * beta;
    var_z = epsilon_z * beta;
    var_yp = epsilon_y * gamma;
    var_zp = epsilon_z * gamma;
    cov_y    = 1 * alpha * epsilon_y;
    cov_z    = 1 * alpha * epsilon_z;

    // Perform Cholesky Decomposition on the covariance matrix for (a b)
    //                                                             (b c)
    G4double x1,x2,y1,y2;
    x1 = G4RandGauss::shoot();
    x2 = G4RandGauss::shoot();
    y1 = G4RandGauss::shoot();
    y2 = G4RandGauss::shoot();

    y_sample  = sqrt(var_y) * x1;
    yp_sample = cov_y/sqrt(var_y)*x1 + sqrt(var_y*var_yp-pow(cov_y,2))/sqrt(var_y)*x2; // in mrad
    z_sample  = sqrt(var_z) * y1;
    zp_sample = cov_z/sqrt(var_z)*y1 + sqrt(var_z*var_zp-pow(cov_z,2))/sqrt(var_z)*y2;
    y_sample  = y_sample * mm;
    z_sample  = z_sample * mm;

    dc_y = sin(yp_sample*0.001);
    dc_z = sin(zp_sample*0.001);
    dc_x = sqrt(1 - pow(dc_y,2) - pow(dc_z,2));

    //Set particlegun
    particleGun->SetParticleMomentumDirection(G4ThreeVector(dc_x,dc_y,dc_z));
    particleGun->SetParticlePosition(G4ThreeVector(xPos,yPos+y_sample,zPos+z_sample));

    /*
    ofstream saveFile;
    saveFile.open("inital_phase_space.txt",ios_base::app);
    if(saveFile.is_open()) {
      saveFile << y_sample << '\t';
      saveFile << yp_sample << '\t';
      saveFile << z_sample << '\t';
      saveFile << zp_sample << '\n';
    }
    saveFile.close();
    */

    particleGun -> GeneratePrimaryVertex( anEvent );
    }

    void ProtontherapyPrimaryGeneratorAction::SetTwissAlpha(G4double value)
    {
      alpha = value; }

    void ProtontherapyPrimaryGeneratorAction::SetTwissBeta(G4double value)
    {
      beta = value; }

    void ProtontherapyPrimaryGeneratorAction::SetTwissEmittance_y(G4double value)
    {
      epsilon_y = value / pi; }

    void ProtontherapyPrimaryGeneratorAction::SetTwissEmittance_z(G4double value)
    {
      epsilon_z = value / pi; }
