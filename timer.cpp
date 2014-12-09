#include "timer.h"
#include <SDL.h>

//////////////////////////////////////////
// Timer constructor
// Initializes all variables for first use
//////////////////////////////////////////

timer::timer()
{
	//Initialize variables
	startTime = 0;
	pausedTime = 0;

	paused = false;
	started = false;

}

///end section

///////////////////////////////////////
// Method: Start timer
// Starts the time
///////////////////////////////////////
void timer::t_Start()
{
	started = true;
	paused = false;

	startTime = SDL_GetTicks();
}

//end section

//////////////////////////////////////
// Method: Stop timer
// Stops the timer
//////////////////////////////////////
void timer::t_Stop()
{
	started = false;
	paused = false;
}

//end section

//////////////////////////////////////
// Method: Get time
// Returns current timer time
//////////////////////////////////////

int timer::t_GetTime()
{
	if(started == true)
	{
		if(paused == true)
		{
			return pausedTime;   //if timer is on pause then return paused time
		}
		else
		{
			return SDL_GetTicks() - startTime; //otherwize return current time
		}

	}
	return 0;
	//if timer is not running
}

//end section

////////////////////////////////////////
// Metod: Pause
// Pauses timer
////////////////////////////////////////

void timer::t_Pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //pause the timer
        paused = true;
    
        //set paused time
        pausedTime = SDL_GetTicks() - startTime;
    }
}

//end section

////////////////////////////////////////
// Metod: Continue
// Continues counting
////////////////////////////////////////

void timer::t_Continue()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;
    
        //Reset the starting time
        startTime = SDL_GetTicks() - pausedTime;
        
        //Reset the paused time
        pausedTime = 0;
    }
}
