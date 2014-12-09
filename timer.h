#ifndef TIMER_H
#define TIMER_H

class timer
{
	private:
		int startTime;  //Keeps starting time
		int pausedTime; //Keeps time at what timer has been paused
		
		bool paused;  //is paused
	
	public:

		bool started; //is working already
		
		timer();	//timer constructor

		void t_Start();
		void t_Stop();
		void t_Pause();
		void t_Continue();

		int t_GetTime();

};

#endif
