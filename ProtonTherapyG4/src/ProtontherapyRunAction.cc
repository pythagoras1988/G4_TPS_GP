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

#include "ProtontherapyRunAction.hh"
#include "ProtontherapyEventAction.hh"
#include "G4Run.hh"
#include "DicomDetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4Timer.hh"
#include "G4THitsMap.hh"
#include "ProtontherapyRun.hh"
#include <vector>
#include <fstream>

ProtontherapyRunAction::ProtontherapyRunAction()
{
    fSDName.push_back(G4String("phantomSD"));

    // Start Analysis manager for LET scoring
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetFileName("LET_D");
    analysisManager->SetActivation(true);
    //analysisManager->Ascii(true);

    G4int nbinX, nbinY, nbinZ;
    G4double binSizeX, binSizeY, binSizeZ;

    nbinX = DicomDetectorConstruction::NumOfVoxels_global[0];
    nbinY = DicomDetectorConstruction::NumOfVoxels_global[1];
    nbinZ = DicomDetectorConstruction::NumOfVoxels_global[2];
    nbinZ = 3;
    nbinY = 512;
    nbinX = 512;

    binSizeX = DicomDetectorConstruction::SizeOfVoxels_global[0];
    binSizeY = DicomDetectorConstruction::SizeOfVoxels_global[1];
    binSizeZ = DicomDetectorConstruction::SizeOfVoxels_global[2];
    binSizeX = 0.976562;
    binSizeY = 0.976562;
    binSizeZ = 2;

    binSizeX = 1;
    binSizeY = 1;
    binSizeZ = 1;

    G4int ih = analysisManager->CreateH3("1", "edep2",
                      nbinX, 0, binSizeX*nbinX,
                      nbinY, 0, binSizeY*nbinY,
                      nbinZ, 30.5, 33.5);
    analysisManager->SetH3Activation(ih, true);
    G4cout<<ih<<G4endl;
    G4cout<<nbinX<<nbinY<<nbinZ<<G4endl;

    ih = analysisManager->CreateH3("2", "edep",
                      nbinX, 0, binSizeX*nbinX,
                      nbinY, 0, binSizeY*nbinY,
                      nbinZ, 30.5, 33.5);
    analysisManager->SetH3Activation(ih, true);
    G4cout<<ih<<G4endl;

    /*
    G4int ih = analysisManager->CreateH3("1", "edep2",
                      nbinX, -binSizeX*nbinX/2, binSizeX*nbinX/2,
                      nbinY, -binSizeY*nbinY/2, binSizeY*nbinY/2,
                      nbinZ, -binSizeZ*nbinZ/2, binSizeZ*nbinZ/2,
                      "mm", "mm", "mm");
    analysisManager->SetH3Activation(ih, true);
    G4cout<<ih<<G4endl;
    G4cout<<nbinX<<nbinY<<nbinZ<<G4endl;

    ih = analysisManager->CreateH3("2", "edep",
                      nbinX, -binSizeX*nbinX/2, binSizeX*nbinX/2,
                      nbinY, -binSizeY*nbinY/2, binSizeY*nbinY/2,
                      nbinZ, -binSizeZ*nbinZ/2, binSizeZ*nbinZ/2,
                      "mm", "mm", "mm");
    analysisManager->SetH3Activation(ih, true);
    G4cout<<ih<<G4endl;
    */

}

ProtontherapyRunAction::~ProtontherapyRunAction()
{
    fSDName.clear();
    delete G4AnalysisManager::Instance();
}

G4Run* ProtontherapyRunAction::GenerateRun() {
    return fProtontherapyrun = new ProtontherapyRun(fSDName);
}


void ProtontherapyRunAction::BeginOfRunAction(const G4Run* aRun)
{
    G4RunManager::GetRunManager()-> SetRandomNumberStore(true);
    G4cout << "Run " << aRun -> GetRunID() << " starts ..." << G4endl;

    electromagnetic = 0;
    hadronic = 0;

    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    if ( analysisManager->IsActive() ) {
      analysisManager->OpenFile();
      G4cout<<"start analysis manager"<<G4endl;
    }
}

void ProtontherapyRunAction::EndOfRunAction(const G4Run* aRun)
{
     G4int nofEvents = aRun->GetNumberOfEvent();
    if (nofEvents == 0) return;


    static double local_total_dose = 0;
    double total_dose = 0;

  const ProtontherapyRun* reRun = static_cast<const ProtontherapyRun*>(aRun);
  //--- Dump all scored quantities involved in Run.
  for ( G4int i = 0; i < (G4int)fSDName.size(); i++ ){
    //
    //---------------------------------------------
    // Dump accumulated quantities for this RUN.
    //  (Display only central region of x-y plane)
    //      0       ConcreteSD/DoseDeposit
    //---------------------------------------------
    G4THitsMap<G4double>* DoseDeposit = reRun->GetHitsMap(fSDName[i]+"/DoseDeposit");

    if( DoseDeposit && DoseDeposit->GetMap()->size() != 0 ) {
      std::map<G4int,G4double*>::iterator itr = DoseDeposit->GetMap()->begin();
      for(; itr != DoseDeposit->GetMap()->end(); itr++) {
        if(!IsMaster()) { local_total_dose += *(itr->second); }
        total_dose += *(itr->second);
      }
    }

  }

  if (IsMaster()) {
      G4cout
        << "\n--------------------End of Global Run-----------------------"
        << " \n The run was " << nofEvents << " events " << G4endl;
      G4cout << "LOCAL TOTAL DOSE : \t" << local_total_dose/gray << " Gy" << G4endl;
      G4cout << "      TOTAL DOSE : \t" << total_dose/gray << " Gy" << G4endl;

    }
  else
    {
      G4cout
        << "\n--------------------End of Local Run------------------------"
        << " \n The run was " << nofEvents << G4endl;
      G4cout << "LOCAL TOTAL DOSE : \t" << local_total_dose/gray << " Gy" << G4endl;
      G4cout << "      TOTAL DOSE : \t" << total_dose/gray << " Gy" << G4endl;

    }

  if(IsMaster()) {
    G4cout << " ###### EndOfRunAction ###### " << G4endl;

    const ProtontherapyRun* re02Run = static_cast<const ProtontherapyRun*>(aRun);
    //--- Dump all scored quantities involved in Run.
    for ( G4int i = 0; i < (G4int)fSDName.size(); i++ ){
          //
          //---------------------------------------------
          // Dump accumulated quantities for this RUN.
          //  (Display only central region of x-y plane)
          //      0       ConcreteSD/DoseDeposit
          //---------------------------------------------
        G4THitsMap<G4double>* DoseDeposit = re02Run->GetHitsMap(fSDName[i]+"/DoseDeposit"); // Name from PS in ScanningProtonBeamlin

        G4cout << "============================================================="
                 <<G4endl;
        G4cout << " Number of event processed : "
                 << aRun->GetNumberOfEvent() << G4endl;
        G4cout << "============================================================="
                 <<G4endl;

        std::ofstream fileout;
        G4String fname = "dose.out";
        fileout.open(fname);
        G4cout << " opened file " << fname << " for dose output" << G4endl;

        if( DoseDeposit && DoseDeposit->GetMap()->size() != 0 ) {
            std::map<G4int,G4double*>::iterator itr =DoseDeposit->GetMap()->begin();
            for(; itr != DoseDeposit->GetMap()->end(); itr++) {
                fileout <<  itr->first <<  "\t"  << *(itr->second)/CLHEP::gray << G4endl;
            }
            G4cout << "============================================="<<G4endl;
        }

        else {
            G4Exception("ProtontherapyRunAction", "000", JustWarning, "DoseDeposit HitsMap is either a null pointer of the HitsMap was empty");
        }
      fileout.close();
      G4cout << " closed file " << fname << " for dose output" << G4endl;

    }
  }

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->IsActive() ) {
  analysisManager->Write();
  analysisManager->CloseFile();
}


  G4cout << "Finished : End of Run Action " << aRun->GetRunID() << G4endl;


}
void ProtontherapyRunAction::AddEMProcess()
{
 electromagnetic += 1;
}
void ProtontherapyRunAction::AddHadronicProcess()
{
  hadronic += 1;
}
