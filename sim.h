//
// Application Independent Simulation Engine Interface
//
//
// Function defined in the simulation engine called by the simulation application
//

// Call this procedure to run the simulation indicating time to end simulation
void RunSim (double EndTime);

// Schedule an event with timestamp ts, event parameters *data
void Schedule (double ts, void *data);

// This function returns the current simulation time
double CurrentTime (void);

//
// Function defined in the simulation application called by the simulation engine
//
//  Event handler function: called to process an event
void EventHandler (void *data);

// Empty the FEL
void RemoveAll(void);

// Print timestamps of all events in the event list
void PrintList (void);