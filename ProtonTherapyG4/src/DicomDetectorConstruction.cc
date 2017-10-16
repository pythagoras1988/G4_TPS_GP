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

#include "globals.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4UIcommand.hh"
#include "G4PhysicalConstants.hh"
#include "G4NistManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Region.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include "DicomDetectorConstruction.hh"
#include "ProtontherapyDicomAsciiReader.hh"
#include "ProtontherapyDicomDetectorConstruction.hh"
#include "MaterialConstruction.hh"
#include "ScanningProtonBeamLine.hh"

#include "G4VisAttributes.hh"
#include "G4Color.hh"

#include <vector>
#include <set>

using namespace std;
G4bool DicomDetectorConstruction::useDicom = true;

using CLHEP::m;
using CLHEP::cm3;
using CLHEP::mole;
using CLHEP::g;
using CLHEP::mg;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DicomDetectorConstruction::DicomDetectorConstruction(G4LogicalVolume* logicTreatmentRoom)
 : fWorld_logic(logicTreatmentRoom),

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
    ReadPhantomData();
    if (!useDicom) {
      G4ThreeVector phantomVoxelSize, phantomDimension;
      phantomVoxelSize = G4ThreeVector(1,1,1); // In mm
      phantomDimension = G4ThreeVector(300,300,300); // In mm

      fVoxelHalfDimX = phantomVoxelSize[0]/2*mm;
      fVoxelHalfDimY = phantomVoxelSize[1]/2*mm;
      fVoxelHalfDimZ = phantomVoxelSize[2]/2*mm;
      fNVoxelX       = static_cast<G4int> (phantomDimension[0]/phantomVoxelSize[0]);
      fNVoxelY       = static_cast<G4int> (phantomDimension[1]/phantomVoxelSize[1]);
      fNVoxelZ       = static_cast<G4int> (phantomDimension[2]/phantomVoxelSize[2]);
    }
    ConstructContainerVolume();

  //  StartConstructPhantom();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DicomDetectorConstruction::~DicomDetectorConstruction()
{
  delete DicomReader;
}

void DicomDetectorConstruction::ReadPhantomData() {
  DicomReader = new ProtontherapyDicomAsciiReader;

  G4ThreeVector pixelNumberVector = DicomReader->GetNumberOfPixels();
  G4ThreeVector pixelSizeVector = DicomReader->GetPixelSize();
  fNVoxelX = pixelNumberVector[0];
  fNVoxelY = pixelNumberVector[1];
  fNVoxelZ = pixelNumberVector[2];
  fVoxelHalfDimX = pixelSizeVector[0]/2*mm;
  fVoxelHalfDimY = pixelSizeVector[1]/2*mm;
  fVoxelHalfDimZ = pixelSizeVector[2]/2*mm;
  fMasterHUData = DicomReader->GetMasterHUData();
  fDirectionCosine_row = DicomReader->GetDirectionCosine_row();
  fDirectionCosine_col = DicomReader->GetDirectionCosine_col();
  fSliceRefPosition = DicomReader->GetSliceRefPosition();
}

void DicomDetectorConstruction::UpdateGeometry() {
  G4RunManager::GetRunManager() -> GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DicomDetectorConstruction::InitialisationOfMaterials()
{
  fAir =  G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"); // Build Air
  MaterialConstruction* materialConstruction = new MaterialConstruction; // Build phantom materials
  fMaterials = materialConstruction->GetMaterialSets();
  fHUThresholdVector = materialConstruction->GetHUThresholdVector();
}

void DicomDetectorConstruction::ConstructContainerVolume() {
   //----- Define the volume that contains all the voxels
  fContainer_solid = new G4Box("phantomContainer",fNVoxelX*fVoxelHalfDimX,
                               fNVoxelY*fVoxelHalfDimY,
                               fNVoxelZ*fVoxelHalfDimZ);
  fContainer_logic =
    new G4LogicalVolume( fContainer_solid,
   //the material is not important, it will be fully filled by the voxels
                         fMaterials[0],
                         "phantomContainer",
                         0, 0, 0 );

  G4double minX(10000);
  G4double minY(10000);
  G4double minZ(10000);
  G4ThreeVector refPosition;

  for (vector<G4ThreeVector>::iterator it=fSliceRefPosition.begin(); it!=fSliceRefPosition.end(); ++it) {
    refPosition = *it;
    if (refPosition[0]<minX) {minX = refPosition[0];}
    if (refPosition[1]<minX) {minY = refPosition[1];}
    if (refPosition[2]<minX) {minZ = refPosition[2];}
  }

  // Use offset from DICOM data;
  //G4double fOffsetX = (minX+fNVoxelX*fVoxelHalfDimX);
  //G4double fOffsetY = (minY+fNVoxelY*fVoxelHalfDimY);
  //G4double fOffsetZ = (minZ+fNVoxelZ*fVoxelHalfDimZ);

  // Take offset as isocentre
  G4double fOffsetX = 0;
  G4double fOffsetY = 0;
  G4double fOffsetZ = 0;

  G4ThreeVector posCentreVoxels(fOffsetX,fOffsetY,fOffsetZ);

  fContainer_phys =
    new G4PVPlacement(0,  // rotation
                      posCentreVoxels,
                      fContainer_logic,     // The logic volume
                      "phantomContainer",  // Name
                      fWorld_logic,  // Mother which is the world
                      false,           // No op. bool.
                      1);              // Copy number

    fContainer_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

    G4Region*  aRegion = new G4Region("phantomContainer");
    fContainer_logic -> SetRegion(aRegion);
    aRegion -> AddRootLogicalVolume(fContainer_logic);
}

void DicomDetectorConstruction::ChangeFieldAngle(G4ThreeVector rotVector) {
  G4RotationMatrix* rm = new G4RotationMatrix;
  rm->rotateY(rotVector[1]*deg);
  rm->rotateZ(rotVector[2]*deg);
  fContainer_phys->SetRotation(rm);
  G4RunManager::GetRunManager() -> GeometryHasBeenModified();
}

void DicomDetectorConstruction::SetScorer(G4LogicalVolume* voxel_logic)
{
  G4cout << "\t SET SCORER : " << voxel_logic->GetName() << G4endl;
  fScorers.insert(voxel_logic);
}

set<G4LogicalVolume*> DicomDetectorConstruction::GetScorerLogicalVolume() {
  return fScorers;
}
