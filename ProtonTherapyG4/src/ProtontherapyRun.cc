#include "ProtontherapyRun.hh"
#include "G4SDManager.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
//  Constructor. 
ProtontherapyRun::ProtontherapyRun()
: G4Run()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
//    Constructor.
//   (The vector of MultiFunctionalDetector name has to given.)
ProtontherapyRun::ProtontherapyRun(const vector<G4String> mfdName): G4Run()
{
    ConstructMFD(mfdName);
}

//
// Destructor
//    clear all data members.
ProtontherapyRun::~ProtontherapyRun()
{
  //--- Clear HitsMap for RUN
  G4int Nmap = fRunMap.size();
  for ( G4int i = 0; i < Nmap; i++){
    if(fRunMap[i] ) fRunMap[i]->clear();
  }
  fCollName.clear();
  fCollID.clear();
  fRunMap.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
// Destructor
//    clear all data members.
void ProtontherapyRun::ConstructMFD(const vector<G4String>& mfdName)
{

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  //=================================================
  //  Initalize RunMaps for accumulation.
  //  Get CollectionIDs for HitCollections.
  //=================================================
  G4int Nmfd = mfdName.size(); // Number of Multifunctional detector

  for ( G4int idet = 0; idet < Nmfd ; idet++) {  // Loop for all MFD.
    G4String detName = mfdName[idet];

    //--- Seek and Obtain MFD objects from SDmanager.
    G4MultiFunctionalDetector* mfd = (G4MultiFunctionalDetector*)(SDman->FindSensitiveDetector(detName));
    //
    if ( mfd ){
      //--- Loop over the registered primitive scorers.
      for (G4int icol = 0; icol < mfd->GetNumberOfPrimitives(); icol++) {

        // Get Primitive Scorer object.
        G4VPrimitiveScorer* scorer = mfd->GetPrimitive(icol);

        // collection name and collectionID for HitsCollection,
        // where type of HitsCollection is G4THitsMap in case 
        // of primitive scorer.
        // The collection name is given by <MFD name>/<Primitive Scorer name>.
        G4String collectionName = scorer->GetName();
        G4String fullCollectionName = detName+"/"+collectionName;
        G4int    collectionID = SDman->GetCollectionID(fullCollectionName);
        //
        if ( collectionID >= 0 ){
          G4cout << "++ "<<fullCollectionName<< " id " << collectionID << G4endl;

          // Store obtained HitsCollection information into data members.
          // And, creates new G4THitsMap for accumulating quantities during RUN.
          // These data are for multiple MFD and Primitive Scorers
          fCollName.push_back(fullCollectionName);
          fCollID.push_back(collectionID);
          fRunMap.push_back(new G4THitsMap<G4double>(detName,collectionName));
        } 

        else {
          G4cout << "** collection " << fullCollectionName << " not found. "
                 <<G4endl;
        }
      }
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
//  RecordEvent is called at end of event.
//  For scoring purpose, the resultant quantity in a event,
//  is accumulated during a Run.
void ProtontherapyRun::RecordEvent(const G4Event* aEvent)
{
  G4Run::RecordEvent(aEvent);
  
  //G4cout << "Dicom Run :: Recording event " << aEvent->GetEventID() 
  //<< "..." << G4endl;
  //=============================
  // HitsCollection of This Event
  //============================
  G4HCofThisEvent* HCE = aEvent->GetHCofThisEvent();
  if (!HCE) return;
  
  //=======================================================
  // Sum up HitsMap of this Event  into HitsMap of this RUN
  //=======================================================
  G4int Ncol = fCollID.size();
  for ( G4int i = 0; i < Ncol ; i++ ) {  // Loop over HitsCollection
    G4THitsMap<G4double>* EvtMap = 0;
    if ( fCollID[i] >= 0 ) {           // Collection is attached to HCE
      EvtMap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID[i]));
    }

    else {
      G4cout <<" Error EvtMap Not Found "<< i << G4endl;
    }
    if ( EvtMap )  {
      //=== Sum up HitsMap of this event to HitsMap of RUN.===
      *fRunMap[i] += *EvtMap;

      //G4cout << "Summing EvtMap into RunMap at " << i << "..." << G4endl;
      //======================================================
    } //else {
    //G4cout << "Null pointer to EvtMap at " << i << "..." << G4endl;
    //}
  }
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Merge hits map from threads
void ProtontherapyRun::Merge(const G4Run* aRun)
{
  const ProtontherapyRun* localRun = static_cast<const ProtontherapyRun*>(aRun);
  Copy(fCollName, localRun->fCollName);
  Copy(fCollID, localRun->fCollID);
  unsigned ncopies = Copy(fRunMap, localRun->fRunMap);
  // copy function returns the fRunMap size if all data is copied
  // so this loop isn't executed the first time around
  G4cout << "DicomRun :: Num copies = " << ncopies << G4endl;
  for(unsigned i = ncopies; i < fRunMap.size(); ++i) {
    *fRunMap[i] += *localRun->fRunMap[i];
  }
  G4Run::Merge(aRun);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//=================================================================
//  Access method for HitsMap of the RUN
//
//-----
// Access HitsMap.
//  By  MultiFunctionalDetector name and Collection Name.
G4THitsMap<G4double>* ProtontherapyRun::GetHitsMap(const G4String& detName,
                                           const G4String& colName) const
{
  G4String fullName = detName+"/"+colName;
  return GetHitsMap(fullName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Access HitsMap.
//  By full description of collection name, that is
//    <MultiFunctional Detector Name>/<Primitive Scorer Name>
G4THitsMap<G4double>* ProtontherapyRun::GetHitsMap(const G4String& fullName) const
{
  
  //G4THitsMap<G4double>* hitsmap = 0;
  
  G4int Ncol = fCollName.size();
  for ( G4int i = 0; i < Ncol; i++){
    if ( fCollName[i] == fullName ){
      return fRunMap[i];
    }
  }

  G4Exception("DicomRun", fullName.c_str(), JustWarning,
              "GetHitsMap failed to locate the requested HitsMap");
  return NULL;
}
