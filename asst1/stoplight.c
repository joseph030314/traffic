/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use any synchronization primitives available to solve
 * the stoplight problem in this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>
#include <machine/spl.h>


/*
 *
 * Constants
 *
 */

/*
 * Number of vehicles created.
 */

#define NVEHICLES 30

typedef struct sleeper_struct
{
	const char *name;
} SleeperStruct;
 
//Inits the route queues and locks
int initRoutesAndLocks();

//creates a sleeper struct with a name
SleeperStruct * createSleeperStruct(const char *name);
 
//convert int type to char type
const char * getType(int type);

//covert direction to string
const char * getRoute(int direction);

//get destination from direction and turn
const char * getDestRoute(unsigned long direction, int turn);

//convert turn to string
const char * getTurn(int direction);
 
 /*Global Variables*/

 //Locks used for the intersections
 struct lock *AB;
 struct lock *BC;
 struct lock *CA;
 
 //Locks used to ensure only 1 vehicle per route can move
 struct lock *routeALock;
 struct lock *routeBLock;
 struct lock *routeCLock;
 
 //Structs for vehicles from various routes to sleep on. This is how the priority of vehicles is achieved.
 SleeperStruct *routeAAmbulance;
 SleeperStruct *routeACar;
 SleeperStruct *routeATruck;
 
 SleeperStruct *routeBAmbulance;
 SleeperStruct *routeBCar;
 SleeperStruct *routeBTruck;
 
 SleeperStruct *routeCAmbulance;
 SleeperStruct *routeCCar;
 SleeperStruct *routeCTruck;
 
 
/*
 *
 * Function Definitions
 *
 */
 
 
/*
 * createSleeperStruct()
 *
 * Arguments:
 *      const char *name: The name of the sleeper struct.
 *   
 *
 * Returns:
 *      SleeperStruct pointer.
 *
 * Notes:
 *    	This is a helper function for creating the SleeperStruct.
 */
SleeperStruct * createSleeperStruct(const char *name)
{
	SleeperStruct *sleeper = kmalloc(sizeof(SleeperStruct));
	if(sleeper == NULL)
	{
		kprintf("Couldn't kmalloc sleeper struct\n");
		return NULL;
	}
	
	sleeper->name = kstrdup(name);
	if(sleeper->name == NULL)
	{
		kprintf("Couldn't kmalloc sleeper name\n.");
		//free(sleeper);
		return NULL;
	}
	
	return sleeper;
	
}
 
 

/*
 * turnleft()
 *
 * Arguments:
 *      unsigned long vehicledirection: the direction from which the vehicle
 *              approaches the intersection.
 *      unsigned long vehiclenumber: the vehicle id number for printing purposes.
 * 		unsigned long vehicletype: the vehicle type for priority handling purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function implements making a left turn through the 
 *      intersection from any direction.
 */

static
void
turnleft(unsigned long vehicledirection,
		unsigned long vehiclenumber,
		unsigned long vehicletype)
{

	//figure out what intersections need to be moved through
	struct lock *intersection1 = NULL;
	struct lock *intersection2 = NULL;
	
	if(vehicledirection == 0)
	{
		intersection1 = AB;
		intersection2 = BC;
	}
	else if(vehicledirection == 1)
	{
		intersection1 = BC;
		intersection2 = CA;
	}
	else
	{
		intersection1 = CA;
		intersection2 = AB;
	}
	
	
	//attempt to move through the intersection
	lock_acquire(intersection1);
	lock_acquire(intersection2);
	
	kprintf("%s %lu from %s to %s to %s ENTERED %s\n", getType(vehicletype), 
													vehiclenumber, 
													getRoute(vehicledirection),
													getTurn(1),
													getDestRoute(vehicledirection, 1),
													intersection1->name);
	
	
	
	kprintf("%s %lu from %s to %s to %s LEFT %s\n", getType(vehicletype), 
													vehiclenumber, 
													getRoute(vehicledirection),
													getTurn(1),
													getDestRoute(vehicledirection, 1),
													intersection1->name);
													
	kprintf("%s %lu from %s to %s to %s ENTERED %s\n", getType(vehicletype), 
													vehiclenumber, 
													getRoute(vehicledirection),
													getTurn(1),
													getDestRoute(vehicledirection, 1),
													intersection2->name);
	
	
	
	kprintf("%s %lu from %s to %s to %s LEFT %s\n", getType(vehicletype), 
													vehiclenumber, 
													getRoute(vehicledirection),
													getTurn(1),
													getDestRoute(vehicledirection, 1),
													intersection2->name);
	
	//release locks on intersections when finished moving through
	lock_release(intersection2);
	lock_release(intersection1);
	
	
}


/*
 * turnright()
 *
 * Arguments:
 *      unsigned long vehicledirection: the direction from which the vehicle
 *              approaches the intersection.
 *      unsigned long vehiclenumber: the vehicle id number for printing purposes.
 * 		unsigned lone vehicletype: the vehicle type for priority handling purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function implements making a right turn through the 
 *      intersection from any direction.
 *  
 */

static
void
turnright(unsigned long vehicledirection,
		unsigned long vehiclenumber,
		unsigned long vehicletype)
{
	/*
	 * Avoid unused variable warnings.
	 */


	//determine which intersection to move through
	struct lock *intersection = NULL;
	if(vehicledirection == 0)
	{
		intersection = AB;
	}
	else if(vehicledirection == 1)
	{
		intersection = BC;
	}
	else
	{
		intersection = CA;
	}
	
	
	//attempt to move through the intersection
	lock_acquire(intersection);
	kprintf("%s %lu from %s to %s to %s ENTERED %s\n", getType(vehicletype), 
													vehiclenumber, 
													getRoute(vehicledirection),
													getTurn(0),
													getDestRoute(vehicledirection, 0),
													intersection->name);
	
	
	kprintf("%s %lu from %s to %s to %s LEFT %s\n", getType(vehicletype), 
													vehiclenumber, 
													getRoute(vehicledirection),
													getTurn(0),
													getDestRoute(vehicledirection, 0),
													intersection->name);
	
	lock_release(intersection);
	
	
	
}


/*
 * wakeupNext()
 *
 * Arguments:
 *      unsigned long vehicledirection: the direction from which the vehicle
 *              approached the intersection.
 *      
 * 		
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function wakes up the highest priority group of sleeping vehicles(threads) from a particular route.
 *  	
 */
 void wakeupNext(int vehicledirection)
 {
	
	//route A
	if(vehicledirection == 0)
	{
		
		if(thread_hassleepers(routeAAmbulance))
			thread_wakeup(routeAAmbulance);
		
		else if(thread_hassleepers(routeACar))
			thread_wakeup(routeACar);
			
		else if(thread_hassleepers(routeATruck))
			thread_wakeup(routeATruck);
			
	}
	
	//route B
	else if(vehicledirection == 1)
	{
		
		if(thread_hassleepers(routeBAmbulance))
			thread_wakeup(routeBAmbulance);
		
		else if(thread_hassleepers(routeBCar))
			thread_wakeup(routeBCar);
			
		
		else if(thread_hassleepers(routeBTruck))
			thread_wakeup(routeBTruck);
			
		
	}
	
	//route C
	else if(vehicledirection == 2)
	{
		if(thread_hassleepers(routeCAmbulance))
			thread_wakeup(routeCAmbulance);
		
		else if(thread_hassleepers(routeCCar))
			thread_wakeup(routeCCar);
			
		else if(thread_hassleepers(routeCTruck))
			thread_wakeup(routeCTruck);
			
	}
	
 }
 
 
/*
 * approachintersection()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long vehiclenumber: holds vehicle id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *     	This function is the main driver of each thread. The function determines the type, turn direction, and type of the vehicle.
 *		Then it declares the approach of the vehicle, determines which lock to wait for and what address to sleep on.
 */

static
void
approachintersection(void * unusedpointer,
		unsigned long vehiclenumber)
{
	int vehicledirection, turndirection, vehicletype;

	/*
	 * Avoid unused variable and function warnings.
	 */

	(void) unusedpointer;

	
	
	//determine vehicle route, turn, and type
	vehicledirection = random() % 3;
	turndirection = random() % 2;
	vehicletype = random() % 3;
	
	
	
	
	//Car 1 from Route A to turn RIGHT to Route B APPROACHES the intersection
	kprintf("%s %lu from %s to %s to %s APPROACHES the intersection\n", getType(vehicletype), 
																	 vehiclenumber, 
																	 getRoute(vehicledirection),
																	 getTurn(turndirection),
																	 getDestRoute(vehicledirection, turndirection));
	
	
	
	//determine desired routeLock and sleeperAdr
	struct lock *routeLock = NULL;
	SleeperStruct *sleepAddr = NULL;
	
	//route A
	if(vehicledirection == 0)
	{
		routeLock = routeALock;
		if(vehicletype == 0)
			sleepAddr = routeAAmbulance;
		
		else if(vehicletype == 1)
			sleepAddr = routeACar;
		
		else if(vehicletype == 2)
			sleepAddr = routeATruck;
		
		
	}
	//route B
	else if(vehicledirection == 1)
	{
		routeLock = routeBLock;
		
		if(vehicletype == 0)
			sleepAddr = routeBAmbulance;
		
		else if(vehicletype == 1)
			sleepAddr = routeBCar;
		
		else if(vehicletype == 2)
			sleepAddr = routeBTruck;
		
	}
	//route C
	else if(vehicledirection == 2)
	{
		routeLock = routeCLock;
		if(vehicletype == 0)
			sleepAddr = routeCAmbulance;
		
		else if(vehicletype == 1)
			sleepAddr = routeCCar;
		
		else if(vehicletype == 2)
			sleepAddr = routeCTruck;
		
	}
	
	
	
	//attempt to be the next vehicle to make a move
	lock_acquire_with_sleepAddr(routeLock, sleepAddr);
	
	
	
	
	if(turndirection == 0)
	{
		turnright(vehicledirection, vehiclenumber, vehicletype);
	}
	
	else
	{
		turnleft(vehicledirection, vehiclenumber, vehicletype);
	}
	
	
	//move has been made
	lock_release(routeLock);
	
	
	int spl = splhigh();
	wakeupNext(vehicledirection);
	splx(spl);
	
	

}

//convert turn to string
const char * getTurn(int direction)
{
	if(direction == 0)
		return "turn RIGHT";
	else
		return "turn LEFT";
}

//get destination from direction and turn
const char * getDestRoute(unsigned long direction, int turn)
{
	//route A
	if(direction == 0)
	{
		//turn right
		if(turn == 0)
			return "Route B";
		else
			return "Route C";
		
	}
	
	//route B
	else if(direction == 1)
	{
		//turn right
		if(turn == 0)
			return "Route C";
		else
			return "Route A";
		
	}
	
	//route C
	else
	{
		//turn right
		if(turn == 0)
			return "Route A";
		else
			return "Route B";
		
	}
	
}

//convert direction into string
const char * getRoute(int direction)
{
	if(direction == 0) 
		return "Route A";
	else if(direction == 1) 
		return "Route B";
	else
		return "Route C";
}

//convert int type into string
const char * getType(int type)
{
	if(type == 0) 
		return "Ambulance";
	else if(type == 1) 
		return "Car";
	else
		return "Truck";
	
}


//Inits the route queues and locks
int initRoutesAndLocks()
{
		
	//intersection locks
	AB = lock_create("AB");
	if(AB == NULL)
	{
		kprintf("Couldn't create lock AB\n");
		return 0;
	}
	
	BC = lock_create("BC");
	if(BC == NULL)
	{
		kprintf("Couldn't create lock BC\n");
		return 0;
	}
	
	CA = lock_create("CA");
	if(CA == NULL)
	{
		kprintf("Couldn't create lock CA\n");
		return 0;
	}
	
	//route queue locks
	routeALock = lock_create("Route A");
	if(routeALock == NULL)
	{
		kprintf("Couldn't create routeA lock\n");
		return 0;
	}
	
	routeBLock = lock_create("Route B");
	if(routeBLock == NULL)
	{
		kprintf("Couldn't create routeB lock\n");
		return 0;
	}
	
	routeCLock = lock_create("Route C");
	if(routeCLock == NULL)
	{
		kprintf("Couldn't create routeC lock\n");
		return 0;
	}
	
	
	routeAAmbulance = createSleeperStruct("Route A Ambulance");
	routeACar = createSleeperStruct("Route A Car");
	routeATruck = createSleeperStruct("Route A Truck");
	
	routeBAmbulance = createSleeperStruct("Route B Ambulance");
	routeBCar = createSleeperStruct("Route B Car");
	routeBTruck = createSleeperStruct("Route B Truck");
	
	routeCAmbulance = createSleeperStruct("Route C Ambulance");
	routeCCar = createSleeperStruct("Route C Car");
	routeCTruck = createSleeperStruct("Route C Truck");
	
	
	return 1;
}

/*
 * createvehicles()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.
 */

int
createvehicles(int nargs,
		char ** args)
{
	int index, error;

	/*
	 * Avoid unused variable warnings.
	 */

	(void) nargs;
	(void) args;
	
	
	//create all locks and sleeper structs
	int initSuccess = initRoutesAndLocks();
	if(initSuccess == 0)
	{
		kprintf("Couldn't init all routes and locks.\n");
		return 1;
	}
	
	//get all the route locks so all the vehicles start at the same time
	lock_acquire(routeALock);
	lock_acquire(routeBLock);
	lock_acquire(routeCLock);
	
	
	
	/*
	 * Start NVEHICLES approachintersection() threads.
	 */

	for (index = 0; index < NVEHICLES; index++) {
		
			
		
		error = thread_fork("approachintersection thread",
				NULL, 
				index,
				approachintersection,
				NULL
				);

		/*
		 * panic() on error.
		 */

		if (error) {

			panic("approachintersection: thread_fork failed: %s\n",
					strerror(error)
				 );
		}
	}
	
	//report all threads created
	kprintf("All %d Vehicles Have Been Created.\n", NVEHICLES);
	
	
	//start all routes at once
	kprintf("Starting intersection.\n");
	
	lock_release_no_wakeup(routeALock);
	lock_release_no_wakeup(routeBLock);
	lock_release_no_wakeup(routeCLock);
	
	
	int spl = splhigh();
	
	//wake up routes A, B, C
	wakeupNext(0);
	wakeupNext(1); 
	wakeupNext(2); 
	
	splx(spl);
	

	return 0;
}
