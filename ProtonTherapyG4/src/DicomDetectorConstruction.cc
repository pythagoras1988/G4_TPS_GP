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
// $Id: DicomDetectorConstruction.cc 105190 2017-07-14 12:08:43Z gcosmo $
//
/// \file  medical/DICOM/src/DicomDetectorConstruction.cc
/// \brief Implementation of the DicomDetectorConstruction class
//

#include "globals.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4UIcommand.hh"
#include "G4PhysicalConstants.hh"
#include "G4NistManager.hh"
#include "G4RunManager.hh"
// #include "G4SystemOfUnits.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include "DicomDetectorConstruction.hh"
#include "ProtontherapyDicomAsciiReader.hh"
#include "MaterialConstruction.hh"

#include "DicomRunAction.hh"
#include "DicomRun.hh"
#include "G4VisAttributes.hh"

using namespace std;

using CLHEP::m;
using CLHEP::cm3;
using CLHEP::mole;
using CLHEP::g;
using CLHEP::mg;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DicomDetectorConstruction::DicomDetectorConstruction(G4VPhysicalVolume* physicalTreatmentRoom)
 : fWorld_phys(physicalTreatmentRoom),

   fNVoxelX(0),
   fNVoxelY(0),
   fNVoxelZ(0),
   fVoxelHalfDimX(0),
   fVoxelHalfDimY(0),
   fVoxelHalfDimZ(0),

   fConstructed(false)
{
  if(!fConstructed) {
    fConstructed = true;
    InitialisationOfMaterials();
    ConstructColorData();
    ReadPhantomData();
    ConstructPhantom();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DicomDetectorConstruction::~DicomDetectorConstruction()
{
}

void DicomDetectorConstruction::ReadPhantomData() {
  DicomReader = new ProtontherapyDicomAsciiReader;

  G4ThreeVector pixelNumberVector = DicomReader->GetNumberOfPixels();
  G4ThreeVector pixelSizeVector = DicomReader->GetPixelSize();
  fNVoxelX = pixelNumberVector[0];
  fNVoxelY = pixelNumberVector[1];
  fNVoxelZ = pixelNumberVector[2];
  fVoxelHalfDimX = pixelSizeVector[0]/2;
  fVoxelHalfDimY = pixelSizeVector[1]/2;
  fVoxelHalfDimZ = pixelSizeVector[2]/2;
  fMasterHUData = DicomReader->GetMasterHUData();
}

void DicomDetectorConstruction::UpdateGeometry() {
  G4RunManager::GetRunManager() -> GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DicomDetectorConstruction::InitialisationOfMaterials()
{
  MaterialConstruction* materialConstruction = new MaterialConstruction;
  fMaterials = materialConstruction->GetMaterialSets();
  fHUThresholdVector = materialConstruction->GetHUThresholdVector();
}

void DicomDetectorConstruction::ConstructColorData(){ 

}
