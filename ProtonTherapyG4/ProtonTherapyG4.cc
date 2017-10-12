

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VModularPhysicsList.hh"
#include "ProtontherapyEventAction.hh"
#include "ProtontherapyPhysicsList.hh"
#include "ProtontherapyPrimaryGeneratorAction.hh"
#include "ProtontherapyRunAction.hh"
#include "Randomize.hh"

#include "G4UImessenger.hh"
#include "globals.hh"
#include "ProtontherapySteppingAction.hh"
#include "ProtontherapyGeometryController.hh"
#include "ProtontherapyGeometryMessenger.hh"
#include "FTFP_BERT.hh"
#include "G4PhysListFactory.hh"

#include "G4ScoringManager.hh"
#include <time.h>

//************************MT*********************
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "ProtontherapyActionInitialization.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc ,char ** argv)
{
    // Set the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());

    //************************MT*********************
    #ifdef G4MULTITHREADED
        G4MTRunManager* runManager = new G4MTRunManager;
        //runManager->SetNumberOfThreads(8); // Is equal to 2 by default, it can be setted also with the macro command: /run/numberOfThread 2
    #else
       G4RunManager* runManager = new G4RunManager;
    #endif


    // Geometry controller is responsible for instantiating the
    // geometries. All geometry specific setup tasks are now in class
    // HadrontherapyGeometryController.
    ProtontherapyGeometryController *geometryController = new ProtontherapyGeometryController();

    // Connect the geometry controller to the G4 user interface
    ProtontherapyGeometryMessenger *geometryMessenger = new ProtontherapyGeometryMessenger(geometryController);

    /*
    G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
    scoringManager->SetVerboseLevel(1);
    */


    // Initialize the default Hadrontherapy geometry
    geometryController->SetGeometry("default");

    // Initialize command based scoring
    //G4ScoringManager::GetScoringManager();

    // Initialize the physics

    G4VModularPhysicsList* phys = 0;

    G4cout << "Using HadrontherapyPhysicsList()" << G4endl;
    phys = new ProtontherapyPhysicsList();

    /*
    G4int verbose = 1;
    G4PhysListFactory factory;
    G4VModularPhysicsList* phys = factory.GetReferencePhysList("QGSP_BIC_EMY");
    phys -> SetVerboseLevel(verbose);
    */
    runManager->SetUserInitialization(phys);


    //************************MT
    runManager->SetUserInitialization(new ProtontherapyActionInitialization);
    G4int totalEvents;
    G4cout<<"Number of total Events = "<<totalEvents<<G4endl;
    //runManager->BeamOn(16570);

    // Get the pointer to the visualization manager
#ifdef G4VIS_USE
    G4VisManager* visManager = new G4VisExecutive;
    visManager -> Initialize();
#endif

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if (argc == 1)   // Define UI session for interactive mode.
    {
#ifdef G4UI_USE
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        G4cout << " UI session starts ..." << G4endl;
        UImanager -> ApplyCommand("/control/execute macro/defaultMacro.mac");
        ui -> SessionStart();
        delete ui;
#endif
    }
    else           // Batch mode
    {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager -> ApplyCommand(command+fileName);
    }


#ifdef G4VIS_USE
    delete visManager;
#endif

    delete geometryMessenger;
    delete geometryController;
    delete runManager;
    return 0;

}
