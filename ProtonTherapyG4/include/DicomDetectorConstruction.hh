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
// $Id: DicomDetectorConstruction.hh 101301 2016-11-14 11:19:22Z gcosmo $
//
/// \file medical/DICOM/include/DicomDetectorConstruction.hh
/// \brief Definition of the DicomDetectorConstruction class
//

#ifndef DicomDetectorConstruction_h
#define DicomDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "ProtontherapyDicomAsciiReader.hh"

#include <set>
#include <map>

class G4Material;
class G4Box;
class G4LogicalVolume;

//*******************************************************
/// Dicom detector construction
///
///      - Start the building of the geometry
///      - Initialisation of materials
///      - Creation of the world
///      - Reading of the DICOM data
///
/// History: 30.11.07  First version
/// \author  P. Arce
//*******************************************************

struct matInfo 
{
  G4double fSumdens;
  G4int fNvoxels;
  G4int fId;
};

class DicomDetectorConstruction : public G4VUserDetectorConstruction
{
public:

    DicomDetectorConstruction();
    ~DicomDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    // trigger the construction of the geometry

protected:
    void InitialisationOfMaterials();
    // create the original materials

    void ReadPhantomData();
    // read the DICOM files describing the phantom

    G4Material* BuildMaterialWithChangingDensity( const G4Material* origMate,
    float density, G4String newMateName );
    // build a new material if the density of the voxel is different to the other voxels

    virtual void ConstructPhantom() = 0;
    // construct the phantom volumes.
    //  This method should be implemented for each of the derived classes
    
protected:
    ProtontherapyDicomAsciiReader* DicomReader; 
    G4Material* fAir;

    // World ...
    G4Box* fWorld_solid;
    G4LogicalVolume* fWorld_logic;
    G4VPhysicalVolume* fWorld_phys;

    G4int fNoFiles; // number of DICOM files
    std::vector<G4Material*> fMaterials;

    G4int fNVoxelX, fNVoxelY, fNVoxelZ;
    G4double fVoxelHalfDimX, fVoxelHalfDimY, fVoxelHalfDimZ;
    
    G4bool fConstructed;
};

#endif

