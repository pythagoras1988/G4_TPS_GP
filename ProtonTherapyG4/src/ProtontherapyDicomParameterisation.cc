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
/// \file medical/DICOM/src/DicomNestedPhantomParameterisation.cc
/// \brief Implementation of the DicomNestedPhantomParameterisation class
//
// $Id: DicomNestedPhantomParameterisation.cc 101905 2016-12-07 11:34:39Z gunter $
//

#include "ProtontherapyDicomParameterisation.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"

#include <vector>

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
ProtontherapyDicomParameterisation::
ProtontherapyDicomParameterisation(const G4ThreeVector voxelSize,
                                   vector<G4Material*> mat,
                                   vector<G4double> HUMasterData, vector<G4double> thresholdVector)
:
  fdX(voxelSize.x()), fdY(voxelSize.y()), fdZ(voxelSize.z()),
  fnX(0), fnY(0), fnZ(0),
  fProgress(0),
  fMasterHUData(HUMasterData),
  fHUThresholdVector(thresholdVector),
  fMaterials(mat),
  fMaterialIndices(0)
{
    ConstructColorData();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
ProtontherapyDicomParameterisation::~ProtontherapyDicomParameterisation()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void ProtontherapyDicomParameterisation::
SetNoVoxel( unsigned int nx, unsigned int ny, unsigned int nz )
{
  fnX = nx;
  fnY = ny;
  fnZ = nz;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4Material* ProtontherapyDicomParameterisation::
ComputeMaterial(G4VPhysicalVolume* physVol, const G4int iz,
                const G4VTouchable* parentTouch)
{

    // protection for initialization and vis at idle state
    //
    if(parentTouch==0) return fMaterials[0];

    // Copy number of voxels.
    // Copy number of X and Y are obtained from replication number.
    // Copy nymber of Z is the copy number of current voxel.
    G4int ix = parentTouch->GetReplicaNumber(0);
    G4int iy = parentTouch->GetReplicaNumber(1);

    G4int copyID = ix + fnX*iy + fnX*fnY*iz; // same formula as compressData method in ProtontherapyDicomAsciiReader class
    //G4cout<<copyID<<G4endl;
    unsigned int matIndex;
    static G4Material* mate = 0;
    G4double voxelHU;
    voxelHU = fMasterHUData[copyID];

    for (matIndex=0; matIndex<fHUThresholdVector.size();matIndex++) {
      if (voxelHU<=fHUThresholdVector[matIndex]) {
        break;
      }
    }
    // To cover the case of HU>1600 for Schneider's model
    if (matIndex==fHUThresholdVector.size()) { matIndex = fHUThresholdVector.size()-1;}

    // Verbosity for debugging use
    if (false) {
    G4cout<<"Finished placing physical volume " << ix <<"," << iy << ","
        << copyID  << "," << matIndex << "," << fHUThresholdVector[matIndex] << "..."<< G4endl;
    }

    //mate = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
    mate = fMaterials[matIndex];
    /*
    if (voxelHU>1400) {physVol->GetLogicalVolume()->SetVisAttributes(fColors[matIndex]);}
    else {physVol->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::GetInvisible());}
    */
    if (ix%6==0 && iy%6==0) {
      physVol->GetLogicalVolume()->SetVisAttributes(fColors[matIndex]);
    }
    else {
      physVol->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::GetInvisible());
    }
    physVol->GetLogicalVolume()->SetMaterial(mate);
    //G4cout<<mate->GetName()<<G4endl;
    return mate;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
unsigned int ProtontherapyDicomParameterisation::
GetMaterialIndex( unsigned int copyNo ) const
{
    //return *(fMaterialIndices+copyNo);
    return fMaterialIndices[copyNo];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Number of Materials
// Material scanner is required for preparing physics tables and so on before
// starting simulation, so that G4 has to know number of materials.
G4int ProtontherapyDicomParameterisation::GetNumberOfMaterials() const
{
    return fMaterials.size();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
// GetMaterial
//  This is needed for material scanner and realizing geometry.
//
G4Material* ProtontherapyDicomParameterisation::GetMaterial(G4int i) const
{
    return fMaterials[i];
}

//
// Transformation of voxels.
//
void ProtontherapyDicomParameterisation::
ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
    // Position of voxels.
    // x and y positions are already defined in DetectorConstruction by using
    // replicated volume. Here only we need to define is z positions of voxels.
    physVol->SetTranslation(G4ThreeVector(0.,0.,(2.*static_cast<double>(copyNo)
                                                +1.)*fdZ - fdZ*fnZ));
    /*
    G4RotationMatrix* rm = new G4RotationMatrix;
    rm->rotateZ(90*deg);
    physVol->SetRotation(rm); */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
// Dimensions are always same in this RE02 example.
//
void ProtontherapyDicomParameterisation::
ComputeDimensions( G4Box& box, const G4int, const G4VPhysicalVolume* ) const
{
    box.SetXHalfLength(fdX);
    box.SetYHalfLength(fdY);
    box.SetZHalfLength(fdZ);
}


void ProtontherapyDicomParameterisation::ConstructColorData() {
  G4double blueFraction, redFraction;
  G4double colorDataLength;
  colorDataLength = fHUThresholdVector.size();
  fColors.reserve(colorDataLength);

  for (G4int k; k<colorDataLength; k++) {
    blueFraction = 1 - k/colorDataLength;
    redFraction  = k/colorDataLength;
    fColors.push_back(new G4VisAttributes(G4Color(redFraction,0,blueFraction)));
    fColors[k] -> SetVisibility(true);
    fColors[k] -> SetForceSolid(true);
  }
}
