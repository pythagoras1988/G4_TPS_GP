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
// #include "G4SystemOfUnits.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include "DicomDetectorConstruction.hh"
#include "DicomPhantomZSliceHeader.hh"

#include "DicomRunAction.hh"
#include "DicomRun.hh"
#include "G4VisAttributes.hh"

using CLHEP::m;
using CLHEP::cm3;
using CLHEP::mole;
using CLHEP::g;
using CLHEP::mg;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DicomDetectorConstruction::DicomDetectorConstruction()
 : G4VUserDetectorConstruction(),
   fAir(0),

   fWorld_solid(0),
   fWorld_logic(0),
   fWorld_phys(0),

   fContainer_solid(0),
   fContainer_logic(0),
   fContainer_phys(0),

   fNoFiles(0),
   fMateIDs(0),

   fZSliceHeaderMerged(0),

   fNVoxelX(0),
   fNVoxelY(0),
   fNVoxelZ(0),
   fVoxelHalfDimX(0),
   fVoxelHalfDimY(0),
   fVoxelHalfDimZ(0),

   fConstructed(false)
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DicomDetectorConstruction::~DicomDetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* DicomDetectorConstruction::Construct()
{
  if(!fConstructed || fWorld_phys == 0) {
    fConstructed = true;
    InitialisationOfMaterials();

    //----- Build world
    G4double worldXDimension = 1.*m;
    G4double worldYDimension = 1.*m;
    G4double worldZDimension = 1.*m;

    fWorld_solid = new G4Box( "WorldSolid",
                             worldXDimension,
                             worldYDimension,
                             worldZDimension );

    fWorld_logic = new G4LogicalVolume( fWorld_solid,
                                       fAir,
                                       "WorldLogical",
                                       0, 0, 0 );

    fWorld_phys = new G4PVPlacement( 0,
                                    G4ThreeVector(0,0,0),
                                    "World",
                                    fWorld_logic,
                                    0,
                                    false,
                                    0 );


    ReadPhantomData();
    ConstructPhantomContainer();
    ConstructPhantom();
  }
    return fWorld_phys;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DicomDetectorConstruction::InitialisationOfMaterials()
{
    // Creating elements :
    G4double z, a, density;
    G4String name, symbol;

    G4Element* elC = new G4Element( name = "Carbon",
                                   symbol = "C",
                                   z = 6.0, a = 12.011 * g/mole );
    G4Element* elH = new G4Element( name = "Hydrogen",
                                   symbol = "H",
                                   z = 1.0, a = 1.008  * g/mole );
    G4Element* elN = new G4Element( name = "Nitrogen",
                                   symbol = "N",
                                   z = 7.0, a = 14.007 * g/mole );
    G4Element* elO = new G4Element( name = "Oxygen",
                                   symbol = "O",
                                   z = 8.0, a = 16.00  * g/mole );
    G4Element* elNa = new G4Element( name = "Sodium",
                                    symbol = "Na",
                                    z= 11.0, a = 22.98977* g/mole );
    G4Element* elS = new G4Element( name = "Sulfur",
                                   symbol = "S",
                                   z = 16.0,a = 32.065* g/mole );
    G4Element* elCl = new G4Element( name = "Chlorine",
                                    symbol = "P",
                                    z = 17.0, a = 35.453* g/mole );
    G4Element* elK = new G4Element( name = "Potassium",
                                   symbol = "P",
                                   z = 19.0, a = 39.0983* g/mole );
    G4Element* elP = new G4Element( name = "Phosphorus",
                                   symbol = "P",
                                   z = 15.0, a = 30.973976* g/mole );
    G4Element* elFe = new G4Element( name = "Iron",
                                    symbol = "Fe",
                                    z = 26, a = 56.845* g/mole );
    G4Element* elMg = new G4Element( name = "Magnesium",
                                    symbol = "Mg",
                                    z = 12.0, a = 24.3050* g/mole );
    G4Element* elCa = new G4Element( name="Calcium",
                                    symbol = "Ca",
                                    z = 20.0, a = 40.078* g/mole );

    // Creating Materials :
    G4int numberofElements;

    // Air
    fAir = new G4Material( "Air",
                          1.290*mg/cm3,
                          numberofElements = 2 );
    fAir->AddElement(elN, 0.7);
    fAir->AddElement(elO, 0.3);
}



#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4PSDoseDeposit.hh"
#include "G4PSDoseDeposit3D.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DicomDetectorConstruction::SetScorer(G4LogicalVolume* voxel_logic)
{

#ifdef G4VERBOSE
  G4cout << "\t SET SCORER : " << voxel_logic->GetName() << G4endl;
#endif
  
  fScorers.insert(voxel_logic);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DicomDetectorConstruction::ConstructSDandField()
{
  
#ifdef G4VERBOSE
  G4cout << "\t CONSTRUCT SD AND FIELD" << G4endl;
#endif
  
  //G4SDManager* SDman = G4SDManager::GetSDMpointer();
  
  //SDman->SetVerboseLevel(1);
  
  //
  // Sensitive Detector Name
  G4String concreteSDname = "phantomSD";
  std::vector<G4String> scorer_names;
  scorer_names.push_back(concreteSDname);
  //------------------------
  // MultiFunctionalDetector
  //------------------------
  //
  // Define MultiFunctionalDetector with name.
  // declare MFDet as a MultiFunctionalDetector scorer
  G4MultiFunctionalDetector* MFDet = 
    new G4MultiFunctionalDetector(concreteSDname);
  G4SDManager::GetSDMpointer()->AddNewDetector( MFDet );
  //G4VPrimitiveScorer* dosedep = new G4PSDoseDeposit("DoseDeposit");
  G4VPrimitiveScorer* dosedep = 
    new G4PSDoseDeposit3D("DoseDeposit", fNVoxelX, fNVoxelY, fNVoxelZ);
  MFDet->RegisterPrimitive(dosedep);
  
  for(std::set<G4LogicalVolume*>::iterator ite = fScorers.begin(); 
      ite != fScorers.end(); ++ite) {
    SetSensitiveDetector(*ite, MFDet);
  }
  
  /*if(DicomRunAction::Instance()->GetDicomRun()) {
      DicomRunAction::Instance()->GetDicomRun()->ConstructMFD(scorer_names);
      }*/
 
}
